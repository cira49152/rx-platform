

/****************************************************************************
*
*  model\rx_model_algorithms.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_model_algorithms
#include "model/rx_model_algorithms.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_async_functions.h"


namespace model {

namespace algorithms {
//////////////////////////////////////////////////////////////////////////////////////////
//explicit instantiation
template class types_model_algorithm<object_type>;
template class types_model_algorithm<port_type>;
template class types_model_algorithm<domain_type>;
template class types_model_algorithm<application_type>;

template class simple_types_model_algorithm<struct_type>;
template class simple_types_model_algorithm<variable_type>;
template class simple_types_model_algorithm<source_type>;
template class simple_types_model_algorithm<filter_type>;
template class simple_types_model_algorithm<event_type>;
template class simple_types_model_algorithm<mapper_type>;

template class runtime_model_algorithm<object_type>;
template class runtime_model_algorithm<port_type>;
template class runtime_model_algorithm<domain_type>;
template class runtime_model_algorithm<application_type>;
//////////////////////////////////////////////////////////////////////////////////////////

// Parameterized Class model::algorithms::types_model_algorithm 


template <class typeT>
void types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref)
{
	std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
		return check_type_sync(loc_name, loc_dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
type_check_context types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	type_check_context ret;
	rx_platform_item::smart_ptr item = dir->get_sub_item(name);
	if (!item)
	{
		ret.add_error(name + " does not exists!");
		return ret;
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// error, item does not have id
		ret.add_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	platform_types_manager::instance().internal_get_type_cache<typeT>().check_type(id, ret);
	return ret;
}

template <class typeT>
void types_model_algorithm<typeT>::create_type (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return create_type_sync(name, base_name, prototype, dir, attributes);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::create_type_sync (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes)
{
	prototype->meta_info().resolve();
	rx_node_id base_id = prototype->meta_info().get_parent();
	rx_node_id item_id = prototype->meta_info().get_id();
	string_type type_name = prototype->meta_info().get_name();
	if (!attributes)
		attributes = prototype->meta_info().get_attributes();

	if (!dir)
	{
		if (!prototype->meta_info().get_path().empty())
			dir = rx_gate::instance().get_root_directory()->get_sub_directory(prototype->meta_info().get_path());
		if(!dir)
			dir = rx_gate::instance().get_root_directory();
	}

	if (type_name.empty())
		type_name = name;

	string_type path;
	auto dir_result = dir->reserve_name(type_name, path);
	if (!dir_result)
		return dir_result.errors();

	if (!base_id && !base_name.empty())
	{
		rx_platform_item::smart_ptr item = dir->get_sub_item(base_name);
		if (!item)
		{// type does not exists
			dir->cancel_reserve(type_name);
			return "Type "s + base_name + " does not exists!";
		}
		base_id = item->meta_info().get_id();
		if (base_id.is_null())
		{// item does not have id
			dir->cancel_reserve(type_name);
			return base_name + " does not have valid Id!";
		}
		if (item->get_type_id() != typeT::type_id)
		{// item is of the wrong type
			dir->cancel_reserve(type_name);
			return base_name + " is wrong type!";
		}
	}
	if (!item_id)
		item_id = rx_node_id::generate_new();

	prototype->meta_info().construct(type_name, item_id, base_id, attributes, path);
	auto result = prototype->resolve(dir);
	if (!result)
	{
		dir->cancel_reserve(type_name);
		return result.errors();
	}

	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().register_type(prototype);
	if (!ret)
	{// error, didn't created runtime
		dir->cancel_reserve(type_name);
		return ret.errors();
	}
	if (!dir->add_item(prototype->get_item_ptr()))
	{
		dir->cancel_reserve(type_name);
		platform_types_manager::instance().internal_get_type_cache<typeT>().delete_type(prototype->meta_info().get_id());
		// error, can't add this name
		return "Unable to add "s + type_name + " to directory!";
	}
	else if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typename typeT::smart_ptr> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
	}
	META_LOG_TRACE("types_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::type_id) + " "s + type_name);
	return prototype;
}

template <class typeT>
void types_model_algorithm<typeT>::delete_type (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	std::function<rx_result(string_type, rx_directory_ptr)> func = [=](string_type name, rx_directory_ptr dir) {
		return delete_type_sync(name, dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
rx_result types_model_algorithm<typeT>::delete_type_sync (const string_type& name, rx_directory_ptr dir)
{
	rx_platform_item::smart_ptr item = dir->get_sub_item(name);
	if (!item)
	{// error, item does not exists
		return name + " does not exists";
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// error, item does not have id
		return name + " does not have id";
	}
	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().delete_type(id);
	if (!ret)
	{// error, didn't deleted runtime
		return ret;
	}
	dir->delete_item(name);
	return true;
}

template <class typeT>
void types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, dir, increment_version);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version)
{
	prototype->meta_info().resolve();
	prototype->meta_info().increment_version(increment_version);
	auto result = prototype->resolve(dir);
	if (!result)
	{
		return result.errors();
	}

	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().update_type(prototype);
	if (!ret)
	{// error, didn't updated type
		return ret.errors();
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typename typeT::smart_ptr> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
	}
	return prototype;
}


// Parameterized Class model::algorithms::simple_types_model_algorithm 


template <class typeT>
void simple_types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref)
{
	std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
		return check_type_sync(loc_name, loc_dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
type_check_context simple_types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	type_check_context ret;
	rx_platform_item::smart_ptr item = dir->get_sub_item(name);
	if (!item)
	{
		ret.add_error(name + " does not exists!");
		return ret;
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// error, item does not have id
		ret.add_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	platform_types_manager::instance().internal_get_simple_type_cache<typeT>().check_type(id, ret);
	return ret;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::create_type (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return create_type_sync(name, base_name, prototype, dir, attributes);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::create_type_sync (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes)
{

	prototype->meta_info().resolve();
	rx_node_id base_id = prototype->meta_info().get_parent();
	rx_node_id item_id = prototype->meta_info().get_id();
	string_type type_name = prototype->meta_info().get_name();
	if (!attributes)
		attributes = prototype->meta_info().get_attributes();

	if (!dir)
	{
		if (!prototype->meta_info().get_path().empty())
			dir = rx_gate::instance().get_root_directory()->get_sub_directory(prototype->meta_info().get_path());
		if (!dir)
			dir = rx_gate::instance().get_root_directory();
	}

	if (type_name.empty())
		type_name = name;

	string_type path;
	auto dir_result = dir->reserve_name(type_name, path);
	if (!dir_result)
		return dir_result.errors();

	if (!base_id && !base_name.empty())
	{
		rx_platform_item::smart_ptr item = dir->get_sub_item(base_name);
		if (!item)
		{// type does not exists
			dir->cancel_reserve(type_name);
			return "Type "s + base_name + " does not exists!";
		}
		base_id = item->meta_info().get_id();
		if (base_id.is_null())
		{// item does not have id
			dir->cancel_reserve(type_name);
			return base_name + " does not have valid Id!";
		}
		if (item->get_type_id() != typeT::type_id)
		{// item is of the wrong type
			dir->cancel_reserve(type_name);
			return base_name + " is wrong type!";
		}
	}
	if (!item_id)
		item_id = rx_node_id::generate_new();

	prototype->meta_info().construct(type_name, item_id, base_id, attributes, path);
	auto result = prototype->resolve(dir);
	if (!result)
	{
		dir->cancel_reserve(type_name);
		return result.errors();
	}

	auto ret = platform_types_manager::instance().internal_get_simple_type_cache<typeT>().register_type(prototype);
	if (!ret)
	{// error, didn't created runtime
		dir->cancel_reserve(type_name);
		return ret.errors();
	}
	if (!dir->add_item(prototype->get_item_ptr()))
	{
		dir->cancel_reserve(type_name);
		platform_types_manager::instance().internal_get_simple_type_cache<typeT>().delete_type(prototype->meta_info().get_id());
		// error, can't add this name
		return "Unable to add "s + type_name + " to directory!";
	}
	else if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typename typeT::smart_ptr> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
	}

	META_LOG_TRACE("simple_types_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::type_id) + " "s + type_name);
	return prototype;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::delete_type (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	std::function<rx_result(string_type, rx_directory_ptr)> func = [=](string_type name, rx_directory_ptr dir) {
		return delete_type_sync(name, dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
rx_result simple_types_model_algorithm<typeT>::delete_type_sync (const string_type& name, rx_directory_ptr dir)
{
	rx_platform_item::smart_ptr item = dir->get_sub_item(name);
	if (!item)
	{// error, item does not exists
		return name + " does not exists!";
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// error, item does not have id
		return name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto ret = platform_types_manager::instance().internal_get_simple_type_cache<typeT>().delete_type(id);
	if (!ret)
	{// error, didn't deleted runtime
		return ret;
	}
	dir->delete_item(name);
	return true;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, dir, increment_version);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version)
{
	prototype->meta_info().resolve();
	prototype->meta_info().increment_version(increment_version);
	auto result = prototype->resolve(dir);
	if (!result)
	{
		return result.errors();
	}

	auto ret = platform_types_manager::instance().internal_get_simple_type_cache<typeT>().update_type(prototype);
	if (!ret)
	{// error, didn't updated type
		return ret.errors();
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typename typeT::smart_ptr> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
	}
	return prototype;
}


// Parameterized Class model::algorithms::runtime_model_algorithm 


template <class typeT>
void runtime_model_algorithm<typeT>::delete_runtime (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	auto result_target = rx_thread_context();
	std::function<void(rx_thread_handle_t, rx_reference_ptr)> func= [=](rx_thread_handle_t result_target, rx_reference_ptr ref)
		{
			auto res = delete_runtime_sync(name, dir, result_target, callback, ref);
			if (!res)
			{
				auto jt = rx_gate::instance().get_infrastructure().get_executer(result_target);
				jt->append(
					rx_create_reference<jobs::lambda_job<rx_result, rx_reference_ptr> >(
						[=](rx_result&& ret_val) mutable
						{
							callback(std::move(ret_val));
						},
						std::move(res), ref));
			}
		};
	rx_post_function_to<rx_reference_ptr, rx_thread_handle_t, rx_reference_ptr>(RX_DOMAIN_META, func
		, ref, result_target, ref);
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (const string_type& name, rx_directory_ptr dir, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	rx_platform_item::smart_ptr item = dir->get_sub_item(name);
	if (!item)
	{// error, item does not exists
		return name + " does not exists!";
	}
	return delete_runtime_sync(item->meta_info(), result_target, callback, ref);
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [info, init_data, dir, ref, instance_data]() mutable {
		return create_runtime_sync(info, init_data, std::move(instance_data), dir, ref);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref)
{

	std::unique_ptr<data::runtime_values_data> temp;
	if (init_data)
		temp = std::unique_ptr<data::runtime_values_data>(init_data);
	string_type name = info.get_name();
	string_type path;

	auto dir_result = dir->reserve_name(name, path);
	if (!dir_result)
		return dir_result.errors();

	rx_node_id type_id = info.get_parent();

	meta_data meta(info);
	meta.resolve();
	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().create_runtime(meta, std::move(instance_data), init_data);
	if (!ret)
	{// error, didn't created runtime
		dir->cancel_reserve(name);
		return ret.errors();
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto init_result = init_runtime(ret);
		if (!init_result)
		{
			dir->cancel_reserve(name);
			platform_types_manager::instance().internal_get_type_cache<typeT>().delete_runtime(ret.value()->meta_info().get_id());
			init_result.register_error("Unable to initialize "s + rx_item_type_name(typeT::RType::type_id) + " " + name);
			return init_result.errors();
		}
	}

	if (!dir->add_item(ret.value()->get_item_ptr()))
	{
		dir->cancel_reserve(name);
		platform_types_manager::instance().internal_get_type_cache<typeT>().delete_runtime(ret.value()->meta_info().get_id());
		return "Unable to add "s + name + " to directory!";
	}
	else if (rx_gate::instance().get_platform_status() == rx_platform_running)
	{
		auto save_result = ret.value()->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typename typeT::RTypePtr> ret(save_result.errors());
			ret.register_error("Error saving type item "s + ret.value()->meta_info().get_full_path());
			return ret;
		}
	}
	if (ret)
	{
		META_LOG_TRACE("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::RType::type_id) + " "s + info.get_name());
	}
	return ret;
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_prototype (const string_type& name, const string_type& type_name, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [name, type_name, dir, attributes]() mutable {
		return create_prototype_sync(name, type_name, dir, attributes);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_prototype_sync (const string_type& name, const string_type& type_name, rx_directory_ptr dir, namespace_item_attributes attributes)
{
	string_type path;
	auto dir_result = dir->reserve_name(name, path);
	if (!dir_result)
		return dir_result.errors();
	else
		dir->cancel_reserve(name);// cancel it straight away;
	rx_platform_item::smart_ptr item = dir->get_sub_item(type_name);
	if (!item)
	{// error, type does not exists
		return "Type "s + type_name + " does not exists!";
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// error, item does not have id
		return type_name + " does not have valid Id!";
	}
	meta_data meta;
	meta.construct(name, rx_node_id::null_id, id, attributes, path);
	typename typeT::instance_data_t data;
	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().create_runtime(meta, std::move(data), nullptr, true);

	return ret;
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime_implicit (const string_type& name, const string_type& type_name, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [name, type_name, attributes, init_data, dir, ref, instance_data]() mutable {
		return create_runtime_implicit_sync(name, type_name, attributes, init_data, std::move(instance_data), dir, ref);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);

}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_implicit_sync (const string_type& name, const string_type& type_name, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref)
{
	string_type path;
	dir->fill_path(path);

	rx_platform_item::smart_ptr item = dir->get_sub_item(type_name);
	if (!item)
	{
		dir->cancel_reserve(name);
		return "Type "s + type_name + " does not exists!";
	}
	rx_node_id type_id = item->meta_info().get_id();
	if (type_id.is_null())
	{
		dir->cancel_reserve(name);
		return type_name + " does not have valid Id!";
	}
	meta_data info;
	info.construct(name, rx_node_id::null_id, type_id, attributes, path);
	return create_runtime_sync(info, init_data, std::move(instance_data), dir, ref);
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::init_runtime (typename typeT::RTypePtr what)
{
	runtime::runtime_init_context ctx;
	auto init_result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(what, ctx);
	if (init_result)
	{// make object running in state

		auto init_result = platform_types_manager::instance().internal_get_type_cache<typeT>().mark_runtime_running(what->meta_info().get_id());
	}
	return init_result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::RTypePtr>;
	auto result_target = rx_thread_context();
	std::function<void(rx_thread_handle_t)> func = [=](rx_thread_handle_t target) {
		auto ret = update_runtime_sync(info, init_data, instance_data, increment_version, dir, callback, ref, target);
		if (!ret)
			rx_post_result_to<decltype(ref), typename typeT::RTypePtr>(result_target, [callback](result_t&& res)
				{
					callback(std::move(res));
				}, ref, result_t(ret.errors()));
	};
	rx_post_function_to<decltype(ref), rx_thread_handle_t>(RX_DOMAIN_META, func, ref, result_target);
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::update_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref, rx_thread_handle_t result_target)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto obj_ptr = platform_types_manager::instance().internal_get_type_cache<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	runtime::runtime_deinit_context ctx;
	meta_data meta_info(info);
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), [meta_info, increment_version, callback, init_data, instance_data, dir, ref] (rx_result&& deinit_result) mutable
		{
			auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().delete_runtime(meta_info.get_id());
			if (ret)
			{
				auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(meta_info.get_path());
				if (dir)
					dir->delete_item(meta_info.get_name());
				if (rx_gate::instance().get_platform_status() == rx_platform_running)
				{
					auto storage_result = meta_info.resolve_storage();
					if (storage_result)
					{
						auto item_result = storage_result.value()->get_item_storage(meta_info);
						if (item_result)
						{
							item_result.value()->delete_item();
							META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + meta_info.get_name());
						}
					}
				}
				meta_info.increment_version(increment_version);
				auto create_result = create_runtime_sync(meta_info, init_data, instance_data, dir, ref);
					callback(std::move(create_result));
			}
			if (!ret)
				callback(std::move(ret.errors()));
		}, ctx);

	return result;
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::helper_delete_runtime_sync (meta_data_t info)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().delete_runtime(id);
	if (ret)
	{
		auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(info.get_path());
		if (dir)
			dir->delete_item(info.get_name());
		if (rx_gate::instance().get_platform_status() == rx_platform_running)
		{
			auto storage_result = info.resolve_storage();
			if (storage_result)
			{
				auto item_result = storage_result.value()->get_item_storage(info);
				if (item_result)
				{
					item_result.value()->delete_item();
					META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + info.get_name());
				}
			}
		}
	}
	return ret;
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (meta_data_t info, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto obj_ptr = platform_types_manager::instance().internal_get_type_cache<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	runtime::runtime_deinit_context ctx;
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), [info, callback](rx_result&& deinit_result)
		{
			auto ret = platform_types_manager::instance().internal_get_type_cache<typeT>().delete_runtime(info.get_id());
			if (ret)
			{
				auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(info.get_path());
				if (dir)
					dir->delete_item(info.get_name());
				if (rx_gate::instance().get_platform_status() == rx_platform_running)
				{
					auto storage_result = info.resolve_storage();
					if (storage_result)
					{
						auto item_result = storage_result.value()->get_item_storage(info);
						if (item_result)
						{
							item_result.value()->delete_item();
							META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + info.get_name());
						}
					}
				}
			}
			callback(std::move(ret));
		}, ctx);

	return result;
}


} // namespace algorithms
} // namespace model




/****************************************************************************
*
*  system\meta\rx_types.cpp
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

#include "rx_configuration.h"

// rx_types
#include "system/meta/rx_types.h"

#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {

namespace meta {

namespace basic_types {

// Class rx_platform::meta::basic_types::event_type 

string_type event_type::type_name = RX_CPP_EVENT_CLASS_TYPE_NAME;

event_type::event_type()
{
}

event_type::event_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



platform_item_ptr event_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result event_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<event_type>::serialize_basic_type(*this, stream, type);
}

rx_result event_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<event_type>::deserialize_basic_type(*this, stream, type);
}

meta_data& event_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& event_type::complex_data ()
{
  return complex_data_;

}

rx_result event_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<event_type>::construct_basic_type(*this, ctx);
}

bool event_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<event_type>::check_basic_type(*this, ctx);
}

rx_result event_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<event_type>::resolve_basic_type(*this, dir);
}

rx_result event_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const def_blocks::complex_data_type& event_type::complex_data () const
{
  return complex_data_;
}

const meta_data& event_type::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::meta::basic_types::filter_type 

string_type filter_type::type_name = RX_CPP_FILTER_CLASS_TYPE_NAME;

filter_type::filter_type()
{
}

filter_type::filter_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



platform_item_ptr filter_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result filter_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<filter_type>::serialize_basic_type(*this, stream, type);
}

rx_result filter_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<filter_type>::deserialize_basic_type(*this, stream, type);
}

meta_data& filter_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& filter_type::complex_data ()
{
  return complex_data_;

}

rx_result filter_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<filter_type>::construct_basic_type(*this, ctx);
}

bool filter_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<filter_type>::check_basic_type(*this, ctx);
}

rx_result filter_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<filter_type>::resolve_basic_type(*this, dir);
}

rx_result filter_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const def_blocks::complex_data_type& filter_type::complex_data () const
{
  return complex_data_;
}

const meta_data& filter_type::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::meta::basic_types::mapper_type 

string_type mapper_type::type_name = RX_CPP_MAPPER_CLASS_TYPE_NAME;

mapper_type::mapper_type()
{
}

mapper_type::mapper_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



platform_item_ptr mapper_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result mapper_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<mapper_type>::serialize_basic_type(*this, stream, type);
}

rx_result mapper_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<mapper_type>::deserialize_basic_type(*this, stream, type);
}

meta_data& mapper_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& mapper_type::complex_data ()
{
  return complex_data_;

}

rx_result mapper_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<mapper_type>::construct_basic_type(*this, ctx);
}

bool mapper_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<mapper_type>::check_basic_type(*this, ctx);
}

rx_result mapper_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<mapper_type>::resolve_basic_type(*this, dir);
}

rx_result mapper_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const meta_data& mapper_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::complex_data_type& mapper_type::complex_data () const
{
  return complex_data_;
}


// Class rx_platform::meta::basic_types::source_type 

string_type source_type::type_name = RX_CPP_SOURCE_CLASS_TYPE_NAME;

source_type::source_type()
{
}

source_type::source_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



platform_item_ptr source_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result source_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<source_type>::serialize_basic_type(*this, stream, type);
}

rx_result source_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<source_type>::deserialize_basic_type(*this, stream, type);
}

meta_data& source_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& source_type::complex_data ()
{
  return complex_data_;

}

rx_result source_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<source_type>::construct_basic_type(*this, ctx);
}

bool source_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<source_type>::check_basic_type(*this, ctx);
}

rx_result source_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<source_type>::resolve_basic_type(*this, dir);
}

rx_result source_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const def_blocks::complex_data_type& source_type::complex_data () const
{
  return complex_data_;
}

const meta_data& source_type::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::meta::basic_types::struct_type 

string_type struct_type::type_name = RX_CPP_STRUCT_CLASS_TYPE_NAME;

struct_type::struct_type()
{
}

struct_type::struct_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



rx_result struct_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<struct_type>::construct_basic_type(*this, ctx);
}

platform_item_ptr struct_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result struct_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<struct_type>::serialize_basic_type(*this, stream, type);
}

rx_result struct_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<struct_type>::deserialize_basic_type(*this, stream, type);
}

meta_data& struct_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& struct_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& struct_type::mapping_data ()
{
  return mapping_data_;

}

bool struct_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<struct_type>::check_basic_type(*this, ctx);
}

rx_result struct_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<struct_type>::resolve_basic_type(*this, dir);
}

rx_result struct_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const def_blocks::complex_data_type& struct_type::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& struct_type::mapping_data () const
{
  return mapping_data_;
}

const meta_data& struct_type::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::meta::basic_types::variable_type 

string_type variable_type::type_name = RX_CPP_VARIABLE_CLASS_TYPE_NAME;

variable_type::variable_type()
{
}

variable_type::variable_type (const type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



rx_result variable_type::construct (RTypePtr& what, construct_context& ctx) const
{
	return basic_types_algorithm<variable_type>::construct_basic_type(*this, ctx);
}

rx_result variable_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return basic_types_algorithm<variable_type>::serialize_basic_type(*this, stream, type);
}

rx_result variable_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return basic_types_algorithm<variable_type>::deserialize_basic_type(*this, stream, type);
}

platform_item_ptr variable_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_value variable_type::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_created_time());
	return temp;
}

meta_data& variable_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& variable_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& variable_type::mapping_data ()
{
  return mapping_data_;

}

def_blocks::variable_data_type& variable_type::variable_data ()
{
  return variable_data_;

}

bool variable_type::check_type (type_check_context& ctx)
{
	return basic_types_algorithm<variable_type>::check_basic_type(*this, ctx);
}

rx_result variable_type::resolve (rx_directory_ptr dir)
{
	return basic_types_algorithm<variable_type>::resolve_basic_type(*this, dir);
}

rx_result variable_type::assign_storage (rx_storage_item_ptr&& item)
{
	return storage_.assign_storage(std::move(item));
}


const meta_data& variable_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::variable_data_type& variable_type::variable_data () const
{
  return variable_data_;
}

const def_blocks::mapped_data_type& variable_type::mapping_data () const
{
  return mapping_data_;
}

const def_blocks::complex_data_type& variable_type::complex_data () const
{
  return complex_data_;
}


} // namespace basic_types
} // namespace meta
} // namespace rx_platform




/****************************************************************************
*
*  system\meta\rx_obj_classes.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_obj_classes
#include "system/meta/rx_obj_classes.h"

#include "rx_configuration.h"
#include "system/meta/rx_objbase.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace meta {

// Parameterized Class rx_platform::meta::base_object_class 

template <class metaT, bool _browsable>
base_object_class<metaT,_browsable>::base_object_class()
{
}

template <class metaT, bool _browsable>
base_object_class<metaT,_browsable>::base_object_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: base_mapped_class<metaT, _browsable>(name, id, parent, system, sealed, abstract)
{
}



template <class metaT, bool _browsable>
bool base_object_class<metaT,_browsable>::serialize_object_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!base_mapped_class<metaT, _browsable>::serialize_mapped_definition(stream, type))
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool base_object_class<metaT,_browsable>::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	if (!base_mapped_class<metaT, _browsable>::deserialize_mapped_definition(stream, type))
		return false;

	return true;
}

template <class metaT, bool _browsable>
void base_object_class<metaT,_browsable>::construct (complex_runtime_ptr what)
{
	base_mapped_class<metaT, _browsable>::construct(what);
}

template <class metaT, bool _browsable>
void base_object_class<metaT,_browsable>::construct (objects::object_runtime_ptr what)
{
	typedef typename metaT::RType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
	runtime->programs_ = programs_;
	construct(runtime->get_complex_item());
}


// Class rx_platform::meta::object_class 

string_type object_class::type_name = RX_CPP_OBJECT_CLASS_TYPE_NAME;

object_class::object_class()
{
}

object_class::object_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_type_adapter(name, id, system)
{
}


object_class::~object_class()
{
}



void object_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

namespace_item_attributes object_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_class | namespace_item_attributes::namespace_item_execute_access | namespace_item_attributes::namespace_item_read_access | (get_meta().get_system() ?  namespace_item_attributes::namespace_item_system : namespace_item_attributes::namespace_item_null) );
}

void object_class::construct (objects::object_runtime_ptr what)
{
	get_meta().construct(what);
}

void object_class::construct (complex_runtime_ptr what)
{
}

bool object_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return get_meta().serialize_object_definition(stream, type);
}

bool object_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return get_meta().deserialize_object_definition(stream, type);
}

platform_item_ptr object_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::domain_class 

string_type domain_class::type_name = RX_CPP_DOMAIN_CLASS_TYPE_NAME;

domain_class::domain_class()
{
}

domain_class::domain_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_type_adapter(name,id,system)
{
}


domain_class::~domain_class()
{
}



void domain_class::construct (objects::object_runtime_ptr what)
{
	get_meta().construct(what->get_complex_item());
}

void domain_class::construct (complex_runtime_ptr what)
{
}

bool domain_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return get_meta().serialize_object_definition(stream, type);
}

bool domain_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return get_meta().deserialize_object_definition(stream, type);
}

platform_item_ptr domain_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::application_class 

string_type application_class::type_name = RX_CPP_APPLICATION_CLASS_TYPE_NAME;

application_class::application_class()
{
}

application_class::application_class (const string_type& name, const rx_node_id& id, bool system)
{
}


application_class::~application_class()
{
}



void application_class::construct (objects::object_runtime_ptr what)
{
	get_meta().construct(what->get_complex_item());
}

void application_class::construct (complex_runtime_ptr what)
{
}

bool application_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return get_meta().serialize_object_definition(stream, type);
}

bool application_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return get_meta().deserialize_object_definition(stream, type);
}

platform_item_ptr application_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::port_class 

string_type port_class::type_name = RX_CPP_PORT_CLASS_TYPE_NAME;

port_class::port_class()
{
}

port_class::port_class (const string_type& name, const rx_node_id& id, bool system)
{
}


port_class::~port_class()
{
}



void port_class::construct (objects::object_runtime_ptr what)
{
	get_meta().construct(what->get_complex_item());
}

void port_class::construct (complex_runtime_ptr what)
{
}

bool port_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return get_meta().serialize_object_definition(stream, type);
}

bool port_class::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
{
	return get_meta().deserialize_object_definition(stream, type);
}

platform_item_ptr port_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}

template class rx_platform::meta::base_object_class<rx_platform::meta::object_class, false>;
template class rx_platform::meta::base_object_class<rx_platform::meta::domain_class, false>;
template class rx_platform::meta::base_object_class<rx_platform::meta::application_class, false>;
template class rx_platform::meta::base_object_class<rx_platform::meta::port_class, false>;
} // namespace meta
} // namespace rx_platform


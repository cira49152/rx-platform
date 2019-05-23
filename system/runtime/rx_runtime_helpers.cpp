

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.cpp
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


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "rx_configuration.h"


namespace rx_platform {

namespace runtime {

// Class rx_platform::runtime::runtime_deinit_context 


// Class rx_platform::runtime::runtime_start_context 


// Class rx_platform::runtime::runtime_stop_context 


// Class rx_platform::runtime::runtime_init_context 


// Class rx_platform::runtime::runtime_path_resolver 


void runtime_path_resolver::push_to_path (const string_type& name)
{
	if (!path_.empty())
		path_ += RX_OBJECT_DELIMETER;
	path_ += name;
}

void runtime_path_resolver::pop_from_path ()
{
	RX_ASSERT(!path_.empty());
	if (!path_.empty())
	{
		size_t idx = path_.rfind(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			path_.clear();
		}
		else
		{
			path_.resize(idx);
		}
	}
}

const string_type& runtime_path_resolver::get_current_path () const
{
	return path_;
}


// Class rx_platform::runtime::variables_stack 


void variables_stack::push_variable (rx_variable_ptr what)
{
	variables_.push(what);
}

void variables_stack::pop_variable ()
{
	if (!variables_.empty())
		variables_.pop();
}

rx_variable_ptr variables_stack::get_current_variable () const
{
	if (!variables_.empty())
		return variables_.top();
	else
		return rx_variable_ptr::null_ptr;
}


// Class rx_platform::runtime::runtime_structure_resolver 

runtime_structure_resolver::runtime_structure_resolver()
      : root_(nullptr)
{
}



void runtime_structure_resolver::push_item (structure::runtime_item& item)
{
	items_.push(item);
}

void runtime_structure_resolver::pop_item ()
{
	RX_ASSERT(!items_.empty());
	if (!items_.empty())
		items_.pop();
}

structure::runtime_item& runtime_structure_resolver::get_current_item ()
{
	RX_ASSERT(!items_.empty());
	static structure::empty_item_data g_empty;
	if (!items_.empty())
		return items_.top();
	else
		return g_empty;
}

blocks::runtime_object* runtime_structure_resolver::get_root ()
{
	return root_;
}

void runtime_structure_resolver::set_root (blocks::runtime_object* item)
{
	root_ = item;
}


} // namespace runtime
} // namespace rx_platform

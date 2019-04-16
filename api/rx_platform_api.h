

/****************************************************************************
*
*  api\rx_platform_api.h
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


#ifndef rx_platform_api_h
#define rx_platform_api_h 1




#include "rx_library.h"
#include "system/server/rx_ns.h"
#include "system/runtime/rx_objbase.h"
#include "system/meta/rx_obj_types.h"
namespace rx_platform
{
namespace api
{



struct query_result_detail
{
	rx_node_id id;
	string_type name; 
	rx_node_id parent;
	uint32_t version;
	rx_time created_time;
	rx_time modified_time;
	namespace_item_attributes attributes;
	string_type path;
};

struct query_result
{
	platform_items_type items;

	bool success = false;
	operator bool() const
	{
		return success;
	}
};



struct rx_context
{
	rx_directory_ptr directory;
	rx_object_ptr object;
};

}
}




#endif

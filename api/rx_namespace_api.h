

/****************************************************************************
*
*  api\rx_namespace_api.h
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


#ifndef rx_namespace_api_h
#define rx_namespace_api_h 1




#include "rx_platform_api.h"
#include "system/server/rx_ns.h"
namespace rx_platform
{
namespace api
{
namespace ns
{

rx_result rx_get_directory(
	const string_type& name // directories's path
	, std::function<void(rx_result_with<rx_directory_ptr>&&)> callback
	, rx_context ctx);

rx_result rx_get_item(
	const string_type& name // directories's path
	, std::function<void(rx_result_with<platform_item_ptr>&&)> callback
	, rx_context ctx);

struct directory_browse_result
{
	platform_directories_type directories;
	platform_items_type items;

	bool success = false;
	operator bool() const
	{
		return success;
	}
};

rx_result rx_list_directory(
	const string_type& name // directories's path
	, std::function<void(rx_result_with<directory_browse_result>&&)> callback
	, rx_context ctx);

}
}
}




#endif
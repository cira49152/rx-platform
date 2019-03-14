

/****************************************************************************
*
*  win32_hosts\rx_win32_pipe.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#include "rx_win32_pipe_version.h"
#include "rx_win32_common.h"

// rx_win32_pipe
#include "win32_hosts/rx_win32_pipe.h"



namespace win32 {

// Class win32::win32_pipe_host 

win32_pipe_host::win32_pipe_host (hosting::rx_host_storages& storage)
	: rx_pipe_host(storage)
{
}


win32_pipe_host::~win32_pipe_host()
{
}



string_type win32_pipe_host::get_config_path () const
{
	string_type ret;
	get_full_path("config", ret);
	return ret;
}

string_type win32_pipe_host::get_default_name () const
{
	string_type ret;
	get_host_name(ret);
	return ret;
}

void win32_pipe_host::get_host_info (string_array& hosts)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_PIPE_HOST_NAME, RX_WIN32_PIPE_HOST_MAJOR_VERSION, RX_WIN32_PIPE_HOST_MINOR_VERSION, RX_WIN32_PIPE_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	host::pipe::rx_pipe_host::get_host_info(hosts);
}


} // namespace win32


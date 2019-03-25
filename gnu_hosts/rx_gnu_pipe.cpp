

/****************************************************************************
*
*  gnu_hosts\rx_gnu_pipe.cpp
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

#include "rx_gnu_pipe_version.h"
#include "rx_gnu_common.h"

// rx_gnu_pipe
#include "gnu_hosts/rx_gnu_pipe.h"



namespace gnu {

// Class gnu::gnu_pipe_host 

gnu_pipe_host::gnu_pipe_host (hosting::rx_host_storages& storage)
{
}


gnu_pipe_host::~gnu_pipe_host()
{
}



string_type gnu_pipe_host::get_config_path () const
{
}

string_type gnu_pipe_host::get_default_name () const
{
}

void gnu_pipe_host::get_host_info (string_array& hosts)
{
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_GNU_PIPE_HOST_NAME, RX_GNU_PIPE_HOST_MAJOR_VERSION, RX_GNU_PIPE_OST_MINOR_VERSION, RX_GNU_PIPE_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	host::pipe::rx_pipe_host::get_host_info(hosts);
}

string_type gnu_pipe_host::get_gnu_pipe_info ()
{
}


} // namespace gnu

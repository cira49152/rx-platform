

/****************************************************************************
*
*  system\server\rx_mngt.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_inf
#include "sys_internal/rx_inf.h"
// rx_commands
#include "terminal/rx_commands.h"
// rx_mngt
#include "system/server/rx_mngt.h"

#include "lib/rx_lib.h"
#include "system/server/rx_server.h"
#include "terminal/rx_vt100.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_objects.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "sys_internal/rx_internal_protocol.h"
#include "protocols/opcua/rx_opcua_mapping.h"

using namespace rx_platform;
using namespace rx_internal::terminal::commands;
using namespace rx;


namespace rx_platform {

namespace mngt {

// Class rx_platform::mngt::server_manager 

server_manager::server_manager()
      : telnet_port_(0)
{
}


server_manager::~server_manager()
{
}



rx_result server_manager::initialize (hosting::rx_platform_host* host, management_data_t& data)
{
    // call up the singletons before start
    /*unassigned_domain_ = ns_data.unassigned_domain;
    system_domain_ = ns_data.system_domain;
    unassigned_app_ = ns_data.unassigned_application;
    system_app_ = ns_data.system_application*/
    
    // handle command stuff!
	server_command_manager::instance()->register_internal_commands();
    // register constructors
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
        RX_COMMANDS_MANAGER_TYPE_ID, [] {
            return server_command_manager::instance();
        });
	// handle rx_protocol stuff!
	result = rx_internal::rx_protocol::messages::rx_message_base::init_messages();
	// register protocol constructors
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_RX_JSON_TYPE_ID, [] {
			return rx_create_reference<rx_internal::rx_protocol::rx_protocol_port>();
		});
    result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
        RX_OPCUA_TRANSPORT_PORT_TYPE_ID, [] {
            return rx_create_reference<protocols::opcua::opcua_transport_port>();
        });
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_VT00_TYPE_ID, [] {
			return rx_create_reference<rx_internal::terminal::rx_vt100::vt100_transport_port>();
		});
    result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
        RX_CONSOLE_TYPE_ID, [] {
            return rx_create_reference<rx_internal::terminal::console::console_runtime>();
        });
	return result;
}

void server_manager::deinitialize ()
{
}

rx_result server_manager::start (hosting::rx_platform_host* host, const management_data_t& data)
{
	if (telnet_port_)
	{
		//TODOIO
		//telnet_listener_ = rx_create_reference<runtime::io_types::transport_port>();
		//telnet_listener_->start_tcpip_4(rx_gate::instance().get_runtime().get_io_pool()->get_pool(), telnet_port_);
	}
    if (data.manager_internal_data)
    {
        for (auto& one : data.manager_internal_data->get_to_register())
        {
            server_command_manager::instance()->register_command(one);
        }
    }
	return true;
}

void server_manager::stop ()
{
	if (telnet_listener_)
	{
		//TODOIO
		//telnet_listener_->stop();
		telnet_listener_ = rx_port_ptr::null_ptr;
	}
}

void server_manager::get_directories (platform_directories_type& dirs)
{
}


// Class rx_platform::mngt::manager_initialization_context 

manager_initialization_context::manager_initialization_context()
{
}

manager_initialization_context::manager_initialization_context(const manager_initialization_context &right)
{
	RX_ASSERT(false);
}


manager_initialization_context::~manager_initialization_context()
{
}


manager_initialization_context & manager_initialization_context::operator=(const manager_initialization_context &right)
{
	RX_ASSERT(false);
	return *this;
}



void manager_initialization_context::register_command (server_command_base_ptr cmd)
{
	to_register_.emplace_back(cmd);
}


} // namespace mngt
} // namespace rx_platform


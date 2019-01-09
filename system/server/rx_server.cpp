

/****************************************************************************
*
*  system\server\rx_server.cpp
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


#include "pch.h"

#include "lib/rx_lib_version.h"

// rx_server
#include "system/server/rx_server.h"

#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_builders.h"
#include "system/python/py_support.h"
#include "model/rx_meta.h"


namespace rx_platform {

runtime::rx_domain_ptr rx_system_domain()
{
	return rx_gate::instance().get_manager().get_system_domain();
}
runtime::rx_application_ptr rx_system_application()
{
	return rx_gate::instance().get_manager().get_system_app();
}

runtime::rx_domain_ptr rx_unassigned_domain()
{
	return rx_gate::instance().get_manager().get_unassigned_domain();
}
runtime::rx_application_ptr rx_unassigned_application()
{
	return rx_gate::instance().get_manager().get_unassigned_app();
}

// Class rx_platform::rx_gate 

rx_gate* rx_gate::g_instance = nullptr;

rx_gate::rx_gate()
      : host_(nullptr),
        started_(rx_time::now()),
        pid_(0),
        security_guard_(std::make_unique<security::security_guard>()),
        shutting_down_(false)
{
	char buff[0x100];
	rx_collect_system_info(buff, 0x100);
	os_info_ = buff;
	pid_ = rx_pid;
	{
		ASSIGN_MODULE_VERSION(rx_version_, RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
	}
	rx_name_ = rx_get_server_name();
	lib_version_ = g_lib_version;
	hal_version_ = g_ositf_version;
	
	

	sprintf(buff, "%s %d.%d.%d",
		RX_COMPILER_NAME,
		RX_COMPILER_VERSION,
		RX_COMPILER_MINOR,
		RX_COMPILER_BUILD);
	comp_version_ = buff;

}


rx_gate::~rx_gate()
{
}



rx_gate& rx_gate::instance ()
{
	if (g_instance == nullptr)
		g_instance = new rx_gate();
	return *g_instance;
}

void rx_gate::cleanup ()
{
	RX_ASSERT(g_instance);
	delete g_instance;
	g_instance = nullptr;
}

uint32_t rx_gate::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
#ifdef PYTHON_SUPPORT
	python::py_script* python = &python::py_script::instance();
	scripts_.emplace(python->get_definition().name, python);
#endif
	
	host_ = host;

	if (runtime_.initialize(host, data.runtime_data))
	{
		if (manager_.initialize(host, data.managment_data))
		{
			if (true)//io_manager_.initialize(host, data.io_manager_data))
			{
				root_ =	sys_internal::builders::rx_platform_builder::buid_platform_system(host, data.namespace_data);

				for (auto one : scripts_)
					one.second->initialize();

				model::internal_types_manager::instance().initialize(host, data.meta_data);

				return RX_OK;
			}
			else
			{
				manager_.deinitialize();
			}
		}
		else
		{
			runtime_.deinitialize();
		}
	}
	return RX_ERROR;
}

uint32_t rx_gate::deinitialize ()
{
	
	for (auto one : scripts_)
		one.second->deinitialize();

	model::internal_types_manager::instance().deinitialize();

	manager_.deinitialize();
	runtime_.deinitialize();
	return RX_OK;
}

uint32_t rx_gate::start (hosting::rx_platform_host* host, const configuration_data_t& data)
{
	if (runtime_.start(host, data.runtime_data))
	{
		if (manager_.start(host, data.managment_data))
		{
			model::internal_types_manager::instance().start(host, data.meta_data);

			host->server_started_event();

			return RX_OK;
		}
		else
		{
			runtime_.stop();
		}
	}
	return RX_ERROR;
}

uint32_t rx_gate::stop ()
{
	model::internal_types_manager::instance().stop();
	manager_.stop();
	runtime_.stop();
	return RX_OK;
}

server_directory_ptr rx_gate::get_root_directory ()
{
	return root_;
}

bool rx_gate::shutdown (const string_type& msg)
{
	if (!security_guard_->check_premissions(security::rx_security_delete_access, security::rx_security_ext_null))
	{
		return false;
	}
	shutting_down_ = true;
	host_->shutdown(msg);
	return true;
}

void rx_gate::interface_bind ()
{
}

void rx_gate::interface_release ()
{
}

bool rx_gate::read_log (const log::log_query_type& query, log::log_events_type& result)
{
	return log::log_object::instance().read_cache(query, result);
}

bool rx_gate::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	if (!security_guard_->check_premissions(security::rx_security_execute_access, security::rx_security_ext_null))
	{
		return host_->do_host_command(line, out_buffer, err_buffer, ctx);
	}
	else
	{
		err_buffer->push_line(ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET "\r\n");
		return false;
	}
}


} // namespace rx_platform




/****************************************************************************
*
*  host\rx_gui_host.h
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


#ifndef rx_gui_host_h
#define rx_gui_host_h 1



// rx_interactive
#include "host/rx_interactive.h"
// rx_host
#include "system/hosting/rx_host.h"
// rx_thread
#include "lib/rx_thread.h"



namespace host {

namespace gui {

typedef std::function<void(jobs::job_ptr)> synchronize_callback_t;




class gui_thread_synchronizer : public rx::threads::job_thread  
{

  public:

      void append (jobs::job_ptr pjob);

      void init_callback (synchronize_callback_t callback);

      void deinit_callback ();


  protected:

  private:


      synchronize_callback_t synchronize_callback_;


};







class gui_platform_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      gui_platform_host (hosting::rx_host_storages& storage);

      ~gui_platform_host();


      void get_host_info (string_array& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      void get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items);

      void get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      bool break_host (const string_type& msg);

      int gui_initialize (int argc, char* argv[], log::log_subscriber::smart_ptr log_subscriber, synchronize_callback_t sync_callback);

      static string_type get_gui_info ();

      bool is_canceling () const;

      bool read_stdin (std::array<char,0x100>& chars, size_t& count);

      bool write_stdout (const void* data, size_t size);

      int gui_deinitialize ();

      rx_result gui_loop ();


  protected:

  private:

      rx_result set_gui_thread_security ();

      rx_result remove_gui_thread_security ();



      rx_reference<rx_platform::hosting::host_security_context> host_security_context_;

      rx_reference<interactive::interactive_security_context> user_security_context_;

      gui_thread_synchronizer thread_synchronizer_;


      bool exit_;


};


} // namespace gui
} // namespace host



#endif
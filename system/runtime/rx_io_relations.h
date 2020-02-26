

/****************************************************************************
*
*  system\runtime\rx_io_relations.h
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


#ifndef rx_io_relations_h
#define rx_io_relations_h 1



// rx_relations
#include "system/runtime/rx_relations.h"



namespace rx_platform {

namespace runtime {

namespace relations {





class port_up_relation : public relation_runtime  
{
    DECLARE_REFERENCE_PTR(port_up_relation);

  public:
      port_up_relation (rx_port_ptr my_port);


      void process_stack ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


  protected:

  private:

      rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      void relation_connected ();

      void relation_disconnected ();



      rx_port_ptr from_;

      rx_port_ptr to_;


};


} // namespace relations
} // namespace runtime
} // namespace rx_platform



#endif

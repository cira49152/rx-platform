

/****************************************************************************
*
*  sys_internal\rx_query_messages.h
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


#ifndef rx_query_messages_h
#define rx_query_messages_h 1



// rx_queries
#include "system/meta/rx_queries.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"



namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace query_messages {





class browse_request_message : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      string_type path;

      string_type filter;

      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class query_response_message : public rx_message_base  
{
	typedef std::vector<std::pair<rx_item_type, meta::meta_data> > query_result_items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      query_result_items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class query_request_message : public rx_request_message  
{
	typedef std::vector<meta::query_ptr> queries_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      queries_type queries;


      static string_type type_name;

      bool intersection;

      static rx_message_type_t type_id;


  protected:

  private:


};






class get_type_request : public rx_request_message  
{
	template<typename T>
	message_ptr do_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>);
	template<typename T>
	message_ptr do_simple_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>);

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      meta_data meta;

      rx_item_type item_type;

      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class browse_response_message : public rx_message_base  
{
	typedef std::vector<std::pair<rx_item_type, meta::meta_data> > browse_result_items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      browse_result_items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






template <class itemT>
class type_response_message : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      typename itemT::smart_ptr item;


  protected:

  private:


};






template <class itemT>
class get_type_response : public type_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};


} // namespace query_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal



#endif
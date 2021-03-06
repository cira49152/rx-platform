

/****************************************************************************
*
*  system\meta\rx_obj_types.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_obj_types_h
#define rx_obj_types_h 1


// rx_logic
#include "system/logic/rx_logic.h"
#include "system/runtime/rx_runtime_instance.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "system/runtime/rx_rt_struct.h"
using rx_platform::meta::construct_context;
using namespace rx_platform::meta::def_blocks;


namespace rx_platform {

namespace meta {
namespace meta_algorithm
{
class relation_type_algorithm;
template<class typeT>
class object_types_algorithm;
}

namespace object_types {





class relation_attribute 
{

  public:

      string_type name;

      rx_item_reference relation_type;

      rx_item_reference target;


  protected:

  private:


};


template<class T>
bool generate_json(T whose, std::ostream& def, std::ostream& err)
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	bool out = whose->serialize_definition(writer, STREAMING_TYPE_OBJECT);

    string_type result;
	if (out)
	{
		writer.write_footer();

		out = writer.get_string(result, true);
	}

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}





class object_data_type 
{
	typedef std::vector<program_runtime_ptr> programs_type;
	typedef std::vector<relation_attribute> relations_type;

    template<class typeT>
    friend class meta_algorithm::object_types_algorithm;
	//typedef std::vector<int> programs_type;

  public:
      object_data_type();

      object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      rx_result resolve (rx_directory_ptr dir);

      bool check_type (type_check_context& ctx);

      rx_result register_relation (const relation_attribute& what, complex_data_type& complex_data);


      const relations_type& get_relations () const
      {
        return relations_;
      }



      bool is_constructable;


  protected:

  private:


      programs_type programs_;

      relations_type relations_;


};






class application_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of application type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<application_type> RType;
	typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::application_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
	typedef typename runtime_data::application_runtime_data instance_data_t;
    typedef typename runtime::items::application_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<application_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;


  protected:

  private:


};






class domain_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of domain type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<domain_type> RType;
	typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::domain_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::domain_runtime_data instance_data_t;
    typedef typename runtime::items::domain_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<domain_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;


  protected:

  private:


};






class object_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_type);
	DECLARE_CODE_INFO("rx", 0, 5, 2, "\
implementation of object type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<object_type> RType;
    typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::object_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::object_runtime_data instance_data_t;
    typedef typename runtime::items::object_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<object_type> algorithm_type;

  public:

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;


  protected:

  private:


};






class port_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of port type");
public:
    static constexpr bool has_default_constructor = false;
    typedef typename runtime::algorithms::runtime_holder<port_type> RType;
    typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::port_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::port_runtime_data instance_data_t;
    typedef typename runtime::items::port_instance_data runtime_data_t;
    typedef typename runtime::items::port_behaviors runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<port_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static const rx_item_type type_id;


  protected:

  private:


};






class relation_type_data 
{

  public:

      rx::data::runtime_values_data& get_overrides () const;


      bool sealed_type;

      bool abstract_type;

      string_type inverse_name;

      bool hierarchical;

      bool symmetrical;

      bool dynamic;


  protected:

  private:


};






class relation_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(relation_type);
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
relation is now full blown object!");
public:
	typedef runtime::relation_runtime_ptr RDataType;
	typedef runtime::relations::relation_runtime RType;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;
	typedef runtime::relation_runtime_ptr RTypePtr;
    typedef meta_algorithm::relation_type_algorithm algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      meta_data meta_info;

      relation_type_data relation_data;


      static rx_item_type type_id;


  protected:

  private:


};


} // namespace object_types
} // namespace meta
} // namespace rx_platform

namespace rx_platform
{

typedef pointers::reference<meta::object_types::object_type> rx_object_type_ptr;
typedef pointers::reference<meta::object_types::domain_type> rx_domain_type_ptr;
typedef pointers::reference<meta::object_types::application_type> rx_application_type_ptr;
typedef pointers::reference<meta::object_types::port_type> rx_port_type_ptr;
typedef pointers::reference<meta::object_types::relation_type> relation_type_ptr;
}


#endif

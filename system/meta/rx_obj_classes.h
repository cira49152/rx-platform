

/****************************************************************************
*
*  system\meta\rx_obj_classes.h
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


#ifndef rx_obj_classes_h
#define rx_obj_classes_h 1



// rx_logic
#include "system/logic/rx_logic.h"
// rx_classes
#include "system/meta/rx_classes.h"



namespace rx_platform {
namespace objects
{
	class object_runtime;
	class struct_runtime;
}

namespace meta {





template <class metaT, bool _browsable = false>
class base_object_class : public base_mapped_class<metaT, _browsable>  
{
	DECLARE_REFERENCE_PTR(base_object_class);
	typedef std::vector<logic::program_runtime_ptr> programs_type;
	//typedef std::vector<int> programs_type;

  public:
      base_object_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      ~base_object_class();


      void construct (complex_runtime_ptr what);

      void construct (objects::object_runtime_ptr what);


  protected:
      base_object_class();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  private:


      programs_type programs_;


};






typedef base_object_class< object_class  > object_class_t;






class object_class : public object_class_t  
{
	DECLARE_REFERENCE_PTR(object_class);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
basic object class.\r\n\
basic implementation of object class");
public:
	typedef objects::object_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      object_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~object_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      namespace_item_attributes get_attributes () const;

      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);


      static string_type type_name;


  protected:

  private:
      object_class();



};

typedef pointers::reference<object_class> object_class_ptr;





typedef base_object_class< domain_class  > domain_class_t;






class domain_class : public domain_class_t  
{
	DECLARE_REFERENCE_PTR(domain_class);
public:
	typedef objects::domain_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      domain_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~domain_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);


      static string_type type_name;


  protected:

  private:
      domain_class();



};

typedef domain_class::smart_ptr domain_class_ptr;





typedef base_object_class< rx_platform::meta::application_class  > application_class_t;






class application_class : public application_class_t  
{
	DECLARE_REFERENCE_PTR(application_class);
public:
	typedef objects::application_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      application_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~application_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);


      static string_type type_name;


  protected:

  private:
      application_class();



};

typedef application_class::smart_ptr application_class_ptr;





typedef base_object_class< port_class  > port_class_t;






class port_class : public port_class_t  
{
	DECLARE_REFERENCE_PTR(port_class);
public:
	typedef objects::port_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      port_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~port_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);


      static string_type type_name;


  protected:

  private:
      port_class();



};


} // namespace meta
} // namespace rx_platform



#endif

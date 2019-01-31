

/****************************************************************************
*
*  testing\rx_test_meta.h
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


#ifndef rx_test_meta_h
#define rx_test_meta_h 1



// rx_test_runtime
#include "testing/rx_test_runtime.h"
// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace basic_tests {

namespace meta_test {



	

 class object_creation_test : public test_case  
 {
	 DECLARE_REFERENCE_PTR(object_creation_test)
	 DECLARE_TEST_CODE_INFO(0, 2, 0, "\
basic testing object creation (all sub-types are created).");

   public:
       object_creation_test();

       virtual ~object_creation_test();


       bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


   protected:

   private:

       rx_node_id create_variable_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id source_id, rx_node_id filter_id, rx_node_id event_id);

       rx_node_id create_struct_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id variable_id);

       rx_node_id create_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id struct_id, rx_node_id mapper_id);

       rx_node_id create_filter_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);

       rx_node_id create_event_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);

       rx_node_id create_source_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);

       rx_node_id create_mapper_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);



 };







 class meta_model_test_category : public test_category  
 {

   public:
       meta_model_test_category();

       virtual ~meta_model_test_category();


   protected:

   private:


 };






 class inheritance_creation_test : public test_case  
 {
	 DECLARE_REFERENCE_PTR(inheritance_creation_test)
	 DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing object creation (inheritance test).");

   public:
       inheritance_creation_test();

       virtual ~inheritance_creation_test();


       bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


   protected:

   private:


 };


} // namespace meta_test
} // namespace basic_tests
} // namespace testing



#endif

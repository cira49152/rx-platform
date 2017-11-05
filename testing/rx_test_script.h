

/****************************************************************************
*
*  testing\rx_test_script.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_test_script_h
#define rx_test_script_h 1


#include "lib/rx_lib.h"

// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace script_test {

namespace py_test {





class dump_version_test : public test_case  
{
	DECLARE_REFERENCE_PTR(dump_version_test)
	DECLARE_TEST_CODE_INFO( 0,1,0, "\
loading a file->reading it line by line->executing.");

  public:
      dump_version_test();

      virtual ~dump_version_test();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};







class python_test : public test_category  
{

  public:
      python_test();

      virtual ~python_test();


  protected:

  private:


};


} // namespace py_test

namespace rxs {





class read_and_run_file : public test_case  
{
	DECLARE_REFERENCE_PTR(read_and_run_file)
	DECLARE_TEST_CODE_INFO(0,1,0, "\
loading a file->reading it line by line->executing.");

  public:
      read_and_run_file();

      virtual ~read_and_run_file();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};







class rx_script_category : public test_category  
{

  public:
      rx_script_category();

      virtual ~rx_script_category();


  protected:

  private:


};


} // namespace rxs
} // namespace script_test
} // namespace testing



#endif


/****************************************************************************
*
*  system\server\rx_ns.cpp
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


#include "stdafx.h"


// rx_ns
#include "system/server/rx_ns.h"

#include "rx_server.h"


//IMPLEMENT_CODE_BEHIND_CLASS(server::ns::rx_server_directory, IEC61850 Engine, IEC61850 Ed 2 Goose Control Block - Stack Based, 1.0.0.0);


namespace server {

namespace ns {
/*
namespace_item_read_access = 1,
namespace_item_write_access = 2,
namespace_item_delete_access = 4,
namespace_item_execute = 8,
namespace_item_system = 0x10,
namespace_item_command = 0x20,
namespace_item_script = 0x40,
namespace_item_class = 0x80,
namespace_item_object = 0x100,
namespace_item_point = 0x200
*/
void fill_namepsace_string(namespace_item_attributes attr, string_type& str)
{
	str.assign(14, '-');
	str[4] = ' ';
	if (attr&namespace_item_read_access)
		str[0] = 'r';
	if (attr&namespace_item_write_access)
		str[1] = 'w';
	if (attr&namespace_item_delete_access)
		str[2] = 'd';
	if (attr&namespace_item_execute)
		str[3] = 'x';
	if (attr&namespace_item_system)
		str[5] = 's';
	if (attr&namespace_item_command)
		str[6] = 'c';
	if (attr&namespace_item_script)
		str[7] = 's';
	if (attr&namespace_item_class)
		str[8] = 'c';
	if (attr&namespace_item_object)
		str[9] = 'o';
	if (attr&namespace_item_variable)
		str[10] = 'v';
	if (attr&namespace_item_application)
		str[11] = 'a';
	if (attr&namespace_item_domain)
		str[12] = 'd';
	if (attr&namespace_item_domain)
		str[12] = 'p';
}

void fill_quality_string(values::rx_value val, string_type& str)
{
	str = "-";
	str += " - - ";
	str += "--------------- --";
	//hi,lo stuff
	str += "--";
	if (val.is_good())
		str[0] = 'g';
	else if (val.is_uncertain())
		str[0] = 'u';
	else if (val.is_bad())
		str[0] = 'b';
	if (val.is_test())
		str[2] = 't';
	if (val.is_substituted())
		str[4] = 's';
}

// Class server::ns::rx_server_item 

rx_server_item::rx_server_item()
{
}


rx_server_item::~rx_server_item()
{
}



void rx_server_item::code_info_to_string (string_type& info)
{
}

void rx_server_item::item_lock ()
{
	m_item_lock.lock();
}

void rx_server_item::item_unlock ()
{
	m_item_lock.unlock();
}

void rx_server_item::item_lock () const
{
}

void rx_server_item::item_unlock () const
{
}

server_directory_ptr rx_server_item::get_parent () const
{
	return m_parent;
}

void rx_server_item::set_parent (server_directory_ptr parent)
{
	item_lock();
	m_parent = parent;
	item_unlock();
}

string_type rx_server_item::get_path () const
{
	server_directory_ptr parent;
	item_lock();
	parent = m_parent;
	item_unlock();
	string_type full;
	if (parent)
	{
		full = parent->get_path();
		full += "/";
	}
	full += get_item_name();
	return full;
}

bool rx_server_item::serialize (base_meta_writter& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_DIRECTORY))
		return false;
	if (!stream.write_footer())
		return false;
	return true;
}

bool rx_server_item::deserialize (base_meta_reader& stream)
{
	int header_type;
	if (!stream.read_header(header_type))
		return false;
	if (!stream.read_footer())
		return false;
	return true;
}


// Class server::ns::rx_server_directory 

rx_server_directory::rx_server_directory()
      : m_created(rx_time::now())
, m_name("<unnamed>")
{
}

rx_server_directory::rx_server_directory (const string_type& name)
      : m_created(rx_time::now())
	, m_name(name)
{
}

rx_server_directory::rx_server_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items)
      : m_created(rx_time::now())
	, m_name(name)
{
	for (auto one : sub_directories)
	{
		auto it = m_sub_directories.find(one->get_name());
		if (it == m_sub_directories.end())
		{
			one->set_parent(smart_this());
			m_sub_directories.emplace(one->get_name(), one);
		}
	}
	for (auto one : items)
	{
		auto it = m_sub_items.find(one->get_item_name());
		if (it == m_sub_items.end())
		{
			one->set_parent(smart_this());
			m_sub_items.emplace(one->get_item_name(), one);
		}
	}
}


rx_server_directory::~rx_server_directory()
{
}



void rx_server_directory::get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const
{
	locks::auto_slim_lock dummy(&m_structure_lock);
	for (const auto& one : m_sub_directories)
	{
		sub_directories.emplace_back(one.second);
	}
	for (const auto& one : m_sub_items)
	{
		sub_items.emplace_back(one.second);
	}
}

void rx_server_directory::structure_lock ()
{
	m_structure_lock.lock();
}

void rx_server_directory::structure_unlock ()
{
	m_structure_lock.unlock();
}

server_directory_ptr rx_server_directory::get_parent () const
{
	server_directory_ptr ret;
	structure_lock();
	if (m_parent)
	{
		m_parent->structure_lock();
		ret = m_parent;
		m_parent->structure_unlock();
	}
	structure_unlock();
	return ret;
}

server_directory_ptr rx_server_directory::get_sub_directory (const string_type& path) const
{
	if (path.empty() || path == ".")
	{
		return smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
	}
	else if (path == "..")
	{// go one up
		server_directory_ptr ret = get_parent();
		if (!ret)
			return smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
		else
			return ret;
	}
	else if (path == "/")
	{// go one up
		return rx_server::instance().get_root_directory();
	}
	else
	{
		server_directory_ptr origin;
		string_type next;
		string_type rest;
		size_t end = path.find('/');
		size_t start = 0;
		if (path[0] == '/')// we checked on first if if path is empty!!!
		{
			end = string_type::npos;
			start = 1;
			origin = rx_server::instance().get_root_directory();
		}
		else if (path[0] == '.')
		{
			if (path.size() > 2 && path[1] == '.')
			{
				server_directory_ptr ret = get_parent();
				if (!ret)
					origin = smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
				else
					origin = ret;

				end = string_type::npos;
				start = 3;
			}
			else
			{
				end = string_type::npos;
				start = 2;
				origin = smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
			}
		}
		if (end != string_type::npos)
		{// we have sub path
			next = path.substr(start, end - start);
			rest = path.substr(end + 1);
		}
		else
		{// this is the last one
			next = path.substr(start);
		}
		if (origin)
		{
			if (next.empty())
				return origin;
			else
				return origin->get_sub_directory(next);
		}
		else
		{// local stuff

			locks::auto_slim_lock dummy(&m_structure_lock);
			auto it = m_sub_directories.find(next);
			if (it != m_sub_directories.end())
			{
				if (rest.empty())
					return it->second;
				else
					return it->second->get_sub_directory(rest);
			}
			else
				return server_directory_ptr::null_ptr;
		}
	}
}

string_type rx_server_directory::get_path () const
{
	string_type path;
	fill_path(path);
	return path;
}

string_type rx_server_directory::get_name (bool plain) const
{
	locks::auto_slim_lock dummy(&m_structure_lock);
	if (plain && m_name.empty())
		return "/";
	else
		return m_name;
}

void rx_server_directory::fill_path (string_type& path) const
{

	structure_lock();
	if (m_parent)
	{
		m_parent->fill_path(path);
	}
	if (m_name.empty())
		path = '/';
	else
	{
		if (path != "/")
			path += '/';
		path += m_name;
	}
	structure_unlock();
}

void rx_server_directory::set_parent (server_directory_ptr parent)
{
	/*if (library::cpp_classes_manager::instance().check_class(parent.get_code_behind()))
	{
		locks::auto_slim_lock dummy(&m_structure_lock);
		m_parent = parent;
	}
	else
		RX_ASSERT(false);*/
	locks::auto_slim_lock dummy(&m_structure_lock);
	m_parent = parent;
}

void rx_server_directory::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_Directory";
	has_own_code_info = true;
}

const string_type& rx_server_directory::get_type_name () const
{
  // generated from ROSE!!!
  static string_type type_name = "DIR";
  return type_name;


}

server_item_ptr rx_server_directory::get_sub_item (const string_type& path) const
{
	size_t idx = path.rfind('/');
	if (idx == string_type::npos)
	{// plain item
		locks::auto_slim_lock dummy(&m_structure_lock);
		auto it = m_sub_items.find(path);
		if (it != m_sub_items.end())
			return it->second;
		else
			return server_item_ptr::null_ptr;
	}
	else
	{// dir + item
		string_type dir_path = path.substr(0, idx);
		string_type item_name = path.substr(idx + 1);
		server_directory_ptr dir = get_sub_directory(dir_path);
		if (dir)
		{
			return dir->get_sub_item(item_name);
		}
		else
		{
			return server_item_ptr::null_ptr;
		}
	}
}

void rx_server_directory::structure_lock () const
{
	const_cast<rx_server_directory*>(this)->m_structure_lock.lock();
}

void rx_server_directory::structure_unlock () const
{
	const_cast<rx_server_directory*>(this)->m_structure_lock.unlock();
}

void rx_server_directory::get_value (rx_value& value)
{
	values::rx_value temp;
	temp.set_time(get_created());
	temp.set_quality(RX_GOOD_QUALITY);
	value = temp;
}

void rx_server_directory::fill_code_info (std::ostream& info)
{
	string_type name = get_name(true);

	fill_code_info(info, name);
}

void rx_server_directory::get_value (const string_type& name, rx_value& value)
{
}

bool rx_server_directory::add_sub_directory (server_directory_ptr who)
{
	////!!!!
	return false;
}


} // namespace ns
} // namespace server




/****************************************************************************
*
*  lib\rx_lib.cpp
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


// rx_lib
#include "lib/rx_lib.h"

#include "version/rx_version.h"

#define MS_IN_DAY (1000*60*60*24)
#define MS_START_1984 0x00000afe0bd9e400L

#define server_time_struct_DIFF_TIMEVAL  116444736000000000ull


#define DEFAULT_TIME_QUALITY (TIME_QUALITY_NOT_SYNCHRONIZED|TIME_QUALITY_CLASS_T1)
#define SYNCHRONIZED_TIME_QUALITY (TIME_QUALITY_CLASS_T1)

namespace rx
{


rx_uuid::rx_uuid()
{
	memzero(&m_uuid, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid& rigth)
{
	memcpy(&m_uuid, &rigth.m_uuid, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid_t& rigth)
{
	memcpy(&m_uuid, &rigth, sizeof(rx_uuid_t));
}
rx_uuid& rx_uuid::operator=(const rx_uuid& right)
{
	memcpy(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t));
	return *this;
}
bool rx_uuid::operator==(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) == 0);
}
bool rx_uuid::operator!=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right, sizeof(rx_uuid_t)) != 0);
}
bool rx_uuid::operator>(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t))>0);
}
bool rx_uuid::operator>=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) >= 0);

}
bool rx_uuid::operator<(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t))<0);
}

bool rx_uuid::operator<=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) <= 0);
}
// {98862E03-F664-48C5-A6AC-24761B5D479F}
rx_uuid g_null_uuid;


rx_uuid rx_uuid::create_new()
{
	rx_uuid ret;
	rx_uuid_t temp;
	rx_generate_new_uuid(&temp);
	memcpy(&ret.m_uuid, &temp, sizeof(rx_uuid_t));
	return ret;
}
const rx_uuid& rx_uuid::null_uuid()
{
	return g_null_uuid;
}
rx_uuid rx_uuid::create_from_string(const string_type& str)
{

	rx_uuid_t temp;
	if (RX_OK == rx_string_to_uuid(str.c_str(), &temp))
		return temp;
	else
		return g_null_uuid;
}
bool rx_uuid::is_null() const
{
	return memcmp(this, &g_null_uuid, sizeof(rx_uuid_t)) == 0;
}

void rx_uuid::to_string(string_type& str) const
{
	char buff[0x100];
	if (RX_OK == rx_uuid_to_string(&m_uuid, buff))
	{
		str = buff;
	}
}


const rx_node_id rx_node_id::null_id;

rx_node_id::rx_node_id()
{
	m_value.int_value = 0;
	m_node_type = numeric_rx_node_id;
}

rx_node_id::rx_node_id(const rx_node_id &right)
{
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (m_node_type)
		{
		case string_rx_node_id:
			m_value.string_value = new string_type(*right.m_value.string_value);
			break;
		case bytes_rx_node_id:
			m_value.bstring_value = new byte_string(*right.m_value.bstring_value);
			break;
		default:;
		}
	}
}

rx_node_id::rx_node_id(const dword& id, const word& namesp)
	: m_namespace(0),
	m_node_type(numeric_rx_node_id)
{
	m_value.int_value = id;
	m_namespace = namesp;
}

rx_node_id::rx_node_id(const char* id, const word& namesp)
{
	m_value.string_value = new string_type(id);
	m_node_type = string_rx_node_id;
	m_namespace = namesp;
}

rx_node_id::rx_node_id(const rx_uuid_t& id, const word& namesp)
{
	m_value.uuid_value = id;
	m_node_type = guid_rx_node_id;
	m_namespace = namesp;
}

rx_node_id::rx_node_id(rx_node_id&& right)
{
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (m_node_type)
		{
		case string_rx_node_id:
			m_value.string_value = right.m_value.string_value;
			break;
		case bytes_rx_node_id:
			m_value.bstring_value = right.m_value.bstring_value;
			break;
		default:;
		}
		// just dummy because
		right.m_node_type = numeric_rx_node_id;
	}
}


rx_node_id::~rx_node_id()
{
	clear_content();
}


rx_node_id & rx_node_id::operator=(const rx_node_id &right)
{
	clear_content();
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (m_node_type)
		{
		case string_rx_node_id:
			m_value.string_value = new string_type(*right.m_value.string_value);
			break;
		case bytes_rx_node_id:
			m_value.bstring_value = new byte_string(*right.m_value.bstring_value);
			break;
		default:;
		}
	}
	return *this;
}


bool rx_node_id::operator==(const rx_node_id &right) const
{
	if (m_namespace != right.m_namespace)
		return false;
	if (m_node_type != right.m_node_type)
		return false;
	switch (m_node_type)
	{
	case numeric_rx_node_id:
		return m_value.int_value == right.m_value.int_value;
	case guid_rx_node_id:
		return memcmp(&m_value.uuid_value, &right.m_value.uuid_value, sizeof(m_value.uuid_value)) == 0;
	case string_rx_node_id:
		return (*(m_value.string_value)) == (*(right.m_value.string_value));
	case bytes_rx_node_id:
		return (*(m_value.bstring_value)) == (*(right.m_value.bstring_value));
	default:
		RX_ASSERT(false);
		return false;
	}
}

bool rx_node_id::operator!=(const rx_node_id &right) const
{
	return !operator==(right);
}



bool rx_node_id::operator < (const rx_node_id& right) const
{


	if (m_namespace<right.m_namespace)
		return true;

	if (m_namespace>right.m_namespace)
		return false;

	if (m_node_type<right.m_node_type)
		return true;

	if (m_node_type>right.m_node_type)
		return false;

	switch (m_node_type)
	{
	case numeric_rx_node_id:
		return m_value.int_value<right.m_value.int_value;
	case guid_rx_node_id:
		return memcmp(&m_value.uuid_value, &right.m_value.uuid_value, sizeof(m_value.uuid_value))<0;
	case string_rx_node_id:
		return (*(m_value.string_value))<(*(right.m_value.string_value));
	case bytes_rx_node_id:
		return (*(m_value.bstring_value))<(*(right.m_value.bstring_value));
	default:
		RX_ASSERT(false);
		return false;
	}
}

void rx_node_id::to_string(string_type& val) const
{
	char buff[0x40];
	const char* type = "err";
	switch (m_node_type)
	{
	case numeric_rx_node_id:
		type = "i";
		break;
	case string_rx_node_id:
		type = "s";
		break;
	case guid_rx_node_id:
		type = "g";
		break;
	case bytes_rx_node_id:
		type = "b";
		break;
	}

	string_type value;

	switch (m_node_type)
	{
	case numeric_rx_node_id:
		snprintf(buff, 0x40, "%d", m_value.int_value);
		value = buff;
		break;
	case string_rx_node_id:
		value = *m_value.string_value;
		break;
	case guid_rx_node_id:
		rx_uuid(m_value.uuid_value).to_string(value);
		break;
	case bytes_rx_node_id:
		{
			for (size_t i = 0; i<m_value.bstring_value->size(); i++)
			{
				snprintf(buff, 0x40, "%02X", (int)m_value.bstring_value->at(i));
				value += buff;
			}
		}
		break;
	}
	if (m_namespace == DEFAULT_NAMESPACE)
	{
		val = type;
		val += ':';
		val += value;
	}
	else
	{
		char buffer[0x20];
		snprintf(buffer,0x20,"%d",(int)m_namespace);
		val = buffer;
		val = type;
		val += ':';
		val += value;
	}
}

rx_node_id rx_node_id::from_string(const char* value)
{

	string_type strid(value);
	rx_node_id ret;
	size_t idx = strid.find(':');
	if (idx != string_type::npos)
	{
		string_type type;
		size_t idx2 = strid.find(':', idx + 1);
		if (idx2 != string_type::npos)
		{
			ret.m_namespace = atoi(strid.substr(0, idx).c_str());
			type = strid.substr(idx + 1, idx2 - idx - 1);
		}
		else
		{
			ret.m_namespace = 0;
			type = strid.substr(0, idx);
			idx2 = idx;
		}
		if (type == "i")
		{
			ret.m_node_type = numeric_rx_node_id;
			ret.m_value.int_value = atoi(strid.substr(idx2 + 1).c_str());
		}
		else if (type == "s")
		{
			ret.m_node_type = string_rx_node_id;
			ret.m_value.string_value = new string_type(strid.substr(idx2 + 1).c_str());
		}
		else if (type == "g")
		{
			ret.m_node_type = guid_rx_node_id;
			rx_string_to_uuid(strid.substr(idx2 + 1).c_str(), &ret.m_value.uuid_value);
		}
		else if (type == "b")
		{
		}
	}
	return ret;
}

bool rx_node_id::is_null() const
{
	return m_namespace == 0 && m_node_type == numeric_rx_node_id && m_value.int_value == 0;
}

bool rx_node_id::is_standard() const
{
	return m_namespace == 1 && m_node_type == numeric_rx_node_id;
}
bool rx_node_id::is_opc() const
{
	return m_namespace == 0 && m_node_type == numeric_rx_node_id;
}

bool rx_node_id::is_simple() const
{
	return m_node_type == numeric_rx_node_id || m_node_type == guid_rx_node_id;
}

void rx_node_id::clear_content()
{
	if (!is_simple())
	{
		switch (m_node_type)
		{
		case string_rx_node_id:
			delete m_value.string_value;
			break;
		case bytes_rx_node_id:
			delete m_value.bstring_value;
			break;
		default:;
		}
	}
}

void rx_node_id::set_string_id(const char* strid)
{
	clear_content();
	m_node_type = string_rx_node_id;
	m_value.string_value = new string_type(strid);
}

bool rx_node_id::is_guid() const
{
	return m_node_type == guid_rx_node_id;
}

bool rx_node_id::get_uuid(rx_uuid_t& id) const
{
	if (m_node_type == guid_rx_node_id)
	{
		id = m_value.uuid_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_numeric(dword& id) const
{
	if (m_node_type == numeric_rx_node_id)
	{
		id = m_value.int_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_string(string_type& id) const
{
	if (m_node_type == string_rx_node_id)
	{
		if (m_value.string_value)
			id = m_value.string_value->c_str();
		return true;
	}
	else
		return false;
}

const word rx_node_id::get_namespace() const
{
	return m_namespace;
}

void rx_node_id::set_namespace(word value)
{
	m_namespace = value;
}

const rx_node_id_type rx_node_id::get_node_type() const
{
	return m_node_type;
}

void rx_node_id::set_node_type(rx_node_id_type value)
{
	clear_content();
	m_node_type = value;
}

const char* g_complie_time;
const char* g_lib_version;

namespace
{
	class dummy_starter
	{
	public:

		char compile_buffer[0x100];
		char version_buffer[0x100];
		dummy_starter()
		{
			char buff[0x100];
			create_module_compile_time_string(__DATE__, __TIME__, compile_buffer);
			g_complie_time = compile_buffer;
			sprintf(buff, "%d.%d.%d", RX_LIB_MAJOR_VERSION, RX_LIB_MINOR_VERSION, RX_LIB_BUILD_NUMBER);
			create_module_version_string(buff, __DATE__, __TIME__, version_buffer);
			g_lib_version = version_buffer;
		}
	};

	dummy_starter g_obj;
}


string_type& to_upper(string_type& str)
{
	for (auto & c : str) c = toupper(c);
	return str;
}

void split_string(const string_type& what, string_vector& result, char delimeter)
{
	size_t idx1 = 0;
	size_t idx2 = what.find(delimeter);
	while (idx2 != string_type::npos)
	{
		result.push_back(what.substr(idx1, idx2 - idx1));
		idx1 = idx2 + 1;
		idx2 = what.find(delimeter, idx1);
	}
	result.push_back(what.substr(idx1));
}


//

string_type get_code_module(const string_type& full)
{
	static size_t len = strlen(SOURCE_CODE_PATH);

	if (full.size() > len)
	{
		size_t idx = full.find(SOURCE_CODE_PATH);
		if (idx != string_type::npos)
		{
			return full.substr(len + idx);
		}
	}
	return full;
}

namespace
{
rx_time g_null_time = { 0 };
std::atomic<sqword> g_current_offset(0);
std::atomic<dword> g_current_time_quality(DEFAULT_TIME_QUALITY);
}
rx_time::rx_time()
{
	t_value = 0;
}

rx_time::rx_time(const timeval& tv)
{
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
		t_value = 0;
	else
	{
		qword temp = ((qword)tv.tv_usec) * 10 + ((qword)tv.tv_sec) * 10000000;
		temp += server_time_struct_DIFF_TIMEVAL;
		t_value = temp;
	}
}
void rx_time::to_timeval(timeval& tv) const
{
	if (is_null())
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
	else
	{
		qword temp = t_value;
		temp -= server_time_struct_DIFF_TIMEVAL;
		temp /= 10;
		tv.tv_sec = (long)(temp / 1000000UL);
		tv.tv_usec = (long)(temp % 1000000UL);
	}
}
rx_time::rx_time(const asn_generalized_time& bt)
{
	rx_full_time full;
	memzero(&full, sizeof(full));
	full.year = bt.year;
	full.month = bt.month;
	full.day = bt.day;
	full.hour = bt.hour;
	full.minute = bt.minute;
	full.second = bt.second;
	full.milliseconds = (bt.fraction / 1000);

	rx_os_collect_time(&full, this);
}
void rx_time::to_asn_generalized_time(asn_generalized_time& bt) const
{
	rx_full_time full;
	rx_os_split_time(this, &full);

	bt.year = full.year;
	bt.month = full.month;
	bt.day = full.day;
	bt.hour = full.hour;
	bt.minute = full.minute;
	bt.second = full.second;
	bt.fraction = full.milliseconds * 1000;// to us
}
rx_time::rx_time(const asn_binary_time& bt)
{
	rx_time now;
	if (bt.days != 0 && bt.mstime != 0)
	{
		if (bt.full)
		{// 6 bytes
			now = rx_time(MS_START_1984);
			now = now + bt.mstime;
			sqword tempdays = ((sqword)bt.days) * 1000 * 60 * 60 * 24;
			now = now + tempdays;
		}
		else
		{// 4 bytes
			now = rx_time::now();
			sqword temp = now.get_longlong_miliseconds() % MS_IN_DAY;
			now = now - temp;
			now = now + bt.mstime;
		}
	}
	memcpy(this, &now, sizeof(rx_time_struct));
}
void rx_time::to_asn_binary_time(asn_binary_time& bt) const
{
	if (is_null())
	{
		bt.days = 0;
		bt.full = true;
		bt.mstime = 0;
	}
	else
	{
		sqword mine = get_longlong_miliseconds();
		mine = mine - MS_START_1984;
		sqword mine_ms = mine%MS_IN_DAY;
		sqword mine_days = mine / MS_IN_DAY;
		bt.full = true;
		bt.days = (word)mine_days;
		bt.mstime = (dword)mine_ms;
	}
}
rx_time::rx_time(const rx_time_struct& ft)
{
	memcpy(this, &ft, sizeof(rx_time_struct));
}
rx_time::rx_time(const qword interval)
{
	t_value = interval*(qword)10000;
}
rx_time& rx_time::operator=(const rx_time_struct& right)
{
	memcpy(this, &right, sizeof(rx_time_struct));
	return *this;
}
rx_time& rx_time::operator=(const qword interval)
{
	t_value = interval*(qword)10000;
	return *this;
}
bool  rx_time::is_valid_time(const rx_time_struct& arg)
{
	return arg.t_value>0x014f373b00000000ul;
}
bool  rx_time::is_valid_time() const
{
	return t_value>0x014f373b00000000ul;
}
rx_time rx_time::null_time()
{
	return g_null_time;
}
rx_time rx_time::now()
{
	rx_time_struct ret;
	rx_os_get_system_time(&ret);
	sqword offset = g_current_offset;
	if (offset)
	{
		ret.t_value = ret.t_value + offset;
	}
	rx_time rxret(ret);
	return ret;
}
dword rx_time::current_time_quality()
{
	return g_current_time_quality;
}
void rx_time::set_current_time_offset(sqword offset)
{
	sqword temp = offset * 10ll;
	g_current_offset.fetch_add(temp);
}
void rx_time::set_synchronized(bool value)
{
	dword temp = value ? SYNCHRONIZED_TIME_QUALITY : DEFAULT_TIME_QUALITY;
	g_current_time_quality.store(temp);
}
rx_time rx_time::operator+(const rx_time_struct& right) const
{
	rx_time res;
	res.t_value = t_value + right.t_value;

	return res;
}
rx_time rx_time::operator+(const qword right) const
{

	rx_time res;
	res.t_value = t_value + right * 10000;

	return res;


}
rx_time rx_time::operator-(const rx_time_struct& right) const
{
	rx_time res;
	res.t_value = t_value - right.t_value;

	return res;


}
rx_time rx_time::operator-(const qword right) const
{
	rx_time res;
	res.t_value = t_value - right * 10000;

	return res;

}

bool rx_time::operator==(const rx_time_struct& right) const
{
	return t_value == right.t_value;
}
bool rx_time::operator!=(const rx_time_struct& right) const
{
	return t_value != right.t_value;
}
bool rx_time::operator>(const rx_time_struct& right) const
{
	return t_value > right.t_value;
}
bool rx_time::operator>=(const rx_time_struct& right) const
{
	return t_value >= right.t_value;
}
bool rx_time::operator<(const rx_time_struct& right) const
{
	return t_value < right.t_value;
}


bool rx_time::operator<=(const rx_time_struct& right) const
{
	return t_value <= right.t_value;
}

rx_time& rx_time::to_local()
{
	rx_os_to_local_time(this);
	return *this;
}
rx_time& rx_time::to_UTC()
{
	rx_os_to_utc_time(this);
	return *this;
}

/*
rx_time& rx_time::to_local_full()
{
FILETIME mine, local;
SYSTEMTIME sys,loc;

mine.dwLowDateTime = (DWORD)(t_value & 0xFFFFFFFF);
mine.dwHighDateTime = (DWORD)(t_value >> 32);
if(FileTimeToSystemTime(&mine,&sys))
{
SystemTimeToTzSpecificLocalTime(NULL,&sys,&loc);
SystemTimeToFileTime(&loc, &local);
t_value = (((qword)local.dwHighDateTime) << 32) + local.dwLowDateTime;
}
return *this;
}
rx_time& rx_time::to_UTC_full()
{
FILETIME mine, utc;
SYSTEMTIME sys, loc;

mine.dwLowDateTime = (DWORD)(t_value & 0xFFFFFFFF);
mine.dwHighDateTime = (DWORD)(t_value >> 32);
if(FileTimeToSystemTime(&mine,&loc))
{
TzSpecificLocalTimeToSystemTime(NULL,&loc,&sys);
SystemTimeToFileTime(&sys, &utc);
t_value = (((qword)utc.dwHighDateTime) << 32) + utc.dwLowDateTime;
}
return *this;
}
*/

dword rx_time::get_miliseconds() const
{
	return (dword)(t_value / 10000);
}

bool rx_time::is_null() const
{
	return t_value == 0;
}
sqword rx_time::get_longlong_miliseconds() const
{
	return (t_value / 10000);
}
sqword rx_time::get_useconds() const
{
	return (t_value / 10);
}

std::string rx_time::get_string() const
{
	rx_full_time full;
	char buff[0x200];

	rx_os_split_time(this, &full);

	std::string ret;
	snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
		full.year, full.month, full.day, full.hour, full.minute, full.second, full.milliseconds);
	ret = buff;

	return ret;
}

void rx_time::get_time_string(char* buff, size_t len) const
{
	qword abs = t_value / 10;

	qword usec = abs % 1000;
	abs = abs / 1000;

	qword msec = abs % 1000;
	abs = abs / 1000;

	qword sec = abs % 60;
	abs = abs / 60;

	qword min = abs % 60;
	abs = abs / 60;

	qword hour = abs % 24;

	_snprintf_s(buff, len, len, "%02d:%02d:%02d.%03d %03d",
		(int)hour, (int)min, (int)sec, (int)msec, (int)usec);

}
rx_time_struct rx_time::from_SNTP_time(dword seconds, dword fraction)
{
	qword temp = ((qword)seconds) * 10000000ull;
	temp = temp + (((qword)fraction) * 10000000ull / 0x100000000ull);


	temp = temp + 0x014f373bfde04000ull;

	rx_time_struct ret;
	ret.t_value = temp;
	return ret;
}
void rx_time::to_SNTP_time(dword& seconds, dword& fraction)
{
	sqword temp = t_value;
	temp = temp - 0x014f373bfde04000ull;

	if (temp > 0)
	{
		seconds = (dword)(temp / 10000000);
		temp = temp % 10000000;
		temp = temp * 0x100000000ull;
		temp = temp / 10000000;

		//fraction = (dword)(temp%1000000);
		fraction = (dword)temp;
	}
	else
	{
		seconds = 0;
		fraction = 0;
	}

}
rx_time_struct rx_time::from_IEC_string(const char* str)
{
	const char* ptr = str;

	rx_full_time os_time;

	os_time.hour = 0;
	os_time.minute = 0;
	os_time.second = 0;
	os_time.milliseconds = 0;

	if (sscanf(ptr, "%4u-%2u-%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 10;
	else if (sscanf(ptr, "%4u%2u%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 8;
	else
		return g_null_time;

	if (*ptr != L'\0')
	{
		if (*ptr != L'T')
			return g_null_time;
		ptr++;

		if (sscanf(ptr, "%2u:%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 5;
		else if (sscanf(ptr, "%2u%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 4;
		else
			return g_null_time;

		if (*ptr == L':')
			ptr++;

		if (*ptr != L'\0')
		{// seconds too
			if (sscanf(ptr, "%2u", &os_time.second) == 1)
				ptr += 2;
			else
				return g_null_time;
			if (*ptr == L'.')
			{
				ptr++;
				if (sscanf(ptr, "%3u", &os_time.milliseconds) == 1)
					ptr += 3;
			}
		}
	}

	rx_time_struct ts;
	if (rx_os_collect_time(&os_time, &ts))
	{
		return ts;
	}
	else
		return g_null_time;
}
string_type rx_time::get_IEC_string() const
{
	char buff[0x200];

	rx_time_struct temp = *this;
	string_type ret;

	rx_full_time os_time;
	if (rx_os_split_time(&temp, &os_time))
	{
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "%04d%02d%02dT%02d%02d%02d.%03dZ",
			os_time.year, os_time.month, os_time.day,
			os_time.hour, os_time.minute, os_time.second, os_time.milliseconds);
		ret += buff;
	}

	return ret;

}

void rx_time::set_as_span(dword days)
{

	qword temp = ((qword)days) * 10000 * 1000 * 3600 * 24;

	t_value = temp;

}
dword rx_time::get_as_span() const
{
	qword temp = t_value;

	return ((dword)(temp / ((qword)10000 * (qword)1000 * (qword)3600 * (qword)24)));

}
time_stamp time_stamp::now()
{
	time_stamp ret;
	ret.rx_time = rx_time::now();
	ret.quality = rx_time::current_time_quality();
	return ret;
}



class rx_thread_data_object
{
private:
	std::map<int, std::stack<qword>* > m_objects;
	rx_thread_data_object()
	{
	}
public:
	static rx_thread_data_object& instance();

	bool push_object(int handle, qword obj)
	{
		auto it = m_objects.find(handle);
		if (it == m_objects.end())
		{
			std::stack<qword>* temp = new std::stack<qword>;
			m_objects.emplace(handle, temp);
			temp->push(obj);
			return true;
		}
		else
		{
			it->second->push(obj);
			return true;
		}
	}
	bool pop_object(int handle)
	{
		auto it = m_objects.find(handle);
		if (it != m_objects.end())
		{
			it->second->pop();
			return true;
		}
		return false;
	}
	qword get_object(int handle)
	{
		auto it = m_objects.find(handle);
		if (it == m_objects.end() || it->second->empty())
			return 0;
		else
		{
			//pointers::reference<T> ret;
			//ret.create_from_pointer
			return it->second->top();
		}
	}
};


rx_thread_data_object& rx_thread_data_object::instance()
{
	rx_thread_data_object* ptr = (rx_thread_data_object*)rx_get_thread_data(rx_tls);
	if (ptr == nullptr)
	{
		ptr = new rx_thread_data_object();
		rx_set_thread_data(rx_tls, ptr);
	}
	return *ptr;
}

#define SECURITY_TLS_DATA 999


rx_security_handle_t rx_security_context()
{
	return rx_thread_data_object::instance().get_object(SECURITY_TLS_DATA);
}
bool rx_push_security_context(rx_security_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(SECURITY_TLS_DATA,obj);
}
bool rx_pop_security_context()
{
    return rx_thread_data_object::instance().pop_object(SECURITY_TLS_DATA);
}


#define THREADING_TLS_DATA 998


rx_thread_handle_t rx_thread_context()
{
	return rx_thread_data_object::instance().get_object(SECURITY_TLS_DATA);
}
bool rx_push_thread_context(rx_thread_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(SECURITY_TLS_DATA, obj);
}
bool rx_pop_thread_context()
{
	return rx_thread_data_object::instance().pop_object(SECURITY_TLS_DATA);
}

}//namespace rx


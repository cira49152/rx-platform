

/****************************************************************************
*
*  lib\rx_lib.h
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


#ifndef rx_lib_h
#define rx_lib_h 1




////////////////////////////////////////////////////////////////////////////////////////////
// main server library file
////////////////////////////////////////////////////////////////////////////////////////////


/*******************************************************************
 Preprocessor switches
 ======================

 #define RX_MIN_MEMORY          - remove some hash containers to gain smaller memory footprint

********************************************************************/


string_type _not_implemented_func(const char* fname);
#define RX_NOT_IMPLEMENTED _not_implemented_func(_rx_func_)
#define RX_INVALID_ARGUMENT "Invalid argument."
#define RX_INVALID_CONVERSION "Invalid conversion."
#define RX_ERROR_STOPPED "Item is not running."
#define RX_NOT_SUPPORTED "Operation not supported."
#define RX_NOT_CONNECTED "Not connected."
#define RX_ALREADY_CONNECTED "Already connected."

namespace rx
{
class base_meta_writer;
class base_meta_reader;
namespace values
{
class rx_simple_value;
class rx_value;
}

struct rx_table_cell_struct
{
	rx_table_cell_struct(const string_type& vvalue)
		: value(vvalue)
	{
	}
	rx_table_cell_struct(const string_type& vvalue, const string_type& vprefix, const string_type& vpostfix)
		: prefix(vprefix), value(vvalue), postfix(vpostfix)
	{
	}
	rx_table_cell_struct(const rx_table_cell_struct& right) = default;
	rx_table_cell_struct(rx_table_cell_struct&& right) noexcept = default;
	string_type prefix;
	string_type value;
	string_type postfix;
};

typedef std::vector<rx_table_cell_struct> rx_row_type;
typedef std::vector<rx_row_type> rx_table_type;

void rx_dump_large_row(rx_row_type row, std::ostream& out, size_t console_width);
void rx_dump_table(const rx_table_type& table, std::ostream& out, bool column_names,bool dot_lines);

//template <typename resultT>
class rx_transaction_type
{
	typedef std::function<void()> rollback_func_type;
	typedef std::stack<rollback_func_type, std::vector<rollback_func_type> > rollbacks_type;
	rollbacks_type rollback_actions_;
	bool is_commited = false;
public:
	rx_transaction_type(rollback_func_type rollback)
	{
		rollback_actions_.push(rollback);
	}
	void push(rollback_func_type rollback)
	{
		rollback_actions_.push(rollback);
	}
	rx_transaction_type() = default;
	rx_transaction_type(const rx_transaction_type& right) = delete;
	rx_transaction_type(rx_transaction_type&& right) noexcept = default;
	rx_transaction_type& operator= (const rx_transaction_type & right) = delete;
	rx_transaction_type& operator= (rx_transaction_type && right) noexcept = default;
	void commit()
	{
		is_commited = true;
	}
	void uncommit()
	{
		is_commited = false;
	}
	~rx_transaction_type()
	{
		if (!is_commited)
		{
			while(!rollback_actions_.empty())
			{
				rollback_actions_.top()();
				rollback_actions_.pop();
			}
		}
	}
};


typedef std::vector<string_type> rx_result_erros_t;

class rx_result
{
	std::unique_ptr<rx_result_erros_t> result_value_;
public:
	rx_result(bool value);
	rx_result(const string_vector& errors);
	rx_result(string_vector&& errors);
	rx_result(const char* error);
	rx_result(const string_type& error);
	rx_result(string_type&& error);
	void register_error(string_type&& error);
	void register_errors(rx_result_erros_t&& errors);
	void register_errors(const rx_result_erros_t& errors);
	operator bool() const;
	const rx_result_erros_t& errors()const;
	string_type errors_line(char delim = ',') const;
	rx_result(const rx_result& right) = delete;// because of the unique_ptr!

	rx_result() = default;
	~rx_result() = default;
	rx_result(rx_result&&) noexcept = default;
	rx_result& operator=(const rx_result&) = delete;
	rx_result& operator=(rx_result&&) noexcept = default;

	static rx_result create_from_last_os_error(const string_type& text);
	static rx_result create_from_c_error(const string_type& text);
};

typedef std::vector<rx_result> results_array;

template<class T>
void rx_dump_error_result(std::ostream& err, const T& result)
{
	for (const auto& one : result.errors())
		err << one << "\r\n";
}

template<class T>
class rx_result_with
{
	T value_;
	std::unique_ptr<rx_result_erros_t> errors_;
public:
	rx_result_with(const T& value)
		: value_(value)
	{
		if (!value_)
			errors_ = std::make_unique<rx_result_erros_t>(string_vector{ "Undefined error!"s });
	}
	rx_result_with(T&& value)
		: value_(std::move(value))
	{
		if (!value_)
			errors_ = std::make_unique<rx_result_erros_t>(string_vector{ "Undefined error!"s });
	}
	rx_result_with(const string_vector& errors)
		: errors_(std::make_unique<rx_result_erros_t>(errors))
	{
	}
	rx_result_with(string_vector&& errors)
		: errors_(std::make_unique<rx_result_erros_t>(std::move(errors)))
	{
	}
	rx_result_with(const char* error)
		: errors_(std::make_unique<rx_result_erros_t>(string_vector{ error }))
	{
	}
	rx_result_with(const string_type& error)
		: errors_(std::make_unique<rx_result_erros_t>(string_vector{ error }))
	{
	}
	rx_result_with(string_type&& error)
		: errors_(std::make_unique<rx_result_erros_t>(string_vector{ std::move(error) }))
	{
	}
	void register_error(string_type&& error)
	{
		if (!errors_)
			errors_ = std::make_unique<rx_result_erros_t>(string_vector{ std::move(error) });
		else
			errors_->emplace_back(std::move(error));
	}
	void register_errors(const rx_result_erros_t& errors)
	{
		if (!errors_)
			errors_ = std::make_unique<rx_result_erros_t>(errors);
		else
		{
			for (const auto& one : errors)
				errors_->emplace_back(one);
		}
	}
	void register_errors(rx_result_erros_t&& errors)
	{
		if (!errors_)
			errors_ = std::make_unique<rx_result_erros_t>(std::move(errors));
		else
		{
			for (auto& one : errors)
				errors_->emplace_back(std::move(one));
		}
	}
	const T& value() const
	{
		return value_;
	}
	T& value()
	{
		return value_;
	}
	T&& move_value()
	{
		return std::move(value_);
	}
	operator bool() const
	{
		return static_cast<bool>(value_);
	}
	const rx_result_erros_t& errors()const
	{
		return *errors_;
	}

	string_type errors_line(char delim = ',') const
	{
		if (*this)
			return "";
		else if (errors_->empty())
			return "No specific errors, it's just empty.!";
		std::ostringstream ss;
		for (const auto& one : *errors_)
		{
			if (!ss.eof())// using this as a bool!
				ss << delim;
			ss << one;
		}
		return ss.str();
	}
	rx_result_with(const rx_result_with& right) = delete;// because of the unique_ptr!

	rx_result_with() = default;
	~rx_result_with() = default;
	rx_result_with(rx_result_with&&) noexcept = default;
	rx_result_with& operator=(const rx_result_with&) = delete;
	rx_result_with& operator=(rx_result_with&&) noexcept = default;
};



rx_result rx_list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files, std::vector<std::string>& directories);
std::string rx_combine_paths(const std::string& path1, const std::string& path2);
std::string rx_get_extension(const std::string& path);
std::string rx_remove_extension(const std::string& path);

rx_result create_directory(const std::string& dir, bool fail_on_exsists);
rx_result rx_delete_all_files(const std::string& dir, const std::string& pattern);

class rx_source_file
{
	sys_handle_t m_handle;
public:
	rx_source_file();
	rx_result open(const char* file_name);
	rx_result open_write(const char* file_name);
	rx_result read_string(std::string& buff);
	rx_result write_string(const std::string& buff);
	~rx_source_file();

};

extern const char* g_complie_time;
extern const char* g_lib_version;


string_type& to_upper(string_type& str);

string_type get_code_module(const string_type& full);

///////////////////////////////////////////////////////////////
// PHYSICAL INTERFACES
///////////////////////////////////////////////////////////////
enum class interface_status_type
{
	status_disconnected = 0,
	status_active
};
#define MAC_ADDR_SIZE 6
struct ETH_interface
{
	uint8_t mac_address[MAC_ADDR_SIZE];
	string_type name;
	size_t index;
	interface_status_type status;
	void init()
	{
		index = 0;
		status = interface_status_type::status_disconnected;
		name = "<unnanmed>";
	}
};



struct IP_interface
{
	string_type ip_address;
	string_type name;
	size_t index;
	interface_status_type status;
	void init()
	{
		index = 0;
		status = interface_status_type::status_disconnected;
		name = "<unnanmed>";
	}
};

///////////////////////////////////////////////////////////////
// TIMES FOR ASN1 MMS IEC TIME VALUES
///////////////////////////////////////////////////////////////

struct asn_generalized_time
{
	uint16_t year;
	uint16_t month;
	uint16_t day;

	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	uint32_t fraction;
};


struct asn_binary_time
{
	bool full;
	uint32_t mstime;
	uint16_t days;
};

class rx_uuid
{
private:
	rx_uuid_t m_uuid;
public:
	rx_uuid();
	rx_uuid(const rx_uuid& rigth);
	rx_uuid(const rx_uuid_t& right);
	rx_uuid& operator=(const rx_uuid& right);

	static rx_uuid create_new();
	static rx_uuid create_from_string(const string_type& str);

	static const rx_uuid& null_uuid();

	const rx_uuid_t& uuid() const
	{
		return m_uuid;
	}

	bool operator==(const rx_uuid& right) const;
	bool operator!=(const rx_uuid& right) const;
	bool operator>(const rx_uuid& right) const;
	bool operator>=(const rx_uuid& right) const;
	bool operator<(const rx_uuid& right) const;
	bool operator<=(const rx_uuid& right) const;

	bool is_null() const;

	void to_string(string_type& buff) const;
	string_type to_string() const;

};

typedef rx_uuid rx_id;
typedef std::complex<double> complex_type;


union rx_node_id_union
{
	uint32_t int_value;
	string_type* string_value;
	byte_string* bstring_value;
	rx_uuid_t uuid_value;
};



enum class rx_node_id_type
{
	numeric = 0,
	string = 1,
	uuid = 2,
	bytes = 3
};


class rx_node_id
{
	friend struct std::hash<rx::rx_node_id>;
public:
	rx_node_id() noexcept;
	rx_node_id(const rx_node_id &right);
	rx_node_id(uint32_t id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(const char* id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(rx_uuid_t id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(const byte_string& id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(rx_node_id&& right)  noexcept;
	~rx_node_id();

	rx_node_id & operator=(const rx_node_id &right);
	rx_node_id & operator=(rx_node_id &&right) noexcept;

	bool operator==(const rx_node_id &right) const;
	bool operator!=(const rx_node_id &right) const;
	bool operator < (const rx_node_id& right) const;

	void to_string(string_type& val) const;
	string_type to_string() const;
	static rx_node_id from_string(const char* value);

	static rx_node_id generate_new(uint16_t namesp = RX_USER_NAMESPACE);
	bool is_null() const;
	operator bool() const;
	bool is_standard() const;
	bool is_opc() const;
	void set_string_id(const char* strid);
	bool is_guid() const;

	bool get_uuid(rx_uuid_t& id) const;
	bool get_numeric(uint32_t& id) const;
	bool get_string(string_type& id) const;
	bool get_bytes(byte_string& id) const;

	const rx_uuid_t& get_uuid() const;
	uint32_t get_numeric() const;
	const string_type& get_string() const;
	const byte_string& get_bytes() const;


	const uint16_t get_namespace() const;
	void set_namespace(uint16_t value);

	const rx_node_id_type get_node_type() const;
	void set_node_type(rx_node_id_type value);

	static const rx_node_id null_id;
private:
	bool is_simple() const;
	void clear_content();
	uint16_t namespace_;
	rx_node_id_type node_type_;
	rx_node_id_union value_;

};

std::ostream & operator << (std::ostream &out, const rx_node_id &val);

typedef std::vector<rx_node_id> rx_node_ids;



class rx_item_reference
{
	union
	{
		string_type path_;
		rx_node_id id_;
	};

public:

	rx_item_reference();

	rx_item_reference(const rx_item_reference& right);
	rx_item_reference(const rx_node_id& right);
	rx_item_reference(const string_type& right);
	rx_item_reference(const char* right);
	rx_item_reference(const values::rx_simple_value& right);
	~rx_item_reference();

	bool is_null() const;

	rx_item_reference& operator=(const rx_item_reference& right);
	rx_item_reference& operator = (const rx_node_id& right);
	rx_item_reference& operator = (const string_type& right);

	bool is_node_id() const;

	string_type to_string() const;

	const string_type& get_path() const;

	const rx_node_id& get_node_id() const;

	values::rx_simple_value to_value() const;

	rx_item_reference& operator = (const values::rx_simple_value& right);
	rx_item_reference(rx_item_reference&& right) noexcept;
	rx_item_reference& operator=(rx_item_reference&& right) noexcept;

	rx_item_reference(rx_node_id&& right) noexcept;
	rx_item_reference(string_type&& right) noexcept;
	rx_item_reference(values::rx_simple_value&& right) noexcept;

	rx_item_reference& operator= (rx_node_id&& right) noexcept;
	rx_item_reference& operator= (string_type&& right) noexcept;
	rx_item_reference& operator= (values::rx_simple_value&& right) noexcept;

	static const rx_item_reference null_ref;
private:
	void clear_content();
	bool is_id_;

};

///////////////////////////////////////////////////////////////
//

#define RX_MODE_MASK_TEST		0x01
#define RX_MODE_MASK_BLOCKED	0x02
#define RX_MODE_MASK_SIMULATE	0x04
#define RX_MODE_MASK_OFF		0x08
#define RX_MODE_MASK_UNASSIGNED	0x10


struct rx_mode_type
{
	rx_mode_type();
	uint32_t raw_format;
	bool can_callculate(uint32_t quality) const;
	bool can_callculate(const values::rx_value& value) const;
	bool is_on() const;
	bool is_test() const;
	bool is_blocked() const;
	bool is_simulate() const;
	bool is_unassigned() const;
	bool is_off() const;
	bool set_test();
	bool reset_test();
	bool set_simulate();
	bool ret_simulate();
	bool set_unassigned();
	bool reset_unassigned();
	bool is_good() const;
	bool set_blocked();
	bool reset_blocked();
	bool turn_on();
	bool turn_off();
};

///////////////////////////////////////////////////////////////

class rx_time : public rx_time_struct
{
public:
	rx_time() noexcept;
	rx_time(const rx_time&) = default;
	rx_time(rx_time&&) noexcept = default;
	rx_time& operator=(const rx_time&) = default;
	rx_time& operator=(rx_time&&) noexcept = default;
	rx_time(const timeval& tv);
	rx_time(const asn_binary_time& bt);
	rx_time(const asn_generalized_time& bt);
	rx_time(const rx_time_struct& ft);
	rx_time(const uint64_t interval);
	rx_time& operator=(const rx_time_struct& right) noexcept;
	rx_time& operator=(const uint64_t interval) noexcept;


	void to_asn_generalized_time(asn_generalized_time& tv) const;
	void to_asn_binary_time(asn_binary_time& tv) const;
	void to_timeval(timeval& tv) const;
	std::string get_string(bool with_date = true) const;
	void get_time_string(char* buff, size_t len) const;
	std::string get_IEC_string() const;
	static rx_time_struct from_IEC_string(const char* str);
	static rx_time_struct from_SNTP_time(uint32_t seconds, uint32_t fraction);
	void to_SNTP_time(uint32_t& seconds, uint32_t& fraction);

	static uint32_t current_time_quality();
	static void set_current_time_offset(int64_t offset);
	static void set_synchronized(bool value);
	static rx_time now();
	static rx_time null_time();
	static bool is_valid_time(const rx_time_struct& arg);
	bool is_valid_time() const;

	rx_time operator+(const rx_time_struct& right) const;
	rx_time operator+(const uint64_t right) const;
	rx_time operator-(const rx_time_struct& right) const;
	rx_time operator-(const uint64_t right) const;

	bool operator==(const rx_time_struct& right) const;
	bool operator!=(const rx_time_struct& right) const;
	bool operator>(const rx_time_struct& right) const;
	bool operator>=(const rx_time_struct& right) const;
	bool operator<(const rx_time_struct& right) const;
	bool operator<=(const rx_time_struct& right) const;

	void swap_bytes() const;

	void set_as_span(uint32_t days);
	uint32_t get_as_span() const;

	rx_time& to_local();
	rx_time& to_UTC();

	/*
	rx_time& to_local_full();
	rx_time& to_UTC_full();
	*/

	uint32_t get_miliseconds() const;
	int64_t get_longlong_miliseconds() const;
	int64_t get_useconds() const;

	bool is_null() const;
};


struct time_stamp
{
	rx_time_struct rx_time;
	uint32_t quality;
	static time_stamp now();
};

void rx_dump_ticks_to_stream(std::ostream& out, rx_timer_ticks_t ticks);


// security related basics for stuff
typedef intptr_t rx_security_handle_t;
typedef intptr_t rx_thread_handle_t;
enum class rx_criticalness
{
	soft,
	medium,
	hard
};
constexpr uint32_t rx_soft_time_offset = 100'000;
constexpr uint32_t rx_medium_time_offset = 10'000;

enum class  rx_access
{
	read	= 0x01,
	write	= 0x02,
	full	= 0xff
};


rx_thread_handle_t rx_thread_context();
bool rx_push_thread_context(rx_thread_handle_t obj);

#define RX_THREAD_NULL 0ull

void rx_split_string(const string_type& what, string_vector& result, char delimeter);
void extract_next(const string_type& path, string_type& name, string_type& rest, char delimeter);
string_type replace_in_string(const string_type& str, const string_type find, const string_type replace);
string_type& rx_trim_in_place(string_type& str);
string_type rx_trim(const string_type str);

}// namespace rx


namespace std
{
template<>
struct hash<rx::rx_node_id>
{
	size_t operator()(const rx::rx_node_id& id) const noexcept
	{
		switch (id.node_type_)
		{
		case rx::rx_node_id_type::numeric:
			return (hash<int32_t>()(id.value_.int_value)
				^ (hash<uint16_t>()(id.namespace_) << 1));
		case rx::rx_node_id_type::string:
			return (hash<string_type>()(*id.value_.string_value)
				^ (hash<uint16_t>()(id.namespace_) << 1));
		case rx::rx_node_id_type::uuid:
			return (hash<uint64_t>()(*((int64_t*)(&id.value_.uuid_value))))
				^ (hash<uint64_t>()(((int64_t*)(&id.value_.uuid_value))[1] << 1))
				^ (hash<uint16_t>()(id.namespace_) << 2);
		case rx::rx_node_id_type::bytes:
		{
			size_t ret = 0;
			size_t count = id.value_.bstring_value->size();
			for (size_t i = 0; i < count; i++)
			{
				ret ^= ((hash<uint8_t>()((*(id.value_.bstring_value))[i])) << i & 0xffff);
			}
			return ret;
		}
		default:
			RX_ASSERT(false);
			return 0;
		}
	}
};

//very useful, maybe it exists in standard already still...
struct null_deleter
{
	void operator()(void const*) const
	{
	}
};

}


#endif



/****************************************************************************
*
*  lib\rx_log.cpp
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

#define STACK_LOG_SIZE 0x400

// rx_log
#include "lib/rx_log.h"



namespace rx {

namespace log {
const char* event_type_to_string(log_event_type type)
{
	switch (type)
	{
	case info_log_event:
		return "INFO";
	case warning_log_event:
		return "WARRNING";
	case error_log_event:
		return "ERROR";
	case debug_log_event:
		return "DEBUG";
	case trace_log_event:
		return "TRACE";
	default:
		return "***UNKNOWN***";
	}
}


void log_event_data::dump_to_stream(std::ostream& stream) const
{
	char buff[STACK_LOG_SIZE];

	int ret = snprintf(buff, STACK_LOG_SIZE, "%s %s@%s %s:%s\r\n%s",
		when.get_string().c_str(),
		event_type_to_string(event_type),
		library.c_str(),
		source.c_str(),
		message.c_str(),
		code.c_str());
	if (ret < STACK_LOG_SIZE)
	{
		stream.write(buff, ret);
	}
	else
	{
		RX_ASSERT(false);
	}
}

#define LOG_SELF_INFO(msg) RX_LOG_INFO(RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, msg);

// Class rx::log::log_object 

log_object *log_object::g_object = nullptr;

log_object::log_object()
{
}

log_object::log_object(const log_object &right)
{
	RX_ASSERT(false);
}


log_object::~log_object()
{
}


log_object & log_object::operator=(const log_object &right)
{
	RX_ASSERT(false);
	return *this;
}



log_object& log_object::instance ()
{
	// this one here doesn't have double lock
	// it forces you on the begining of your process to start the log
	// :)

	if (g_object == nullptr)
		g_object = new log_object;
	return *g_object;
}

void log_object::log_event_fast (log_event_type event_type, const char* library, const string_type& source, word level, const char* code, locks::event* sync_event, const char* message)
{
	// just fire the job and let worker take care of it!
	log_event_job::smart_ptr my_job(event_type, library, source, level, code, message,sync_event);
	m_worker.append(my_job);
}

void log_object::log_event (log_event_type event_type, const char* library, const string_type& source, word level, const char* code, locks::event* sync_event, const char* message, ... )
{
	char buff[STACK_LOG_SIZE];
	va_list args;
	va_start(args, message);

	int ret=vsnprintf(buff, STACK_LOG_SIZE, message, args);
	if (ret < STACK_LOG_SIZE)
		log_event_fast(event_type, library, source, level, code, sync_event,buff);
	else
	{
		RX_ASSERT(false);
	}
	va_end(args);
}

void log_object::sync_log_event (log_event_type event_type, const char* library, const char* source, int level, const char* code, const char* message, locks::event* sync_event, rx_time when)
{
	std::vector<log_subscriber::smart_ptr> temp_array;
	temp_array.reserve(0x10);
	lock();
	for (auto one : m_subscribers)
		temp_array.emplace_back(one);
	unlock();
	for (auto one : temp_array)
		one->log_event(event_type, library, source, level, code, message,when);
	if (sync_event)
		sync_event->set();
}

void log_object::register_subscriber (log_subscriber::smart_ptr who)
{
	locks::auto_lock dummy(this);
	m_subscribers.insert(who);
}

void log_object::unregister_subscriber (log_subscriber::smart_ptr who)
{
	locks::auto_lock dummy(this);
	m_subscribers.erase(who);
}

void log_object::deinitialize ()
{
	m_worker.end();
	const char* line = "Log Stopped!";
	LOG_CODE_PREFIX
	sync_log_event(rx::log::info_log_event, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, LOG_CODE_INFO, line, nullptr, rx_time::now());
	LOG_CODE_POSTFIX

	if (m_cache)
	{
		unregister_subscriber(m_cache);
	}
	delete g_object;
	g_object = nullptr;
}

bool log_object::start (std::ostream& out, bool test, size_t log_cache_size, int priority)
{
	if (log_cache_size)
	{
		m_cache = cache_log_subscriber::smart_ptr(log_cache_size);
		register_subscriber(m_cache);
	}
	const char* line = "Log started!";
	LOG_CODE_PREFIX
	sync_log_event(rx::log::info_log_event, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, LOG_CODE_INFO, line, nullptr, rx_time::now());
	LOG_CODE_POSTFIX
	out << line << "\r\n";

	m_worker.start(priority);

	if (test)
	{
		char buffer[0x100];

		line = "Performing initial log test...";
		LOG_SELF_INFO(line);
		out << line << "\r\n";

		double spans[4];

		for (size_t i = 0; i < sizeof(spans) / sizeof(spans[0]); i++)
		{
			snprintf(buffer, sizeof(buffer), "Initial log test pass %d...", (int)i);
			rx::locks::event ev(false);
			qword first_tick = rx_get_us_ticks();
			RX_LOG_TEST(buffer, &ev);
			ev.wait_handle();
			qword second_tick = rx_get_us_ticks();
			double ms = (double)(second_tick - first_tick) / 1000.0;
			snprintf(buffer, sizeof(buffer), "Initial log test %d passed. Delay time: %g ms...", (int)i, ms);
			LOG_SELF_INFO(buffer);
			out << buffer <<"\r\n";
			spans[i] = ms;
			rx_msleep(10);
		}

		double val = 0.0;
		size_t count = sizeof(spans) / sizeof(spans[0]);
		if (count > 1)
		{
			for (size_t i = 1; i < count; i++)
			{
				val += spans[i];
			}
			val = val / (double(count - 1));
		}
		else
			val = spans[0];
		snprintf(buffer, sizeof(buffer), "Average response time: %g ms...", val);
		line = buffer;
		LOG_SELF_INFO(line);
		out << line << "\r\n";

		line = "Initial log test completed.";
		LOG_SELF_INFO(line);
		out << line << "\r\n";
	}
	return true;
}

bool log_object::read_cache (const log_query_type& query, log_events_type& result)
{
	return m_cache->read_log(query, result);
}


// Class rx::log::log_subscriber 

log_subscriber::log_subscriber()
{
}


log_subscriber::~log_subscriber()
{
}



// Class rx::log::log_event_job 

log_event_job::log_event_job (log_event_type event_type, const char* library, const string_type& source, word level, const string_type& code, const string_type& message, locks::event* sync_event, rx_time when)
      : m_sync_event(sync_event),
        m_event_type(event_type),
        m_library(library),
        m_source(source),
        m_code(code),
        m_level(level),
        m_message(message),
        m_when(when)
{
}


log_event_job::~log_event_job()
{
}



void log_event_job::process ()
{
	log_object::instance().sync_log_event(m_event_type, m_library.c_str(), m_source.c_str(), m_level, m_code.c_str(), m_message.c_str(),m_sync_event, m_when);
}


// Class rx::log::stream_log_subscriber 

stream_log_subscriber::stream_log_subscriber (std::ostream* stream)
      : m_stream(*stream)
{
}


stream_log_subscriber::~stream_log_subscriber()
{
}



void stream_log_subscriber::log_event (log_event_type event_type, const string_type& library, const string_type& source, word level, const string_type& code, const string_type& message, rx_time when)
{

	log_event_data one = { event_type,library,source,level,code,message,when };

	one.dump_to_stream(m_stream);
}


// Class rx::log::cache_log_subscriber 

cache_log_subscriber::cache_log_subscriber (size_t max_size)
      : m_max_size(max_size),
        m_current_size(0)
{
}


cache_log_subscriber::~cache_log_subscriber()
{
}



void cache_log_subscriber::log_event (log_event_type event_type, const string_type& library, const string_type& source, word level, const string_type& code, const string_type& message, rx_time when)
{
	log_event_data one = { event_type,library,source,level,code,message,when };
	locks::auto_lock dummy(&m_cache_lock);
	m_events_cache.emplace(when, one);
}

bool cache_log_subscriber::read_log (const log_query_type& query, log_events_type& result)
{
	locks::auto_lock dummy(&m_cache_lock);
	auto start_it = m_events_cache.begin();
	auto end_it = m_events_cache.end();
	if (!query.start_time.is_null())
	{// we have start time
		start_it = m_events_cache.lower_bound(query.start_time);
	}
	if (!query.stop_time.is_null())
	{// we have start time
		end_it = m_events_cache.upper_bound(query.start_time);
	}
	for (events_cache_type::const_iterator it=start_it; it!=end_it; it++)
	{
		result.emplace_back(it->second);
	}
	return true;
}


} // namespace log
} // namespace rx

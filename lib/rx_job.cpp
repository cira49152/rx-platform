

/****************************************************************************
*
*  lib\rx_job.cpp
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


#include "pch.h"


// rx_job
#include "lib/rx_job.h"



namespace rx {

rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();

namespace jobs {

// Class rx::jobs::job 

job::job()
      : canceled_(false)
	, security_context_(rx_security_context())
	, criticalness_(rx_criticalness::medium)
	, value_factor_(rx_default_value_factor)
	, complexity_(rx_default_complextiy)
	, access_type_(rx_access::full)
{
}



void job::cancel ()
{
	canceled_ = true;
}

bool job::is_canceled () const
{
	return canceled_;
}

void job::un_cancel ()
{
	canceled_ = false;
}

job* job::get_unsafe_ptr ()
{
	bind();
	return this;
}

void job::release_unsafe_ptr ()
{
	release();
}


// Class rx::jobs::timer_job 

timer_job::timer_job()
      : next_(0x0),
        period_(0x0)
  , executer_(nullptr), my_timer_(nullptr)
{
}



void timer_job::set_executer (threads::job_thread* executer)
{
	locks::auto_lock dummy(&lock_);
	executer_ = executer;
}

void timer_job::lock ()
{
}

void timer_job::unlock ()
{
}


// Class rx::jobs::post_period_job 

post_period_job::post_period_job()
{
}



rx_timer_ticks_t post_period_job::tick (rx_timer_ticks_t current_tick, bool& remove)
{
	if (((next_ - current_tick) & 0x80000000) || (next_ - current_tick) == 0)
	{

		// should be done
		executer_->append(smart_this());// add job to right thread

		remove = true;// we're done

		return 0;// return for how long
	}
	else
		return std::min(max_sleep_period, next_ - current_tick);// not jet so send how much more to timer
}


// Class rx::jobs::periodic_job 

periodic_job::periodic_job()
{
}



rx_timer_ticks_t periodic_job::tick (rx_timer_ticks_t current_tick, bool& remove)
{
	if (current_tick >= next_)
	{
		// should be done
		executer_->append(smart_this());// add job to right thread

		do
		{
			next_ = next_ + period_;// new time

		} while (next_ < current_tick);

		return next_ - current_tick;// return for how long
	}
	else
		return next_ - current_tick;// not jet so send how match more to timer
}


} // namespace jobs
} // namespace rx


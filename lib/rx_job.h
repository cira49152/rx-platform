

/****************************************************************************
*
*  lib\rx_job.h
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


#ifndef rx_job_h
#define rx_job_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_thread
#include "lib/rx_thread.h"



namespace rx {
namespace threads
{
class smart_thread_pool;

void execute_job(void* arg);

}

namespace jobs {
typedef int rx_complexity_t;
const rx_complexity_t rx_default_complextiy = 0x10000;

typedef int job_value_factor_t;
const job_value_factor_t rx_default_value_factor = 0x10000;

//	basic job class



class job : private pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(job);

  public:
      job();

      virtual ~job();


      void cancel ();

      bool is_canceled () const;

      virtual void process () = 0;


      const rx_security_handle_t get_security_context () const
      {
        return security_context_;
      }


      const rx_thread_handle_t get_destination () const
      {
        return destination_;
      }


      const rx_criticalness get_criticalness () const
      {
        return criticalness_;
      }


      const job_value_factor_t get_value_factor () const
      {
        return value_factor_;
      }


      const rx_complexity_t get_complexity () const
      {
        return complexity_;
      }


      const rx_access get_access_type () const
      {
        return access_type_;
      }



  protected:

      void un_cancel ();


  private:

      job* get_unsafe_ptr ();

      void release_unsafe_ptr ();



      bool canceled_;

      rx_security_handle_t security_context_;

      rx_thread_handle_t destination_;

      rx_criticalness criticalness_;

      job_value_factor_t value_factor_;

      rx_complexity_t complexity_;

      rx_access access_type_;

	  friend void threads::execute_job(void*);
    friend class threads::dispatcher_pool;
};






class timer_job : public job  
{
	DECLARE_REFERENCE_PTR(timer_job);

  public:
      timer_job();

      virtual ~timer_job();


      virtual rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove) = 0;

      void set_executer (threads::job_thread* executer);

      void lock ();

      void unlock ();


  protected:

      threads::job_thread *executer_;


      rx_timer_ticks_t next_;

      rx_timer_ticks_t period_;


  private:


      threads::timer *my_timer_;


      locks::lockable lock_;


    friend class threads::timer;
};






class post_period_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(post_period_job);

  public:
      post_period_job();

      virtual ~post_period_job();


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove);


  protected:

  private:


};






template <typename argT>
class lambda_job : public job  
{
	DECLARE_REFERENCE_PTR(lambda_job);

  public:
      lambda_job (std::function<void(argT)> f, argT arg)
            : f_(f),
              arg_(arg)
      {
      }

      virtual ~lambda_job()
      {
      }


      void process ()
      {
		  (f_)(arg_);
      }


  protected:

  private:


      std::function<void(argT)> f_;

      argT arg_;


};






class periodic_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(periodic_job);

  public:
      periodic_job();

      virtual ~periodic_job();


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove);


  protected:

  private:


};






template <typename argT>
class lambda_timer_job : public post_period_job  
{
	DECLARE_REFERENCE_PTR(lambda_timer_job);

  public:
      lambda_timer_job (std::function<void(argT)> f, argT arg)
            : f_(f),
              arg_(arg)
      {
      }

      virtual ~lambda_timer_job()
      {
      }


      void process ()
      {
		  cancel();
		  (f_)(arg_);
      }


  protected:

  private:


      std::function<void(argT)> f_;

      argT arg_;


};


// Parameterized Class rx::jobs::lambda_job 


} // namespace jobs
} // namespace rx



#endif

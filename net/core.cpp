#include "core.h"
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>
#include "Glog.h"
using namespace std;

namespace na
{
	namespace net
	{
		core* const core::coreMgr = new core();

		core::core(void)//:strand_(io_service_)
		{
		}


		core::~core(void)
		{
		}

		boost::asio::io_service& core::get_io(const int COREID)
		{
			return *io_pool_[COREID];
		}

		void core::init(const size_t count)
		{
			for (size_t n = 0; n < count; ++n)
			{
				io_pool_.push_back(new boost::asio::io_service());
			}
			for (size_t i = 0; i < io_pool_.size(); ++i)
			{
				work_ptr w(new boost::asio::io_service::work(*io_pool_[i]));
				work_pool_.push_back(w);
			}
		}

		void core::run()
		{			
			for (size_t i = 0; i < io_pool_.size(); ++i)
			{
				boost::shared_ptr<boost::thread> t(new boost::thread(
					boost::bind(&boost::asio::io_service::run, &(*io_pool_[i]))));
				threads.push_back(t);
			}
		}

		void core::stop()
		{
			for (size_t i = 0; i < io_pool_.size(); ++i)
			{
				(*io_pool_[i]).stop();
			}
			// Wait for all threads in the pool to exit.
			for (std::size_t i = 0; i < threads.size(); ++i)
			{
				LogS << color_pink("start join thread	") << i << LogEnd;
				threads[i]->join();
				LogS << color_pink("join thread complete	") << i << LogEnd;
			}
		}	
	}
}


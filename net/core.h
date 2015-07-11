#pragma once
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <vector>
#include <boost/thread/detail/singleton.hpp>

//#define net_core boost::detail::thread::singleton<na::net::core>::instance()
#define coreM (*na::net::core::coreMgr)

namespace na
{
	namespace net
	{
		enum
		{
			base_io,
			logic_io,
			base_count,
		};

		class core :
			private boost::noncopyable
		{
			typedef boost::shared_ptr<boost::asio::io_service>			io_service_ptr;
			typedef boost::shared_ptr<boost::asio::io_service::work>	work_ptr;
		public:
			static core* const coreMgr;
			core(void);
			~core(void);
			void							init(const size_t count);
			boost::asio::io_service& get_io(const int COREID);

			void							run();
			void							stop();
			//boost::asio::io_service::strand&	get_strand();
		private:
			std::vector<boost::asio::io_service*> io_pool_;
			std::vector<work_ptr>			work_pool_;
			//boost::asio::io_service::strand strand_;

			// Create a pool of threads to run all of the io_services.
			std::vector<boost::shared_ptr<boost::thread> > threads;
		};
	}
}




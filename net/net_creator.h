#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include "nedhelper.hpp"
#include <boost/thread/mutex.hpp>
using namespace std;

namespace na
{
	namespace net
	{
		const static int vNetOffset = 100;
		class NetCreator;
		class NetInstance;
		typedef boost::shared_ptr<NetInstance> NetInsPtr;
		class NetInstance
		{
			friend class NetCreator;
		public:
			inline int getID(){return netID;}
			int getVtID();
		private:
			NetInstance(const int nID);
			~NetInstance();
			const int netID;
			const int virtualNetID;
		};

		class NetCreator
		{
		public:
			static NetInsPtr Create2(const int netID)
			{
				void *point = ::GLMEMORY::GLNew(sizeof(NetInstance));
				return NetInsPtr(new(point) NetInstance(netID), Destory2);
			}
			NetCreator();
			~NetCreator();
			void initData(int beginID, int endID, NetCreator* hanlder = NULL);
			NetInsPtr getNewNetPrt();
			void stop();
		private:
			static void Destory2(NetInstance* point)
			{
				point->~NetInstance();
				::GLMEMORY::GLDelete(point);
			}
			static NetInsPtr Create(const int netID)
			{
				void *point = ::GLMEMORY::GLNew(sizeof(NetInstance));
				return NetInsPtr(new(point) NetInstance(netID), Destory);
			}
			static void Destory(NetInstance* point)
			{
				if(staticHanlder != NULL)staticHanlder->Recovery(point->getID());
				point->~NetInstance();
				::GLMEMORY::GLDelete(point);
			}
			static NetCreator* staticHanlder;
			bool beInit;
			void Recovery(const int netID);
			typedef vector<NetInsPtr> VECNETINS;
			VECNETINS ownVec;
			boost::mutex _mutex;
		};
	}
}
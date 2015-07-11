#include "net_creator.h"
#include "commom.h"

namespace na
{
	namespace net
	{
		NetInstance::NetInstance(const int nID) : netID(nID), virtualNetID(-1)
		{
			int vN = commom_sys.randomBetween(0, 99);
			memcpy((void*)&virtualNetID, &vN, sizeof(int));
		}
		NetInstance::~NetInstance(){}

		int NetInstance::getVtID()
		{
			int rVal = netID < 0 ? netID * vNetOffset - virtualNetID : netID * vNetOffset + virtualNetID;
			return rVal;
		}

		//////////////////////////////////////////////////////////////////////////
		NetCreator* NetCreator::staticHanlder = NULL;

		NetCreator::NetCreator() : beInit(false)
		{
			ownVec.clear();
		}

		NetCreator::~NetCreator()
		{

		}

		void NetCreator::initData(int beginID, int endID, NetCreator* hanlder /* = NULL */)
		{
			if(beInit)return;
			
			for (int i =  beginID; i <= endID; ++i)
			{
				NetInsPtr ptr = Create(i);
				if(ptr == NULL)continue;
				ownVec.push_back(ptr);
			}

			staticHanlder = hanlder;
			beInit = true;
		}

		void NetCreator::Recovery(const int netID)
		{
			NetInsPtr ptr = Create(netID);
			if(ptr == NULL)return;
			boost::mutex::scoped_lock l(_mutex);
			ownVec.insert(ownVec.begin(), ptr);
		}

		void NetCreator::stop()
		{
			staticHanlder = NULL;
		}

		NetInsPtr NetCreator::getNewNetPrt()
		{
			boost::mutex::scoped_lock l(_mutex);
			if(ownVec.empty())return NetInsPtr();
			NetInsPtr ptr = ownVec.back();
			ownVec.pop_back();
			return ptr;
		}
	}
}
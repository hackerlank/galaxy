#pragma once

#include "playerManager.h"
#include "json/json.h"
#include <string>
#include <boost/unordered_map.hpp>
#include "helper.h"
#include "time_helper.h"

#define danmu_sys (*gg::barrage::barrageSys)

namespace gg
{
	class barrage
	{
	public:
		barrage();
		~barrage(){}
		static barrage* const barrageSys;
		void DanmuReq(msg_json& m, Json::Value& r);
		void ListenerTick(msg_json& m, Json::Value& r);

		void initial();
		void update();
	private:
		class Channel
		{
		public:
			Channel(const int ID = error_channel) : _cID(ID){}
			Channel& operator=(const Channel& source)
			{
				memmove((void*)&_cID, &source._cID, sizeof(_cID));
				_listener = source._listener;
				return *this;
			}
			inline bool isVaild(){ return _cID != error_channel; }
			void addListener(const int playerID, const int netID)
			{
				_listener[playerID] = lisener(playerID, netID);
			}
			const int _cID;
			sendList toList()
			{
				sendList sL;
				for (listenerIt it = _listener.begin(); it != _listener.end();)
				{
					lisener& ls = it->second;
					listenerIt oldit = it;
					++it;
					if (ls.isDead())_listener.erase(oldit);
					else sL.push_back(sendMember(ls._pID, ls._nID));
				}
				return sL;
			}
		private:
			class lisener
			{
			public:
				lisener() :
					_pID(-1), _nID(-1), _add_life(0)
				{
				}
				lisener(const int playerID, const int netID, const unsigned life = 200) :
					_pID(playerID), _nID(netID), _add_life(life)
				{
					_dead = na::time_helper::get_current_time() + life;
				}
				bool isDead()
				{
					return na::time_helper::get_current_time() > _dead;
				}
				void keepLife()
				{
					_dead = na::time_helper::get_current_time() + _add_life;
				}
				lisener& operator = (const lisener& source)
				{
					memmove(this, &source, sizeof(lisener));
					return *this;
				}
				~lisener(){}
				const int _pID;
				const int _nID;
			private:
				const unsigned _add_life;
				unsigned _dead;
			};
			typedef boost::unordered_map<int, lisener> boostListenerMap;
			boostListenerMap _listener;
			typedef boostListenerMap::iterator listenerIt;
		};
		static Channel nullChannel;
		Channel& getChannel(const int ID);
		typedef boost::unordered_map<int, Channel> boostChannelMap;
		boostChannelMap _channel;
		typedef boostChannelMap::iterator channelIt;
	};

}
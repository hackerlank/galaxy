#include "block.h"
#include "playerManager.h"
#include "commom.h"

namespace gg
{
	short State::state = -1;
	int NumberCounter::counter = 0;
// 	bool GlobalState::GlobalInit = false;
// 	bool GlobalState::GlobalMain = false;

// 	void Block::addPackage(const short protocol, Json::Value& msg)
// 	{
// 		string str = msg.toStyledString();		
// 		msg_json::ptr p = msg_json::create(own.playerID, own.netID, protocol, str);
// 		msgQueue.push(p);
// 	}

// 	void Block::sendAllMsgJson()
// 	{
// 		while(!msgQueue.empty()){
// 			player_mgr.sendToPlayer(*msgQueue.front());
// 			msgQueue.pop();
// 		}
// 	}

// 	bool Block::begin_save()
// 	{
// 		if(!own.isVaild())return false;
// 		return save();
// 	}
}
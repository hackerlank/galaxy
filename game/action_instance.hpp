#pragma once

#include "action_def.h"
#include "helper.h"
#include "params.hpp"

namespace gg
{
	namespace ACTION
	{
		const static string strValue = "value";
		const static string strBase = "base";
		const static string strAddItemRate = "addItemRate";
		const static string addNumStr = "addNum";
		const static string addItemIDStr = "itemID";

		//param
		const static string strCurrentWarStoryID = "FromWSID";
		const static string strFromWarStoryWin = "FromWSWin";
		const static string strFromFirstWarStoryWin = "FirstWSWin";
		const static string strPilotList = "PilotList";
		const static string strPilotActiveList = "ActiveList";
		const static string strYaoSaiParamBase = "YaoSaiBase";
		const static string strYaoSaiParamWinNum = "YaoSaiWinNum";
		const static string strAddPlayerExpRate = "addPlayerExpRate";
		const static string strAddItemExtraRate = "addItemExtraRate";
		const static string strDisintegrateTimes = "disintegrationTimes";
		const static string strDisintegrateCast = "disCastTimes";
		const static string strKejiBuffer = "KejiBuffer";
		const static string strSecretaryList = "SecretaryList";
	}


	typedef actionPtr (*delegateCreate)( Json::Value& ) ;

#define initialAction(Name) \
	Name(Json::Value& dataJson) : actionBase(dataJson){}\
	virtual ~Name(){};\
	static actionPtr Create(Json::Value& dataJson)\
	{\
		void* m = GGNew(sizeof(Name));\
		actionPtr acPtr = actionPtr(new(m) Name(dataJson), actionBase::destory);\
		if(!acPtr->isValid())return actionPtr();\
		return acPtr;\
	}\

#define initialAction2(Name) \
	Name(Json::Value& dataJson);\
	virtual ~Name();\
	static actionPtr Create(Json::Value& dataJson)\
	{\
	void* m = GGNew(sizeof(Name));\
	actionPtr acPtr = actionPtr(new(m) Name(dataJson), actionBase::destory);\
	if(!acPtr->isValid())return actionPtr();\
	return acPtr;\
	}\

#define initialActionNoDeFine(Name) \
	Name(Json::Value& dataJson);\
	virtual ~Name(){};\
	static actionPtr Create(Json::Value& dataJson)\
	{\
	void* m = GGNew(sizeof(Name));\
	actionPtr acPtr = actionPtr(new(m) Name(dataJson), actionBase::destory);\
	if(!acPtr->isValid())return actionPtr();\
	return acPtr;\
	}\

#define initialEasyClass(Name, Call)\
	class Name : public actionBase\
	{\
	public:\
		initialAction(Name)\
		virtual int Check(playerDataPtr d, Json::Value& Param){\
			return 0;\
		}\
		virtual Json::Value Do(playerDataPtr d, Json::Value& Param){\
			Json::Value doJson;\
			int tmpValue = value[ACTION::strValue].asInt();\
			d->Call(tmpValue);\
			doJson.append(getID());\
			doJson.append(tmpValue);\
			return doJson;\
		}\
	};\

#define initialEasyClassWithOutDefine(Name)\
	class Name : public actionBase\
	{\
	public:\
	initialAction(Name)\
	virtual int Check(playerDataPtr d, Json::Value& Param);\
	virtual Json::Value Do(playerDataPtr d, Json::Value& Param);\
	};\



	initialEasyClass(actionAddSilver, Base.alterSilver);
	initialEasyClass(actionAddGoldTicket, Base.alterGoldTicket);
	initialEasyClass(actionAddGold, Base.alterGold);
//	initialEasyClass(actionAddKeji, Base.alterKeJi);
	initialEasyClass(actionAddWeiWang, Base.alterWeiWang);
	initialEasyClass(actionAddWork, Workshop.addPresentation);
	initialEasyClass(actionAddJunling, Campaign.alterJunling);
	initialEasyClass(actionSetVip, Vip.setVip);
	initialEasyClass(actionAddPaper, Paper.alterToken);
	
	initialEasyClassWithOutDefine(actionAddKeji);
	initialEasyClassWithOutDefine(actionAddPilotExp);
	initialEasyClassWithOutDefine(actionDealWar);
	initialEasyClassWithOutDefine(actionActivePilot);
	initialEasyClassWithOutDefine(actionAddPilot);
	initialEasyClassWithOutDefine(actionAddRoleExp);
	initialEasyClassWithOutDefine(actionAddSilverRes);
	initialEasyClassWithOutDefine(actionAddGoldRes);
	initialEasyClassWithOutDefine(actionAddKeJiRes);
	initialEasyClassWithOutDefine(actionAddWWRes);

	class actionAddKejiYaoSai : public actionBase
	{
	public:
		initialAction2(actionAddKejiYaoSai)
		virtual int Check(playerDataPtr d, Json::Value& Param);
		virtual Json::Value Do(playerDataPtr d, Json::Value& Param);
		virtual void clearStaticData();
	private:
		bool RET;
	};

	class actionAddItem : public actionBase
	{
	public:
		   initialAction(actionAddItem)
		   virtual int Check(playerDataPtr d, Json::Value& Param);
		   virtual Json::Value Do(playerDataPtr d, Json::Value& Param);
		   virtual void clearStaticData();
	};

// 	class actionAddBuff : public actionBase
// 	{
// 	public:
// 		initialAction(actionAddBuff)
// 		virtual int Check(playerDataPtr d, Json::Value& Param);
// 		virtual Json::Value Do(playerDataPtr d, Json::Value& Param);
// 	};

	class actionRateAddItem : public actionBase
	{
	public:
		initialActionNoDeFine(actionRateAddItem)
		virtual int Check(playerDataPtr d, Json::Value& Param);
		virtual Json::Value Do(playerDataPtr d, Json::Value& Param);
		virtual void clearStaticData();
	private:
		bool ADD;
	};

	class actionAddSecretary : public actionBase
	{
		public:
			initialActionNoDeFine(actionAddSecretary)
			virtual int Check(playerDataPtr d, Json::Value& Param);
			virtual Json::Value Do(playerDataPtr d, Json::Value& Param);
			virtual void clearStaticData();
	};



}
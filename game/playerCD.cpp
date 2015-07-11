#include "playerCD.h"
#include "time_helper.h"

namespace gg
{
	const playerCD playerCD::NullValue = playerCD(-1000);

	playerCD::playerCD(const int id /* = -1  */, const unsigned st /* = 0 */, const unsigned cd /* = 0 */, const bool l /* = false) : CDID(id */) : CDID(id), setLockTime(st){
		Cdd.CD = cd;
		Cdd.Lock = l;
	}

	void playerCD::refresh()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now > Cdd.CD && Cdd.Lock)
			Cdd.Lock = false;
	}

	CDData& playerCD::getCD()
	{
		refresh();
		return Cdd;
	}

	void playerCD::addCD(unsigned s)
	{
		refresh();
		unsigned now = na::time_helper::get_current_time();
		if(now > Cdd.CD)	Cdd.CD = now + s;
		else	Cdd.CD += s;
		if(Cdd.CD -  now > setLockTime)
			Cdd.Lock = true;
	}

	void playerCD::reduceCD(unsigned s)
	{
		unsigned now = na::time_helper::get_current_time();
		Cdd.CD -= s;
		refresh();
	}

	void playerCD::clearCD()
	{
		Cdd.CD = 0;
		Cdd.Lock = false;
	}

	playerCDMulti::playerCDMulti()
	{

	}

	playerCDMulti::~playerCDMulti()
	{

	}

	playerCD& playerCDMulti::getPCD(const int ID)
	{
		static playerCD nullCD = playerCD::NullValue;
		CDMap::iterator it = ownMap.find(ID);
		if(it == ownMap.end())return nullCD = playerCD::NullValue;
		return it->second;
	}
}
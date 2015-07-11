#pragma once

#include <iostream>
#include <string.h>
#include <map>
using namespace std;

namespace gg
{
	struct CDData
	{
		CDData()
		{
			CD = 0;
			Lock = false;
		}
		unsigned CD;
		bool Lock;
	};

	class playerCD
	{
	public:
		playerCD(const int id = -1 , const unsigned st = 0, const unsigned cd = 0, const bool l = false);
		bool operator ==(const playerCD& pCD) const{
			return CDID == pCD.CDID && setLockTime == pCD.setLockTime; 
		}
		virtual playerCD& operator =(const playerCD& pCD){
			memmove(this, &pCD, sizeof(playerCD));
			return *this;
		}
		virtual void reduceCD(unsigned s);
		virtual void addCD(unsigned s);
		virtual void clearCD();
		virtual CDData& getCD();
		inline bool vaildPCD(){return CDID != -1000;}
		const int CDID;
		const unsigned setLockTime;
		const static playerCD NullValue;
		CDData Cdd;	
	private:	
		void refresh();
	};

	class playerCDMulti
	{
	public:
		playerCDMulti();
		~playerCDMulti();
		playerCD& getPCD(const int ID);
	private:
		typedef map<int, playerCD> CDMap;
		CDMap ownMap;
	};
}
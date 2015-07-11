#pragma once

#include "block.h"
#include <boost/unordered_map.hpp>

namespace gg
{
	const static string marketDBStr = "gl.player_market";
	const static string market_next_update_time_field_str = "nut";
	const static string market_key_id_field_str = "kid";
	const static string market_id_to_num_field_str = "iton";

	class playerMarket : public Block
	{
		public:
			playerMarket(playerData& own) : Block(own), _next_update_time(0), _key_id(-1){}
			virtual void autoUpdate(){}
			virtual bool get();
			virtual bool save();

			void checkAndUpdate(unsigned now);
			int getNum(int key_id, int id);
			unsigned getCd(int key_id, int id);
			void alterNum(int id, int num = 1);

		private:
			struct numCd
			{
				public:
					numCd(int num, unsigned cd)
						: _num(num), _cd(cd){}

					unsigned _cd;
					int _num;
			};

			typedef boost::unordered_map<int, numCd> Id2Num;
			int _key_id;
			unsigned _next_update_time;
			Id2Num _id2num;
	};
}
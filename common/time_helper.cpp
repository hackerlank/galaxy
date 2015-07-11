#include "time_helper.h"

namespace na
{
	namespace time_helper
	{
		unsigned Deviation::_Dev = 0;

		unsigned getDeviation()
		{
			return Deviation::_Dev;
		}

		void setDeviation(const unsigned val)
		{
			Deviation::_Dev += val;
		}
	}
}
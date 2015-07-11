#include "action_format.hpp"

namespace gg
{
	actionFormat::actionFormatMap actionFormat::FormatMap;
	Json::Value actionFormat::LastDoJson = Json::arrayValue;
}
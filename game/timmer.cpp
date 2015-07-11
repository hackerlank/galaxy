#include "timmer.hpp"

Timer::HandlerManager Timer::HandlerMgr = Timer::HandlerManager();
bool Timer::_isSort = false;
Timer::CallVec Timer::_ownVec;
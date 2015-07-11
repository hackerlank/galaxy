sSMap::iterator sSMapManager::END()
{
	return ownMap.end();
}

void sSMapManager::insert(string& str)
{
	ownMap.insert(str);
}

bool sSMapManager::match(char* c, size_t sz)
{
	string str(c, sz);
	return ownMap.find(str) != END();
}

bool sSMapManager::filt(char* c, size_t sz)
{
	if(!match(c, sz))return false;
	memset(c, '*', sz);
	return true;
}

//////////////////////////////////////////////////////////////////////////

cbMap::iterator cbMapManager::END()
{
	return ownMap.end();
}

void cbMapManager::insert(string& str)
{
	char c = *str.begin();
	sSMMPtr ptr = getCarry(c);
	if(ptr == NULL)
	{
		ptr = sSMapManager::Create();
		ownMap[c] = ptr;
	}
	ptr->insert(str);
}

sSMMPtr cbMapManager::getCarry(char c)
{
	cbMap::iterator  it = ownMap.find(c);
	if(it == END())return sSMMPtr();
	return it->second;
}

bool cbMapManager::filt(char* c, size_t sz)
{
	sSMMPtr ptr = getCarry(*c);
	if(ptr == NULL)return false;
	return ptr->filt(c, sz);
}

//////////////////////////////////////////////////////////////////////////

McbMap::iterator McbMapManager::END()
{
	return ownMap.end();
}

cbMMPtr McbMapManager::getCarry(size_t sz)
{
	McbMap::iterator  it = ownMap.find(sz);
	if(it == END())return cbMMPtr();
	return it->second;
}

void McbMapManager::insert(string& str)
{
	size_t sz = str.length();
	cbMMPtr ptr = getCarry(sz);
	if(ptr == NULL)
	{
		ptr = cbMapManager::Create();
		ownMap[sz] = ptr;
	}
	ptr->insert(str);
}

bool McbMapManager::filt(char* c, size_t sz)
{
	cbMMPtr ptr = getCarry(sz);
	if(ptr == NULL)return false;
	return ptr->filt(c, sz);
}

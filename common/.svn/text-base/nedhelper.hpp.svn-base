#pragma once

#include "nedmalloc/nedmalloc.h"

class GLMEMORY
{
public:
	inline static void* GLNew(size_t n)
	{
		return nedalloc::nedmalloc(n);
	}

	inline static void GLDelete(void* point)
	{
		nedalloc::nedfree(point);
	}
};
#ifndef __SPLITTER
#define __SPLITTER

#include "AABB.h"
#include "PrimitiveArray.h"

class Splitter
{
public:
	virtual void split(PrimitiveArray const & input, PrimitiveArray & left, PrimitiveArray & right) const = 0;
};

#endif


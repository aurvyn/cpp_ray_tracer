#ifndef __SPATIALMEDIAN
#define __SPATIALMEDIAN

#include "Splitter.h"
#include <assert.h>

class SpatialMedian : public Splitter
{
public:
	virtual void split(PrimitiveArray const & input, PrimitiveArray & left, PrimitiveArray & right) const
	{
		Vector3 dim = input.getDim();
		int maxDim = dim.maxComponent();
		Vector3 splitCenter = input.getCenter();
		float splitPos = splitCenter[maxDim];
		
		assert( left.size() == 0);
		assert( right.size() == 0);
		
		for(int i=0; i<input.size(); i++)
		{
			Vector3 center = input[i]->getCenter();
			bool lessThan = center[maxDim] < splitPos;
			if(lessThan)
				left.add(input[i]);
			else
				right.add(input[i]);
		}
		
		bool emptyChild = left.size() == 0 || right.size() == 0;
		if(emptyChild)
		{
			left = PrimitiveArray();
			right = PrimitiveArray();
			for(int i=0; i<input.size()/2; i++)
				left.add(input[i]);
			for(int i=input.size()/2; i<input.size(); i++)
				right.add(input[i]);
		}

	}
};

#endif


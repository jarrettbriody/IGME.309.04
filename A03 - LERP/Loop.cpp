#include "Loop.h"



Loop::Loop()
{
}


Loop::~Loop()
{
	if(vertices != nullptr)
		delete[] vertices;
}

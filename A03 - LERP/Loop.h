#pragma once

#include "Simplex\Simplex.h"
using namespace Simplex;
struct Loop
{
	vector3* vertices = nullptr;
	int vertCount = 3;
	int currentIndex = 0;
	int nextIndex = 1;
	float timeSinceLast = 0.0f;
	const float timeBetween = 0.5f;
	Loop();
	~Loop();
};


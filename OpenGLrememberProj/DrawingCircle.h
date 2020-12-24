#pragma once
#include <corecrt_math.h>
void FindingPointCircle(double r, double t, double Point[3])
{

	Point[0] = cos(t) * r;
	Point[1] = sin(t) * r;
	Point[2] = 0;
}

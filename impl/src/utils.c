
#include <math.h>

float square(float x)
{
	return x * x;
}

float dist(float xa, float ya, float xb, float yb)
{
	return sqrtf((xa-xb)*(xa-xb) + (ya-yb)*(ya-yb));
}

unsigned int umax(unsigned int a, unsigned int b)
{
	return a > b ? a : b;
}

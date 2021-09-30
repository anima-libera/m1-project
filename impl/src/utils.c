
#include <math.h>

float dist(float xa, float ya, float xb, float yb)
{
	return sqrtf((xa-xb)*(xa-xb) + (ya-yb)*(ya-yb));
}

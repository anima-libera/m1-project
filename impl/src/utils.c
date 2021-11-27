
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float square(float x)
{
	return x * x;
}

float dist(float xa, float ya, float xb, float yb)
{
	return sqrtf((xa-xb)*(xa-xb) + (ya-yb)*(ya-yb));
}

float clamp(float x, float inf, float sup)
{
	if (x < inf)
	{
		return inf;
	}
	else if (x > sup)
	{
		return sup;
	}
	else
	{
		return x;
	}
}

unsigned int umax(unsigned int a, unsigned int b)
{
	return a > b ? a : b;
}

#if 0
char* filepath_join(const char* left, const char* right)
{
	int file_path_len = 99;
	char* file_path = malloc(file_path_len);
	int file_path_required_len;
	if ((file_path_required_len = snprintf(file_path, file_path_len, "%s/%s",
		left, right) + 1) > file_path_len)
	{
		file_path_len = file_path_required_len;
		file_path = realloc(file_path, file_path_len);
		sprintf(file_path, "%s/%s", left, right);
	}
	return file_path;
}
#endif

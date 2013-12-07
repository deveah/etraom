
#include <math.h>
#include <sys/select.h>

#include "etraom.h"

int distance( int x1, int y1, int x2, int y2 )
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

void msleep( int ms )
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;

	select( 0, NULL, NULL, NULL, &tv );
}


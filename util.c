
#include <math.h>

#ifndef _WIN32
#include <sys/select.h>
#else
#include <windows.h>
#endif

#include "etraom.h"

int distance( int x1, int y1, int x2, int y2 )
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

void msleep( int ms )
{
#ifndef _WIN32
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;

	select( 0, NULL, NULL, NULL, &tv );
#else
	Sleep( ms );
#endif
}


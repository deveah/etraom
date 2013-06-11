
#include <math.h>

#include "etraom.h"

int distance( int x1, int y1, int x2, int y2 )
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}


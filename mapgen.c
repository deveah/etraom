
#include <stdlib.h>

#include "etraom.h"

int make_dummy_map( map_t *m, int nwalls )
{
	int i;
	int x, y;

	if( !m )
	{
		log_add( "make_dummy_map called with NULL map.\n" );
		return 0;
	}

	log_add( "Generating dummy map on 0x%08x...\n", m );

	for( x = 0; x < m->width; x++ )
	{
		for( y = 0; y < m->height; y++ )
		{
			m->terrain[x][y] = &tile_floor;
		}
	}

	for( i = 0; i < nwalls; i++ )
	{
		x = rand() % m->width;
		y = rand() % m->height;

		m->terrain[x][y] = &tile_wall;
	}

	log_add( "Done generating dummy map.\n" );

	return 1;
}

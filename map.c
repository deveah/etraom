
#include <stdlib.h>

#include "etraom.h"

map_t *alloc_map( char *name, int width, int height )
{
	int i, j;
	map_t *m = (map_t*) malloc( sizeof(map_t) );

	m->width = width;
	m->height = height;

	m->terrain = (tile_t***) malloc( sizeof(tile_t*) * width );

	for( i = 0; i < width; i++ )
	{
		m->terrain[i] = (tile_t**) malloc( sizeof(tile_t*) * height );
		for( j = 0; j < height; j++ )
		{
			m->terrain[i][j] = NULL;
		}
	}

	clear_map( m );
	m->name = name;

	log_add( "Allocated map (0x%08x) of width %i and height %i.\n",
		m, width, height );

	return m;
}

void free_map( map_t *m )
{
	int i;

	if( m )
	{
		if( m->terrain )
		{
			for( i = 0; i < m->width; i++ )
			{
				if( m->terrain[i] )
					free( m->terrain[i] );
			}

			free( m->terrain );
		}

		free( m );
		log_add( "Freed map 0x%08x.\n", m );
	}
}

void clear_map( map_t *m )
{
	m->name = "";
}

void debug_dump_map( map_t *m )
{
	int i, j;

	log_add( "Dumping map 0x%08x - width: %i, height: %i\n", m, m->width,
		m->height );

	for( j = 0; j < m->height; j++ )
	{
		for( i = 0; i < m->width; i++ )
		{
			if( m->terrain[i][j] )
				log_add( "%c", m->terrain[i][j]->face );
			else
				log_add( "?" );
		}
		log_add( "\n" );
	}
}

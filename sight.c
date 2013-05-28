
#include <math.h>

#include "etraom.h"

/*	this file features code from Eligloscode (source: Roguebasin)
	cheers to its author!
*/

/*	TODO fix artifacts where light is not continuous along walls */

void clear_lightmap( entity_t *e, int n )
{
	int i, j;

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			if(	( e == player ) &&
				( e->lightmap[n][i][j] > 0.0f ) )
				dungeon_memory[n][i][j] = dungeon[n]->terrain[i][j]->face;
			e->lightmap[n][i][j] = 0.0f;
		}
	}
}

void do_fov( entity_t *e, int radius )
{
	float x, y;
	int i;
	
	clear_lightmap( e, e->z );
	for( i = 0; i < 360; i++ )
	{
		x = cos( (float)i * 0.01745f );
		y = sin( (float)i * 0.01745f );
		cast_ray( e, x, y, radius );
	}
}

void cast_ray( entity_t *e, float x, float y, int radius )
{
	int i;
	float ox,oy;
	
	ox = (float)e->x + 0.5f;
	oy = (float)e->y + 0.5f;
	
	for( i = 0; i < radius; i++ )
	{
		e->lightmap[e->z][(int)ox][(int)oy] = 1.0f / (float)(radius+1);
		if( dungeon[e->z]->terrain[(int)ox][(int)oy]->flags & TILEFLAG_OPAQUE )
			return;
		
		ox += x;
		oy += y;

		if( !is_legal( ox, oy ) )
			return;
	}
}


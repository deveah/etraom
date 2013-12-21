
#include <math.h>

#include "etraom.h"

void reveal_map( int n )
{
	int i, j;

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			dungeon[n]->memory[i][j] = dungeon[n]->terrain[i][j];
		}
	}
}

void clear_lightmap( entity_t *e, int n )
{
	int i, j;

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			if(	( e == player ) &&
				( e->lightmap[n][i][j] > 0.0f ) )
				dungeon[n]->memory[i][j] = dungeon[n]->terrain[i][j];

			e->lightmap[n][i][j] = 0.0f;
		}
	}
}

void clone_lightmap( entity_t *dest, entity_t *src )
{
	int i, j, k;

	for( i = 0; i < MAX_LEVELS; i++ )
	{
		for( j = 0; j < MAP_WIDTH; j++ )
		{
			for( k = 0; k < MAP_HEIGHT; k++ )
			{
				dest->lightmap[i][j][k] = src->lightmap[i][j][k];
			}
		}
	}
}

#if 0
int do_ray( int z, int x1, int y1, int x2, int y2 )
{
	int cx = x1, cy = y1;
	int dx, dy, sx, sy;
	float err, e2;

	dx = abs( x2-x1 );
	dy = abs( y2-y1 );

	if( x1 < x2 ) sx = 1; else sx = -1;
	if( y1 < y2 ) sy = 1; else sy = -1;
	err = dx-dy;

	while( 1 )
	{
		if( ( cx == x2 ) && ( cy == y2 ) )
			return 1;

		if(	( is_legal( cx, cy ) ) &&
			( dungeon[z]->terrain[cx][cy]->flags & TILEFLAG_OPAQUE ) )
			return 0;

		e2 = 2 * err;

		if( e2 > -dy )
		{
			err -= dy;
			cx += sx;
		}
		
		if( e2 < dx )
		{
			err += dx;
			cy += sy;
		}

	}

	return 1;
}

void do_fov( entity_t *e, int radius )
{
	int i, j;

	clear_lightmap( e, e->z );

	for( i = e->x-radius; i <= e->x+radius; i++ )
		for( j = e->y-radius; j <= e->y+radius; j++ )
		{
			if( is_legal( i, j ) &&
				( distance( e->x, e->y, i, j ) <= radius ) &&
				do_ray( e->z, e->x, e->y, i, j ) )
				e->lightmap[e->z][i][j] = 1.0f;
		}
}
#endif

/*	code partially taken from Eligloscode:
	http://roguebasin.roguelikedevelopment.org/index.php?title=Eligloscode */
/* TODO: precompute sin/cos tables */
/* this is an attempt at making a permissive FOV algorithm */
void do_fov( entity_t *e, int radius )
{
	int i, j;
	float ox, oy;
	float x, y;
	
	int a = 45;

	clear_lightmap( e, e->z );

	for( i = 0; i < 360; i++ )
	{
		x = cos( (float)i * 0.01745f );
		y = sin( (float)i * 0.01745f );

		ox = (float)( e->x ) + 0.5f;
		oy = (float)( e->y ) + 0.5f;

		for( j = 0; j < radius; j++ )
		{
			if( is_legal( (int)ox, (int)oy ) )
			{
				e->lightmap[e->z][(int)ox][(int)oy] = 1.0f;

				if( dungeon[e->z]->terrain[(int)ox][(int)oy]->flags & TILEFLAG_OPAQUE )
				{
					if( ( i >= 0 ) && ( i < a ) && 
						is_legal( (int)ox + 1, (int)oy + 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox+1][(int)oy+1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= (90-a) ) && ( i < 90 ) && 
						is_legal( (int)ox - 1, (int)oy - 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox-1][(int)oy-1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= 90 ) && ( i < (90+a) ) && 
						is_legal( (int)ox + 1, (int)oy - 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox+1][(int)oy-1]->flags & TILEFLAG_OPAQUE ) )
						continue;
						
					else if( ( i >= (180-a) ) && ( i < 180 ) && 
						is_legal( (int)ox - 1, (int)oy + 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox-1][(int)oy+1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= 180 ) && ( i < (180+a) ) && 
						is_legal( (int)ox - 1, (int)oy - 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox-1][(int)oy-1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= (270-a) ) && ( i < 270 ) && 
						is_legal( (int)ox + 1, (int)oy + 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox+1][(int)oy+1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= 270 ) && ( i < (270+a) ) && 
						is_legal( (int)ox - 1, (int)oy + 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox-1][(int)oy+1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else if( ( i >= (360-a) ) && ( i < 360 ) && 
						is_legal( (int)ox + 1, (int)oy - 1 ) &&
						!( dungeon[e->z]->terrain[(int)ox+1][(int)oy-1]->flags & TILEFLAG_OPAQUE ) )
						continue;

					else
						break;
				}
			}
			else
			{
				break;
			}

			ox += x;
			oy += y;
		}
	}
}


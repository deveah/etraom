
#include <math.h>
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

void dig_room( map_t *m, int x, int y, int w, int h )
{
	int i, j;

	for( i = x; i < x+w; i++ )
	{
		for( j = y; j < y+h; j++ )
		{
			if( is_legal( i, j ) )
				m->terrain[i][j] = &tile_floor;
		}
	}
}

void dig_cooridor( map_t *m, int x1, int y1, int x2, int y2 )
{
	int cx = x1,
		cy = y1,
		xo = ( x2 > x1 ) ? 1 : -1,
		yo = ( y2 > y1 ) ? 1 : -1;
	
	while( cx != x2 )
	{
		if(	( is_legal( cx, cy ) ) &&
			( m->terrain[cx][cy] == &tile_wall ) )
				m->terrain[cx][cy] = &tile_floor;
		
		cx += xo;
	}

	while( cy != y2 )
	{
		if(	( is_legal( cx, cy ) ) &&
			( m->terrain[cx][cy] == &tile_wall ) )
			m->terrain[cx][cy] = &tile_floor;

		cy += yo;
	}
}

int closest_room( room_t **r, int nrooms, int n, float loop_chance )
{
	int bd = 999, bi = -1;
	int z = 0;

	int i;

	for( i = 0; i < nrooms; i++ )
	{
		z = distance( r[i]->x, r[i]->y, r[n]->x, r[n]->y );

		if(	( i != n ) &&
			( z < bd ) &&
			!( r[i]->linked || ( (float)(rand()%100)/100.0f < loop_chance ) ) )
		{
			bd = z;
			bi = i;
		}
	}

	return bi;
}

int all_rooms_linked( room_t **r, int nrooms )
{
	int i;

	for( i = 0; i < nrooms; i++ )
	{
		if( r[i]->linked == 0 )
			return 0;
	}
	
	return 1;
}

int make_grid_map(	map_t *m, int cell_width, int cell_height,
					float room_chance, float node_chance,
					float door_chance, float loop_chance )
{
	int rx, ry, rw, rh;
	int room_count = 0;
	int max_rooms = ( MAP_WIDTH/cell_width ) * ( MAP_HEIGHT/cell_height );

	int i, j;

	room_t **rooms = (room_t**) malloc( sizeof(room_t*) * max_rooms );
	for( i = 0; i < max_rooms; i++ )
	{
		rooms[i] = (room_t*) malloc( sizeof(room_t) );
	}

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			m->terrain[i][j] = &tile_wall;
		}
	}

	for( i = 0; i < ( MAP_WIDTH / cell_width ); i++ )
	{
		for( j = 0; j < ( MAP_HEIGHT / cell_height ); j++ )
		{
			if( (float)(rand()%100)/100.0f < node_chance )
			{
				while( 1 )
				{
					rx = rand() % (cell_width-1) + 2;
					ry = rand() % (cell_height-1) + 2;
					rw = rand() % (cell_width-1) + 2;
					rh = rand() % (cell_height-1) + 2;

					if(	( rx+rw < cell_width ) &&
						( ry+rh < cell_height ) && 
						( rw > cell_width/3 ) &&
						( rh > cell_height/3 ) )
						break;
				}
				
				if( (float)(rand()%100)/100.0f < room_chance )
					dig_room( m, i*cell_width+rx, j*cell_height+ry, rw, rh );
					
				rooms[room_count]->x = i*cell_width+rx + rw/2;
				rooms[room_count]->y = j*cell_height+ry + rh/2;
				if( rooms[room_count]->x % 2 == 0 )
					rooms[room_count]->x++;
				if( rooms[room_count]->y % 2 == 0 )
					rooms[room_count]->y++;
				rooms[room_count]->linked = 0;
				room_count++;
			}
		}
	}

	int ra = rand() % room_count,
		rb = 0;
	
	rooms[ra]->linked = 1;

	while( all_rooms_linked( rooms, room_count ) == 0 )
	{
		rb = closest_room( rooms, room_count, ra, loop_chance );
		if( rb != -1 )
		{
			dig_cooridor( m, rooms[ra]->x, rooms[ra]->y, rooms[rb]->x, rooms[rb]->y );
			if( rooms[ra]->linked == 1 )
				rooms[rb]->linked = 1;
			ra = rb;
		}
	}

	for( i = 0; i < max_rooms; i++ )
		free( rooms[i] );
	free( rooms );

	return room_count;
}

/* TODO: door placement, opening/closing */

void make_drunken_walk_cave( map_t *m, int n )
{
	int i, j;
	int cx, cy;
	int d, dx, dy;

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			m->terrain[i][j] = &tile_wall;
		}
	}

	cx = rand()%(MAP_WIDTH-10) + 5;
	cy = rand()%(MAP_HEIGHT-10) + 5;

	m->terrain[cx][cy] = &tile_floor;

	i = 0;
	while( i < n )
	{
		d = rand() % 4;
		switch( d )
		{
		case 0: dx =  0; dy =  1; break;
		case 1: dx =  0; dy = -1; break;
		case 2: dx =  1; dy =  0; break;
		case 3: dx = -1; dy =  0; break;
		default: dx = 0; dy =  0; break;
		}

		if( ( cx+dx > 0 ) &&
			( cy+dy > 0 ) &&
			( cx+dx < MAP_WIDTH-1 ) &&
			( cy+dy < MAP_HEIGHT-1 ) )
		{
			cx += dx;
			cy += dy;
			m->terrain[cx][cy] = &tile_floor;
			i++;
		}
		else
		{
			int r;
			do
			{
				cx = rand()%(MAP_WIDTH-10) + 5;
				cy = rand()%(MAP_HEIGHT-10) + 5;
				r = count_neighbours( m, cx, cy, &tile_floor );
			} while( ( r == 0 ) || ( r > 3 ) );
		}
	}
}

int count_neighbours( map_t *m, int x, int y, tile_t *w )
{
	int i, j;
	int r = 0;

	for( i = x-1; i <= x+1; i++ )
	{
		for( j = y-1; j <= y+1; j++ )
		{
			if( ( is_legal( i, j ) ) &&
				( m->terrain[i][j] == w ) )
				r++;
		}
	}

	return r;
}


#include <math.h>
#include <stdlib.h>
#include <curses.h>

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

void dig_cooridor( map_t *m, int x1, int y1, int x2, int y2, tile_t *w )
{
	int cx = x1,
		cy = y1,
		xo = ( x2 > x1 ) ? 1 : -1,
		yo = ( y2 > y1 ) ? 1 : -1;
	
	while( cx != x2 )
	{
		if(	( is_legal( cx, cy ) ) &&
			( m->terrain[cx][cy] == &tile_wall ) )
				m->terrain[cx][cy] = w;
		
		cx += xo;
	}

	while( cy != y2 )
	{
		if(	( is_legal( cx, cy ) ) &&
			( m->terrain[cx][cy] == &tile_wall ) )
			m->terrain[cx][cy] = w;

		cy += yo;
	}

	if( is_legal( cx, cy ) )
		m->terrain[cx][cy] = w;
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
					float loop_chance )
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
					rx = rand() % (cell_width-1) + 1;
					ry = rand() % (cell_height-1) + 1;
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
			dig_cooridor( m, rooms[ra]->x, rooms[ra]->y, rooms[rb]->x, rooms[rb]->y, &tile_cooridor );
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

int count_neighbours_q( map_t *m, int x, int y, tile_t *w )
{
	int n = 0;
	if( is_legal( x-1, y ) && ( m->terrain[x-1][y] == w ) ) n++;
	if( is_legal( x+1, y ) && ( m->terrain[x+1][y] == w ) ) n++;
	if( is_legal( x, y-1 ) && ( m->terrain[x][y-1] == w ) ) n++;
	if( is_legal( x, y+1 ) && ( m->terrain[x][y+1] == w ) ) n++;

	return n;
}

void post_process_map( map_t *m )
{
	int i, j;

	/* cooridor tiles are only usable by the map generator */
	for( i = 0; i < m->width; i++ )
	{
		for( j = 0; j < m->height; j++ )
		{
			if( m->terrain[i][j] == &tile_cooridor )
			{
				m->terrain[i][j] = &tile_floor;
			}
		}
	}

	/* cut off dead ends */
	int n = 0;
	do
	{
		n = 0;
		for( i = 0; i < m->width; i++ )
		{
			for( j = 0; j < m->height; j++ )
			{
				if( ( ( m->terrain[i][j] == &tile_floor ) ||
					  ( m->terrain[i][j] == &tile_door_closed ) ) &&
					( count_neighbours_q( m, i, j, &tile_floor ) +
						count_neighbours_q( m, i, j, &tile_door_closed ) < 2 ) )
				{
					m->terrain[i][j] = &tile_wall;
					n++;
				}
			}
		}
	}
	while( n > 0 );

	/* remove walls that have no neighbouring floor tiles */
	for( i = 0; i < m->width; i++ )
	{
		for( j = 0; j < m->height; j++ )
		{
			if( ( m->terrain[i][j] == &tile_wall ) &&
				( count_neighbours( m, i, j, &tile_floor ) == 0 ) )
			{
				m->terrain[i][j] = &tile_void;
			}
		}
	}

	/* remove (some) pillars */
	for( i = 0; i < m->width; i++ )
	{
		for( j = 0; j < m->height; j++ )
		{
			if( ( m->terrain[i][j] == &tile_wall ) &&
				( count_neighbours( m, i, j, &tile_floor ) == 8 ) )
			{
				/* TODO move this chance into a #define */
				if( rand()%10 == 0 )
				{
					m->terrain[i][j] = &tile_pillar;
				}
				else
				{
					m->terrain[i][j] = &tile_floor;
				}
			}
		}
	}
}

void link_dungeon_levels( void )
{
	int i;
	int cx, cy;
	int dx, dy;
	int tries;

	for( i = 0; i < MAX_LEVELS-1; i++ )
	{
		cx = 0;
		cy = 0;
		tries = 0;

		while( 1 )
		{
			tries++;
			cx = rand() % MAP_WIDTH;
			cy = rand() % MAP_HEIGHT;
	
			/*	stairs that have the same position on both maps being linked
				look nicer, but if that can't happen (tries>MAGIC_NUMBER),
				fall back to a random placement */

			if( ( dungeon[i  ]->terrain[cx][cy] == &tile_floor ) &&
				( dungeon[i+1]->terrain[cx][cy] == &tile_floor ) )
			{
				link_t *la = alloc_link();
				la->face = '>';
				la->color = COLOR_PAIR( C_WHITE );
				la->src_x = cx;
				la->src_y = cy;
				la->src_z = i;
				la->dest_x = cx;
				la->dest_y = cy;
				la->dest_z = i+1;
				list_add_tail( link_list, (void*)la );

				link_t *lb = alloc_link();
				lb->face = '<';
				lb->color = COLOR_PAIR( C_WHITE );
				lb->src_x = cx;
				lb->src_y = cy;
				lb->src_z = i+1;
				lb->dest_x = cx;
				lb->dest_y = cy;
				lb->dest_z = i;
				list_add_tail( link_list, (void*)lb );

				break;
			}

			if( tries > MAGIC_NUMBER )
			{
				cx = 0;
				cy = 0;
				
				dx = 0;
				dy = 0;

				do
				{
					cx = rand() % MAP_WIDTH;
					cy = rand() % MAP_HEIGHT;
				}
				while( dungeon[i]->terrain[cx][cy] != &tile_floor );
				
				do
				{
					dx = rand() % MAP_WIDTH;
					dy = rand() % MAP_HEIGHT;
				}
				while( dungeon[i+1]->terrain[dx][dy] != &tile_floor );
				
				link_t *la = alloc_link();
				la->face = '>';
				la->color = COLOR_PAIR( C_WHITE );
				la->src_x = cx;
				la->src_y = cy;
				la->src_z = i;
				la->dest_x = dx;
				la->dest_y = dy;
				la->dest_z = i+1;
				list_add_tail( link_list, (void*)la );

				link_t *lb = alloc_link();
				lb->face = '<';
				lb->color = COLOR_PAIR( C_WHITE );
				lb->src_x = dx;
				lb->src_y = dy;
				lb->src_z = i+1;
				lb->dest_x = cx;
				lb->dest_y = cy;
				lb->dest_z = i;
				list_add_tail( link_list, (void*)lb );
			}
		}
	}

}

int count_neighbours_sparse( map_t *m, int x, int y, tile_t *w )
{
	int i, j;
	int n = 0;

	for( i = x-2; i <= x+2; i += 2 )
	{
		for( j = y-2; j <= y+2; j += 2 )
		{
			if( ( is_legal_strict( i, j ) ) &&
				( m->terrain[i][j] == w ) )
				n++;
		}
	}

	return n;
}

void dig_room_with_doors( map_t *m, int x, int y, int w, int h )
{
	int i, j;

	for( i = x; i < x+w; i++ )
	{
		for( j = y; j < y+h; j++ )
		{
			if( is_legal( i, j ) )
			{
				if( ( i == x ) || ( j == y ) ||
					( i == x+w-1 ) || ( j == y+h-1 ) )
				{
					if( m->terrain[i][j] == &tile_cooridor )
						m->terrain[i][j] = &tile_door_closed;
					/*else
						m->terrain[i][j] = &tile_wall;*/
				}
				else
				{
					m->terrain[i][j] = &tile_floor;
				}
			}
		}
	}
}

/* returns number of cells dug */
/* TODO: add arguments such as number of rooms, number of tries etc. */
int make_dla_dungeon( map_t *m )
{
	int i, j;
	int rx, ry;
	int dx, dy;
	int n;
	int builder_spawned = 0;
	int tries = 0;
	int r = 0;
	int linked = 0;

	for( i = 0; i < MAP_WIDTH; i++ )
	{
		for( j = 0; j < MAP_HEIGHT; j++ )
		{
			m->terrain[i][j] = &tile_wall;
		}
	}

	rx = rand() % ( MAP_WIDTH - 2 );
	ry = rand() % ( MAP_HEIGHT - 2 );

	if( ( rx % 2 ) == 0 )
		rx++;
	if( ( ry % 2 ) == 0 )
		ry++;

	m->terrain[rx][ry] = &tile_cooridor;

	while( tries < 900 )
	{
		if( builder_spawned )
		{
			do
			{
				dx = ( rand() % 3 ) - 1;
				dy = ( rand() % 3 ) - 1;
			}
			while( dx*dx + dy*dy != 1 );

			rx += dx*2;
			ry += dy*2;

			if( !( is_legal_strict( rx, ry ) ) )
			{
				builder_spawned = 0;
			}
			else
			{
				n = count_neighbours_sparse( m, rx, ry, &tile_cooridor );

				if( n > 0 )
				{
					linked = 0;

					for( i = rx-2; i <= rx+2; i+=2 )
					{
						for( j = ry-2; j <= ry+2; j+=2 )
						{
							if( ( is_legal_strict( i, j ) &&
								( m->terrain[i][j] == &tile_cooridor ) ) &&
								( !linked || ( rand()%7 == 0 ) ) )
							{
								dig_cooridor( m, i, j, rx, ry, &tile_cooridor );
								linked = 1;
							}
						}
					}

					r++;
					builder_spawned = 0;
				}
			}
		}
		else
		{
			do
			{
				tries++;

				rx = rand() % ( MAP_WIDTH - 1 );
				ry = rand() % ( MAP_HEIGHT - 1 );

				if( ( rx % 2 ) == 0 )
					rx++;
				if( ( ry % 2 ) == 0 )
					ry++;
			}
			while( m->terrain[rx][ry] != &tile_wall );

			builder_spawned = 1;
		}

	}

	for( i = 0; i < 10; i++ )
	{
		do
		{
			rx = rand() % ( MAP_WIDTH-2 );
			ry = rand() % ( MAP_HEIGHT-2 );
			dx = rand() % 7 + 4;
			dy = rand() % 5 + 4;
		}
		while(	( !is_legal_strict( rx+dx, ry+dy ) ) ||
				( m->terrain[rx][ry] != &tile_cooridor ) );

		if( ( rx % 2 ) == 1 )
			rx++;
		if( ( ry % 2 ) == 1 )
			ry++;

		if( ( dx % 2 ) == 0 )
			dx++;
		if( ( dy % 2 ) == 0 )
			dy++;

		dig_room_with_doors( m, rx, ry, dx, dy );
	}

	return r;
}

void cavernize( map_t *m, float chance )
{
	int i, j;

	for( i = 0; i < m->width; i++ )
	{
		for( j = 0; j < m->height; j++ )
		{
			if( ( ( (float)(rand()%1000) / 1000.0f ) < chance ) &&
				( m->terrain[i][j] == &tile_floor ) &&
				( count_neighbours( m, i, j, &tile_floor ) > 6 ) )
				m->terrain[i][j] = &tile_wall;
		}
	}
}


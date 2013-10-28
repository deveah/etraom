
#include <curses.h>

#include "etraom.h"

list_t *entity_list = NULL;
entity_t *player = NULL;

entity_t *alloc_entity( buf_t *name )
{
	int i, j, k;
	entity_t *e = malloc( sizeof(entity_t) );

	e->name = bufcpy( name );
	e->face = 'z';
	e->color = COLOR_PAIR( COLOR_WHITE );
	e->flags = 0;

	e->lightmap = (float***) malloc( sizeof(float**) * MAX_LEVELS );

	e->inventory = alloc_list();

	for( i = 0; i < MAX_LEVELS; i++ )
	{
		e->lightmap[i] = (float**) malloc( sizeof(float*) * MAP_WIDTH );
		for( j = 0; j < MAP_WIDTH; j++ )
		{
			e->lightmap[i][j] = (float*) malloc( sizeof(float) * MAP_HEIGHT );
			for( k = 0; k < MAP_HEIGHT; k++ )
			{
				e->lightmap[i][j][k] = 0.0f;
			}
		}
	}

	log_add( "[alloc_entity] Allocated entity 0x%08x('%s').\n", e,
		e->name->data );

	return e;
}

void free_entity( entity_t *e )
{
	int i, j;

	if( e )
	{
		for( i = 0; i < MAX_LEVELS; i++ )
		{
			if( e->lightmap[i] )
			{
				for( j = 0; j < MAP_WIDTH; j++ )
				{
					if( e->lightmap[i][j] )
						free( e->lightmap[i][j] );
				}

				free( e->lightmap[i] );
			}
		}
		free( e->lightmap );

		list_element *el = e->inventory->head;

		while( el )
		{
			free_item( (item_t*)el->data );
			el = el->next;
		}

		free_list( e->inventory );

		bufdestroy( e->name );
		free( e );
		log_add( "[free_entity] Freed entity 0x%08x.\n", e );
	}
}

void free_entities( void )
{
	list_element *e = entity_list->head;

	while( e )
	{
		free_entity( (entity_t*)e->data );
		e = e->next;
	}
}

void entity_act( entity_t *e )
{
	int key;

	e->ap += e->agility;

	while( e->ap > 0 )
	{
		log_add( "[entity_act] Act: 0x%08x('%s').\n", e, e->name->data );
		log_add( "[entity_act]\tentity has agility %i and ap %i.\n", e->agility, e->ap );

		if( e->flags & ENTITYFLAG_PLAYERCONTROL )
		{
			do_fov( e, 5 );
			draw_main_screen();
			
			key = getch();

			if( handle_key( key ) == 1 )
				e->ap -= 10; /* TODO action point consumption */

			if( !running )
				return;
		}
		else
		{
			if( entity_dumb_ai( e ) )
				e->ap -= 10;
		}
	}
}

/* entity_move_rel should return 1 if the action is successful, otherwise 0 */
int entity_move_rel( entity_t *e, int dx, int dy )
{
	if( is_legal( e->x+dx, e->y+dy ) )
	{
		if( dungeon[e->z]->terrain[e->x+dx][e->y+dy]->flags & TILEFLAG_SOLID )
		{
			if( e == player )
			{
				buf_t *msg = bufnew( "Yep, it's solid alright." );
				push_message( msg );
				bufdestroy( msg );
			}

			return 0;
		}
		else
		{
			entity_t *ee = entity_find_by_position( e->x+dx, e->y+dy, e->z );
			if( ee )
			{
				buf_t *msg = bufnew( "You hit the " );
				bufcat( msg, ee->name );
				bufcats( msg, "!" );
				push_message( msg );
				bufdestroy( msg );

				melee_attack( e, ee );

				return 1;
			}
			else
			{
				e->x += dx;
				e->y += dy;

				return 1;
			}
		}
	}

	/* TODO: reachable? */
	return 0;
}

int open_door( entity_t *e, int x, int y )
{
	if( !is_legal( x, y ) )
		return 0;
	
	if( dungeon[e->z]->terrain[x][y] == &tile_door_closed )
	{
		dungeon[e->z]->terrain[x][y] = &tile_door_open;

		if( e == player )
		{
			buf_t *msg = bufnew( "You open the door." );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			/* different messages if the door being opened is seen or not */
			if( player->lightmap[e->z][e->x][e->y] > 0.0f )
			{
				buf_t *msg = bufnew( "A door opens." );
				push_message( msg );
				bufdestroy( msg );
			}
			else
			{
				buf_t *msg = bufnew( "You hear a door open." );
				push_message( msg );
				bufdestroy( msg );
			}
		}

		return 1;
	}
	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There's no closed door there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

int close_door( entity_t *e, int x, int y )
{
	if( !is_legal( x, y ) )
		return 0;
	
	if( dungeon[e->z]->terrain[x][y] == &tile_door_open )
	{
		dungeon[e->z]->terrain[x][y] = &tile_door_closed;

		if( e == player )
		{
			buf_t *msg = bufnew( "You close the door." );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			/* different messages if the door being opened is seen or not */
			if( player->lightmap[e->z][e->x][e->y] > 0.0f )
			{
				buf_t *msg = bufnew( "A door closes." );
				push_message( msg );
				bufdestroy( msg );
			}
			else
			{
				buf_t *msg = bufnew( "You hear a door close." );
				push_message( msg );
				bufdestroy( msg );
			}
		}

		return 1;
	}
	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There's no open door there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

entity_t *entity_find_by_position( int x, int y, int z )
{
	list_element *e = entity_list->head;

	while( e )
	{
		entity_t *ent = (entity_t*)e->data;
		if( ( x == ent->x ) && ( y == ent->y ) && ( z == ent->z ) )
			return ent;

		e = e->next;
	}

	return NULL;
}

void entity_die( entity_t *e )
{
	buf_t *msg = bufnew( "The " );
	bufcat( msg, e->name );
	bufcats( msg, " dies!" );
	push_message( msg );
	bufdestroy( msg );

	log_add( "[entity_die] Entity 0x%08x (%s) has died.\n", e, e->name->data );
	int i = list_find( entity_list, (void*)e );
	list_remove_index( entity_list, i );
	free_entity( e );
}

int entity_dumb_ai( entity_t *e )
{
	int rx, ry;
	int tries = 0;

	while( 1 )
	{
		tries++;
		/* skip a turn if there's no viable output */
		if( tries > 100 )
			return 1;

		rx = rand()%3 - 1;
		ry = rand()%3 - 1;
	
		if( ( rx*rx + ry*ry > 0 ) &&
			( is_legal( e->x+rx, e->y+ry ) ) &&
			!( dungeon[e->z]->terrain[e->x+rx][e->y+ry]->flags & TILEFLAG_SOLID ) &&
			( !entity_find_by_position( e->x+rx, e->y+ry, e->z ) ) )
			break;
	}

	return entity_move_rel( e, rx, ry );
}

int entity_follow_stairs( entity_t *e )
{
	tile_t *t = dungeon[e->z]->terrain[e->x][e->y];

	if( t == &tile_stairs_down )
	{
		e->z++;
		e->x = dungeon[e->z]->entrance_x;
		e->y = dungeon[e->z]->entrance_y;

		if( e == player )
		{
			buf_t *msg = bufnew( "You descend." );
			push_message( msg );
			bufdestroy( msg );
		}
		
		return 1;
	}

	else if( t == &tile_stairs_up )
	{
		e->z--;
		e->x = dungeon[e->z]->exit_x;
		e->y = dungeon[e->z]->exit_y;

		if( e == player )
		{
			buf_t *msg = bufnew( "You ascend." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 1;
	}

	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There are no stairs there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

int entity_pick_up( entity_t *e )
{
	list_t *li = item_find_by_position( e->x, e->y, e->z );

	if( !li )
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There's nothing to pick up." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}

	if( li->length == 1 )
	{
		item_t *i = (item_t*)li->head->data;
		int j = list_find( item_list, i );
		list_remove_index( item_list, j );

		inventory_add_item( e, i );

		if( e == player )
		{
			buf_t *msg = bufnew( "You pick up " );
			bufcat( msg, i->name );
			bufcats( msg, "." );
			push_message( msg );
			bufdestroy( msg );
		}
		
		free_list( li );
		return 1;
	}
	else
	{
		/* TODO: unimplemented */
		/* should pop up a menu, asking what to pick up */
		
		free_list( li );
		return 1;
	}

	/* else(?) fail */
	return 0;
}

int entity_drop( entity_t *e, item_t *i )
{
	(void) e;
	(void) i;

	/* TODO: unimplemented */

	return 0;
}



#include <string.h>
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

	return move_relative( e, rx, ry );
}



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
			draw_main_screen(); /* TODO the right place for this? */

			
			key = getch();

			if( handle_key( key ) == 1 )
				e->ap -= 10; /* TODO action point consumption */

			if( !running )
				return;
		}
		else
		{
			/* TODO */
			e->ap -= 10;
		}
	}
}

/* entity_move_rel should return 1 if the action is successful, otherwise 0 */
int entity_move_rel( entity_t *e, int dx, int dy )
{
	if( 	( is_legal( e->x+dx, e->y+dy ) ) &&
			!( dungeon[e->z]->terrain[e->x+dx][e->y+dy]->flags & TILEFLAG_SOLID ) )
	{
		entity_t *ee = entity_find_by_position( e->x+dx, e->y+dy, e->z );
		if( ee )
		{
			ee->hp--;
			if( ee->hp <= 0 )
				entity_die( ee );
			
			buf_t *msg = bufnew( "You hit the thing!" );
			push_message( msg );
			bufdestroy( msg );
			return 1;
		}
		else
		{
			e->x += dx;
			e->y += dy;

			return 1;
		}
	}
	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "Yep, it's solid alright." );
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
			return e;

		e = e->next;
	}

	return NULL;
}

void entity_die( entity_t *e )
{
	log_add( "[entity_die] Entity 0x%08x (%s) has died.\n", e, e->name );
	int i = list_find( entity_list, (void*)e );
	list_remove_index( entity_list, i );
	free_entity( e );
}


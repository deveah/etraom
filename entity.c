
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
	e->color = COLOR_PAIR( C_WHITE );
	e->flags = 0;

	e->lightmap = (float***) malloc( sizeof(float**) * MAX_LEVELS );

	e->inventory = alloc_list();

	e->agility = 0;
	e->ap = 0;

	e->in_hand = NULL;
	e->worn = NULL;

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
		if( e->lightmap )
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
		}

		if( e->inventory )
		{
			list_element *el = e->inventory->head;

			while( el )
			{
				free_item( (item_t*)el->data );
				el = el->next;
			}

			free_list( e->inventory );
		}

		if( e->in_hand )
			free_item( e->in_hand );
		if( e->worn )
			free_item( e->worn );

		if( e->ai )
			free_ai( e->ai );

		bufdestroy( e->name );
		free( e );
		log_add( "[free_entity] Freed entity 0x%08x.\n", e );
	}
}

entity_t *clone_entity( entity_t *e )
{
	if( !e )
		return NULL;
	
	entity_t *ce = alloc_entity( e->name );
	ce->face = e->face;
	ce->color = e->color;
	ce->ap = e->ap;
	ce->agility = e->agility;
	ce->hp = e->hp;
	ce->max_hp = e->max_hp;
	ce->x = e->x;
	ce->y = e->y;
	ce->z = e->z;

	clone_lightmap( ce, e );

	/* TODO: should clone inventory? */

	ce->worn = clone_item( e->worn );
	ce->in_hand = clone_item( e->in_hand );

	ce->flags = e->flags;

	return ce;
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

		do_fov( e, 5 );
		
		if( e->flags & ENTITYFLAG_PLAYERCONTROL )
		{
			draw_main_screen();
			
			key = getch();

			if( handle_key( key ) == 1 )
			{
				e->ap -= 10; /* TODO action point consumption */
				player_turns++;
			}

			if( !running )
				return;
		}
		else
		{
			if( run_ai( e ) )
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
	if( e == player )
	{
		clear();
		mvprintw( 0, 0, "You die. Press any key to exit." );
		running = 0;
		getch();

		return;
	}

	buf_t *msg = bufnew( "The " );
	bufcat( msg, e->name );
	bufcats( msg, " dies!" );
	push_message( msg );
	bufdestroy( msg );

	/* place corpse */
	buf_t *b = bufnew( e->name->data );
	bufcats( b, " corpse" );
	item_t *c = alloc_item( b );
	bufdestroy( b );
	c->face = '%';
	c->color = e->color;
	c->quantity = 1;
	c->quality = 1.0f;
	c->type = ITEMTYPE_JUNK;
	c->place = ITEMPLACE_DUNGEON;
	c->x = e->x;
	c->y = e->y;
	c->z = e->z;
	c->flags = ITEMFLAG_STACKABLE | ITEMFLAG_PICKABLE;
	list_add_tail( item_list, (void*)c );

	log_add( "[entity_die] Entity 0x%08x (%s) has died.\n", e, e->name->data );
	int i = list_find( entity_list, (void*)e );
	list_remove_index( entity_list, i );
	free_entity( e );
}


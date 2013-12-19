
#include <string.h>
#include <curses.h>

#include "etraom.h"

list_t *item_list = NULL;

item_t *alloc_item( buf_t *name )
{
	item_t *i = malloc( sizeof(item_t) );

	i->name = bufcpy( name );
	i->face = '?';
	i->color = COLOR_PAIR( C_RED );
	i->flags = 0;
	i->type = ITEMTYPE_NONE;

	i->place = ITEMPLACE_VOID;

	i->specific = NULL;

	return i;
}

void free_item( item_t *i )
{
	if( i )
	{
		bufdestroy( i->name );

		if( i->specific )
		{
			if( i->type == ITEMTYPE_WEAPON )
			{
				weapon_t *w = (weapon_t*)i->specific;

				if( w->ammo_type )
					free_item( w->ammo_type );
			}

			free( i->specific );
		}

		free( i );
	}
}

item_t *clone_item( item_t *i )
{
	if( !i )
		return 0;
	
	item_t *ci = alloc_item( i->name );

	ci->face = i->face;
	ci->color = i->color;
	ci->quantity = i->quantity;
	ci->quality = i->quality;
	ci->type = i->type;
	
	ci->specific = NULL;
	if( i->type == ITEMTYPE_WEAPON )
	{
		ci->specific = malloc( sizeof(weapon_t) );
		memcpy( i->specific, ci->specific, sizeof(weapon_t) );

		weapon_t *w = (weapon_t*)i->specific;
		weapon_t *cw = (weapon_t*)ci->specific;
		cw->ammo_type = clone_item( w->ammo_type );
	}
	if( i->type == ITEMTYPE_ARMOR )
	{
		ci->specific = malloc( sizeof(armor_t) );
		memcpy( i->specific, ci->specific, sizeof(armor_t) );
	}

	ci->place = i->place;
	ci->x = i->x;
	ci->y = i->y;
	ci->z = i->z;

	ci->flags = i->flags;

	return ci;
}

void free_items( void )
{
	list_element *e = item_list->head;

	while( e )
	{
		free_item( (item_t*)e->data );
		e = e->next;
	}
}

/* multiple items can be found, so return a list */
list_t *item_find_by_position( int x, int y, int z )
{
	list_element *e = item_list->head;
	list_t *li = alloc_list();

	while( e )
	{
		item_t *it = (item_t*)e->data;
		if( ( x == it->x ) && ( y == it->y ) && ( z == it->z ) &&
			( it->place == ITEMPLACE_DUNGEON ) )
			list_add_head( li, it );

		e = e->next;
	}

	if( li->length == 0 )
	{
		free_list( li );
		return NULL;
	}
	else
		return li;
}

int items_alike( item_t *a, item_t *b )
{
	if( !a )
		return 0;
	if( !b )
		return 0;
	
	return( ( strcmp( a->name->data, b->name->data ) == 0 ) &&
			( a->type == b->type ) &&
			( a->quality == b->quality ) );
}

int count_items( list_t *li, item_t *i )
{
	list_element *el = li->head;
	int c = 0;

	while( el )
	{
		item_t *j = (item_t*)el->data;
		
		if( items_alike( i, j ) )
		{
			c += j->quantity;
		}

		el = el->next;
	}

	return c;
}



#include <curses.h>

#include "etraom.h"

list_t *item_list = NULL;

item_t *alloc_item( buf_t *name )
{
	item_t *i = malloc( sizeof(item_t) );

	i->name = bufcpy( name );
	i->face = '?';
	i->color = COLOR_PAIR( COLOR_RED );
	i->flags = 0;

	i->place = ITEMPLACE_VOID;

	i->specific = NULL;

	return i;
}

void free_item( item_t *i )
{
	if( i )
	{
		bufdestroy( i->name );
		free( i );
	}
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
			list_add_tail( li, it );

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


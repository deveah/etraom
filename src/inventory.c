
#include <string.h>

#include "etraom.h"

int inventory_add_item( entity_t *e, item_t *i )
{
	list_element *el = e->inventory->head;
	item_t *found = NULL;

	if( i->flags & ITEMFLAG_STACKABLE )
	{
		if( items_alike( e->in_hand, i ) )
			found = e->in_hand;
		if( items_alike( e->worn, i ) )
			found = e->worn;

		while( el )
		{
			item_t *ii = (item_t*)el->data;

			if( items_alike( i, ii ) )
			{
				found = ii;
				break;
			}

			el = el->next;
		}

		if( found )
		{
			found->quantity += i->quantity;
			free_item( i );
		}
		else
		{
			list_add_head( e->inventory, i );
			i->place = ITEMPLACE_ENTITY;
		}
	}
	else
	{
		list_add_head( e->inventory, i );
		i->place = ITEMPLACE_ENTITY;
	}

	/* TODO: shouldn't always return successfully */
	return 1;
}



#include <string.h>

#include "etraom.h"

int inventory_add_item( entity_t *e, item_t *i )
{
	list_element *el = e->inventory->head;
	item_t *found = NULL;

	while( el )
	{
		item_t *ii = (item_t*)el->data;

		/* TODO: better comparison? */
		if(	( strcmp( i->name->data, ii->name->data ) == 0 ) &&
			/* can't stack items of different quality */
			( i->quality == ii->quality ) )
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
		list_add_tail( e->inventory, i );
		i->place = ITEMPLACE_ENTITY;
	}

	/* TODO: shouldn't always return successfully */
	return 1;
}


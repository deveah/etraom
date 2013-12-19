
#include "etraom.h"

list_t *link_list = NULL;

link_t *alloc_link( void )
{
	link_t *l = malloc( sizeof(link_t) );

	l->flags = 0;

	return l;
}

void free_link( link_t *l )
{
	if( l )
		free( l );
}

void free_links( void )
{
	list_element *e = link_list->head;

	while( e )
	{
		free_link( (link_t*)e->data );
		e = e->next;
	}
}

/* there can only be one link per point */
link_t *link_find_by_position( int x, int y, int z )
{
	list_element *el = link_list->head;

	while( el )
	{
		link_t *l = (link_t*)el->data;

		if( ( l->src_z == z ) && ( l->src_x == x ) &&
			( l->src_y == y ) )
		{
			return l;
		}

		el = el->next;
	}

	return NULL;
}


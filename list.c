
#include <stdio.h>
#include <stdlib.h>

#include "etraom.h"

list_t *alloc_list( void )
{
	list_t *l = malloc( sizeof( list_t ) );
	l->head = NULL;
	l->tail = NULL;
	l->length = 0;

	return l;
}

void list_add_head( list_t *l, void *e )
{
	list_element *el = malloc( sizeof( list_element ) );
	el->data = e;

	if( l->head )
	{
		el->next = l->head;
		l->head = el;
	}
	else
	{
		l->head = el;
		l->tail = el;
		el->next = NULL;
	}

	l->length++;
}

void list_add_tail( list_t *l, void *e )
{
	list_element *el = malloc( sizeof( list_element ) );
	el->data = e;

	if( l->tail )
	{
		l->tail->next = el;
		l->tail = el;
		el->next = NULL;
	}
	else
	{
		l->tail = el;
		l->head = el;
		el->next = NULL;
	}

	l->length++;
}

void print_list( list_t *l )
{
	list_element *el = l->head;

	printf( "Printing list 0x%08x: ", (int)l );

	while( el )
	{
		printf( "%i ", (int) el->data );
		el = el->next;
	}

	printf( "\n" );
}

void *list_get_index( list_t *l, int i )
{
	list_element *el = l->head;
	int j = 0;

	while( el )
	{
		if( i == j )
			return el->data;

		el = el->next;
		j++;
	}

	return NULL;
}

void list_remove_index( list_t *l, int i )
{
	list_element *el = l->head;
	list_element *temp;
	int j = 0;

	while( el )
	{
		if( j+1 == i )
		{
			temp = el->next;
			el->next = temp->next;

			free( temp );

			l->length--;
		}

		el = el->next;
		j++;
	}
}

void free_list( list_t *l )
{
	list_element *el = l->head;
	list_element *temp;

	while( el )
	{
		temp = el;
		el = el->next;

		free( temp );
	}

	free( l );
}

void list_traverse( list_t *l, void (*func)(void*) )
{
	list_element *el = l->head;
	list_element *temp;

	while( el )
	{
		temp = el;
		el = el->next;
		func( temp->data );
	}
}

int list_find( list_t *l, void *item )
{
	list_element *el = l->head;
	int i = 0;

	while( el )
	{
		if( el->data == item )
			return i;

		el = el->next;
	}
	
	return -1;
}


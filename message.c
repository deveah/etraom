
#include "etraom.h"

list_t *message_list = NULL;

void push_message( buf_t *b )
{
	message_t *m = malloc( sizeof(message_t) );

	m->msg = bufcpy( b );
	m->flags = MESSAGEFLAG_UNREAD;

	list_add_tail( message_list, (void*)m );
}

void free_message( message_t *m )
{
	bufdestroy( m->msg );
	free( m );
}

void free_messages( void )
{
	list_element *m = message_list->head;

	while( m )
	{
		free_message( (message_t*)m );
		m = m->next;
	}
}


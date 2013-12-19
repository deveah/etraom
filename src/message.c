
#include "etraom.h"

list_t *message_list = NULL;

void push_message( buf_t *b )
{
	message_t *m = malloc( sizeof(message_t) );

	m->msg = bufcpy( b );
	m->flags = MESSAGEFLAG_UNREAD;
	m->turn = player_turns;

	log_add( "[message] %s\n", b->data );

	list_add_tail( message_list, (void*)m );
}

void free_message( message_t *m )
{
	bufdestroy( m->msg );
	free( m );
}

void free_messages( void )
{
	list_element *m = message_list->head, *temp;

	while( m )
	{
		temp = m;
		m = m->next;
		free_message( (message_t*)temp->data );
	}
}


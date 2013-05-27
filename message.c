
#include "etraom.h"

list_t *message_list = NULL;

void push_message( buf_t *b )
{
	message_t *m = malloc( sizeof(message_t) );

	m->msg = bufcpy( b );
	m->flags = 0;

	list_add_tail( message_list, (void*)m );
}

void free_message( void *m )
{
	bufdestroy( ((message_t*)m)->msg );
	free( m );
}

void free_message_list( void )
{
	list_traverse( message_list, free_message );
	free_list( message_list );
}


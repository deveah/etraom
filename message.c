
#include "etraom.h"

list_t *message_list = NULL;

void init_message_list( void )
{
	message_list = alloc_list();
}

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
}

void test_messages( void )
{
	init_message_list();

	buf_t *a = bufnew( "test message bla" );
	buf_t *b = bufnew( "test message 2" );

	push_message( a );
	push_message( b );

	bufdestroy( a );
	bufdestroy( b );

	free_message_list();
}


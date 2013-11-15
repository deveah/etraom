
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "etraom.h"

buf_t *bufnew( char* str )
{
	buf_t *b = (buf_t*)malloc( sizeof(buf_t) );

	if( !b )
		fprintf( stderr, "malloc failed.\n" );

	if( str )
	{
		b->data = malloc( sizeof(char) * ( strlen(str) + 1 ) );
		strcpy( b->data, str );
		b->length = strlen( str ) + 1;
	}
	else
	{
		b->data = NULL;
		b->length = 0;
	}

	return b;
}

void bufcats( buf_t *b, char* s )
{
	int l = strlen( s );

	if( b->data )
	{
		b->data = realloc( b->data, sizeof(char) * ( l + b->length ) );
		strcat( b->data, s );
		b->length += l;
	}
	else
	{
		b->data = malloc( sizeof(char) * l );
		strcpy( b->data, s );
		b->length = l;
	}
}

void bufcat( buf_t *dest, buf_t *src )
{
	if( !src->data )
		return;

	if( dest->data )
	{
		dest->data = realloc( dest->data,
			sizeof(char) * ( dest->length + src->length ) );
		strcat( dest->data, src->data );
		dest->length += src->length;
	}
	else
	{
		dest->data = malloc( sizeof(char) * src->length );
		strcpy( dest->data, src->data );
		dest->length = src->length;
	}
}

buf_t *bufcpy( buf_t *src )
{
	if( !src )
		return NULL;
	
	buf_t *b = malloc( sizeof(buf_t) );

	if( src->data )
	{
		b->data = malloc( sizeof(char) * src->length );
		memcpy( b->data, src->data, src->length );
		b->length = src->length;
	}
	else
	{
		b->data = NULL;
		b->length = 0;
	}

	return b;
}

void bufprintf( buf_t *b, char* format, ... )
{
	/* TODO */
	/* malloc a temp string, snprintf on it, replace buf data with it, free it */

	(void) b;
	(void) format;
}

void bufdestroy( buf_t *b )
{
	if( b )
	{
		if( b->data )
			free( b->data );

		free( b );
	}
}


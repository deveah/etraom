
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "etraom.h"

FILE *logfile = NULL;

int open_logfile( void )
{
	logfile = fopen( "./log", "w" );

	if( !logfile )
		return 0;
	
	//fprintf( logfile, "Log begins at %i.\n", time( 0 ) );
	fprintf( logfile, "Etraom version %s.\n\n", ETRAOM_VERSION );

	return 1;
}

int close_logfile( void )
{
	if( !logfile )
		return 0;
	//fprintf( logfile, "\nLog ends at %i.\n", time( 0 ) );
	fclose( logfile );

	return 0;
}

void log_add( char *format, ... )
{
	char buf[MAX_STRING_LENGTH];
	va_list args;

	if( !format )
		return;
	if( !logfile )
		return;
	
	va_start( args, format );
	vsnprintf( buf, MAX_STRING_LENGTH, format, args );
	fputs( buf, logfile );

	va_end( args );
}


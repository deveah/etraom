
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "etraom.h"

FILE *logfile = NULL;

int open_logfile( void )
{
	if( !( game_flags & GAMEFLAG_DEVELOPER ) )
		return 1;

	logfile = fopen( "./log", "w" );

	if( !logfile )
		return 0;
	
	fprintf( logfile, "Etraom version %s.\n\n", ETRAOM_VERSION );

	return 1;
}

int close_logfile( void )
{
	if( !logfile )
		return 0;
	fclose( logfile );

	return 0;
}

void log_add( char *format, ... )
{
	va_list args;

	if( !format )
		return;
	if( !logfile )
		return;
	
	va_start( args, format );
	vfprintf( logfile, format, args );
	va_end( args );
}


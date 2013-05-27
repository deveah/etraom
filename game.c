
#include <stdlib.h>
#include <time.h>

#include "blesteme.h"
#include "etraom.h"

int running = 0;

unsigned int nlevels = 0;
map_t **dungeon = NULL;
int main_seed = 0;

void new_game( unsigned int seed )
{
	unsigned int i;

	log_add( "Creating new game...\n" );

	main_seed = seed;
	srand( seed );

	nlevels = MAX_LEVELS;

	dungeon = (map_t**) malloc( nlevels * sizeof(map_t*) );

	for( i = 0; i < nlevels; i++ )
	{
		dungeon[i] = alloc_map( "Dungeon", 80, 25 );
		make_dummy_map( dungeon[i], 80 );
	}

	log_add( "Done creating new game.\n" );
}

int init_game( int argc, char** argv )
{
	(void) argc;
	(void) argv;

	if( init_ui() == -1 )
	{
		printf( "etraom: unable to initialize display via blesteme.\n" );
		exit( 0 );
	}

	open_logfile();
	printf( "Etraom version %s, by Vlad Dumitru.\n", ETRAOM_VERSION );

	new_game( time( 0 ) );

	test_messages();

	running = 1;

	return 1;
}

int terminate_game( void )
{
	unsigned int i;

	log_add( "Terminating game...\n" );

	for( i = 0; i < nlevels; i++ )
	{
		free_map( dungeon[i] );
	}

	free( dungeon );
	
	log_add( "Done terminating game.\n" );

	terminate_ui();
	close_logfile();

	return 1;
}

int game_loop( void )
{
	bl_input_t i;

	while( running )
	{
		bl_clear_input( &i );

		draw_main_screen();

		if( bl_input( &i ) != -1 )
		{
			if( i.key != -1 )
				handle_key( i.key, i.mod );

			if( i.quit )
				running = 0;
		}

		bl_delay( 10 );
	}
	
	return 1;
}

void handle_key( int key, int mod )
{
	log_add( "Received key 0x%08x(%c), mod 0x%08x\n", key, key, mod );

	if( key == 'q' )
		running = 0;
}


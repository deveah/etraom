
#include <stdlib.h>
#include <time.h>

#include "blesteme.h"
#include "etraom.h"

int running = 0;
int global_turns = 0;

unsigned int nlevels = 0;
map_t **dungeon = NULL;
int main_seed = 0;

void new_game( unsigned int seed )
{
	unsigned int i;
	buf_t *map_name = bufnew( "Dungeon" );

	log_add( "Creating new game...\n" );

	main_seed = seed;
	srand( seed );

	nlevels = MAX_LEVELS;

	dungeon = (map_t**) malloc( nlevels * sizeof(map_t*) );

	for( i = 0; i < nlevels; i++ )
	{
		dungeon[i] = alloc_map( map_name, MAP_WIDTH, MAP_HEIGHT );
		make_dummy_map( dungeon[i], 80 );
	}

	bufdestroy( map_name );

	buf_t *player_name = bufnew( "Player" );
	player = alloc_entity( player_name );
	bufdestroy( player_name );

	player->x = 10;
	player->y = 10;
	player->z = 0;
	player->flags = ENTITYFLAG_PLAYERCONTROL;

	list_add_head( entity_list, (void*)player );

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

	message_list = alloc_list();
	entity_list = alloc_list();

	new_game( time( 0 ) );

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

	free_entities();
	free_list( entity_list );
	free_messages();
	free_list( message_list );

	log_add( "Done terminating game.\n" );

	terminate_ui();
	close_logfile();

	return 1;
}

int game_loop( void )
{
	list_element *e;
	
	while( running )
	{
		draw_main_screen();

		e = entity_list->head;
		while( e )
		{
			entity_act( (entity_t*)e->data );
			do_fov( (entity_t*)e->data, 10 );
			e = e->next;
		}

		global_turns++;

		bl_delay( 10 );
	}
	
	return 1;
}

void handle_key( int key, int mod )
{
	entity_t *player = (entity_t*)(entity_list->head->data);
	
	log_add( "[handle_key] Received key 0x%08x(%c), mod 0x%08x\n", key, key,
		mod );

	if( key == 'q' )
		running = 0;
	
	if( key == 'h' )
		entity_move_rel( player, -1,  0 );
	if( key == 'j' )
		entity_move_rel( player,  0,  1 );
	if( key == 'k' )
		entity_move_rel( player,  0, -1 );
	if( key == 'l' )
		entity_move_rel( player,  1,  0 );
}


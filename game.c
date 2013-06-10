
#include <stdlib.h>
#include <time.h>

#include "etraom.h"

int running = 0;
int global_turns = 0;

unsigned int nlevels = 0;
map_t **dungeon = NULL;
int main_seed = 0;

int ***dungeon_memory = NULL;

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
		//make_dummy_map( dungeon[i], 80 );
		make_grid_map( dungeon[i], 20, 12, 0.6, 0.8, 0.2, 0.3 );
	}

	bufdestroy( map_name );

	buf_t *player_name = bufnew( "Player" );
	player = alloc_entity( player_name );
	bufdestroy( player_name );

	player->x = 0;
	player->y = 0;
	player->z = 0;

	while( dungeon[player->z]->terrain[player->x][player->y] != &tile_floor )
	{
		player->x = rand() % MAP_WIDTH;
		player->y = rand() % MAP_HEIGHT;
	}
	
	player->flags = ENTITYFLAG_PLAYERCONTROL;
	player->agility = 15;
	player->ap = 0;

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

	alloc_dungeon_memory();

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

	free_dungeon_memory();

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
		e = entity_list->head;
		while( e )
		{
			entity_act( (entity_t*)e->data );
			e = e->next;
		}

		global_turns++;
	}
	
	return 1;
}

/* handle_key should return 1 if action is successful, otherwise 0 */
int handle_key( int key )
{
	entity_t *player = (entity_t*)(entity_list->head->data);
	
	log_add( "[handle_key] Received key 0x%08x(%c)\n", key, key );

	if( key == 'q' )
		running = 0;
	
	if( key == 'h' )
		return entity_move_rel( player, -1,  0 );
	if( key == 'j' )
		return entity_move_rel( player,  0,  1 );
	if( key == 'k' )
		return entity_move_rel( player,  0, -1 );
	if( key == 'l' )
		return entity_move_rel( player,  1,  0 );
	if( key == 'y' )
		return entity_move_rel( player, -1, -1 );
	if( key == 'u' )
		return entity_move_rel( player,  1, -1 );
	if( key == 'b' )
		return entity_move_rel( player, -1,  1 );
	if( key == 'n' )
		return entity_move_rel( player,  1,  1 );

	return 0;
}

void alloc_dungeon_memory( void )
{
	int i, j, k;
	
	dungeon_memory = (int***) malloc( sizeof(int**) * MAX_LEVELS );
	for( k = 0; k < MAX_LEVELS; k++ )
	{
		dungeon_memory[k] = (int**) malloc( sizeof(int*) * MAP_WIDTH );
		for( i = 0; i < MAP_WIDTH; i++ )
		{
			dungeon_memory[k][i] = (int*) malloc( sizeof(int) * MAP_WIDTH );
			for(  j = 0; j < MAP_HEIGHT; j++ )
			{
				dungeon_memory[k][i][j] = 0;
			}
		}
	}
}

void free_dungeon_memory( void )
{
	int i, j;

	for( i = 0; i < MAX_LEVELS; i++ )
	{
		for( j = 0; j < MAP_WIDTH; j++ )
		{
			free( dungeon_memory[i][j] );
		}
		free( dungeon_memory[i] );
	}

	free( dungeon_memory );
}

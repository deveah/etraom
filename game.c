
#include <stdlib.h>
#include <time.h>
#include <curses.h>

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

		int r;
		
		/*do
		{
			clear_map( dungeon[i] );
			r = make_grid_map( dungeon[i], 10, 7, 0.6, 0.9, 0.5 );
		}
		while( r < 20 ); */

		do
		{
			clear_map( dungeon[i] );
			r = make_dla_dungeon( dungeon[i] );
		}
		while( r < 150 );
		
		post_process_map( dungeon[i] );
		
		debug_dump_map( dungeon[i] );
	}

	link_dungeon_levels();

	bufdestroy( map_name );

	buf_t *player_name = bufnew( "Player" );
	player = alloc_entity( player_name );
	bufdestroy( player_name );

	player->face = '@';

	player->x = 0;
	player->y = 0;
	player->z = 0;

	player->hp = 5;
	player->max_hp = 5;

	player->in_hand = NULL;
	player->worn = NULL;

	while( dungeon[player->z]->terrain[player->x][player->y] != &tile_floor )
	{
		player->x = rand() % MAP_WIDTH;
		player->y = rand() % MAP_HEIGHT;
	}
	
	player->flags = ENTITYFLAG_PLAYERCONTROL;
	player->agility = 15;
	player->ap = 0;

	list_add_head( entity_list, (void*)player );

	make_random_entities( 10 );
	make_random_objects( 160 );

	log_add( "Done creating new game.\n" );
}

int init_game( int argc, char** argv )
{
	(void) argc;
	(void) argv;

	if( !init_ui() )
	{
		printf( "Your terminal needs to be at least 80x25.\n" );
		exit( 0 );
	}

	open_logfile();
	printf( "Etraom version %s, by Vlad Dumitru.\n", ETRAOM_VERSION );

	message_list = alloc_list();
	entity_list = alloc_list();
	item_list = alloc_list();

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
	free_items();
	free_list( item_list );

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
	point_t p;
	
	log_add( "[handle_key] Received key 0x%08x(%c)\n", key, key );

	switch( key )
	{
	case 'q':
		running = 0;
		break;
	case 'm':
		reveal_map( player->z );
		break;

	case '4':
	case KEY_LEFT:
	case 'h':
		return move_relative( player, -1,  0 );
	case '2':
	case KEY_DOWN:
	case 'j':
		return move_relative( player,  0,  1 );
	case '8':
	case KEY_UP:
	case 'k':
		return move_relative( player,  0, -1 );
	case '6':
	case KEY_RIGHT:
	case 'l':
		return move_relative( player,  1,  0 );
	case '7':
	case 'y':
		return move_relative( player, -1, -1 );
	case '9':
	case 'u':
		return move_relative( player,  1, -1 );
	case '1':
	case 'b':
		return move_relative( player, -1,  1 );
	case '3':
	case 'n':
		return move_relative( player,  1,  1 );

	case '5':
	case '.':
		/* wait a turn */
		return 1;

	case '>':
		return follow_stairs( player );

	case ',':
		/*return entity_pick_up( player );*/
		return draw_pick_up_screen( player );
	case 'i':
		return draw_inventory_screen( player );

	case 'o':
		p = input_direction( "Open where?" );
		return open_door( player, player->x + p.x, player->y + p.y );
	case 'c':
		p = input_direction( "Close where?" );
		return close_door( player, player->x + p.x, player->y + p.y );

	/* TODO: should the melee attack require an extra keystroke? */
	/*case 'z':
		p = input_direction( "Attack where?" );
		break;*/
	
	default:
		/* TODO warn unknown keystroke */
		break;
	}

	return 0;
}

void make_random_entities( int n )
{
	int i;
	buf_t *name;
	entity_t *ent;

	for( i = 0; i < n; i++ )
	{
		name = bufnew( "Entity" );
		ent = alloc_entity( name );
		bufdestroy( name );

		ent->x = 0;
		ent->y = 0;
		ent->z = 0;

		ent->hp = 2;
		ent->max_hp = 2;

		while( dungeon[ent->z]->terrain[ent->x][ent->y] != &tile_floor )
		{
			ent->x = rand() % MAP_WIDTH;
			ent->y = rand() % MAP_HEIGHT;
		}
	
		ent->flags = 0;
		ent->agility = rand() % 5 + 10;
		ent->ap = 0;

		list_add_head( entity_list, (void*)ent );
	}
}

void make_random_objects( int n )
{
	int i;
	buf_t *name_a = bufnew( "Red Item" );
	buf_t *name_b = bufnew( "Green Item" );
	buf_t *name_c = bufnew( "Blue Item" );
	item_t *it;
	int r = 0;

	for( i = 0; i < n; i++ )
	{
		r = rand() % 3;

		if( r == 0 )
			it = alloc_item( name_a );
		else if( r == 1 )
			it = alloc_item( name_b );
		else
			it = alloc_item( name_c );

		it->place = ITEMPLACE_DUNGEON;
		it->x = 0;
		it->y = 0;
		it->z = 0;

		it->face = '*';

		if( r == 0 )
			it->color = COLOR_PAIR( COLOR_RED );
		else if( r == 1 )
			it->color = COLOR_PAIR( COLOR_GREEN );
		else
			it->color = COLOR_PAIR( COLOR_BLUE );

		it->quantity = 1;
		it->quality = 1.0f;

		while( dungeon[it->z]->terrain[it->x][it->y] != &tile_floor )
		{
			it->x = rand() % MAP_WIDTH;
			it->y = rand() % MAP_HEIGHT;
		}

		it->flags = ITEMFLAG_PICKABLE | ITEMFLAG_STACKABLE;

		list_add_head( item_list, (void*)it );
	}

	bufdestroy( name_a );
	bufdestroy( name_b );
	bufdestroy( name_c );
}


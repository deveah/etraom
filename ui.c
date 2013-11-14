
#include <curses.h>

#include "etraom.h"

int init_ui( void )
{
	initscr();
	keypad( stdscr, TRUE );
	raw();
	noecho();
	curs_set( 0 );

	getmaxyx( stdscr, term_h, term_w );

	/* TODO: fail loudly? */
	if( ( term_w < 80 ) || ( term_h < 25 ) )
	{
		return -1;
	}

	start_color();

#ifndef _WIN32
	use_default_colors();
#endif

	int i;
	for( i = 0; i < 9; i++ )
	{
#ifndef _WIN32
		init_pair( i, i, -1 );
#else
		init_pair( i, i, 0 );
#endif
	}

	return 1;
}

int terminate_ui( void )
{
	endwin();

	return 1;
}

int draw_main_screen( void )
{
	int i, j;
	list_element *k;

	log_add( "[draw_main_screen]\n" );

	move( 0, 0 ); clrtoeol();

	for( i = 0; i < dungeon[player->z]->width; i++ )
	{
		for( j = 0; j < dungeon[player->z]->height; j++ )
		{
			tile_t *t = dungeon[player->z]->terrain[i][j];

			if( player->lightmap[player->z][i][j] > 0.0f )
			{
				attrset( t->fg );
				mvaddch( j+1, i, t->face );
			}
			else if( dungeon[player->z]->memory[i][j] )
			{
				attrset( COLOR_PAIR( COLOR_BLUE ) );
				mvaddch( j+1, i, dungeon[player->z]->memory[i][j]->face );
			}
			else
			{
				mvaddch( j+1, i, ' ' );
			}
		}
	}

	k = item_list->head;
	while( k )
	{
		item_t *it = (item_t*)k->data;
		if( ( it->z == player->z ) && ( it->place == ITEMPLACE_DUNGEON ) &&
			( player->lightmap[player->z][it->x][it->y] > 0.0f ) )
		{
			attrset( it->color );

			/*	an item on the ground should not prevent the stairs from being
				visible */
			if( ( dungeon[it->z]->terrain[it->x][it->y] == &tile_stairs_down ) ||
				( dungeon[it->z]->terrain[it->x][it->y] == &tile_stairs_up ) )
				attron( A_REVERSE );

			mvaddch( it->y+1, it->x, it->face );
		}
		k = k->next;
	}

	k = entity_list->head;
	while( k )
	{
		entity_t *e = (entity_t*)k->data;
		if(	( player->z == e->z ) &&
			( player->lightmap[player->z][e->x][e->y] > 0.0f ) )
		{
			attrset( e->color );
			mvaddch( e->y+1, e->x, e->face );
		}
		k = k->next;
	}

	buf_t *msgline = bufnew( "" );

	k = message_list->head;
	while( k )
	{
		/* TODO support for more than one message via --MORE-- */
		message_t *m = (message_t*)k->data;
		if( m->flags & MESSAGEFLAG_UNREAD )
		{
			bufcat( msgline, m->msg );
			bufcats( msgline, " " );

			m->flags &= ~MESSAGEFLAG_UNREAD;
		}
		k = k->next;
	}

	mvprintw( 0, 0, "%s", msgline->data );

	bufdestroy( msgline );

	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( 23, 0, "Nectarie the Gunslinger" );
	mvprintw( 24, 0, "hp: 16/16 ac: 8 lv: 1" ); 

	return 1;
}

int draw_inventory_screen( entity_t *e )
{
	list_element *el = e->inventory->head;
	int i = 0;

	log_add( "[draw_inventory_screen] Displaying %08x('%s')'s inventory:\n",
		e, e->name->data );

	move( 0, 0 ); clrtoeol();
	attrset( COLOR_PAIR( COLOR_WHITE ) | A_REVERSE );
	mvprintw( 0, 0, "-- Inventory --" );

	while( el )
	{
		item_t *it = (item_t*)el->data;

		log_add( "[draw_inventory_screen] %c) [%c] %s (%i)\n", 'a'+i, it->face,
			it->name->data, it->quantity );

		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( i+1, 0, "%c) [ ] %s (%i)", 'a' + i, it->name->data, it->quantity );
		attrset( it->color );
		mvaddch( i+1, 4, it->face );

		i++;

		el = el->next;
	}

	getch();

	log_add( "[draw_inventory_screen] Done displaying inventory.\n" );

	return 1;
}

int draw_pick_up_screen( entity_t *e )
{
	list_t *li = item_find_by_position( e->x, e->y, e->z );
	list_element *el;
	int i = 0;

	if( !li )
	{
		buf_t *msg = bufnew( "There's nothing to pick up." );
		push_message( msg );
		bufdestroy( msg );

		return 0;
	}

	move( 0, 0 ); clrtoeol();
	attrset( COLOR_PAIR( COLOR_WHITE ) | A_REVERSE );
	mvprintw( 0, 0, "-- Pick Up --" );

	el = li->head;

	while( el )
	{
		item_t *it = (item_t*)el->data;

		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( i+1, 0, "%c) [ ] %s (%i)", 'a' + i, it->name->data, it->quantity );
		attrset( it->color );
		mvaddch( i+1, 4, it->face );

		i++;

		el = el->next;
	}

	int k = getch();
	k -= 'a';

	if( ( k < 0 ) || ( k >= i ) )
	{
		buf_t *msg = bufnew( "Okay, then." );
		push_message( msg );
		bufdestroy( msg );

		free_list( li );
		return 0;
	}

	item_t *it = (item_t*)list_get_index( li, k );
	int q = 1;

	if( it->quantity > 1 )
	{
		move( 0, 0 ); clrtoeol();
		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( 0, 0, "How many? " );
		echo();
		scanw( "%i", &q );
		noecho();

		if( ( k <= 0 ) || ( k > it->quantity ) )
		{
			buf_t *msg = bufnew( "Okay, then." );
			push_message( msg );
			bufdestroy( msg );

			free_list( li );
			return 0;
		}
	}

	buf_t *msg = bufnew( "You pick up " );
	bufcat( msg, it->name );
	bufcats( msg, "." );
	push_message( msg );
	bufdestroy( msg );

	entity_pick_up( e, it, q );
	free_list( li );
	return 1;
}

int draw_drop_screen( entity_t *e )
{
	list_t *li = e->inventory;
	list_element *el;
	int i = 0;

	if( ( !li ) || ( li->length == 0 ) )
	{
		buf_t *msg = bufnew( "There's nothing to drop." );
		push_message( msg );
		bufdestroy( msg );

		return 0;
	}

	move( 0, 0 ); clrtoeol();
	attrset( COLOR_PAIR( COLOR_WHITE ) | A_REVERSE );
	mvprintw( 0, 0, "-- Drop --" );

	el = li->head;

	while( el )
	{
		item_t *it = (item_t*)el->data;

		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( i+1, 0, "%c) [ ] %s (%i)", 'a' + i, it->name->data, it->quantity );
		attrset( it->color );
		mvaddch( i+1, 4, it->face );

		i++;

		el = el->next;
	}

	int k = getch();
	k -= 'a';

	if( ( k < 0 ) || ( k >= i ) )
	{
		buf_t *msg = bufnew( "Okay, then." );
		push_message( msg );
		bufdestroy( msg );

		free_list( li );
		return 0;
	}

	item_t *it = (item_t*)list_get_index( li, k );
	int q = 1;

	if( it->quantity > 1 )
	{
		move( 0, 0 ); clrtoeol();
		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( 0, 0, "How many? " );
		echo();
		scanw( "%i", &q );
		noecho();

		if( ( k <= 0 ) || ( k > it->quantity ) )
		{
			buf_t *msg = bufnew( "Okay, then." );
			push_message( msg );
			bufdestroy( msg );

			free_list( li );
			return 0;
		}
	}

	buf_t *msg = bufnew( "You drop " );
	bufcat( msg, it->name );
	bufcats( msg, "." );
	push_message( msg );
	bufdestroy( msg );

	entity_drop( e, it, q );
	free_list( li );
	return 1;
}

point_t input_direction( char *msg )
{
	int k;
	point_t p;

	if( msg )
	{
		move( 0, 0 ); clrtoeol();
		attrset( COLOR_PAIR( COLOR_WHITE ) );
		mvprintw( 0, 0, "%s", msg );
		refresh();
	}

	k = getch();

	switch( k )
	{
	case '4':
	case KEY_LEFT:
	case 'h':
		p.x = -1; p.y =  0;
		break;
	case '2':
	case KEY_DOWN:
	case 'j':
		p.x =  0; p.y =  1;
		break;
	case '8':
	case KEY_UP:
	case 'k':
		p.x =  0; p.y = -1;
		break;
	case '6':
	case KEY_RIGHT:
	case 'l':
		p.x =  1; p.y =  0;
		break;
	case '7':
	case 'y':
		p.x = -1; p.y = -1;
		break;
	case '9':
	case 'u':
		p.x =  1; p.y = -1;
		break;
	case '1':
	case 'b':
		p.x = -1; p.y =  1;
		break;
	case '3':
	case 'n':
		p.x =  1; p.y =  1;
		break;
	default:
		p.x = 0; p.y = 0;
	}

	return p;
}


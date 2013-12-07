
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

	if( ( term_w < 80 ) || ( term_h < 25 ) )
	{
		return 0;
	}

	start_color();

#ifndef _WIN32
	use_default_colors();
#endif

#ifndef _WIN32
	init_pair( C_BLACK, COLOR_BLACK, -1 );
	init_pair( C_RED, COLOR_RED, -1 );
	init_pair( C_GREEN, COLOR_GREEN, -1 );
	init_pair( C_YELLOW, COLOR_YELLOW, -1 );
	init_pair( C_BLUE, COLOR_BLUE, -1 );
	init_pair( C_MAGENTA, COLOR_MAGENTA, -1 );
	init_pair( C_CYAN, COLOR_CYAN, -1 );
	init_pair( C_WHITE, COLOR_WHITE, -1 );
#else
	init_pair( C_BLACK, COLOR_BLACK, 0 );
	init_pair( C_RED, COLOR_RED, 0 );
	init_pair( C_GREEN, COLOR_GREEN, 0 );
	init_pair( C_YELLOW, COLOR_YELLOW, 0 );
	init_pair( C_BLUE, COLOR_BLUE, 0 );
	init_pair( C_MAGENTA, COLOR_MAGENTA, 0 );
	init_pair( C_CYAN, COLOR_CYAN, 0 );
	init_pair( C_WHITE, COLOR_WHITE, 0 );
#endif

	return 1;
}

int terminate_ui( void )
{
	endwin();

	return 1;
}

void draw_title_screen( void )
{
	char *title[] = {
		"@@@@@@@@ @@@@@@@ @@@@@@@   @@@@@@   @@@@@@  @@@@@@@@@@ ",
		"@@!        @@!   @@!  @@@ @@!  @@@ @@!  @@@ @@! @@! @@!",
		"@!!!:!     @!!   @!@!!@!  @!@!@!@! @!@  !@! @!! !!@ @!@",
		"!!:        !!:   !!: :!!  !!:  !!! !!:  !!! !!:     !!:",
		": :: :::    :     :   : :  :   : :  : :. :   :      :  "
	};

	int i, j;

	if( game_flags & GAMEFLAG_DEVELOPER )
	{
		return;
	}

	for( j = 0; j < 5; j++ )
	{
		for( i = 0; i < 55; i++ )
		{
			switch( title[j][i] )
			{
			case '@':
			case '!':
				attrset( COLOR_PAIR( C_GREEN ) );
				break;
			case ':':
			case '.':
				attrset( COLOR_PAIR( C_YELLOW ) );
				break;
			default:
				attrset( COLOR_PAIR( C_WHITE ) );
			}

			mvaddch( 8 + j, 12+i, title[j][i] );
		}
	}

	attrset( COLOR_PAIR( C_BLACK ) | A_BOLD );
	mvprintw( 14, 12, "A science-fiction roguelike." );
	mvprintw( 15, 12, "http://github.com/deveah/etraom" );
	mvprintw( 16, 12, "See LICENSE for licensing information." );

	getch();
}

int draw_main_screen( void )
{
	int i, j;
	list_element *k;

	log_add( "[draw_main_screen]\n" );

	/* clear message lines */
	move( 0, 0 ); clrtoeol();
	move( 1, 0 ); clrtoeol();

	attrset( COLOR_PAIR( C_WHITE ) );

	for( i = 0; i < dungeon[player->z]->width; i++ )
	{
		for( j = 0; j < dungeon[player->z]->height; j++ )
		{
			tile_t *t = dungeon[player->z]->terrain[i][j];

			if( player->lightmap[player->z][i][j] > 0.0f )
			{
				attrset( t->fg );
				mvaddch( j+2, i, t->face );
			}
			else if( dungeon[player->z]->memory[i][j] )
			{
				attrset( COLOR_PAIR( C_BLACK ) | A_BOLD );
				mvaddch( j+2, i, dungeon[player->z]->memory[i][j]->face );
			}
			else
			{
				mvaddch( j+2, i, ' ' );
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

			mvaddch( it->y+2, it->x, it->face );
		}
		k = k->next;
	}

	k = link_list->head;
	while( k )
	{
		link_t *l = (link_t*)k->data;
		if( l->src_z == player->z )
		{
			if( player->lightmap[player->z][l->src_x][l->src_y] > 0.0f )
			{
				attrset( l->color );
				mvaddch( l->src_y+2, l->src_x, l->face );
			}
			else if( dungeon[player->z]->memory[l->src_x][l->src_y] )
			{
				attrset( COLOR_PAIR( C_WHITE ) );
				mvaddch( l->src_y+2, l->src_x, l->face );
			}
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
			mvaddch( e->y+2, e->x, e->face );
		}
		k = k->next;
	}

	list_t *li = item_find_by_position( player->x, player->y, player->z );
	if( li )
	{
		if( li->length == 1 )
		{
			item_t *ii = (item_t*)li->head->data;
			buf_t *b = bufnew( "You stand on " );
			bufcats( b, ii->name->data );
			bufcats( b, "." );
			push_message( b );
			bufdestroy( b );
		}
		else if( li->length > 1 )
		{
			buf_t *b = bufnew( "You stand on several items." );
			push_message( b );
			bufdestroy( b );
		}
		
		free_list( li );
	}

	buf_t *msgline = bufnew( "" );

	k = message_list->head;
	while( k )
	{
		message_t *m = (message_t*)k->data;
		if( m->flags & MESSAGEFLAG_UNREAD )
		{
			bufcat( msgline, m->msg );
			bufcats( msgline, " " );

			m->flags &= ~MESSAGEFLAG_UNREAD;
		}
		k = k->next;
	}

	/* two lines should be enough for everybody */
	mvprintw( 0, 0, "%s", msgline->data );

	bufdestroy( msgline );

	move( 23, 0 ); clrtoeol();
	move( 24, 0 ); clrtoeol();

	attron( COLOR_PAIR( C_WHITE ) );
	mvprintw( 23, 0, "%s (%i/%i)", player->name->data, player->hp, player->max_hp );

	if( player->in_hand )
	{
		if( player->in_hand->type == ITEMTYPE_WEAPON )
		{
			weapon_t *w = (weapon_t*)player->in_hand->specific;
			if( w->clip_size > 0 )
			{
				mvprintw( 23, 40, "w: %s (%i/%i) (%s)", player->in_hand->name->data,
					w->ammo_loaded, count_ammo( player->inventory, w->ammo_type ),
					w->ammo_type->name->data );
			}
			else
			{
				mvprintw( 23, 40, "w: %s", player->in_hand->name->data );
			}
		}
		else
		{
			mvprintw( 23, 40, "w: %s", player->in_hand->name->data );
		}
	}
	else
	{
		mvprintw( 23, 40, "w: -" );
	}

	if( player->worn )
	{
		mvprintw( 24, 40, "a: %s (AC:%i)", player->worn->name->data,
			get_item_ac( player->worn ) );
	}
	else
		mvprintw( 24, 40, "a: -" );

	return 1;
}

int draw_inventory_screen( entity_t *e )
{
	list_t *li = e->inventory;
	list_element *el = li->head;

	int i = 0;
	int start = 0;
	int pos = 0;

	int k = 0;

	while( 1 )
	{
		clear();

		i = 0;
		el = li->head;

		attrset( COLOR_PAIR( C_WHITE ) | A_REVERSE );
		mvprintw( 0, 0, "-- Inventory -- " );

		if( li->length == 0 )
		{
			attrset( COLOR_PAIR( C_WHITE ) );
			mvprintw( 4, 0, "Your inventory is empty." );
		}

		attrset( COLOR_PAIR( C_WHITE ) );
		mvprintw( 1, 0, "in hand:" );

		if( e->in_hand )
		{
			if( e->in_hand->type == ITEMTYPE_WEAPON )
			{
				weapon_t *w = (weapon_t*)e->in_hand->specific;
				if( w->clip_size > 0 )
				{
					mvprintw( 1, 9, "[ ] %s (%i/%i) (%s)", e->in_hand->name->data,
						w->ammo_loaded, count_ammo( e->inventory, w->ammo_type ),
						w->ammo_type->name->data );
				}
				else
				{
					mvprintw( 1, 9, "[ ] %s", player->in_hand->name->data );
				}

				attrset( e->in_hand->color );
				mvaddch( 1, 10, e->in_hand->face );
			}
			else
			{
				mvprintw( 23, 40, "w: %s", player->in_hand->name->data );
			}
		}
		else
		{
			mvprintw( 1, 9, "(nothing)" );
		}

		attrset( COLOR_PAIR( C_WHITE ) );
		mvprintw( 2, 3, "worn:" );

		if( e->worn )
		{
			mvprintw( 2, 9, "[ ] %s (AC:%i)", e->worn->name->data, get_item_ac( e->worn ) );
			attrset( e->worn->color );
			mvaddch( 2, 10, e->worn->face );
		}
		else
		{
			mvprintw( 2, 9, "(nothing)" );
		}

		while( el )
		{
			item_t *ii = (item_t*)el->data;

			if( ( i >= start ) && ( i < start + term_h - 6 ) )
			{
				attrset( COLOR_PAIR( C_WHITE ) );
				if( pos == i )
					mvprintw( i - start + 4, 0, ">" );

				if( ii->flags & ITEMFLAG_STACKABLE )
					mvprintw( i - start + 4, 2, "[ ] %s (%i)", ii->name->data, ii->quantity );
				else
					mvprintw( i - start + 4, 2, "[ ] %s", ii->name->data );

				attrset( ii->color );
				mvaddch( i - start + 4, 3, ii->face );
			}
			
			i++;
			el = el->next;
		}

		attrset( COLOR_PAIR( C_WHITE ) );
		mvprintw( term_h - 1, 0, "drop | wield | Wear | put down | take off | use" );
		
		attrset( COLOR_PAIR( C_CYAN ) );
		mvaddch( term_h - 1, 0, 'd' );
		mvaddch( term_h - 1, 7, 'w' );
		mvaddch( term_h - 1, 15, 'W' );
		mvaddch( term_h - 1, 22, 'p' );
		mvaddch( term_h - 1, 33, 't' );

		k = getch();

		switch( k )
		{
		case 'j':
		case KEY_DOWN:
		case '2':
		{
			if( pos < li->length-1 )
				pos++;
			if( pos - start > term_h - 7 )
				start++;
			break;
		}
		case 'k':
		case KEY_UP:
		case '8':
		{
			if( pos > 0 )
				pos--;
			if( pos - start < 0 )
				start--;
			break;
		}
		case 'd':
		{
			if( li->length == 0 )
			{
				buf_t *msg = bufnew( "There's nothing to drop." );
				push_message( msg );
				bufdestroy( msg );
				clear();
				return 0;
			}
			
			item_t *it = (item_t*)list_get_index( li, pos );
			int q = 1;

			if( it->quantity > 1 )
			{
				move( 0, 0 ); clrtoeol();
				attrset( COLOR_PAIR( C_WHITE ) );
				mvprintw( 0, 0, "How many? " );
				echo();
				scanw( "%i", &q );
				noecho();

				if( ( q <= 0 ) || ( q > it->quantity ) )
				{
					buf_t *msg = bufnew( "Okay, then." );
					push_message( msg );
					bufdestroy( msg );

					clear();
					return 0;
				}
			}

			clear();
			return drop_item( e, it, q );
		}
		case 'w':
		{
			if( li->length == 0 )
			{
				buf_t *msg = bufnew( "There's nothing to wield." );
				push_message( msg );
				bufdestroy( msg );
				clear();
				return 0;
			}

			item_t *it = (item_t*)list_get_index( li, pos );

			clear();
			return wield_item( e, it );
		}
		case 'W':
		{
			if( li->length == 0 )
			{
				buf_t *msg = bufnew( "There's nothing to wear." );
				push_message( msg );
				bufdestroy( msg );
				clear();
				return 0;
			}

			item_t *it = (item_t*)list_get_index( li, pos );

			clear();
			return wear_item( e, it );
		}
		case 'p':
		{
			return put_down_weapon( e );
		}
		case 't':
		{
			return take_off_armor( e );
		}
		default:
			goto hell;
		}
	}

	hell:
	clear();
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
	attrset( COLOR_PAIR( C_WHITE ) | A_REVERSE );
	mvprintw( 0, 0, "-- Pick Up --" );

	el = li->head;

	while( el )
	{
		item_t *it = (item_t*)el->data;

		attrset( COLOR_PAIR( C_WHITE ) );
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
		attrset( COLOR_PAIR( C_WHITE ) );
		mvprintw( 0, 0, "How many? " );
		echo();
		scanw( "%i", &q );
		noecho();

		if( ( q <= 0 ) || ( q > it->quantity ) )
		{
			buf_t *msg = bufnew( "Okay, then." );
			push_message( msg );
			bufdestroy( msg );

			free_list( li );
			return 0;
		}
	}

	pick_up_item( e, it, q );
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
		attrset( COLOR_PAIR( C_WHITE ) );
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

/* TODO: move up/down around buffer */
int draw_message_buffer( void )
{
	list_t *li = message_list;
	list_element *el = li->head;
	int i = 0;
	int pos = 0;

	clear();

	attrset( COLOR_PAIR( C_WHITE ) | A_REVERSE );
	mvprintw( 0, 0, "-- Message Buffer --" );

	attrset( COLOR_PAIR( C_WHITE ) );

	if( li->length == 0 )
	{
		mvprintw( 1, 0, "(empty)" );
		
		getch();
		clear();
		return 1;
	}

	while( el )
	{
		message_t *m = (message_t*)el->data;

		if( i > li->length - term_h + 1 )
		{
			/* TODO: dynamic turn number padding */
			mvprintw( pos + 1, 0, "%4i: %s", m->turn, m->msg->data );
			pos++;
		}

		el = el->next;
		i++;
	}

	getch();
	clear();

	return 1;
}

int look_at( void )
{
	int k = 0;
	int cx = player->x, cy = player->y;

	buf_t *info;

	while( 1 )
	{
		draw_main_screen();
		attrset( COLOR_PAIR( C_WHITE ) | A_REVERSE );
		mvaddch( cy+2, cx, mvinch( cy+2, cx ) & 0xFF );

		move( 0, 0 ); clrtoeol();
		attrset( COLOR_PAIR( C_WHITE ) );
		if( player->lightmap[player->z][cx][cy] > 0.0f )
		{
			info = bufnew( "Look: " );

			entity_t *e = entity_find_by_position( cx, cy, player->z );
			if( e )
			{
				bufcat( info, e->name );
				bufcats( info, "; " );
			}

			list_t *li = item_find_by_position( cx, cy, player->z );
			if( li )
			{
				if( li->length == 1 )
				{
					item_t *it = (item_t*)li->head->data;
					bufcat( info, it->name );
					bufcats( info, "; " );
				}
				else
				{
					bufcats( info, "Several items; " );
				}
			}

			if( li )
				free_list( li );

			link_t *l = link_find_by_position( cx, cy, player->z );
			if( l )
			{
				if( l->face == '>' )
				{
					bufcats( info, "Stairs down." );
				}
				if( l->face == '<' )
				{
					bufcats( info, "Stairs up." );
				}
			}
			else
			{
				bufcats( info, dungeon[player->z]->terrain[cx][cy]->name );
				bufcats( info, "." );
			}

			mvprintw( 0, 0, info->data );
			bufdestroy( info );
		}
		else
		{
			if( dungeon[player->z]->memory[cx][cy] )
			{
				mvprintw( 0, 0, "Look: (You remember) %s.",
					dungeon[player->z]->terrain[cx][cy]->name );
			}
			else
			{
				mvprintw( 0, 0, "Look: You haven't been there yet." );
			}
		}

		k = getch();

		switch( k )
		{
		case 'h':
		case '4':
		case KEY_LEFT:
			if( cx > 0 )
				cx--;
			break;
		case 'j':
		case '2':
		case KEY_DOWN:
			if( cy < MAP_HEIGHT-1 )
				cy++;
			break;
		case 'k':
		case '8':
		case KEY_UP:
			if( cy > 0 )
				cy--;
			break;
		case 'l':
		case '6':
		case KEY_RIGHT:
			if( cx < MAP_WIDTH-1 )
				cx++;
			break;
		case 'q':
			return 1;
		}
	}

	return 1;
}

int fire_at( void )
{
	int k = 0;
	int cx = player->x, cy = player->y;

	buf_t *info;

	while( 1 )
	{
		draw_main_screen();
		attrset( COLOR_PAIR( C_WHITE ) | A_REVERSE );
		mvaddch( cy+2, cx, mvinch( cy+2, cx ) & 0xFF );

		move( 0, 0 ); clrtoeol();
		attrset( COLOR_PAIR( C_WHITE ) );
		if( player->lightmap[player->z][cx][cy] > 0.0f )
		{
			info = bufnew( "Fire: " );

			entity_t *e = entity_find_by_position( cx, cy, player->z );
			if( e )
			{
				bufcat( info, e->name );
			}

			mvprintw( 0, 0, info->data );
			bufdestroy( info );
		}
		else
		{
			mvprintw( 0, 0, "Fire: You can't see over there." );
		}

		k = getch();

		switch( k )
		{
		case 'h':
		case '4':
		case KEY_LEFT:
			if( cx > 0 )
				cx--;
			break;
		case 'j':
		case '2':
		case KEY_DOWN:
			if( cy < MAP_HEIGHT-1 )
				cy++;
			break;
		case 'k':
		case '8':
		case KEY_UP:
			if( cy > 0 )
				cy--;
			break;
		case 'l':
		case '6':
		case KEY_RIGHT:
			if( cx < MAP_WIDTH-1 )
				cx++;
			break;
		case 'f':
			return ranged_attack( player, cx, cy );
			break;
		case 'q':
			return 0;
		}
	}

	return 1;
}


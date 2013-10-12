
#include <curses.h>

#include "etraom.h"

int init_ui( void )
{
	/* TODO handle terminal capabilities and error handling */

	initscr();
	keypad( stdscr, TRUE );
	raw();
	noecho();
	curs_set( 0 );

	start_color();

#ifndef _WIN32
	use_default_colors();
#endif

	int i;
	for( i = 0; i < 9; i++ )
	{
		init_pair( i, i, 0 );
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

	for( i = 0; i < dungeon[player->z]->width; i++ )
	{
		for( j = 0; j < dungeon[player->z]->height; j++ )
		{
			tile_t *t = dungeon[player->z]->terrain[i][j];

			if( player->lightmap[player->z][i][j] > 0.0f )
			{
				attron( t->fg );
				mvaddch( j+1, i, t->face );
			}
			else if( dungeon[player->z]->memory[i][j] )
			{
				attron( COLOR_PAIR( COLOR_BLUE ) );
				mvaddch( j+1, i, dungeon[player->z]->memory[i][j]->face );
			}
			else
			{
				mvaddch( j+1, i, ' ' );
			}
		}
	}

	k = entity_list->head;
	while( k )
	{
		entity_t *e = (entity_t*)k->data;
		if(	( player->z == e->z ) &&
			( player->lightmap[player->z][e->x][e->y] > 0.0f ) )
		{
			attron( e->color );
			mvaddch( e->y+1, e->x, e->face );
		}
		k = k->next;
	}

	k = message_list->head;
	while( k )
	{
		/* TODO support for more than one message via --MORE-- */
		message_t *m = (message_t*)k->data;
		if( m->flags & MESSAGEFLAG_UNREAD )
		{
			mvprintw( 0, 0, "%s", m->msg->data );
			m->flags &= ~MESSAGEFLAG_UNREAD;
		}
		k = k->next;
	}

	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( 23, 0, "Nectarie the Gunslinger" );
	mvprintw( 24, 0, "hp: 16/16 ac: 8 lv: 1" ); 

	return 1;
}


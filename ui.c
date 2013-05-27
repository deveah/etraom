
#include "blesteme.h"
#include "etraom.h"

int init_ui( void )
{
	return bl_initialize( "DejaVuSansMono.ttf", 16, 80, 30, "Etraom", 0 );
}

int terminate_ui( void )
{
	return bl_terminate();
}

int draw_main_screen( void )
{
	int i, j;
	list_element *k;

	bl_clear();

	bl_foreground( 255, 255, 255 );
	bl_background(   0,   0,   0 );
	
	for( i = 0; i < dungeon[player->z]->width; i++ )
	{
		for( j = 0; j < dungeon[player->z]->height; j++ )
		{
			tile_t *t = dungeon[player->z]->terrain[i][j];
			bl_foreground( t->fg[0], t->fg[1], t->fg[2] );
			bl_background( t->bg[0], t->bg[1], t->bg[2] );

			if( player->lightmap[player->z][i][j] > 0.0f )
				bl_addch( i, j+1, t->face );
		}
	}

	k = entity_list->head;
	while( k )
	{
		entity_t *e = (entity_t*)k->data;
		bl_foreground( e->color[0], e->color[1], e->color[2] );
		bl_addch( e->x, e->y+1, e->face );
		k = k->next;
	}

	k = message_list->head;
	while( k )
	{
		/* TODO support for more than one message via --MORE-- */
		message_t *m = (message_t*)k->data;
		if( m->flags & MESSAGEFLAG_UNREAD )
		{
			bl_printf( 0, 0, "%s", m->msg->data );
			m->flags &= ~MESSAGEFLAG_UNREAD;
		}
		k = k->next;
	}

	bl_foreground( 128, 128, 128 );
	bl_background(   0,   0,   0 );
	bl_printf( 0, 26, "Nectarie the Gunslinger" );
	bl_printf( 0, 27, "hp: 16/16 ac: 8 lv: 1" ); 

	bl_refresh();

	return 1;
}

void wait_key( int *key, int *mod, int *quit )
{
	bl_input_t i;
	bl_clear_input( &i );

	while(	( bl_input( &i ) == -1 ) ||
			( i.key == -1 ) ) /* ignore key up events */
	{
		bl_delay( 10 );
	}

	*key = i.key;
	*mod = i.mod;
	*quit = i.quit;
}

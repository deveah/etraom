
#include "blesteme.h"
#include "etraom.h"

int init_ui( void )
{
	return bl_initialize( "DejaVuSansMono.ttf", 16, 80, 30, "Etraom", -1 );
}

int terminate_ui( void )
{
	return bl_terminate();
}

int draw_main_screen( void )
{
	int i, j;

	bl_foreground( 255, 255, 255 );
	bl_background(   0,   0,   0 );
	
	for( i = 0; i < dungeon[0]->width; i++ )
	{
		for( j = 0; j < dungeon[0]->height; j++ )
		{
			tile_t *t = dungeon[0]->terrain[i][j];
			bl_foreground( t->fg[0], t->fg[1], t->fg[2] );
			bl_background( t->bg[0], t->bg[1], t->bg[2] );
			bl_addch( i, j, t->face );
		}
	}

	bl_foreground( 128, 128, 128 );
	bl_background(   0,   0,   0 );
	bl_printf( 0, 25, "Nectarie the Gunslinger" );
	bl_printf( 0, 26, "hp: 16/16 ac: 8 lv: 1" ); 

	bl_refresh();

	return 1;
}

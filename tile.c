
#include <curses.h>

#include "etraom.h"

tile_t tile_void = {
	.face = ' ',
	.fg = COLOR_PAIR( 0 ),
	.bg = COLOR_PAIR( 0 ),
	.flags = TILEFLAG_SOLID
};

tile_t tile_floor = {
	.face = '.',
	.fg = COLOR_PAIR( 7 ),
	.bg = COLOR_PAIR( 0 ),
	.flags = 0
};

tile_t tile_wall = {
	.face = '#',
	.fg = COLOR_PAIR( 4 ),
	.bg = COLOR_PAIR( 0 ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

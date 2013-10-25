
#include <curses.h>

#include "etraom.h"

tile_t tile_void = {
	.face = ' ',
	.fg = COLOR_PAIR( COLOR_BLACK ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = TILEFLAG_SOLID
};

tile_t tile_floor = {
	.face = '.',
	.fg = COLOR_PAIR( COLOR_WHITE ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = 0
};

tile_t tile_cooridor = {
	.face = '.',
	.fg = COLOR_PAIR( COLOR_WHITE ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = 0
};

tile_t tile_wall = {
	.face = '#',
	.fg = COLOR_PAIR( COLOR_CYAN ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

tile_t tile_door_closed = {
	.face = '+',
	.fg = COLOR_PAIR( COLOR_YELLOW ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

tile_t tile_door_open = {
	.face = '\'',
	.fg = COLOR_PAIR( COLOR_YELLOW ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

tile_t tile_stairs_down = {
	.face = '>',
	.fg = COLOR_PAIR( COLOR_CYAN ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = 0
};

tile_t tile_stairs_up = {
	.face = '<',
	.fg = COLOR_PAIR( COLOR_CYAN ),
	.bg = COLOR_PAIR( COLOR_BLACK ),
	.flags = 0
};


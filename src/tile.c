
#include <curses.h>

#include "etraom.h"

tile_t tile_void = {
	.name = "Nothing",
	.face = ' ',
	.fg = COLOR_PAIR( C_BLACK ),
	.flags = TILEFLAG_SOLID
};

tile_t tile_floor = {
	.name = "Floor",
	.face = '.',
	.fg = COLOR_PAIR( C_BLUE ),
	.flags = 0
};

tile_t tile_cooridor = {
	.name = "Cooridor (Bug)",
	.face = '~',
	.fg = COLOR_PAIR( C_WHITE ),
	.flags = 0
};

tile_t tile_wall = {
	.name = "Wall",
	.face = '#',
	.fg = COLOR_PAIR( C_CYAN ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

tile_t tile_door_closed = {
	.name = "Closed Door",
	.face = '+',
	.fg = COLOR_PAIR( C_YELLOW ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

tile_t tile_door_open = {
	.name = "Open Door",
	.face = '\'',
	.fg = COLOR_PAIR( C_YELLOW ),
	.flags = 0
};

tile_t tile_pillar = {
	.name = "Pillar",
	.face = '#',
	.fg = COLOR_PAIR( C_YELLOW ),
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};



#include "etraom.h"

tile_t tile_void = {
	.face = ' ',
	.fg = {   0,   0,   0 },
	.bg = {   0,   0,   0 },
	.flags = TILEFLAG_SOLID
};

tile_t tile_floor = {
	.face = '.',
	.fg = { 128, 128, 128 },
	.bg = {   0,   0,   0 },
	.flags = 0
};

tile_t tile_wall = {
	.face = '#',
	.fg = {  64,  64,  64 },
	.bg = { 128, 128, 128 },
	.flags = TILEFLAG_SOLID | TILEFLAG_OPAQUE
};

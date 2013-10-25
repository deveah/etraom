
#include <stdio.h>
#include <stdlib.h>

#ifndef _ETRAOM_H_
#define _ETRAOM_H_

#define ETRAOM_VERSION "0.01"
#define MAX_STRING_LENGTH 256
#define MAX_LEVELS 10

#define MAP_WIDTH 80
#define MAP_HEIGHT 22

#define MAGIC_NUMBER 1994

#define TILEFLAG_SOLID				(1<<0)
#define TILEFLAG_OPAQUE				(1<<1)
#define TILEFLAG_SEEN				(1<<2)

#define ENTITYFLAG_PLAYERCONTROL	(1<<0)

#define MESSAGEFLAG_UNREAD			(1<<0)

#define ITEMPLACE_VOID				(0)
#define ITEMPLACE_DUNGEON			(1)
#define ITEMPLACE_ENTITY			(2)

#define ITEMFLAG_PICKABLE			(1<<0)

typedef struct
{
	char* data;
	int length;
} buf_t;

struct _list_element
{
	void *data;
	struct _list_element *next;
};

typedef struct _list_element list_element;

typedef struct
{
	list_element *head, *tail;
	int length;
} list_t;

typedef struct
{
	buf_t *msg;
	int flags;
} message_t;

typedef struct
{
	int face;
	int fg, bg;
	int flags;
} tile_t;

extern tile_t 
	tile_void,
	tile_floor,
	tile_cooridor,
	tile_wall,
	tile_door_closed,
	tile_door_open,
	tile_stairs_down,
	tile_stairs_up;

typedef struct
{
	buf_t *name;
	int width, height;

	tile_t ***terrain;
	tile_t ***memory;

	int entrance_x, entrance_y;
	int exit_x, exit_y;

	int flags;
} map_t;

typedef struct
{
	int x, y, linked;
} room_t;

typedef struct
{
	buf_t *name;
	int face, color;

	int quantity;
	float quality;

	int type;
	void *specific;

	int place;
	int x, y, z;

	int flags;
} item_t;

typedef struct
{
	buf_t *name;
	int face, color;

	int ap;
	int agility;

	int hp, max_hp;

	int x, y, z;

	float ***lightmap;

	list_t *inventory;
	item_t *worn, *in_hand;

	int flags;
} entity_t;

extern FILE *logfile;

extern int running;
extern int global_turns;

extern unsigned int nlevels;
extern map_t **dungeon;
extern int main_seed;

extern list_t *message_list;
extern list_t *entity_list;
extern list_t *item_list;
extern entity_t *player; /* shortcut to player struct */
extern int ***dungeon_memory;

/* terminal width/height */
int term_w, term_h;

/* util.c */
int distance( int x1, int y1, int x2, int y2 );

/* sight.c */
void reveal_map( int n );
void clear_lightmap( entity_t *e, int n );
void do_fov( entity_t *e, int radius );
int do_ray( entity_t *e, int x2, int y2 );

/* entity.c */
entity_t *alloc_entity( buf_t *name );
void free_entity( entity_t *e );
void free_entities( void );
void entity_act( entity_t *e );
int entity_move_rel( entity_t *e, int dx, int dy );
entity_t *entity_find_by_position( int x, int y, int z );
void entity_die( entity_t *e );
int entity_dumb_ai( entity_t *e );
int entity_follow_stairs( entity_t *e );
int entity_pick_up( entity_t *e );
int entity_drop( entity_t *e, item_t *i );

/* item.c */
item_t *alloc_item( buf_t *name );
void free_item( item_t *i );
void free_items( void );
list_t *item_find_by_position( int x, int y, int z );

/* inventory.c */
int inventory_add_item( entity_t *e, item_t *i );

/* message.c */
void push_message( buf_t *b );
void free_message( message_t *m );
void free_messages( void );

/* buf.c */
buf_t *bufnew( char *str );
void bufcats( buf_t *b, char *s );
void bufcat( buf_t *dest, buf_t *src );
buf_t *bufcpy( buf_t *src );
void bufdestroy( buf_t *b );

/* list.c */
list_t *alloc_list( void );
void list_add_head( list_t *l, void *e );
void list_add_tail( list_t *l, void *e );
void print_list( list_t *l );
void *list_get_index( list_t *l, int i );
void list_remove_index( list_t *l, int i );
void free_list( list_t *l );
void list_traverse( list_t *l, void (*func)(void*) );
int list_find( list_t *l, void* item );

/* game.c */
void new_game( unsigned int seed );
int init_game( int argc, char** argv );
int terminate_game( void );
int game_loop( void );
int handle_key( int key );
void make_random_entities( int n );
void make_random_objects( int n );

/* ui.c */
int init_ui( void );
int terminate_ui( void );
int draw_main_screen( void );
int draw_inventory_screen( entity_t *e );

/* log.c */
int open_logfile( void );
int close_logfile( void );
void log_add( char *format, ... );

/* map.c */
map_t *alloc_map( buf_t *name, int width, int height );
void free_map( map_t *m );
void clear_map( map_t *m );
void debug_dump_map( map_t *m );
int is_legal( int x, int y );

/* mapgen.c */
int make_dummy_map( map_t *m, int nwalls );
void dig_room( map_t *m, int x, int y, int w, int h );
void dig_cooridor( map_t *m, int x1, int y1, int x2, int y2 );
int closest_room( room_t **r, int nrooms, int n, float loop_chance );
int all_rooms_linked( room_t **r, int nrooms );
int make_grid_map(	map_t *m, int cell_width, int cell_height,
					float room_chance, float node_chance,
					float loop_chance );
void make_drunken_walk_cave( map_t *m, int n );
int count_neighbours( map_t *m, int x, int y, tile_t *w );
void post_process_map( map_t *m );
void link_dungeon_levels( void );

/* combat.c */
int melee_attack( entity_t *atk, entity_t *def );

#endif

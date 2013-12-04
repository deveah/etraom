
#include <stdio.h>
#include <stdlib.h>

#ifndef _ETRAOM_H_
#define _ETRAOM_H_

#define ETRAOM_VERSION				"0.0.1"
#define MAX_STRING_LENGTH			256
#define MAX_LEVELS					10

#define ENTITIES_FILE				"data/entities"
#define ITEMS_FILE					"data/items"
#define WEAPONS_FILE				"data/weapons"
#define ARMORS_FILE					"data/armors"

#define MAP_WIDTH					80
#define MAP_HEIGHT					21

#define MAGIC_NUMBER				1994

#define TILEFLAG_SOLID				(1<<0)
#define TILEFLAG_OPAQUE				(1<<1)
#define TILEFLAG_SEEN				(1<<2)

#define ENTITYFLAG_PLAYERCONTROL	(1<<0)

#define MESSAGEFLAG_UNREAD			(1<<0)

#define ITEMFLAG_PICKABLE			(1<<0)
#define ITEMFLAG_STACKABLE			(1<<1)

#define LINKFLAG_USED				(1<<0)

#define C_BLACK						1
#define C_RED						2
#define C_GREEN						3
#define C_YELLOW					4
#define C_BLUE						5
#define C_MAGENTA					6
#define C_CYAN						7
#define C_WHITE						8

enum item_type
{
	ITEMTYPE_NONE,
	ITEMTYPE_WEAPON,
	ITEMTYPE_ARMOR,
	ITEMTYPE_AMMO,
	ITEMTYPE_JUNK,
	ITEMTYPE_MAX
};

enum item_place
{
	ITEMPLACE_VOID,
	ITEMPLACE_ENTITY,
	ITEMPLACE_DUNGEON
};

enum weapon_type
{
	WEAPONTYPE_MELEE,
	WEAPONTYPE_HANDGUN,
	WEAPONTYPE_MAX
};

typedef struct
{
	int x, y;
} point_t;

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
	int turn;
} message_t;

typedef struct
{
	char *name;
	int face;
	int fg;
	int flags;
} tile_t;

extern tile_t 
	tile_void,
	tile_floor,
	tile_cooridor,
	tile_wall,
	tile_door_closed,
	tile_door_open,
	tile_pillar;

typedef struct
{
	buf_t *name;
	int width, height;

	tile_t ***terrain;
	tile_t ***memory;

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

typedef struct
{
	int src_x, src_y, src_z;
	int dest_x, dest_y, dest_z;

	int face, color;
	int flags;
} link_t;

typedef struct
{
	int type;
	int accuracy;
	int min_damage, max_damage;

	item_t *ammo_type;
	int ammo_loaded, clip_size;
	
	/*int neffects;
	int effect[EFFECT_MAX];*/
} weapon_t;

typedef struct
{
	/* TODO different responses to different damage types? */
	int ac;
} armor_t;

extern FILE *logfile;

extern int running;
extern int global_turns;
extern int player_turns;

extern unsigned int nlevels;
extern map_t **dungeon;
extern int main_seed;

extern list_t *entity_type_list;

extern list_t *message_list;
extern list_t *entity_list;
extern list_t *item_list;
extern list_t *link_list;
extern entity_t *player; /* shortcut to player struct */

/* terminal width/height */
int term_w, term_h;

/* parser.c */
int parse_color( int c );
int parse_entities( char *fn );
void free_entity_types( void );

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
entity_t *clone_entity( entity_t *e );
void free_entities( void );
void entity_act( entity_t *e );
entity_t *entity_find_by_position( int x, int y, int z );
void entity_die( entity_t *e );
int entity_dumb_ai( entity_t *e );

/* item.c */
item_t *alloc_item( buf_t *name );
void free_item( item_t *i );
item_t *clone_item( item_t *i );
void free_items( void );
list_t *item_find_by_position( int x, int y, int z );
int items_alike( item_t *a, item_t *b );
int count_items( list_t *li, item_t *i );

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
int draw_pick_up_screen( entity_t *e );
point_t input_direction( char *msg );
int draw_message_buffer( void );
int fire_at( void );
int look_at( void );

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
int is_legal_strict( int x, int y );

/* mapgen.c */
int make_dummy_map( map_t *m, int nwalls );
void dig_room( map_t *m, int x, int y, int w, int h );
void dig_cooridor( map_t *m, int x1, int y1, int x2, int y2, tile_t *w );
int closest_room( room_t **r, int nrooms, int n, float loop_chance );
int all_rooms_linked( room_t **r, int nrooms );
int make_grid_map(	map_t *m, int cell_width, int cell_height,
					float room_chance, float node_chance,
					float loop_chance );
void make_drunken_walk_cave( map_t *m, int n );
int count_neighbours( map_t *m, int x, int y, tile_t *w );
int count_neighbours_q( map_t *m, int x, int y, tile_t *w );
void post_process_map( map_t *m );
void link_dungeon_levels( void );
int count_neighbours_sparse( map_t *m, int x, int y, tile_t *w );
void dig_room_with_doors( map_t *m, int x, int y, int w, int h );
int make_dla_dungeon( map_t *m );
void cavernize( map_t *m, float chance );

/* action.c */
int move_relative( entity_t *e, int dx, int dy );
int open_door( entity_t *e, int x, int y );
int close_door( entity_t *e, int x, int y );
int follow_stairs( entity_t *e );
int pick_up_item( entity_t *e, item_t *i, int quantity );
int drop_item( entity_t *e, item_t *i, int quantity );
int put_down_weapon( entity_t *e );
int take_off_armor( entity_t *e );
int wield_item( entity_t *e, item_t *i );
int wear_item( entity_t *e, item_t *i );
int reload_weapon( entity_t *e );
int unload_weapon( entity_t *e );

/* combat.c */
int melee_attack( entity_t *atk, entity_t *def );
int ranged_attack( entity_t *atk, entity_t *def );
int get_item_ac( item_t *i );
int count_ammo( list_t *li, item_t *a );

/* link.c */
link_t *alloc_link( void );
void free_link( link_t *l );
void free_links( void );
link_t *link_find_by_position( int x, int y, int z );

#endif

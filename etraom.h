
#include <stdio.h>
#include <stdlib.h>

#ifndef _ETRAOM_H_
#define _ETRAOM_H_

#define ETRAOM_VERSION "0.01"
#define MAX_STRING_LENGTH 256
#define MAX_LEVELS 10

#define TILEFLAG_SOLID		(1<<0)
#define TILEFLAG_OPAQUE		(1<<1)

typedef struct
{
	char* data;
	int length;
} buf_t;

struct _list_element
{
	void *data;
	struct _list_element *next;
	int allocated;
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
	unsigned char fg[3], bg[3];
	int flags;
} tile_t;

extern tile_t 
	tile_void,
	tile_floor,
	tile_wall;

typedef struct
{
	char *name;
	int width, height;

	tile_t ***terrain;

	int flags;
} map_t;

extern FILE *logfile;

extern int running;

extern unsigned int nlevels;
extern map_t **dungeon;
extern int main_seed;

extern list_t *message_list;

/* message.c */
void init_message_list( void );
void push_message( buf_t *b );
void free_message( void *m );
void free_message_list( void );
void test_messages( void );

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

/* game.c */
void new_game( unsigned int seed );
int init_game( int argc, char** argv );
int terminate_game( void );
int game_loop( void );
void handle_key( int key, int mod );

/* ui.c */
int init_ui( void );
int terminate_ui( void );
int draw_main_screen( void );

/* log.c */
int open_logfile( void );
int close_logfile( void );
void log_add( char *format, ... );

/* map.c */
map_t *alloc_map( char *name, int width, int height );
void free_map( map_t *m );
void clear_map( map_t *m );
void debug_dump_map( map_t *m );

/* mapgen.c */
int make_dummy_map( map_t *m, int nwalls );

#endif

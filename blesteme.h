
/*
	blesteme.h
	blesteme - curses-like library intended for use in roguelikes

	This tiny library is built in order to simplify the work one has to do when
	working with SDL and SDL_ttf. In other words, it's just a roguelike-
	specific wrapper. Due to the size of the library, it's intended to be
	statically-linked. So, if you'll need it in your project, you only need to
	mention 'blesteme.c' among the input files and link against SDL and SDL_ttf.

	This project is public domain, and has no warranty. Use it at your own risk
	and feel free to contribute if you feel like it.
*/

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#ifndef _BLESTEME_H_
#define _BLESTEME_H_

#define BL_MAX_STRING_LENGTH 1024

typedef struct
{
	int key;
	int mod;
	int mouse_x;
	int mouse_y;
	int mouse_btn;
	int quit;
} bl_input_t;

extern int bl_initialized;

char *bl_fontFile;
int bl_fontSize;
int bl_cols;
int bl_rows;
char *bl_windowTitle;
int bl_deltax;

int bl_charHeight;
int bl_charWidth;

unsigned char bl_fg[3], bl_bg[3];

SDL_Surface *bl_screen;
TTF_Font *bl_font;

/*	initialization procedure for blesteme;
	arguments are self-explanatory
*/
int bl_initialize( char* fontFile, int fontSize, int cols, int rows, char *windowTitle, int deltax );

/*	termination procedure for blesteme;
	must be called
*/
int bl_terminate( void );

/*	writes a string to the screen at position (x,y);
	the function should expect a UTF8-encoded string (currently broken)
*/
int bl_printf( int x, int y, char* format, ... );

/*	writes a character to the screen at position (x,y );
	accepts UTF8-encoded characters
*/
int bl_addch( int x, int y, int c );

/*	reads keyboard and mouse input;
	allows for keys to be keep-pressed;
	returns modifiers - alt, ctrl, shift;
	if there is no key pressed, then 'key' is -1;
	it also returns whether the user sent a quit signal ('quit'=1)
	on no input, it returns -1
*/
int bl_input( bl_input_t *i );

/*	clear a bl_input_t structure */
int bl_clear_input( bl_input_t *i );

/*	functions for setting the background and foreground colors;
	arguments are in 0 ... 255 range
*/
int bl_background( unsigned char r, unsigned char g, unsigned char b );
int bl_foreground( unsigned char r, unsigned char g, unsigned char b );

/*	clears the screen, filling it with the current background color */
int bl_clear( void );

/*	updates the screen */
int bl_refresh( void );

/*	delay; time is expressed in milliseconds */
int bl_delay( int ms );

#endif

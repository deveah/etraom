
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "blesteme.h"

int bl_initialized = 0;

int bl_initialize(	char* fontFile,
					int fontSize,
					int cols,
					int rows,
					char* windowTitle,
					int deltax )
{
	/*	save parameters for later use */
	bl_fontFile = fontFile;
	bl_fontSize = fontSize;
	bl_cols = cols;
	bl_rows = rows;
	bl_windowTitle = windowTitle;
	bl_deltax = deltax;

	/*	initialize SDL and SDL_ttf */
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		return -1;
	
	if( TTF_Init() < 0 )
		return -1;

	/*	load the font */
	bl_font = TTF_OpenFont( fontFile, fontSize );
	if( bl_font == NULL )
		return -1;

	/*	set the font hinting; this is just a matter of preference */
	TTF_SetFontHinting( bl_font, TTF_HINTING_NONE );

	/*	get glyph metrics;
		assuming that a monospace font is loaded, get the metrics of an
		arbitrary character */
	TTF_SizeText( bl_font, "#", &bl_charWidth, &bl_charHeight );

	/*	set the window caption */
	SDL_WM_SetCaption( windowTitle, NULL );

	/*	initialize main screen */
	bl_screen = SDL_SetVideoMode( bl_cols*bl_charWidth + ( bl_deltax*bl_cols ),
		bl_rows*bl_charHeight, 32, SDL_DOUBLEBUF|SDL_HWSURFACE );

	bl_initialized = 1;

	return 0;
}

int bl_terminate( void )
{
	if( !bl_initialized )
		return -1;
	
	TTF_CloseFont( bl_font );
	TTF_Quit();
	SDL_FreeSurface( bl_screen );
	SDL_Quit();

	return 0;
}

int bl_printf( int x, int y, char* format, ... )
{
	unsigned int i;
	char buf[BL_MAX_STRING_LENGTH];
	va_list args;

	if( format == NULL )
		return -1;

	va_start( args, format );
	vsnprintf( buf, BL_MAX_STRING_LENGTH, format, args );

	/* TODO breaks UTF8-compliance */
	for( i = 0; i < strlen( buf ); i++ )
	{
		bl_addch( x+i, y, buf[i] );
	}

	va_end( args );
	return 0;
}

int bl_addch( int x, int y, int c )
{
	SDL_Color fg, bg;
	SDL_Surface *renderedText;
	SDL_Rect destRect;

	if( !bl_initialized )
		return -1;

	fg.r = bl_fg[0]; fg.g = bl_fg[1]; fg.b = bl_fg[2];
	bg.r = bl_bg[0]; bg.g = bl_bg[1]; bg.b = bl_bg[2];

	char s[5];
	s[0] = ( c & 0xFF000000 ) >> 24;
	s[1] = ( c & 0x00FF0000 ) >> 16;
	s[2] = ( c & 0x0000FF00 ) >>  8;
	s[3] = ( c & 0x000000FF );
	s[4] = 0;

	unsigned int sp = 0;
	if( s[0] == 0 ) sp++;
	if( s[1] == 0 ) sp++;
	if( s[2] == 0 ) sp++;

	renderedText = TTF_RenderUTF8_Blended( bl_font, s+sp, fg );
	
	destRect.x = x * bl_charWidth + ( bl_deltax * x );
	destRect.y = y * bl_charHeight;
	destRect.w = bl_charWidth;
	destRect.h = bl_charHeight;

	SDL_FillRect( bl_screen, &destRect, SDL_MapRGB( bl_screen->format,
		bg.r, bg.g, bg.b ) );
	SDL_BlitSurface( renderedText, NULL, bl_screen, &destRect );

	SDL_FreeSurface( renderedText );

	return 0;
}

int bl_refresh( void )
{
	if( !bl_initialized )
		return -1;

	SDL_Flip( bl_screen );

	return 0;
}

int bl_delay( int ms )
{
	if( !bl_initialized )
		return -1;

	SDL_Delay( ms );

	return 0;
}

int bl_background( unsigned char r, unsigned char g, unsigned char b )
{
	if( !bl_initialized )
		return -1;

	bl_bg[0] = r;
	bl_bg[1] = g;
	bl_bg[2] = b;

	return 0;
}

int bl_foreground( unsigned char r, unsigned char g, unsigned char b )
{
	if( !bl_initialized )
		return -1;

	bl_fg[0] = r;
	bl_fg[1] = g;
	bl_fg[2] = b;

	return 0;
}

int bl_clear( void )
{
	int color;

	if( !bl_initialized )
		return -1;

	color = SDL_MapRGB( bl_screen->format, bl_bg[0], bl_bg[1], bl_bg[2] );

	SDL_FillRect( bl_screen, NULL, color );

	return 0;
}

int bl_input( bl_input_t *i )
{
	SDL_Event e;

	if( !bl_initialized )
		return -1;

	/* this is quite important */
	i->quit = 0;

	/* this is independent of the type of event */
	i->mod = SDL_GetModState();

	if( SDL_PollEvent( &e ) )
	{
		switch( e.type )
		{
		case SDL_KEYDOWN:
			i->key = e.key.keysym.sym;
			break;
		case SDL_KEYUP:
			i->key = -1;
			i->mod = 0;
			break;
		case SDL_MOUSEMOTION:
			i->mouse_x = e.motion.x / ( bl_charWidth + bl_deltax );
			i->mouse_y = e.motion.y / bl_charHeight;
			break;
		case SDL_MOUSEBUTTONDOWN:
			i->mouse_x = e.button.x / ( bl_charWidth + bl_deltax );
			i->mouse_y = e.button.y / bl_charHeight;
			i->mouse_btn = e.button.button;
			break;
		case SDL_MOUSEBUTTONUP:
			i->mouse_x = e.button.x;
			i->mouse_y = e.button.y;
			i->mouse_btn = -1;
			break;
		case SDL_QUIT:
			i->quit = 1;
			break;
		default:
			return -1;
		}
	}
	else
		/*	no input */
		return -1;

	return 0;
}

int bl_clear_input( bl_input_t *i )
{
	i->key = -1;
	i->mod = 0;
	i->mouse_x = -1;
	i->mouse_y = -1;
	i->mouse_btn = 0;
	i->quit = 0;

	return 0;
}

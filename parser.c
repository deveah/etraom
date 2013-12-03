
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "etraom.h"

list_t *entity_type_list = NULL;

/*	color code parser (assures portability)
	color can be:
		1 = black
		2 = red
		3 = green
		4 = yellow
		5 = blue
		6 = magenta
		7 = cyan
		8 = white

	adding 10 to these values makes them bold
*/
int parse_color( int c )
{
	int r = 0;
	
	if( c >= 10 )
	{
		c -= 10;
		r |= A_BOLD;
	}

	switch( c )
	{
	case 1:
		r |= COLOR_PAIR( C_BLACK );
		break;
	case 2:
		r |= COLOR_PAIR( C_RED );
		break;
	case 3:
		r |= COLOR_PAIR( C_GREEN );
		break;
	case 4:
		r |= COLOR_PAIR( C_YELLOW );
		break;
	case 5:
		r |= COLOR_PAIR( C_BLUE );
		break;
	case 6:
		r |= COLOR_PAIR( C_MAGENTA );
		break;
	case 7:
		r |= COLOR_PAIR( C_CYAN );
		break;
	case 8:
		r |= COLOR_PAIR( C_WHITE );
		break;
	default:
		/* invalid color */
		return 0;
	}

	return r;
}

/*	entity file parser

	entry format is: <name> <face> <color> <hp> <agility>
*/
int parse_entities( char *fn )
{
	char *name = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char face;
	int color;
	int hp;
	int agility;
	int r;
	int nentities = 0;
	int i, l;

	entity_t *e = NULL;

	log_add( "[parse_entities] Parsing entities...\n" );
	fflush( logfile );

	FILE *f = fopen( fn, "r" );
	
	if( !f )
		return -1;

	entity_type_list = alloc_list();

	do
	{

		r = fscanf( f, "%s %c %i %i %i",
			name,
			&face,
			&color,
			&hp,
			&agility
		);

		if( feof( f ) )
		{
			free( name );
			return nentities;
		}

		if( r < 5 )
		{
			free( name );
			return -1;
		}
		
		/* strip spaces only for the first column (name) */
		l = strlen( name );
		for( i = 0; i < l; i++ )
		{
			if( name[i] == '+' )
			{
				name[i] = ' ';
				continue;
			}

			if( name[i] == ' ' )
				break;
		}

		log_add( "[parse_entities] Parsed %s(%c):\n\tHP:  %i\n\tAgi: %i\n",
			name,
			face,
			hp,
			agility
		);

		/* finally make the entity */
		buf_t *bname = bufnew( name );
		e = alloc_entity( bname );
		bufdestroy( bname );

		e->hp = hp;
		e->max_hp = hp;
		e->agility = agility;
		e->face = (int)face;
		e->color = parse_color( color );

		list_add_tail( entity_type_list, (void*)e );

		nentities++;
	}
	while( !feof( f ) );

	nentities--;

	log_add( "[parse_entities] Done parsing entities.\n" );

	free( name );
	return nentities;
}

void free_entity_types( void )
{
	list_element *el = entity_type_list->head;

	while( el )
	{
		entity_t *e = (entity_t*)el->data;
		free_entity( e );
		el = el->next;
	}

	free_list( entity_type_list );
}


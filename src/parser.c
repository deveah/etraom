
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "etraom.h"

list_t *entity_type_list = NULL;
list_t *ammo_type_list = NULL;

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
int parse_entity_types( char *fn )
{
	char *name = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char face;
	int color;
	int hp;
	int agility;
	int r;
	int nentitytypes = 0;
	int i, l;

	entity_t *e = NULL;

	log_add( "[parse_entities] Parsing entities...\n" );

	FILE *f = fopen( fn, "r" );
	
	if( !f )
	{
		free( name );
		return -1;
	}

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
			return nentitytypes;
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

		nentitytypes++;
	}
	while( !feof( f ) );

	nentitytypes--;

	free( name );
	return nentitytypes;
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

int parse_ammo_types( char *fn )
{
	char *name = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char face;
	int color;
	int r, l, j;
	int nammotypes = 0;

	item_t *i = NULL;

	log_add( "[parse_ammo_types] Parsing ammo types...\n" );

	FILE *f = fopen( fn, "r" );
	if( !f )
	{
		free( name );
		return -1;
	}

	ammo_type_list = alloc_list();

	do
	{
		r = fscanf( f, "%s %c %i",
			name,
			&face,
			&color
		);

		if( feof( f ) )
		{
			free( name );
			return nammotypes;
		}

		if( r < 3 )
		{
			free( name );
			return -1;
		}
		
		l = strlen( name );
		for( j = 0; j < l; j++ )
		{
			if( name[j] == '+' )
			{
				name[j] = ' ';
				continue;
			}

			if( name[j] == ' ' )
				break;
		}

		log_add( "[parse_ammo_types] Parsed %s(%c): color %i\n",
			name,
			face,
			color
		);

		buf_t *aname = bufnew( name );
		i = alloc_item( aname );
		bufdestroy( aname );

		i->face = face;
		i->color = parse_color( color );
		i->type = ITEMTYPE_AMMO;
		i->specific = NULL;
		i->quantity = 0;
		i->quality = 1.0f;
		i->flags = ITEMFLAG_PICKABLE | ITEMFLAG_STACKABLE;

		list_add_tail( ammo_type_list, (void*)i );

		nammotypes++;
	}
	while( !feof( f ) );

	nammotypes--;
	
	free( name );
	return nammotypes;
}

void free_ammo_types( void )
{
	list_element *el = ammo_type_list->head;

	while( el )
	{
		item_t *i = (item_t*)el->data;

		free_item( i );

		el = el->next;
	}

	free_list( ammo_type_list );
}


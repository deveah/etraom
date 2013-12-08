
#include "etraom.h"

ai_t *alloc_ai( int type )
{
	ai_t *a = malloc( sizeof(ai_t) );

	a->type = type;
	a->lx = 0;
	a->ly = 0;
	a->lz = 0;
	a->flags = 0;

	return a;
}

void free_ai( ai_t *a )
{
	if( a )
	{
		free( a );
	}
}

int run_ai( entity_t *e )
{
	/* no ai means it doesn't do nothing */
	if( !e->ai )
	{
		return 1;
	}

	/* update player coordinates */
	if( e->lightmap[player->z][player->x][player->y] > 0.0f )
	{
		e->ai->lx = player->x;
		e->ai->ly = player->y;
		e->ai->lz = player->z;
		/* seeing an enemy triggers an alert */
		e->ai->flags |= AIFLAG_ALERT;
	}

	switch( e->ai->type )
	{
	case AITYPE_DUMB:
		return dumb_ai( e );
	case AITYPE_BASICMELEE:
		return basic_melee_ai( e );
	default:
		return 0;
	}
}

int dumb_ai( entity_t *e )
{
	int rx, ry;
	int tries = 0;

	while( 1 )
	{
		tries++;
		/* skip a turn if there's no viable output */
		if( tries > 100 )
			return 1;

		rx = rand()%3 - 1;
		ry = rand()%3 - 1;
	
		if( ( rx*rx + ry*ry > 0 ) &&
			( is_legal( e->x+rx, e->y+ry ) ) &&
			!( dungeon[e->z]->terrain[e->x+rx][e->y+ry]->flags & TILEFLAG_SOLID ) &&
			( !entity_find_by_position( e->x+rx, e->y+ry, e->z ) ) )
			break;
	}

	return move_relative( e, rx, ry );
}

int basic_melee_ai( entity_t *e )
{
	int dx, dy;

	/* do nothing if not alert */
	if( !( e->ai->flags & AIFLAG_ALERT ) )
	{
		return 1;
	}

	if( distance( e->x, e->y, player->x, player->y ) == 1 )
	{
		melee_attack( e, player );
		return 1;
	}
	else
	{
		/* tries to go to the last place it's seen an enemy */
		dx = ( e->x > e->ai->lx ) ? -1 : 1;
		dy = ( e->y > e->ai->ly ) ? -1 : 1;

		if( is_legal( e->x+dx, e->y ) &&
			!( dungeon[e->z]->terrain[e->x+dx][e->y]->flags & TILEFLAG_SOLID ) &&
			!( entity_find_by_position( e->x+dx, e->y, e->z ) ) )
		{
			e->x += dx;
		}
		
		if( is_legal( e->x, e->y+dy ) &&
			!( dungeon[e->z]->terrain[e->x][e->y+dy]->flags & TILEFLAG_SOLID ) &&
			!( entity_find_by_position( e->x, e->y+dy, e->z ) ) )
		{
			e->y += dy;
		}

		return 1;
	}
}


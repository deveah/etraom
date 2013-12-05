
#include <unistd.h>
#include <curses.h>

#include "etraom.h"

int melee_attack( entity_t *atk, entity_t *def )
{
	int damage = 0;

	log_add( "[melee_attack] Entity 0x%08x attacks entity 0x%08x.\n", atk, def );
	
	if( atk->in_hand )
	{
		if( atk->in_hand->type == ITEMTYPE_WEAPON )
		{
			weapon_t *w = (weapon_t*)atk->in_hand->specific;
			if( w->type == WEAPONTYPE_MELEE )
			{
				int r = rand() % ( w->max_damage - w->min_damage ) + w->min_damage;
				damage = r;
			}
			else
			{
				/* TODO: ranged weapons that also do melee damage? */
				damage = 0;
			}
		}
		else
		{
			/* TODO: special cases? */
			damage = 0;
		}
	}
	else
	{
		/* TODO: no weapon -> unarmed damage? */
		damage = 0;
	}

	if( atk == player )
	{
		if( damage == 0 )
		{
			buf_t *msg = bufnew( "You hit the " );
			bufcat( msg, def->name );
			bufcats( msg, " for no damage!" );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			buf_t *msg = bufnew( "You hit the " );
			bufcat( msg, def->name );
			bufcats( msg, "!" );
			push_message( msg );
			bufdestroy( msg );
		}
	}

	def->hp -= damage;

	log_add( "[melee_attack] Attacked entity now has %i hp.\n", def->hp );

	if( def->hp <= 0 )
		entity_die( def );

	return 1;
}

/* TODO this function needs some logic redesign */
int ranged_attack( entity_t *atk, entity_t *def )
{
	int x1 = atk->x, y1 = atk->y;
	int x2 = def->x, y2 = def->y;
	int cx = x1, cy = y1;
	int dx, dy, sx, sy;
	float err, e2;

	int hit = 0;
	int damage;

	if( atk->in_hand )
	{
		if( atk->in_hand->type == ITEMTYPE_WEAPON )
		{
			weapon_t *w = (weapon_t*)atk->in_hand->specific;
			
			/* should include all non-melee weapon types */
			if( w->type == WEAPONTYPE_HANDGUN )
			{
				if( w->ammo_loaded >= 1 )
				{
					/* TODO: different ammo consumption? */
					w->ammo_loaded -= 1;
				}
				else
				{
					buf_t *msg = bufnew( "The clip is empty!" );
					push_message( msg );
					bufdestroy( msg );
					return 0;
				}

				if( rand()%100 > (int)(w->accuracy*100.0f) )
				{
					x2 += rand()%3 - 1;
					y2 += rand()%3 - 1;
				}

				dx = abs( x2-x1 );
				dy = abs( y2-y1 );

				if( x1 < x2 ) sx = 1; else sx = -1;
				if( y1 < y2 ) sy = 1; else sy = -1;
				err = dx-dy;

				while( 1 )
				{
					entity_t *e = entity_find_by_position( cx, cy, atk->z );
					/* you don't want to shoot yourself. */
					if( ( cx != x1 ) && ( cy != y1 ) && e )
					{
						/* target reached, deal damage */
						damage = rand() % ( w->max_damage - w->min_damage ) + w->min_damage;
						hit = 1;
						break;
					}

					if( !is_legal( cx, cy ) )
					{
						hit = 0;
						break;
					}

					if( dungeon[atk->z]->terrain[cx][cy]->flags & TILEFLAG_SOLID )
					{
						hit = 0;
						break;
					}

					e2 = 2 * err;
					if( e2 > -dy )
					{
						err -= dy;
						cx += sx;
					}
					if( e2 < dx )
					{
						err += dx;
						cy += sy;
					}

					/* TODO: differently coloured bullets? */
					attrset( COLOR_PAIR( C_RED ) | A_BOLD );
					mvaddch( cy+2, cx, '*' );
				}
			}
			else
			{
				damage = -1;
			}
		}
		else
		{
			/* TODO: special cases? */
			damage = -1;
		}
	}
	else
	{
		damage = 0;
	}

	if( atk == player )
	{
		if( !hit )
		{
			buf_t *msg = bufnew( "You miss the " );
			bufcat( msg, def->name );
			bufcats( msg, "." );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			if( damage == -1 )
			{
				buf_t *msg = bufnew( "You don't know how to fire that." );
				push_message( msg );
				bufdestroy( msg );
				return 0;
			}
			else if( damage == 0 )
			{
				buf_t *msg = bufnew( "You shoot the " );
				bufcat( msg, def->name );
				bufcats( msg, " for no damage!" );
				push_message( msg );
				bufdestroy( msg );
			}
			else
			{
				buf_t *msg = bufnew( "You shoot the " );
				bufcat( msg, def->name );
				bufcats( msg, "!" );
				push_message( msg );
				bufdestroy( msg );
			}
		}
	}

	if( damage > 0 )
		def->hp -= damage;

	if( def->hp <= 0 )
		entity_die( def );

	refresh();
	usleep( 100000 );

	return 1;
}

int get_item_ac( item_t *i )
{
	if( i )
	{
		if( i->type == ITEMTYPE_ARMOR )
		{
			armor_t *a = (armor_t*)i->specific;
			return a->ac;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		/* TODO: no armor -> 0 ac? natural resist? */
		return 0;
	}
}

int count_ammo( list_t *li, item_t *a )
{
	list_element *el = li->head;
	int n = 0;

	while( el )
	{
		item_t *i = (item_t*)el->data;

		if( items_alike( a, i ) )
		{
			n += i->quantity;
		}

		el = el->next;
	}

	return n;
}


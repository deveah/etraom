
#include <unistd.h>
#include <curses.h>

#include "etraom.h"

int melee_attack( entity_t *atk, entity_t *def )
{
	if( !atk->in_hand )
	{
		if( atk == player )
		{
			buf_t *msg = bufprintf( "You %s the %s.", atk->natural->attack_name, def->name->data );
			push_message( msg );
			bufdestroy( msg );
		}
		else if( def == player )
		{
			buf_t *msg = bufprintf( "The %s %ss you.", atk->name->data, atk->natural->attack_name );
			push_message( msg );
			bufdestroy( msg );
		}

		take_damage( def, atk->natural );

		return 1;
	}

	if( atk->in_hand->type != ITEMTYPE_WEAPON )
	{
		if( atk == player )
		{
			buf_t *msg = bufnew( "You're not wielding a weapon." );
			push_message( msg );
			bufdestroy( msg );
		}
		return 0;
	}

	weapon_t *wpn = (weapon_t*)atk->in_hand->specific;

	if( wpn->type != WEAPONTYPE_MELEE )
	{
		/* TODO: ranged weapons that also have melee attacks? */
		if( atk == player )
		{
			buf_t *msg = bufnew( "You're not wielding a melee weapon." );
			push_message( msg );
			bufdestroy( msg );
		}
		return 0;
	}

	if( atk == player )
	{
		buf_t *msg = bufprintf( "You hit the %s!", def->name->data );
		push_message( msg );
		bufdestroy( msg );
	}
	else if( def == player )
	{
		buf_t *msg = bufprintf( "The %s hits you!", atk->name->data );
		push_message( msg );
		bufdestroy( msg );
	}

	take_damage( def, wpn );

	return 1;
}

/* TODO: accuracy */
int ranged_attack( entity_t *atk, int tx, int ty )
{
	weapon_t *wpn;
	int cx = atk->x, cy = atk->y;

	if( !atk->in_hand )
	{
		if( atk == player )
		{
			buf_t *msg = bufnew( "You're not wielding anything!" );
			push_message( msg );
			bufdestroy( msg );
		}
		return 0;
	}

	if( atk->in_hand->type != ITEMTYPE_WEAPON )
	{
		if( atk == player )
		{
			buf_t *msg = bufnew( "You're not wielding a weapon!" );
			push_message( msg );
			bufdestroy( msg );
		}
		return 0;
	}

	wpn = (weapon_t*)atk->in_hand->specific;

	if( wpn->type != WEAPONTYPE_HANDGUN )
	{
		if( atk == player )
		{
			buf_t *msg = bufnew( "You're not wielding a ranged weapon!" );
			push_message( msg );
			bufdestroy( msg );
		}
		return 0;
	}

	/* consume ammo */
	if( wpn->ammo_loaded >= 1 )
	{
		wpn->ammo_loaded -= 1;
	}
	else
	{
		buf_t *msg = bufnew( "The clip is empty!" );
		push_message( msg );
		bufdestroy( msg );
		return 0;
	}

	if( atk == player )
	{
		buf_t *msg = bufnew( "You shoot your weapon!" );
		push_message( msg );
		bufdestroy( msg );
	}

	while( 1 )
	{
		cx += tx;
		cy += ty;
		
		if( is_legal( cx, cy ) &&
			( dungeon[atk->z]->terrain[cx][cy]->flags &= TILEFLAG_SOLID ) )
		{
			/* you hit something solid */
			break;
		}
	
		attrset( COLOR_PAIR( C_RED ) );
		mvaddch( cy+2, cx, '*' );

		entity_t *e = entity_find_by_position( cx, cy, atk->z );
		if( e )
		{
			buf_t *msg = bufprintf( "You hit the %s!", e->name->data );
			push_message( msg );
			bufdestroy( msg );

			take_damage( e, wpn );

			break;
		}
	}

	refresh();
	msleep( 100 );

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

void take_damage( entity_t *e, weapon_t *w )
{
	int damage = rand() % ( w->max_damage - w->min_damage ) + w->min_damage;

	e->hp -= damage;

	if( e->hp <= 0 )
		entity_die( e );
}


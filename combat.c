
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

int ranged_attack( entity_t *atk, entity_t *def )
{
	int damage;

	/* TODO: shouldn't be 100% hit chance */

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
				int r = rand() % ( w->max_damage - w->min_damage ) + w->min_damage;
				damage = r;
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

	if( damage > 0 )
		def->hp -= damage;

	if( def->hp <= 0 )
		entity_die( def );

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


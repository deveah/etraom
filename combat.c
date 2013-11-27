
#include "etraom.h"

int melee_attack( entity_t *atk, entity_t *def )
{
	int damage;

	log_add( "[melee_attack] Entity 0x%08x attacks entity 0x%08x.\n", atk, def );
	
	/* TODO: make this dynamic */
	damage = get_item_damage( atk->in_hand );

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

	def->hp -= damage;

	log_add( "[melee_attack] Attacked entity now has %i hp.\n", def->hp );

	if( def->hp <= 0 )
		entity_die( def );

	return 1;
}

int get_item_damage( item_t *i )
{
	if( i )
	{
		if( i->type == ITEMTYPE_WEAPON )
		{
			/* TODO: different formulae for different weapon types */
			weapon_t *w = (weapon_t*)i->specific;
			int r = rand() % ( w->max_damage - w->min_damage ) + w->min_damage;
			return r;
		}
		else
		{
			/* TODO: special cases? */
			return 0;
		}
	}
	else
	{
		/* TODO: no weapon -> unarmed? */
		return 0;
	}
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


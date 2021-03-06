
#include <string.h>

#include "etraom.h"

/* entity_move_rel should return 1 if the action is successful, otherwise 0 */
int move_relative( entity_t *e, int dx, int dy )
{
	if( is_legal( e->x+dx, e->y+dy ) )
	{
		if( dungeon[e->z]->terrain[e->x+dx][e->y+dy]->flags & TILEFLAG_SOLID )
		{
			if( e == player )
			{
				buf_t *msg = bufnew( "Yep, it's solid alright." );
				push_message( msg );
				bufdestroy( msg );
			}

			return 0;
		}
		else
		{
			entity_t *ee = entity_find_by_position( e->x+dx, e->y+dy, e->z );
			if( ee )
			{
				melee_attack( e, ee );

				return 1;
			}
			else
			{
				log_add( "[entity_move_rel] Entity 0x%08x('%s') has moved to "
					"(%i, %i, %i)\n", e, e->name->data, e->x+dx, e->y+dy, e->z );
				e->x += dx;
				e->y += dy;

				return 1;
			}
		}
	}

	/* TODO: reachable? */
	return 0;
}

int open_door( entity_t *e, int x, int y )
{
	if( !is_legal( x, y ) )
		return 0;
	
	if( dungeon[e->z]->terrain[x][y] == &tile_door_closed )
	{
		dungeon[e->z]->terrain[x][y] = &tile_door_open;

		if( e == player )
		{
			buf_t *msg = bufnew( "You open the door." );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			/* different messages if the door being opened is seen or not */
			if( player->lightmap[e->z][e->x][e->y] > 0.0f )
			{
				buf_t *msg = bufnew( "A door opens." );
				push_message( msg );
				bufdestroy( msg );
			}
			else
			{
				buf_t *msg = bufnew( "You hear a door open." );
				push_message( msg );
				bufdestroy( msg );
			}
		}

		return 1;
	}
	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There's no closed door there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

int close_door( entity_t *e, int x, int y )
{
	if( !is_legal( x, y ) )
		return 0;
	
	if( dungeon[e->z]->terrain[x][y] == &tile_door_open )
	{
		dungeon[e->z]->terrain[x][y] = &tile_door_closed;

		if( e == player )
		{
			buf_t *msg = bufnew( "You close the door." );
			push_message( msg );
			bufdestroy( msg );
		}
		else
		{
			/* different messages if the door being opened is seen or not */
			if( player->lightmap[e->z][e->x][e->y] > 0.0f )
			{
				buf_t *msg = bufnew( "A door closes." );
				push_message( msg );
				bufdestroy( msg );
			}
			else
			{
				buf_t *msg = bufnew( "You hear a door close." );
				push_message( msg );
				bufdestroy( msg );
			}
		}

		return 1;
	}
	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There's no open door there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

int follow_stairs( entity_t *e )
{
	link_t *l = link_find_by_position( e->x, e->y, e->z );

	if( l )
	{
		player->x = l->dest_x;
		player->y = l->dest_y;
		player->z = l->dest_z;
		l->flags &= LINKFLAG_USED;

		buf_t *b;
		if( l->face == '>' )
		{
			b = bufnew( "You descend." );
		}
		else if( l->face == '<' )
		{
			b = bufnew( "You ascend." );
		}
		else
		{
			b = bufnew( "Bug." );
		}

		push_message( b );
		bufdestroy( b );

		return 0;
	}
	else
	{
		buf_t *b = bufnew( "There are no stairs here." );
		push_message( b );
		bufdestroy( b );
		return 0;
	}
}

int pick_up_item( entity_t *e, item_t *i, int quantity )
{
	if( !( i->flags & ITEMFLAG_PICKABLE ) )
	{
		buf_t *b = bufnew( "You can't pick that up." );
		push_message( b );
		bufdestroy( b );
		return 0;
	}

	buf_t *msg = bufprintf( "You pick up %s.", i->name->data );
	push_message( msg );
	bufdestroy( msg );

	if( i->quantity == quantity )
	{
		inventory_add_item( e, i );
		
		int j = list_find( item_list, (void*)i );
		list_remove_index( item_list, j );
	}
	else
	{
		item_t *ii = alloc_item( i->name );
		ii->face = i->face;
		ii->color = i->color;
		ii->quantity = quantity;
		ii->quality = i->quality;
		ii->type = i->type;
		ii->place = i->place;
		ii->x = i->x;
		ii->y = i->y;
		ii->z = i->z;
		ii->flags = i->flags;

		/* TODO: weapons aren't stackable, so this shouldn't happen */
		if( i->type == ITEMTYPE_WEAPON )
		{
			ii->specific = malloc( sizeof(weapon_t) );
			memcpy( ii->specific, i->specific, sizeof(weapon_t) );
			
			weapon_t *w = (weapon_t*)ii->specific;
			w->ammo_type = clone_item( i->specific );
		}
		if( i->type == ITEMTYPE_ARMOR )
		{
			ii->specific = malloc( sizeof(armor_t) );
			memcpy( ii->specific, i->specific, sizeof(armor_t) );
		}
		/* TODO handle other item types */

		i->quantity -= quantity;

		inventory_add_item( e, ii );
	}
	
	return 1;
}

int drop_item( entity_t *e, item_t *i, int quantity )
{
	list_t *li = item_find_by_position( e->x, e->y, e->z );
	list_element *el = NULL;
	item_t *found = NULL;

	if( li )
		el = li->head;

	if( i->flags & ITEMFLAG_STACKABLE )
	{
		while( el )
		{
			item_t *ij = (item_t*)el->data;
		
			if( items_alike( ij, i ) )
			{
				found = ij;
				break;
			}

			el = el->next;
		}
	}

	if( i->quantity == quantity )
	{
		int j = list_find( e->inventory, (void*)i );
		list_remove_index( e->inventory, j );

		if( found )
		{
			found->quantity += quantity;
			free_item( i );
		}
		else
		{
			list_add_head( item_list, i );

			i->x = e->x;
			i->y = e->y;
			i->z = e->z;
			i->place = ITEMPLACE_DUNGEON;
		}
	}
	else
	{
		if( found )
		{
			found->quantity += quantity;
		}
		else
		{
			item_t *ii = alloc_item( i->name );
			ii->face = i->face;
			ii->color = i->color;
			ii->quantity = quantity;
			ii->quality = i->quality;
			ii->type = i->type;
			ii->x = e->x;
			ii->y = e->y;
			ii->z = e->z;
			ii->place = ITEMPLACE_DUNGEON;
			ii->flags = i->flags;

			if( i->type == ITEMTYPE_WEAPON )
			{
				ii->specific = malloc( sizeof(weapon_t) );
				memcpy( ii->specific, i->specific, sizeof(weapon_t) );

				weapon_t *w = (weapon_t*)ii->specific;
				w->ammo_type = clone_item( i->specific );
			}
			if( i->type == ITEMTYPE_ARMOR )
			{
				ii->specific = malloc( sizeof(armor_t) );
				memcpy( ii->specific, i->specific, sizeof(armor_t) );
			}
			/* TODO handle other item types */

			list_add_head( item_list, ii );
		}

		i->quantity -= quantity;
	}

	buf_t *msg = bufprintf( "You drop %s.", i->name->data );
	push_message( msg );
	bufdestroy( msg );

	return 1;
}

int put_down_weapon( entity_t *e )
{
	if( e->in_hand )
	{
		item_t *i = e->in_hand;
		e->in_hand = NULL;
		inventory_add_item( e, i );

		buf_t *msg = bufnew( "You put down your weapon." );
		push_message( msg );
		bufdestroy( msg );
		
		return 1;
	}
	else
	{
		buf_t *msg = bufnew( "You aren't wielding anything." );
		push_message( msg );
		bufdestroy( msg );
		
		return 0;
	}
}

int take_off_armor( entity_t *e )
{
	if( e->worn )
	{
		item_t *i = e->worn;
		e->worn = NULL;
		inventory_add_item( e, i );

		buf_t *msg = bufnew( "You take off your armor." );
		push_message( msg );
		bufdestroy( msg );
		
		return 1;
	}
	else
	{
		buf_t *msg = bufnew( "You aren't wearing anything." );
		push_message( msg );
		bufdestroy( msg );
		
		return 0;
	}
}

int wield_item( entity_t *e, item_t *i )
{
	int j;

	put_down_weapon( e );

	j = list_find( e->inventory, (void*)i );
	list_remove_index( e->inventory, j );
	
	e->in_hand = i;

	buf_t *msg = bufprintf( "You now wield %s.", i->name->data );
	push_message( msg );
	bufdestroy( msg );

	/* TODO: can this fail? */
	return 1;
}

int wear_item( entity_t *e, item_t *i )
{
	int j;

	take_off_armor( e );

	j = list_find( e->inventory, (void*)i );
	list_remove_index( e->inventory, j );

	e->worn = i;
	
	buf_t *msg = bufprintf( "You now wear %s.", i->name->data );
	push_message( msg );
	bufdestroy( msg );

	/* TODO: can this fail? */
	return 1;
}

int reload_weapon( entity_t *e )
{
	list_element *el = e->inventory->head;
	
	if( e->in_hand->type != ITEMTYPE_WEAPON )
	{
		buf_t *msg = bufnew( "You can't reload that." );
		push_message( msg );
		bufdestroy( msg );
		return 0;
	}

	weapon_t *w = (weapon_t*)e->in_hand->specific;

	/* weapons that consume no ammo have 0 clip size */
	if( w->clip_size == 0 )
	{
		buf_t *msg = bufnew( "This weapon doesn't require ammo." );
		push_message( msg );
		bufdestroy( msg );
		return 0;
	}

	while( el )
	{
		item_t *i = (item_t*)el->data;
		
		if( items_alike( i, w->ammo_type ) )
		{
			int q = w->clip_size - w->ammo_loaded;

			if( i->quantity > q )
			{
				int q = w->clip_size - w->ammo_loaded;
				i->quantity -= q;
				w->ammo_loaded += q;
			}
			else
			{
				w->ammo_loaded += i->quantity;

				int j = list_find( e->inventory, (void*)i );
				list_remove_index( e->inventory, j );
				free_item( i );
			}
			
			buf_t *msg = bufnew( "You reload your weapon." );
			push_message( msg );
			bufdestroy( msg );
			return 1;
		}

		el = el->next;
	}
	
	buf_t *msg = bufnew( "You're out of ammo!" );
	push_message( msg );
	bufdestroy( msg );

	return 0;
}

int unload_weapon( entity_t *e )
{	
	if( e->in_hand->type != ITEMTYPE_WEAPON )
	{
		buf_t *msg = bufnew( "You can't unload that." );
		push_message( msg );
		bufdestroy( msg );
		return 0;
	}

	weapon_t *w = (weapon_t*)e->in_hand->specific;

	/* weapons that consume no ammo have 0 clip size */
	if( ( w->clip_size == 0 ) || ( w->ammo_loaded == 0 ) )
	{
		buf_t *msg = bufnew( "This weapon doesn't have any ammo loaded." );
		push_message( msg );
		bufdestroy( msg );
		return 0;
	}

	item_t *i = clone_item( w->ammo_type );
	i->quantity = w->ammo_loaded;
	w->ammo_loaded = 0;

	inventory_add_item( e, i );

	buf_t *msg = bufnew( "You unload your weapon." );
	push_message( msg );
	bufdestroy( msg );

	return 1;
}


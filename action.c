
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
				buf_t *msg = bufnew( "You hit the " );
				bufcat( msg, ee->name );
				bufcats( msg, "!" );
				push_message( msg );
				bufdestroy( msg );

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
	tile_t *t = dungeon[e->z]->terrain[e->x][e->y];

	if( t == &tile_stairs_down )
	{
		e->z++;
		e->x = dungeon[e->z]->entrance_x;
		e->y = dungeon[e->z]->entrance_y;

		if( e == player )
		{
			buf_t *msg = bufnew( "You descend." );
			push_message( msg );
			bufdestroy( msg );
		}
		
		return 1;
	}

	else if( t == &tile_stairs_up )
	{
		e->z--;
		e->x = dungeon[e->z]->exit_x;
		e->y = dungeon[e->z]->exit_y;

		if( e == player )
		{
			buf_t *msg = bufnew( "You ascend." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 1;
	}

	else
	{
		if( e == player )
		{
			buf_t *msg = bufnew( "There are no stairs there." );
			push_message( msg );
			bufdestroy( msg );
		}

		return 0;
	}
}

int pick_up_item( entity_t *e, item_t *i, int quantity )
{
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
		/* TODO: should clone 'specific' ? */
		ii->specific = i->specific;
		ii->place = i->place;
		ii->x = i->x;
		ii->y = i->y;
		ii->z = i->z;
		ii->flags = i->flags;

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
			/* TODO: should clone 'specific' ? */
			ii->specific = i->specific;
			ii->x = e->x;
			ii->y = e->y;
			ii->z = e->z;
			ii->place = ITEMPLACE_DUNGEON;
			ii->flags = i->flags;

			list_add_head( item_list, ii );
		}

		i->quantity -= quantity;
	}

	return 1;
}

int wield_item( entity_t *e, item_t *i )
{
	int j;

	if( e->in_hand )
	{
		inventory_add_item( e, e->in_hand );	
	}

	j = list_find( e->inventory, (void*)i );
	list_remove_index( e->inventory, j );
	
	e->in_hand = i;

	/* TODO: can this fail? */
	return 1;
}


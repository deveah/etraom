
#include "etraom.h"

int melee_attack( entity_t *atk, entity_t *def )
{
	log_add( "[melee_attack] Entity 0x%08x attacks entity 0x%08x.\n", atk, def );
	
	/* TODO: make this dynamic */
	def->hp -= 1;
	
	log_add( "[melee_attack] Attacked entity now has %i hp.\n", def->hp );

	if( def->hp <= 0 )
		entity_die( def );

	return 1;
}


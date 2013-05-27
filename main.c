
#include "etraom.h"

int main( int argc, char **argv )
{
	init_game( argc, argv );
	game_loop();
	terminate_game();

	return 0;
}


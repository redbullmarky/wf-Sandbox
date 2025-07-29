#include "Squishies.h"
#include <iostream>
#include <memory>

//#define MEMDEBUG

#ifdef MEMDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

int main()
{
	bool result{ false };

#ifdef MEMDEBUG
	//_CrtSetBreakAlloc(5829);
#endif
	{
		std::unique_ptr<Squishies::Squishies> game = std::make_unique<Squishies::Squishies>();

		bool result = game->init();

		if (result) {
			game->run();
		}

		game->shutdown();
		game.reset();

		if (!result) {
			printf("Error starting game\n");
			system("PAUSE");
		}
	}

#ifdef MEMDEBUG
	_CrtDumpMemoryLeaks();
#endif
	return result ? 0 : 1;
}

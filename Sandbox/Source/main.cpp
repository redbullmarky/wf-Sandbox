#include "Sandbox.h"
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
		std::unique_ptr<Sandbox::Sandbox> game = std::make_unique<Sandbox::Sandbox>();

		bool result = game->init();

		if (result) {
			game->run();
		}

		game->shutdown();
		game.reset();
		system("PAUSE");
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

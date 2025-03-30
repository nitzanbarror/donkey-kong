#include "game.h"  
#include "gameFromInput.h"
#include "gameFromFile.h"

int main(int argc, char** argv) {

	bool isLoad = argc > 1 && std::string(argv[1]) == "-load";
	bool isSilent = isLoad && argc > 2 && std::string(argv[2]) == "-silent";
	bool isSave = argc > 1 && std::string(argv[1]) == "-save";
	
	Game* g;
	if (isLoad)
		g = new GameFromFile;
	else 
		g = new GameFromInput;
	g->setIsLoad(isLoad);
	g->setIsSilent(isSilent);
	g->setIsSave(isSave);
	g->run();

	delete g;
}
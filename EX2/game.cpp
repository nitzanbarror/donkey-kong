#include "game.h"

/*
Main game loop that handles the gameplay mechanics, including user input,
barrel movement, hero movement, and game end conditions.
*/
void Game::playGame() {

	for (int i = fileNumber; i < fileNames.size(); ++i) {

		if (i == fileNames.size() - 1)
			isLastScreen = true;
		setFileNames(i);
		initializeTypeGame();
		if (!isGoodStepsFile || !isGoodResultFile) {
			//Verify if the results and the steps file exists.
			if (i != fileNames.size() - 1)
				++fileNumber;
			else
				isPlaying = false;
			break;
		}
		srand(random_seed);
		board.clearGhostVec();
		board.load(fileNames[i]);
		if (board.isBoardInvalid()) {
			clearConsole();
			continue;
		}
		resetBoardAndHero();
		resetGameState();
		if (!isSilent) printLegend();
		ghostsVec.clear();
		createVecGhosts();

		while (true) {
			isFileValid = true;
			++iteration;
			nextLifeLostEvent = 0;
			setNextIterationEvents();
			handleUserInput();
			createBarrel();
			moveBarrels();
			moveGhosts();
			checkHeroFall();
			moveHero(prevKey);

			if (marioHitBarrel || marioHitGhost || marioFallDeadly || marioNearExplosion) {
				marioHitBarrel = false; marioHitGhost = false; marioFallDeadly = false; marioNearExplosion = false;
				marioLossLife();
				if (!isFileValid) break;
			}
			else {
				validateLifeLoss();
				if (!isFileValid) break;
			}
			updateMarioArmedState();
			if (hero.getPosition() == board.getPaulinePos()) {   // check for end screen condition
				score += 100;
				marioMeetsPauline();
				if (!isFileValid) break;
				if (isLastScreen) {
					isArmed = false;
					handleGameEnd(true);
				}
				break;
			}
			else {
				validatePaulineMeeting();
				if (!isFileValid) break;
			}
			checkResultsState();
			if (!isFileValid) break;

			if (heroLives == 0) {  // check for game over condition
				isArmed = false;
				handleGameEnd(false);
				break;
			}
		}

		completeProcess();
		results.clearResultsList();
		steps.clearStepsList();
		iteration = 0;
		nextPaulineEvent = 0;
		if (isLastScreen) break;
		++fileNumber;
	}
}

/*
This method resets the game board and hero to their initial states.
*/
void Game::resetBoardAndHero() {
	board.reset();
	if (!isSilent)
		board.print();
	hero.setPosition(board.getMarioInitPos());
	hero.setBoard(board); // set Mario's pointer to the board
	hero.setDir(Keys::STAY);
}

/*
This method displays the number of lives remaining for the hero.
Uses heart icons in color mode, or a number in non-color mode.
*/
void Game::printLegend() {

	if (colorModeEnabled) {
		for (int i = initIconPos.getX() + 2; i < initIconPos.getX() + heroLives + 2; i++) { // place of heart
			drawHeartIcon(i);
		}
	}
	else {
		gotoxy(initIconPos.getX() + 2, initIconPos.getY());
		std::cout << heroLives;
	}

	gotoxy(initIconPos.getX(), initIconPos.getY() + 1);
	std::cout << "SCORE:";
	printScore();
}

/* Handles the usage of the hammer by the player.
   The hammer moves either left or right depending on the previous key pressed,
   and then checks if any ghosts or barrels are at the new position.
*/
void Game::handleHammerUsage() {
	Point nextPos;

	if (prevKey == (char)Keys::LEFT)
		nextPos = { hammerPos.getX() - 1, hammerPos.getY() };
	else {
		nextPos = { hammerPos.getX() + 1, hammerPos.getY() };
	}
	// Check for ghosts or barrels at the new position and kill them.
	killGhostOrBarrel(nextPos);
}

/* Checks if there are any ghosts or barrels at the given position and
   kills them (activates their explosion) if they are present.
   It also increases the score based on the type of object killed.
*/
void Game::killGhostOrBarrel(Point position) {
	// Check for active ghosts at the specified position.
	for (int i = 0; i < ghostsVec.size(); i++) {
		if (ghostsVec[i]->getPosition() == position && ghostsVec[i]->getActiveStatus()) {
			ghostsVec[i]->explosion();
			score += 20;
		}
	}
	// Check for active barrels at the specified position.
	for (int i = 0; i < barrelVec.size(); i++) {
		if (barrelVec[i].getPosition() == position && barrelVec[i].getActiveStatus()) {
			barrelVec[i].explosion();
			score += 10;
		}
	}
	// Update and print the new score.
	printScore();
}

/* Initializes the vector of Ghost objects by reading positions from the board
   and setting up each ghost's position, direction, and board.
*/
void Game::createVecGhosts() {
	std::vector<ghostData> ghostsData = board.getGhostPos();

	for (int i = 0; i < ghostsData.size(); i++) {
		entityGhost* g = nullptr;
		if (ghostsData[i].ghostType == (char)Characters::GHOST) {
			g = new Ghost();
			g->setPosition(ghostsData[i].initPos);
			g->setBoard(board);
			ghostsVec.push_back(g);
		}
		else {
			g = new climbingGhost();
			g->setPosition(ghostsData[i].initPos);
			g->setBoard(board);
			ghostsVec.push_back(g);
		}
	}
}

/*
This method creates and adds new barrels to the game at regular intervals.
*/
void Game::createBarrel() {
	int logSize = barrelVec.size();
	if (iteration % 30 == 0) {
		Barrel b;
		b.setPosition(board.getDonkeyPos());
		barrelVec.push_back(b);
		barrelVec[logSize].setBoard(board);
		if (!isSilent)
			barrelVec[logSize].draw();
	}
}

/*
This method moves all active barrels and handles collisions with the hero.
*/
void Game::moveBarrels() {
	for (int i = 0; i < barrelVec.size(); i++) {
		if (barrelVec[i].getActiveStatus()) {
			barrelVec[i].setDir();
			if (handleMarioCollisionWithBarrel(i)) {  // check for collision before movement 
				return;
			}
			if (!isSilent) {
				barrelVec[i].erase();
				barrelVec[i].move();
				barrelVec[i].draw();
			}
			else {
				barrelVec[i].move();
			}
			if (handleMarioCollisionWithBarrel(i)) { // check for collision after movement
				return;
			}
		}
		else if (!barrelVec[i].isBarrelExplode()) {
			barrelVec[i].explosion();
			handleMarioIsInExplosionRange(i); // check if Mario is near explosion
		}
	}
}

/* Moves all active ghosts in the game. For each ghost, it checks for collisions, handles
   potential Mario collisions, sets their direction, erases their old position, moves them,
   and draws them in the new position.
*/
void Game::moveGhosts() {

	for (int i = 0; i < ghostsVec.size(); i++) {
		if (ghostsVec[i]->getActiveStatus()) {
			handleMarioCollisionWithGhost(i);
			checkGhostCollision(i);
			if (!isSilent)
				ghostsVec[i]->erase();
			ghostsVec[i]->move();
			handleMarioCollisionWithGhost(i);
			ghostsVec[i]->setCollision(false);
			if (!isSilent)
				ghostsVec[i]->draw();
		}
	}
}

/* Checks if the ghost at the given index collides with any other ghosts.
   If two ghosts are on the same row and moving towards each other, they collide.
*/
void Game::checkGhostCollision(int index) {
	for (int j = 0; j < ghostsVec.size(); j++) {
		if (j == index) {
			continue;
		}
		if (ghostsVec[index]->getPosition().getY() == ghostsVec[j]->getPosition().getY()) {
			if (ghostsVec[index]->getPosition().getX() + ghostsVec[index]->getDirX() == ghostsVec[j]->getPosition().getX()) {
				ghostsVec[index]->setCollision(true);
				ghostsVec[j]->setCollision(true);
			}
		}
	}
}

/* Handles a potential collision between Mario and a ghost.
   If Mario and the ghost are at the same position, the player loses a life and the game restarts.
   Returns true if there was a collision, false otherwise.
*/
bool Game::handleMarioCollisionWithGhost(int index) {
	Point marioPos = hero.getPosition();
	Point ghostPos = ghostsVec[index]->getPosition();

	if (marioPos == ghostPos) {
		decreaseLivesAndRestart();
		marioHitGhost = true;
		return true;
	}
	else {
		return false;
	}
}

/*
This method checks if the hero has fallen and handles the consequences.
*/
void Game::checkHeroFall() {
	if (hero.isFallDeadly()) { // check if the hero's fall is considered deadly
		marioFallDeadly = true;
		decreaseLivesAndRestart();
	}
}

/*
This method moves the hero character on the game board.
*/
void Game::moveHero(char key) {
	if (!isSilent)
		hero.erase();
	if (isArmed) {
		if (!isSilent)
			deletePrevHammer();
		moveWithHammer((char)Characters::AIR, key);
	}
	hero.move();
	if (!isSilent)
		hero.draw();
	if (isArmed) {
		moveWithHammer((char)Characters::HAMMER, key);
	}
}

/* Moves the hammer based on the direction key pressed (left or right) relative to Mario's position.
   If the hammer reaches the screen boundaries, it will reverse direction.
   Then, it updates the screen with the appropriate character (either background or the hammer itself).
*/
void Game::moveWithHammer(char c, char key) {
	if (key == (char)Keys::RIGHT) {
		hammerPos = { hero.getPosition().getX() + 1, hero.getPosition().getY() };
		if (hammerPos.getX() == (int)Boundaries::MAX_X - 1) {
			hammerPos = { hero.getPosition().getX() - 1, hero.getPosition().getY() };
		}
	}
	else if (key == (char)Keys::LEFT) {
		hammerPos = { hero.getPosition().getX() - 1, hero.getPosition().getY() };
		if (hammerPos.getX() == 0) {
			hammerPos = { hero.getPosition().getX() + 1, hero.getPosition().getY() };
		}
	}

	if (!isSilent) {
		// Move the cursor to the hammer's new position on the screen.
		gotoxy(hammerPos.getX(), hammerPos.getY());
		// If no object to display (background), print the background character.
		if (c == (char)Characters::AIR) {
			char backgroundChar = board.getChar(hammerPos);
			std::cout << backgroundChar;
		}
		// If within the board's boundaries, display the hammer character.
		else if (board.isInBoundaries(hammerPos)) {
			std::cout << c;
		}
	}
}

/* Deletes the previous hammer position on the screen.
   It checks if the previous position has certain characters (like ghost, Mario, or hammer),
   and if so, erases them by printing AIR. Otherwise, it restores the background character.
*/
void Game::deletePrevHammer() {

	gotoxy(hammerPos.getX(), hammerPos.getY());
	char backgroundChar = board.getChar(hammerPos);
	if (backgroundChar == (char)Characters::GHOST || backgroundChar == (char)Characters::MARIO || backgroundChar == (char)Characters::HAMMER)
		std::cout << (char)Characters::AIR;
	else
		std::cout << backgroundChar;
}

/* Sets the file names for steps and results based on the provided index.
   The filename prefix is derived from the fileNames list by removing the file extension.
   It then sets the filenames for steps and results by appending ".steps" and ".result" respectively. */
void Game::setFileNames(int index) {
	filename_prefix = fileNames[index].substr(0, fileNames[index].find_last_of('.'));
	stepsFilename = filename_prefix + ".steps";
	resultsFilename = filename_prefix + ".result";
}

/*
This method restarts the game after the hero loses a life.
This function was generated with the assistance of ChatGPT.
*/
void Game::restartGame() {
	resetGameState();
	if (!isSilent) {
		if (colorModeEnabled) {
			// handle number of life
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			COORD pos = { static_cast<SHORT>(initIconPos.getX() + heroLives + 2) , initIconPos.getY() };
			SetConsoleCursorPosition(hConsole, pos);
			std::cout << ' ';
		}
		else {
			printLegend();
		}
	}
}

/*
This method handles collision between Mario and a barrel.
*/
bool Game::handleMarioCollisionWithBarrel(int index) {
	Point marioPos = hero.getPosition();
	Point barrelPos = barrelVec[index].getPosition();

	if (marioPos == barrelPos) {
		decreaseLivesAndRestart();
		marioHitBarrel = true;
		return true;
	}
	else {
		return false;
	}
}

/*
This method checks if Mario is within the explosion range of a barrel.
*/
void Game::handleMarioIsInExplosionRange(int index) {
	Point marioPos = hero.getPosition();
	Point barrelPos = barrelVec[index].getPosition();

	if (abs(barrelPos.getX() - marioPos.getX()) <= radiusExp && abs(barrelPos.getY() - marioPos.getY()) <= radiusExp) {
		decreaseLivesAndRestart();
		marioNearExplosion = true;
	}
}

/*
Updates Mario's armed state and displays the status if Mario is armed. */
void Game::updateMarioArmedState() {
	if (hero.getPosition() == board.getHammerPos() && board.isHaveAHammer()) {
		isArmed = true;
		board.setCurrentBoard((char)Characters::AIR);
	}
	if (isArmed && !isSilent) {
		gotoxy(board.getLegendPos().getX(), board.getLegendPos().getY() + 2);
		std::cout << "MARIO IS ARMED";
	}
}

/*
This method decreases the hero's lives and restarts the game.
*/
void Game::decreaseLivesAndRestart() {
	heroLives--;
	isArmed = false;

	if (!isSilent) {
		// Display explosion effect at Mario's position
		displayExplosionEffect(hero.getPosition());

		gotoxy(board.getLegendPos().getX(), board.getLegendPos().getY() + 2);
		std::cout << "              ";
	}
	restartGame();
}

/*
This method draws a heart icon at the specified position to represent a life.
We are using color codes as specified in the assignment instructions.
This function was generated with the assistance of ChatGPT.
*/
void Game::drawHeartIcon(int x) const {
	// Get the console handle
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// Set the position for printing
	COORD pos = { static_cast<SHORT>(x) , initIconPos.getY() };
	SetConsoleCursorPosition(hConsole, pos);
	// Save the current color attributes
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	WORD originalAttributes = csbi.wAttributes;
	// Change the text color to bright red
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	// Switch output mode to Unicode
	_setmode(_fileno(stdout), _O_U16TEXT);
	// Print the heart shape
	std::wcout << L"\u2665";
	// Restore the output mode to normal
	_setmode(_fileno(stdout), _O_TEXT);
	// Restore the original color attributes
	SetConsoleTextAttribute(hConsole, originalAttributes);
}

/*
This method resets the game state, including barrels and hero position.
*/
void Game::resetGameState() {
	for (int i = 0; i < barrelVec.size(); i++) { // reset barrels array
		barrelVec[i].erase();
		barrelVec[i].setBarrel();
	}
	for (int i = 0; i < ghostsVec.size(); i++) {
		ghostsVec[i]->setActiveStatus();
		ghostsVec[i]->setExplodeStatus();
	}
	barrelVec.clear();
	if (!isSilent) {
		deletePrevHammer();
		if (board.isHaveAHammer()) {
			gotoxy(board.getHammerPos().getX(), board.getHammerPos().getY());
			std::cout << (char)Characters::HAMMER;
			board.setCurrentBoard((char)Characters::HAMMER);
		}
		hero.erase();
	}
	hero.setDir(Keys::STAY);
	hero.setPosition(board.getMarioInitPos());
	initIconPos = board.getLegendPos();
	

}

/*
This method handles the end of the game, displaying appropriate messages.
*/
void Game::handleGameEnd(bool isVictory) {
	clearConsole();
	if (!isSilent) {
		if (isVictory) {
			displayVictoryMessage();
		}
		else {
			displayGameOverMessage();
		}
	}
	completeProcess();
	Sleep(isSilent ? 0 : 2000);
	resetGameState();
	heroLives = 3;
	score = 0;
	fileNames.clear();
	clearConsole();
	results.clearResultsList();
	steps.clearStepsList();
	for (entityGhost* g : ghostsVec) {
		delete g;
	}
	run();
}

/* This method displays the game over message.
We are using color codes as specified in the assignment instructions.
*/
void Game::displayGameOverMessage() const {
	const char* youLostMessage[] = {
		"__     ______  _    _         _      ____   _____ _______     ",
		"\\ \\   / / __ \\| |  | |       | |    / __ \\ / ____|__   __|",
		" \\ \\_/ / |  | | |  | |       | |   | |  | | (___    | |     ",
		"  \\   /| |  | | |  | |       | |   | |  | |\\___ \\   | |    ",
		"   | | | |__| | |__| |       | |___| |__| |____) |  | |       ",
		"   |_|  \\____/ \\____/        |______\\____/|_____/   |_|    "
	};
	const int ROWS = 6;

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (colorModeEnabled) {
		SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
	}
	for (int i = 0; i < ROWS; i++) {
		std::cout << youLostMessage[i] << std::endl;
	}
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "\n\n You scored: " << score;
}

/*
This method displays the victory message.
We are using color codes as specified in the assignment instructions.
*/
void Game::displayVictoryMessage() const {
	const char* youWonMessage[] = {
	"__     ______  _    _         __          ______  _   _         ",
	"\\ \\   / / __ \\| |  | |        \\ \\        / / __ \\| \\ | | ",
	" \\ \\_/ / |  | | |  | |         \\ \\  /\\  / / |  | |  \\| |  ",
	"  \\   /| |  | | |  | |          \\ \\/  \\/ /| |  | | . ` |    ",
	"   | | | |__| | |__| |           \\  /\\  / | |__| | |\\  |     ",
	"   |_|  \\____/ \\____/             \\/  \\/   \\____/|_| \\_|  "
	};
	const int ROWS = 6;

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (colorModeEnabled) {
		SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
	}
	for (int i = 0; i < ROWS; i++) {
		std::cout << youWonMessage[i] << std::endl;
	}
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "\n\nCongratulations! You scored: " << score;
}

/* Displays the current game score at a specific position.
*/
void Game::printScore() {
	gotoxy(initIconPos.getX() + 7, initIconPos.getY() + 1);

	std::cout << score;
}

/* This method displays an explosion effect around Mario's position when he dies.
*/
void Game::displayExplosionEffect(Point marioPos) {
	const char explosionChars[] = { '*', 'o', 'x', '+', '-' }; // Characters to simulate explosion
	int explosionRadius = 2; // Radius of the explosion
	int yCoordinate;

	if (board.getChar({ marioPos.getX(), marioPos.getY() + 1 }) != (char)Characters::WALL)
		yCoordinate = marioPos.getY() + explosionRadius;
	else
		yCoordinate = marioPos.getY();

	for (int frame = 0; frame < 5; ++frame) { // Number of explosion frames
		// Display explosion
		for (int y = marioPos.getY() - explosionRadius; y <= yCoordinate; ++y) {
			for (int x = marioPos.getX() - explosionRadius; x <= marioPos.getX() + explosionRadius; ++x) {
				if (board.isInBoundaries(Point(x, y))) { // Ensure within boundaries
					gotoxy(x, y);
					std::cout << explosionChars[frame % 5]; // Cycle through explosion characters
				}
			}
		}
		Sleep(isSilent ? 0 : 40);
		clearExplosion(marioPos, explosionRadius); // Clear the previous frame
	}
}

/* This method clears the explosion effect.
*/
void Game::clearExplosion(Point marioPos, int radius) {
	for (int y = marioPos.getY() - radius; y <= marioPos.getY() + radius; ++y) {
		for (int x = marioPos.getX() - radius; x <= marioPos.getX() + radius; ++x) {
			if (board.isInBoundaries(Point(x, y)) && board.getChar({ x,y }) != (char)Characters::WALL) { // Ensure within boundaries
				gotoxy(x, y);
				std::cout << board.getChar(Point(x, y)); // Restore the original character
			}
		}
	}
}



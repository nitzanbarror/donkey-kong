#include "gameFromInput.h"

/* This method runs the main game loop, hides the cursor, displays the menu, and handles user choices.
*/
void GameFromInput::run() {
	ShowConsoleCursor(false);
	board.getAllBoardFileNames(fileNames);
	while (isPlaying) {
		menu.displayMenu(MenuType::MAIN_MENU);
		int choice = getUserChoice();
		handleMenuChoice(choice);
	}
}

/* This method handles user input for movement and other actions such as pausing or using a hammer.
*/
void GameFromInput::handleUserInput() {
	bool marioKeyPressed = false;
	bool hammerKeyPressed = false;

	for (int i = 0; i < 5; i++) {
		Sleep(12);
		if (_kbhit()) {
			key = std::tolower(_getch());
			if (isValidKey(key)) {
				if (key == (char)Keys::ESC) {
					pauseGame();
					board.print();
					printLegend();
				}
				if (key == (char)Keys::LEFT || key == (char)Keys::RIGHT)
					prevKey = key;
				if (isArmed && key == (char)Keys::HAMMER && !hammerKeyPressed) {
					hammerKeyPressed = true;
					handleHammerUsage();
					gotoxy(hammerPos.getX(), hammerPos.getY());
					std::cout << '+';
					Sleep(20);
					steps.addStep(iteration, key);
				}
				else if (!marioKeyPressed) {
					marioKeyPressed = true;
					hero.setDir((Keys)key);
					steps.addStep(iteration, key);
				}
			}
		}
	}
}

/* This method initializes the game by setting the random seed and initializing steps.
*/
void GameFromInput::initializeTypeGame() {
	if (isSave) {
		random_seed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
		steps.setRandomSeed(random_seed);
	}
}

/* This method handles the event when Mario hits an entity, such as a bomb.
*/
void GameFromInput::marioLossLife() {
	if (isSave)
		results.addResult(iteration, ResultValue::DISQUALIFIED);
}

/* This method handles the event when Mario reaches Pauline, marking the level as finished.
*/
void GameFromInput::marioMeetsPauline() {
	if (isSave)
		results.addResult(iteration, ResultValue::FINISHED);
}

/* This method completes the game process by saving steps and results if required.
*/
void GameFromInput::completeProcess() {
	if (isSave) {
		steps.saveSteps(stepsFilename);
		results.saveResults(resultsFilename, score, colorModeEnabled);
	}
}

/* This method handles the user's menu choices and directs the game flow accordingly.
*/
void GameFromInput::handleMenuChoice(char choice) {
	bool validChoice = false;  // flag to track if a valid choice has been made

	while (!validChoice) {  // loop until a valid choice is made
		fileNumber = 0;
		switch (choice) {
		case '1': // option 1: Disable color mode and start the game
			clearConsole();
			colorModeEnabled = false;
			playGame();
			validChoice = true; // mark the choice as valid
			break;
		case '2': // option 2: Enable color mode and start the game
			clearConsole();
			colorModeEnabled = true;
			playGame();
			validChoice = true; // mark the choice as valid
			break;
		case '3': // option 8: Show instructions
			clearConsole();
			printFilesOptions();
			fileNumber--;
			clearConsole();
			colorModeEnabled = false;
			playGame();
			validChoice = true; // mark the choice as valid
			break;
		case '8': // option 8: Show instructions
			clearConsole();
			menu.displayMenu(MenuType::INSTRUCTIONS);
			_getch();
			clearConsole();
			validChoice = true; // mark the choice as valid
			break;
		case '9':  // option 9: Exit the game
			clearConsole();
			std::cout << "Thanks for playing! Goodbye!" << std::endl;
			isPlaying = false; // set the game state to not playing
			validChoice = true; // mark the choice as valid
			break;
		default: // handle invalid input
			choice = _getch(); // prompt the user for a new choice
			break;
		}
	}
}

/* This method handles the game pause functionality.
*/
void GameFromInput::pauseGame() {
	clearConsole();
	menu.displayMenu(MenuType::PAUSE_MENU);

	while (true) {
		if (_kbhit()) {
			char key = std::tolower(_getch());
			if (key == (char)Keys::ESC) {
				clearConsole();
				return;
			}
		}
	}
}

/* This method checks if the provided key is valid for game input.
   It returns true if the key matches one of the valid movement or action keys,
   otherwise it returns false. */
bool GameFromInput::isValidKey(char key) {
	return (key == (char)Keys::UP ||
		key == (char)Keys::DOWN ||
		key == (char)Keys::LEFT ||
		key == (char)Keys::RIGHT ||
		key == (char)Keys::STAY ||
		key == (char)Keys::HAMMER);
}

/*
This method gets a single character input from the user.
*/
char GameFromInput::getUserChoice() const {
	char choice;
	choice = _getch();
	return choice;
}

/* Handles the display and selection of file options for the game.
*/
void GameFromInput::printFilesOptions() {
	printHeader();
	int size = fileNames.size();
	printFileOptions(0, 9, 7);

	// If there are more than 9 files, provide additional options for selection.
	if (size > 9) {
		printAdditionalInfo(17);
		fileNumber = getFileNumber();
		// If the user selects 0, show the next set of file options.
		if (fileNumber == 0) {
			clearConsole();
			printHeader();
			printFileOptions(9, size, 7);
			int num = getFileNumber();
			if (num != 0) {
				fileNumber = num + 9; // Adjust the file number based on the second set.
			}
		}
	}
	else {
		fileNumber = getFileNumber(); // Get the user's selection if files are 9 or fewer.
	}
}

/* Prints the header for the file selection menu.
*/
void GameFromInput::printHeader() {
	gotoxy(21, 4);
	std::cout << "===================================" << std::endl;
	gotoxy(28, 5);
	std::cout << " CHOOSE YOUR SCREEN: " << std::endl;
	gotoxy(21, 6);
	std::cout << "===================================" << std::endl;
}

/* Prints a range of file options for the user to select from.
   startIndex: Starting index in the fileNames vector.
   endIndex: Ending index in the fileNames vector.
   startLineNum: Starting line number for display on the console.
*/
void GameFromInput::printFileOptions(int startIndex, int endIndex, int startLineNum) {
	int lineNum = startLineNum;
	for (int i = startIndex; i < endIndex && i < fileNames.size(); i++) {
		gotoxy(28, lineNum);
		std::cout << i - startIndex + 1 << ". ";
		std::cout << fileNames[i] << std::endl;
		lineNum++;
	}
}

/* Displays additional information if there are more than 9 file options.
   lineNum: Starting line number for the message display.
*/
void GameFromInput::printAdditionalInfo(int lineNum) {
	gotoxy(14, lineNum++);
	std::cout << "There are over 10 screen options. Showing first 9.";
	gotoxy(17, lineNum++);
	std::cout << "Press 0 for more, or any key for main menu";
}

/* Captures the user's file selection as a number.
   Returns the selected file number.
*/
int GameFromInput::getFileNumber() {
	return _getch() - '0';
}

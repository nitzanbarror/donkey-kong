#include "gameFromFile.h"

/* Main game loop that hides the console cursor, retrieves all board file names,
   and continues playing while the game is in the 'playing' state. */
void GameFromFile::run() {
	ShowConsoleCursor(false);
	board.getAllBoardFileNames(fileNames);

	while (isPlaying) {
		playGame();
	}
}

/* Processes user input by looping through the steps, updating the hero's direction,
   handling hammer usage, and adding delays based on the 'isSilent' mode. */
void GameFromFile::handleUserInput() {
	for (int i = 0; i < 2; i++) {
		if (steps.isNextStepOnIteration(iteration)) {
			char key = steps.popStep();

			if (key == (char)Keys::LEFT || key == (char)Keys::RIGHT)
				prevKey = key;
			else if (isArmed && key == (char)Keys::HAMMER) {
				handleHammerUsage();
				gotoxy(hammerPos.getX(), hammerPos.getY());
				std::cout << '+';
			}
			hero.setDir((Keys)key);
		}
		Sleep(isSilent ? 0 : 10);
	}
}

/* Initializes the game by loading steps and results from files, checking validity,
   and determining whether color mode is enabled. */
void GameFromFile::initializeTypeGame() {
	steps = Steps::loadSteps(stepsFilename);
	isGoodStepsFile = steps.getGoodFile();
	random_seed = steps.getRandomSeed();
	results = Results::loadResults(resultsFilename);
	isGoodResultFile = results.getGoodFile();
	if (!isGoodStepsFile || !isGoodResultFile) {
		Sleep(1000);
		clearConsole();
	}

	colorModeEnabled = (bool)results.getColorMode(resultsFilename);
}

/* Handles Mario hitting an entity, checks if the results match, and reports errors if needed.
*/
void GameFromFile::marioLossLife() {
	if (results.popResult() != std::pair{ iteration, ResultValue::DISQUALIFIED }) {
		results.reportResultError("TEST FAILED: ", "Mario lost a life in the game, but it is missing from the results file", fileNames[fileNumber], iteration, score);
		unMachingResult = true;
		isFileValid = false;
	}
	nextLifeLostEvent = results.getNextBombIteration();
}

/* Completes the game process by verifying if the results match the expected values for completion
   and handling extra events after the game has finished. */
void GameFromFile::completeProcess() {
	if (!unMachingResult) {
		if (results.popResult().second != ResultValue::NO_RESULT) {
			results.reportResultError("TEST FAILED: ", "The results file includes extra events after the game ended", fileNames[fileNumber], iteration, score);
			unMachingResult = true;
		}
	}
}

/* Checks if the results indicate that the game has finished at the current iteration and validates
whether Pauline was reached, handling both success and failure conditions.*/
void GameFromFile::checkResultsState() {
	if (results.isFinishedBy(iteration)) {
		clearConsole();
		isFileValid = false;
		if (!results.isReachedPauline()) {
			results.reportResultError("TEST FAILED: ", "The results file ended, but the game was not fully completed", fileNames[fileNumber], iteration, score);
			isLastScreen = true;
			handleGameEnd(false);
			unMachingResult = true;
		}
	}
}

/* Validates the bomb event by checking if a bomb-hit event occurred at the current iteration
   and reporting an error if the event is missing or mismatched in the results file. */
void GameFromFile::validateLifeLoss() {
	if (iteration == nextLifeLostEvent) {
		results.reportResultError("TEST FAILED: ", "The results file indicates a life loss event, but no such event occurred in the game", fileNames[fileNumber], iteration, score);
		unMachingResult = true;
		isFileValid = false;
	}
}

/* Validates the Pauline event by checking if a Pauline-hit event occurred at the current iteration
   and reporting an error if the event is missing or mismatched in the results file. */
void GameFromFile::validatePaulineMeeting() {
	if (iteration == nextPaulineEvent) {
		results.reportResultError("TEST FAILED: ", "The results file indicates reaching Pauline, but no such event occurred in the game", fileNames[fileNumber], iteration, score);
		unMachingResult = true;
		isFileValid = false;
	}
}

/* 
Handles Mario meeting Pauline, checks if the results match, and reports errors if needed. */
void GameFromFile::marioMeetsPauline() {
	if (results.popResult() != std::pair{ iteration, ResultValue::FINISHED }) {
		results.reportResultError("TEST FAILED: ", "Mario reached Pauline in the game, but it is missing from the results file", fileNames[fileNumber], iteration, score);
		unMachingResult = true;
		isFileValid = false;
	}
	else {
		if (isLastScreen) {
			if (isSilent) {
				results.reportResultError("TEST PASSED: ", "You completed all the screens. YOU WON!", fileNames[fileNumber], iteration, score);
				isPlaying = false;
			}
		}
		else {
			if (isSilent)
				results.reportResultError("TEST PASSED: ", "You reached Pauline and moved on to the next screen", fileNames[fileNumber], iteration, score);
		}
	}
}

/* 
Sets the next events for life loss and Pauline interaction if the results are not finished.*/
void GameFromFile::setNextIterationEvents() {
	if (!results.isFinishedBy(iteration)) {
		nextLifeLostEvent = results.getNextBombIteration();
		nextPaulineEvent = results.getNextPaulineIteration();
	}
}

/*
 Finishes the game and displays appropriate messages.*/
void GameFromFile::handleGameEnd(bool isVictory) {
	clearConsole();
	if (!isSilent) {
		if (isVictory) {
			displayVictoryMessage();
		}
		else {
			displayGameOverMessage();
		}
	}
	Sleep(isSilent ? 0 : 2000);
	completeProcess();
	resetGameState();
	clearConsole();
	fileNames.clear();
	results.clearResultsList();
	steps.clearStepsList();
	isPlaying = false;
}


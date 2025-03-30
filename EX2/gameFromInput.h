#pragma once
#include "game.h"

class GameFromInput : public Game{
public:
	// Virtual functions
	virtual void run() override;
	virtual void handleUserInput() override;
	virtual void initializeTypeGame() override;
	virtual void marioLossLife() override;
	virtual void completeProcess() override;
	virtual void marioMeetsPauline() override;
	// Placeholder functions with empty bodies
	virtual void checkResultsState() { ; }
	virtual void validateLifeLoss() { ; }
	virtual void validatePaulineMeeting() { ; }
	virtual void setNextIterationEvents() { ; }

	void handleMenuChoice(char choice);
	void pauseGame();
	bool isValidKey(char key);
	char getUserChoice() const;
	void printFilesOptions();
	void printHeader();
	void printFileOptions(int startIndex, int endIndex, int startLineNum);
	void printAdditionalInfo(int lineNum);
	int getFileNumber();
};


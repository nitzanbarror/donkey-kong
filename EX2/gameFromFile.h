#pragma once
#include "game.h"

class GameFromFile : public Game{

public:
	virtual void run() override;
	virtual void handleUserInput() override;
	virtual void initializeTypeGame() override;
	virtual void marioLossLife() override;
	virtual void completeProcess() override;
	virtual void checkResultsState() override;
	virtual void validateLifeLoss() override;
	virtual void validatePaulineMeeting() override;
	virtual void marioMeetsPauline() override;
	virtual void setNextIterationEvents() override;
	virtual void handleGameEnd(bool isVictory) override;

};


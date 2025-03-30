#pragma once
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <vector>

#include "board.h"
#include "utils.h"
#include "point.h"
#include "mario.h"
#include "gameConfig.h"
#include "barrel.h"    
#include "menu.h"
#include "ghost.h"
#include "entityGhost.h"
#include "climbingGhost.h"
#include "steps.h"
#include "results.h"

class Game {
protected:
	Board board;
	Mario hero;
	Menu menu;
	Point hammerPos;
	Point initIconPos;
	Steps steps;
	Results results;
	std::vector<entityGhost*> ghostsVec;
	std::vector<Barrel> barrelVec;
	std::vector<std::string> fileNames;
	std::string filename_prefix;
	std::string stepsFilename;
	std::string resultsFilename;
	size_t nextLifeLostEvent = 0;
	size_t nextPaulineEvent = 0;
	size_t iteration = 0;

	long random_seed;
	bool isLoad = false;
	bool isSilent = false;
	bool isSave = false;
	bool isLastScreen = false;
	bool isGoodStepsFile = true;
	bool isGoodResultFile = true;
	bool unMachingResult = false;
	bool colorModeEnabled = false;
	bool isFileValid = true;
	bool marioHitBarrel = false;
	bool marioHitGhost = false;
	bool marioFallDeadly = false;
	bool marioNearExplosion = false;
	bool isPlaying = true;
	bool isArmed = false;
	int fileNumber = 0;
	int heroLives = 3;
	int score = 0;
	char key;
	char prevKey;
	static constexpr int radiusExp = 2;

	void playGame();
	void setFileNames(int index);
	void resetBoardAndHero();
	void killGhostOrBarrel(Point position);
	void createVecGhosts();
	void createBarrel();
	void moveBarrels();
	void moveGhosts();
	void moveHero(char key);
	void moveWithHammer(char c, char key);
	void checkHeroFall();
	void checkGhostCollision(int index);
	bool handleMarioCollisionWithGhost(int index);
	bool handleMarioCollisionWithBarrel(int index);
	void handleHammerUsage();
	void handleMarioIsInExplosionRange(int index);
	void updateMarioArmedState();
	void deletePrevHammer();
	void printLegend();
	void printScore();
	void drawHeartIcon(int x) const;
	void decreaseLivesAndRestart();
	void displayExplosionEffect(Point marioPos);
	void clearExplosion(Point marioPos, int radius);
	void resetGameState();
	void restartGame();

public:
	//virtual functions
	virtual void run() = 0;
	virtual void handleUserInput() = 0;
	virtual void initializeTypeGame() = 0;
	virtual void completeProcess() = 0;
	virtual void checkResultsState() = 0;
	virtual void marioLossLife() = 0;
	virtual void validateLifeLoss() = 0;
	virtual void marioMeetsPauline() = 0;
	virtual void validatePaulineMeeting() = 0;
	virtual void setNextIterationEvents() = 0;
	virtual void handleGameEnd(bool isVictory);

	void displayGameOverMessage() const;
	void displayVictoryMessage() const;
	void setIsLoad(bool _isLoad) { isLoad = _isLoad; }
	void setIsSilent(bool _isSilent) { isSilent = _isSilent; }
	void setIsSave(bool _isSave) { isSave = _isSave; }
};
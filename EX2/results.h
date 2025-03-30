#pragma once
#include <list>
#include <fstream>
#include <iostream>
#include <limits>
#include <conio.h>
#include "gameConfig.h"

class Results {
private:
	std::list<std::pair<size_t, ResultValue>> results; // pair: iteration, step
public:
	
	static Results loadResults(const std::string& filename);
	void saveResults(const std::string& filename, int score, bool colorModeEnabled) const;
	std::pair<size_t, ResultValue> popResult();
	size_t getNextBombIteration() const;
	size_t getNextPaulineIteration() const;
	void reportResultError(const std::string& messageToScreen, const std::string& message, const std::string& filename, size_t iteration, int score);

	void addResult(size_t iteration, ResultValue result) {
		results.push_back({ iteration, result });
	}
	
	bool isFinishedBy(size_t iteration) const {
		return results.empty() || results.back().first <= iteration  ;
	}

	int getColorMode(const std::string& filename);
	void clearResultsList();
	bool isReachedPauline();
	bool getGoodFile() { return !results.empty(); }

};


#include "results.h"

/* This method loads the results from a specified file.
   It reads the color mode, number of results, and iterates over the results.
   If the file does not exist, it will notify the user and move to the next screen. */
Results Results::loadResults(const std::string& filename) {
	Results results;
	std::ifstream results_file(filename);
	if (!results_file.good()) {
		std::cout << "The results file for " << filename << " does not exist. \nYou are moving to the next screen.";
	}
	else {
		size_t colorMode;
		results_file >> colorMode;
		size_t size;
		results_file >> size;
		while (!results_file.eof() && size-- != 0) {
			size_t iteration;
			int result;
			results_file >> iteration >> result;
			results.addResult(iteration, static_cast<ResultValue>(result));
		}
	}

	return results;
}

/* This method saves the current results to a specified file.
   It writes the color mode, the number of results, each result with iteration and its corresponding value,
   and the final score. */
void Results::saveResults(const std::string& filename, int score, bool colorModeEnabled) const {
	std::ofstream results_file(filename);
	results_file << colorModeEnabled << '\n';
	results_file << results.size();
	for (const auto& result : results) {
		results_file << '\n' << result.first << ' ' << (int)result.second;
	}
	results_file << '\n' << score;

	results_file.close();
}

/* This method returns the iteration number for the next bomb hit, if any.
   If no bomb hit is found, it returns a very large number to indicate no bomb hit. */
size_t Results::getNextBombIteration() const {
	if (!results.empty() && results.front().second == ResultValue::DISQUALIFIED) {
		return results.front().first;
	}
	else return std::numeric_limits<size_t>::max(); // a big number we will never reach
}

size_t Results::getNextPaulineIteration() const {
	if (!results.empty() && results.front().second == ResultValue::FINISHED) {
		return results.front().first;
	}
	else return std::numeric_limits<size_t>::max(); // a big number we will never reach
}

/* This method displays an error message for result mismatches.
   It shows the error message, iteration, and score, and waits for the user to press a key to continue. */
void Results::reportResultError(const std::string& messageToScreen, const std::string& message, const std::string& filename, size_t iteration, int score) {
	system("cls");
	std::cout << "Screen " << filename << '\n';
	std::cout << messageToScreen << '\n';
	std::cout << message << '\n';
	std::cout << "Iteration: " << iteration << '\n';
	std::cout << "You scored: " << score << '\n' << '\n';
	std::cout << "Press any key to continue to next screens (if any)" << std::endl;
	_getch();
}

/* This method returns the color mode setting from the specified results file.
   It reads the color mode value (enabled/disabled) from the file. */
int Results::getColorMode(const std::string& filename) {
	
	std::ifstream results_file(filename);
	int colorModeEnabled;
	results_file >> colorModeEnabled;

	return colorModeEnabled;
}

/* This method clears the list of results. 
*/
void Results::clearResultsList() {
	results.clear();
}

/* This method pops and returns the first result from the list of results.
   If the list is empty, it returns a default "no result" pair. */
std::pair<size_t, ResultValue> Results::popResult() {
	if (results.empty())
		return { 0, ResultValue::NO_RESULT };
	auto result = results.front();
	results.pop_front();
	return result;
}

/* This method checks if Pauline was not reached (i.e., the game was disqualified).
   It returns true if the last result indicates disqualification, otherwise false. */
bool Results::isReachedPauline() {
	
	if (!results.empty() && results.back().second == ResultValue::FINISHED)
		return true;
	else {
		return false;
	}
}


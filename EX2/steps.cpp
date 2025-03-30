#include "steps.h"

/* This method loads steps from a file, parsing the file to extract the random seed and the steps data. 
*/
Steps Steps::loadSteps(const std::string& filename) {
	Steps steps;
	std::ifstream steps_file(filename);
	if (!steps_file.good()) {
		std::cout << "The steps file for " << filename << " does not exist. \nYou are moving to the next screen.";
	}
	else {
		steps_file >> steps.randomSeed;
		size_t size;
		steps_file >> size;
		while (!steps_file.eof() && size-- != 0) {
			size_t iteration;
			char step;
			steps_file >> iteration >> step;
			steps.addStep(iteration, step);
		}
		steps_file.close();
	}

	return steps;
}

/* This method saves the steps, including the random seed and the steps themselves, to a file. 
*/
void Steps::saveSteps(const std::string& filename) const {
	std::ofstream steps_file(filename);
	steps_file << randomSeed << '\n' << steps.size();
	for (const auto& step : steps) {
		steps_file << '\n' << step.first << ' ' << step.second;
	}
	steps_file.close();
}

/* This method pops the next step from the list and returns the corresponding step character. 
*/
char Steps::popStep() {
	char step = steps.front().second;
	steps.pop_front();
	return step;
}

/* This method clears the list of steps. 
*/
void Steps::clearStepsList() {
	steps.clear();
}

#include "Board.h"

/* Loads the game board from a file.
   - Initializes game state variables (e.g., `haveMario`, `havePauline`).
   - Reads each character in the file and processes it based on its type.
   - Validates the integrity of the board and handles special game objects. */
void Board::load(const std::string& filename) {
	haveHammer = false;
	haveMario = false;
	havePauline = false;
	fillWall = false;
	countErrors = 0;

	std::ifstream screen_file(filename);
	if (!screen_file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
	}
	int curr_row = 0;
	int curr_col = 0;
	char c;

	while (!screen_file.get(c).eof() && curr_row < (int)Boundaries::MAX_Y) {
		if (c == '\n') {
			handleNewLine(curr_row, curr_col);
			++curr_row;
			curr_col = 0;
			continue;
		}

		bool isNeedDeleted = false;
		if (curr_col < (int)Boundaries::MAX_X) {
			handleSpecialChars(c, curr_row, curr_col);
			if (c == (char)Characters::GHOST) {
				handleGhost(curr_row, curr_col, c);
				isNeedDeleted = true;
			}
			else if (c == (char)Characters::BIG_GHOST) {
				handleGhost(curr_row, curr_col, c);
				isNeedDeleted = true;
			}
			else if (c == (char)Characters::MARIO) {
				handleMario(curr_row, curr_col);
				isNeedDeleted = true;
			} 
			if (!isNeedDeleted)
				originalBoard[curr_row][curr_col++] = c;
			else
				originalBoard[curr_row][curr_col++] = (char)Characters::AIR;
		}
	}

	validateMarioPauline();
	validateRowAndColumn(curr_row, curr_col);

	if (fillWall) {
		addFrame(curr_row);
	}
	if (curr_row < (int)Boundaries::MAX_Y) {
		addDummyRow(curr_row);
	}
	validateGhostPositions();
}

/* Handles the logic for processing a newline character.
   - Ensures missing columns in the current row are filled with AIR.
   - Marks the end of the row with a newline character. */
void Board::handleNewLine(int& curr_row, int& curr_col) {
	if (curr_col < (int)Boundaries::MAX_X) {
		handelMissingCols(curr_row, curr_col);
	}
	originalBoard[curr_row][(int)Boundaries::MAX_X] = '\n';
}

/* Processes special characters on the board and updates game state accordingly.
   - Handles Mario, Pauline, Donkey Kong, Wall, Hammer, and Legend positions.
   - Reports errors for unrecognized characters. */
void Board::handleSpecialChars(char c, int& curr_row, int& curr_col) {
	if (c == (char)Characters::PAULINE) {
		handlePauline(curr_row, curr_col);
	}
	else if (c == (char)Characters::DONKEY_KONG) {
		donkeyPos = { curr_col, curr_row };
	}
	else if (c == (char)Characters::WALL) {
		handleWall(curr_row, curr_col);
	}
	else if (c == (char)Characters::LEGEND) {
		legendPos = { curr_col, curr_row };
	}
	else if (c == (char)Characters::HAMMER) {
		haveHammer = true;
		hammerPos = { curr_col, curr_row };
	}
	else if (c != (char)Characters::GHOST &&
		c != (char)Characters::LEGEND &&
		c != (char)Characters::HAMMER &&
		c != (char)Characters::WALL &&
		c != (char)Characters::LADDER &&
		c != (char)Characters::DONKEY_KONG &&
		c != (char)Characters::PAULINE &&
		c != (char)Characters::MARIO &&
		c != (char)Characters::AIR &&
		c != (char)Characters::BIG_GHOST &&
		!isFloorChar(c)) {

		countErrors++;
		errorMessage.push_back("ERROR: there is unknown character in your game: '" + std::string(1, c) + "'!\n"); 
	}
}

/* Handles Mario's position on the board.
   Ensures there is only one Mario and updates its position. */
void Board::handleMario(int curr_row, int curr_col) {
	if (haveMario == 0) {
		marioPos = { curr_col, curr_row };
	}
	else {
		countErrors++;
		errorMessage.push_back("ERROR: there is more than one Mario in your game!\n");
	}
	haveMario++;
	//originalBoard[curr_row][curr_col++] = (char)Characters::AIR;

}

/* Handles Pauline's position on the board.
   Ensures there is only one Pauline and updates its position. */
void Board::handlePauline(int curr_row, int curr_col) {
	if (havePauline == 0) {
		paulinePos = { curr_col, curr_row };
		havePauline = true;
	}
	else {
		countErrors++;
		errorMessage.push_back("ERROR: there is more than one Pauline in your game!\n");
	}
	havePauline++;
}

/* Handles wall positions on the board.
   Marks whether the frame should be filled with walls if on boundaries. */
void Board::handleWall(int curr_row, int curr_col) {
	if (curr_col == 0 || curr_col == (int)Boundaries::MAX_X || curr_row == 0 || curr_row == (int)Boundaries::MAX_Y) {
		fillWall = true;
	}
}

/* Handles ghost positions on the board.
   Updates the initial positions and replaces ghost positions with AIR. */
void Board::handleGhost(int& curr_row, int& curr_col, char type) {
	ghostData g;
	g.initPos = { curr_col, curr_row };
	g.ghostType = type;
	ghostsInitPos.push_back(g);
}

/* Validates the positions of all ghosts.
   Ensures each ghost is standing on a valid floor character. */
void Board::validateGhostPositions() {
	if (ghostsInitPos.size() != 0) {
		for (auto& ghostPos : ghostsInitPos) {
			if (!isFloorChar(originalBoard[ghostPos.initPos.getY() + 1][ghostPos.initPos.getX()])) {
				countErrors++;
				errorMessage.push_back("ERROR: ghost isn't standing on a floor!\n");
				break;
			}
		}
	}
}

/* Validates that both Mario and Pauline are present on the board. 
*/
void Board::validateMarioPauline() {
	if (haveMario == 0) {
		countErrors++;
		errorMessage.push_back("ERROR: mario isn't in the game!\n");
	}

	if (havePauline == 0) {
		countErrors++;
		errorMessage.push_back("ERROR: pauline isn't in the game!\n");
	}
}

/* Validates the row and column limits of the board.
   Ensures the screen dimensions do not exceed predefined boundaries. */
void Board::validateRowAndColumn(int curr_row, int curr_col) {
	if (curr_row == (int)Boundaries::MAX_Y) {
		countErrors++;
		errorMessage.push_back("ERROR: You have exceeded the limits of the screen!\n");
	}

	if (curr_col < (int)Boundaries::MAX_X) {
		handelMissingCols(curr_row, curr_col);
	}
}

/* Adds a frame of WALL characters around the board.
   Ensures boundary rows and columns are filled with walls. */
void Board::addFrame(int last_row) {
	if (legendPos.getY() != 0) {
		// Add the first and last rows
		// Fill with WALL characters
#pragma warning(suppress : 4996)
		strcpy(originalBoard[0], std::string((int)Boundaries::MAX_X, (char)Characters::WALL).c_str());
#pragma warning(suppress : 4996)
		strcpy(originalBoard[last_row], std::string((int)Boundaries::MAX_X, (char)Characters::WALL).c_str());
		originalBoard[0][(int)Boundaries::MAX_X] = '\n';
		originalBoard[last_row][(int)Boundaries::MAX_X] = '\0';
	}
	else {
		// Add the last row
		// Fill with WALL characters
#pragma warning(suppress : 4996)
		strcpy(originalBoard[last_row], std::string((int)Boundaries::MAX_X, (char)Characters::WALL).c_str());
		originalBoard[last_row][(int)Boundaries::MAX_X] = '\0';
	}

	// Add the first and last columns
	for (int row = 1; row < last_row; ++row) {
		originalBoard[row][0] = (char)Characters::WALL;
		originalBoard[row][(int)Boundaries::MAX_X - 1] = (char)Characters::WALL;
	}
}

/* Adds a dummy row at the bottom of the board.
   Ensures the board layout aligns correctly with boundaries. */
void Board::addDummyRow(int last_row) {
#pragma warning(suppress : 4996)
	strcpy(originalBoard[last_row + 1], std::string((int)Boundaries::MAX_X, (char)Characters::WALL).c_str());
	originalBoard[last_row + 1][(int)Boundaries::MAX_X] = '\0';
}

/* Handles missing columns in a row.
   Fills missing columns with AIR characters to complete the row. */
void Board::handelMissingCols(int row, int col) {
	// add spaces for missing cols
#pragma warning(suppress : 4996) // to allow strcpy
	strcpy(originalBoard[row] + col, std::string((int)Boundaries::MAX_X - col, (char)Characters::AIR).c_str());

}

/* This method prints the current state of the game board to the console.
It prints each row of the board, with a newline character after each row
except for the last one. */
void Board::print() const {
	clearConsole();
	std::cout << currentBoard[0];
}

/* Populates the provided vector with all board file names in the current directory.
   Only files starting with "dkong" and having the ".screen" extension are included.
   The resulting vector is sorted alphabetically. */
void Board::getAllBoardFileNames(std::vector<std::string>& vec_to_fill) {
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(fs::current_path())) {
		auto filename = entry.path().filename();
		auto filenameStr = filename.string();
		if (filenameStr.substr(0, 5) == "dkong" && filename.extension() == ".screen") {
			vec_to_fill.push_back(filenameStr);
		}
	}
	std::sort(vec_to_fill.begin(), vec_to_fill.end());
}

/* This method resets the current game board to its original state by copying
the contents of the original board to the current board for each row. */
void Board::reset() {
	memcpy(currentBoard, originalBoard, (int)Boundaries::MAX_Y * ((int)Boundaries::MAX_X + 1));
}

/* This method checks if a given point on the board is a valid position.
A position is considered valid if it's within the board boundaries
and is not occupied by a floor character. */
bool Board::isValidPosition(const Point& point) const {
	char position = getChar(point);
	if (isInBoundaries(point)) {
		if (!isFloorChar(position))
			return true;
	}
	return false;
}

/* This method checks if a given point is on a floor.
It does this by checking the character directly below the given point. */
bool Board::isOnFloor(const Point& point) const {
	int x = point.getX();
	int y = point.getY() + 1;
	Point p = { x, y };
	char position = getChar(p);
	return (isFloorChar(position));
}

/* This method checks if a given character represents a floor.
Floor characters can be LEFT, RIGHT, or PREV. */
bool Board::isFloorChar(char ch) const {
	return (ch == (char)FloorCharacters::LEFT || ch == (char)FloorCharacters::RIGHT || ch == (char)FloorCharacters::PREV || ch == (char)Characters::WALL);
}

/* This method checks if a given point is in the air.
A point is considered in the air if the character directly below it is AIR. */
bool Board::isInAir(const Point& point) const {
	int x = point.getX();
	int y = point.getY() + 1;
	Point charBelow = { x, y };
	return (getChar(charBelow) == (char)Characters::AIR);
}

/* This method checks if a given point is within the boundaries of the board. 
*/
bool Board::isInBoundaries(const Point& point) const {
	int x = point.getX();
	int y = point.getY();
	return ((x < (int)Boundaries::MAX_X - 1 && x > 0) && (y < (int)Boundaries::MAX_Y && y > 0));
}

/* Returns Mario's initial position on the board. 
*/
Point Board::getMarioInitPos() const {
	return marioPos;
}

/* Returns Pauline's position on the board. 
*/
Point Board::getPaulinePos() const {
	return paulinePos;
}

/* Returns Donkey Kong's position on the board. 
*/
Point Board::getDonkeyPos() const {
	return donkeyPos;
}

/* Returns the hammer's position on the board. 
*/
Point Board::getHammerPos() const {
	return hammerPos;
}

/* Returns the legend's position on the board. 
*/
Point Board::getLegendPos() const {
	return legendPos;
}

/* Returns the initial positions of all ghosts on the board. 
*/
std::vector<ghostData> Board::getGhostPos() const {
	return ghostsInitPos;
}

/* Clears the vector containing the initial positions of all ghosts. 
*/
void Board::clearGhostVec() {
	ghostsInitPos.clear();
}

/* Checks if the character at the given point is a ghost. 
*/
bool Board::isGhostChar(const Point& point) const {
	return (getChar(point) == (char)Characters::GHOST);;
}

/* Checks if the board contains a hammer. 
*/
bool Board::isHaveAHammer() {
	return haveHammer;
}

/* Validates the board state and checks for errors.
   If the board is invalid, displays error messages and returns true. */
bool Board::isBoardInvalid()  {
	int sizeErrorMesVec = errorMessage.size();
	if (sizeErrorMesVec > 0) {
		for (const std::string& str : errorMessage) {
			std::cout << str << std::endl;
		}
		std::cout << "Your board is invalid, you are moved to the next board" << std::endl;
		Sleep(3000);
		errorMessage.clear();
		return true;
	}
	return false;
}

/* Updates the current board at the hammer's position with the given character. 
*/
void Board::setCurrentBoard(char c) {
	currentBoard[hammerPos.getY()][hammerPos.getX()] = c;
}

/* This method checks if a given point on the board is a ladder. 
*/
bool Board::isLadder(const Point& point) const {
	return (getChar(point) == (char)Characters::LADDER);
}


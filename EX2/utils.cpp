#include "utils.h"

/*
This function moves the console cursor to the specified (x, y) coordinates.
This function was taken from the material we worked on in the lecture.
*/

void gotoxy(int x, int y) {
    std::cout.flush();
    COORD coord; // Create a COORD structure 
    coord.X = x; // Set the X coordinate 
    coord.Y = y; // Set the Y coordinate 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); // Move the cursor 
}

/*
This function shows or hides the console cursor based on the showFlag parameter.
This function was taken from the material we worked on in the lecture.
*/

void ShowConsoleCursor(bool showFlag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

/*
This function clears the console screen.
This function was taken from the material we worked on in the lecture.
*/

void clearConsole() {
    system("cls");
}


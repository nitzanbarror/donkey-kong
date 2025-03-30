#include "menu.h"

/*
This method displays the appropriate menu based on the provided MenuType.
It uses a switch statement to call the correct display function for each menu type.
*/

void Menu::displayMenu(MenuType menuType) const {
	switch (menuType) {
	case MenuType::MAIN_MENU:
		displayMainMenu();
		break;
	case MenuType::INSTRUCTIONS:
		displayInstructions();
		break;
	case MenuType::PAUSE_MENU:
		displayPauseBoard();
		break;
	default:
		return;
	}
}

#pragma once
#include "ofMain.h"
#include "Button.h"
#include "Menu.h"

class MenuSystem {

public:
	MenuSystem():
		_cMenu(0) {};
	bool addMenu(Menu menu);
	void clearMenus() {
		_menus.clear();
		_cMenu = 0;
	}

	vector<Button*> hover(vector<ofVec2f> positions);
	void prev();
	void next();
	void restart();
	void update();
	void draw();

	void setPosition(int menuIdx, ofVec2f pos);
	ofVec2f getPosition(int menuIdx);
	void resetPosition(int menuIdx, ofVec2f pos);
	void setPositions(ofVec2f pos);
	void resetPositions();

	void resetHovers(); // reset hover states of all menus
	void resetHover(int menuIdx);
	void resetHover() { resetHover(_cMenu); } // reset current menu

	void resetCarousels();
	bool carouselsPageUp();
	bool carouselsPageDown();

	int getMenuIndex() { return _cMenu; }
	int getMenuIndexByName(string name) {
		for (int i = 0; i < _menus.size(); i++) {
			if (_menus[i].getName() == name) return i;
		}
		return -1; // no menu by that name
	}
	string getMenuName(int menuIdx) {
		if (exists(menuIdx)) return _menus[menuIdx].getName();
		return "";
	}
	string getMenuName() { return getMenuName(_cMenu); }
	Menu* getMenuPtr(int menuIdx) {
		if (exists(menuIdx)) return &_menus[menuIdx];
		return nullptr;
	}
	Menu* getMenuPtr() {
		return getMenuPtr(_cMenu);
	}
	Menu* getMenuPtrByName(string name) { 
		return getMenuPtr(getMenuIndexByName(name)); 
	}
	vector<Menu>& getMenus() { return _menus; }
	int getNumMenus() { return _menus.size(); }

	bool goToMenu(int menuIdx);
	bool goToMenu(string name);

private:
	vector<Menu> _menus;
	int _cMenu; // current menu

	bool exists(int menuIdx){ 
		if (menuIdx >= 0 && menuIdx < _menus.size()) return true;
		return false;
	}
};
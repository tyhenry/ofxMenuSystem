#include "MenuSystem.h"

bool MenuSystem::addMenu(Menu menu) {
	_menus.push_back(menu);
	return true;
}

vector<Button*> MenuSystem::hover(vector<ofVec2f> positions) {
	vector<Button*> hitBtns;
	if (exists(_cMenu)){
		hitBtns = _menus[_cMenu].hover(positions);
		_menus[_cMenu].update(); // update animations
	}
	return hitBtns;
}

void MenuSystem::prev() {
	if (_cMenu > 0) {
		resetHovers();
		_cMenu--;
	}
}

void MenuSystem::next() {
	if (_cMenu < _menus.size()-1) {
		resetHovers();
		_cMenu++;
	}
}

void MenuSystem::restart() {
	resetHovers();
	resetPositions();
	_cMenu = 0;
}

void MenuSystem::update() {
	for (auto& menu : _menus) {
		menu.update();
	}
}

void MenuSystem::draw() {
	ofLogVerbose("MenuSystem") << "drawing menu [" << _cMenu << "]";
	if (exists(_cMenu)) {
		_menus[_cMenu].draw();
	}
	else {
		ofLogError("MenuSystem") << "drawing menu [" << _cMenu << "] but doesn't exist!";
	}
}

void MenuSystem::setPosition(int menuIdx, ofVec2f pos) {
	if (exists(menuIdx)) {
		_menus[menuIdx].setPos(pos);
	}
}
ofVec2f MenuSystem::getPosition(int menuIdx) {
	if (exists(menuIdx)) {
		return _menus[menuIdx].getPos();
	}
	return ofVec2f();
}
void MenuSystem::resetPosition(int menuIdx, ofVec2f pos) {
	if (exists(menuIdx)) {
		_menus[menuIdx].resetPos();
	}
}

void MenuSystem::setPositions(ofVec2f pos) {
	for (auto& menu : _menus) {
		menu.setPos(pos);
	}
}

void MenuSystem::resetPositions() {
	for (auto& menu : _menus) {
		menu.resetPos();
	}
}

void MenuSystem::resetHovers() {
	for (int i = 0; i < _menus.size(); i++) {
		_menus[i].resetHover();
	}
}
void MenuSystem::resetHover(int menuIdx) {
	if (exists(menuIdx)) _menus[menuIdx].resetHover();
}

void MenuSystem::resetCarousels()
{
	for (auto& menu : _menus)
		menu.resetCarousels();
}

bool MenuSystem::carouselsPageUp()
{
	bool did = false;
	if (exists(_cMenu)){
		auto& csls = _menus[_cMenu].getCarousels();
		for (auto& csl : csls) {
			did = true;
			csl.prevPage();
		}
	}
	return did;
}

bool MenuSystem::carouselsPageDown()
{
	bool did = false;
	if (exists(_cMenu)){
		auto& csls = _menus[_cMenu].getCarousels();
		for (auto& csl : csls) {
			did = true;
			csl.nextPage();
		}
	}
	return did;
}

bool MenuSystem::goToMenu(int menuIdx) {
	if (exists(menuIdx)) {
		resetHover(_cMenu);
		_cMenu = menuIdx;
		return true;
	}
	return false;
}
bool MenuSystem::goToMenu(string name) {
	return goToMenu(getMenuIndexByName(name));
}
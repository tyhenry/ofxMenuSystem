#include "Menu.h"

Menu::Menu(string name, float hoverWait, float x, float y, float width, float height)
	: _name(name), _x(x), _y(y), _w(width), _h(height), _hoverWait(hoverWait)
{
	_origPos = ofVec2f(x, y);
}

void Menu::setBackground(ofImage bg, float x, float y, float w, float h) {
	_bg = bg;
	_bgBounds.set(x, y, w, h);
	_bDrawBg = true;
}

void Menu::addButton(ofImage img, string name, float x, float y, float w, float h, ofImage hoverImg) {
	_buttons.push_back(Button(img, name, ofVec2f(x, y), w, h, _hoverWait));
	if (hoverImg.isAllocated()) {
		_buttons.back().setHoverImg(hoverImg);
	}
}

bool Menu::addCarousel(string name, vector<ofImage>& items, vector<string> names, float x, float y, float w, float itemH, int nItemsDisplay, float itemGap, ofImage hoverImg) {
	_carousels.push_back(Carousel(name));
	bool good = _carousels.back().setup(items, names, x, y, w, itemH, nItemsDisplay, itemGap, hoverImg);
	if (good) _carousels.back().setHoverWait(_hoverWait);
	else _carousels.pop_back();
	return good;
}

void Menu::setHoverWait(float seconds) {
	_hoverWait = seconds;
	for (auto& button : _buttons) {
		button.setHoverWait(_hoverWait);
	}
	for (auto& carousel : _carousels) {
		carousel.setHoverWait(_hoverWait);
	}
}

void Menu::setPos(ofVec2f pos){

	// get translation
	ofVec2f d = pos - ofVec2f(_x, _y);
	_x = pos.x;
	_y = pos.y;
	// translate
	if (d.x != 0 || d.y != 0) {
		//buttons
		for (auto& btn : _buttons) {
			btn.translate(d);
		}
		//carousels
		for (auto& carousel : _carousels) {
			carousel.translate(d);
		}
		//bg
		_bgBounds.translate(d);
	}
}

vector<Button*> Menu::hover(vector<ofVec2f> positions) {
	vector<Button*> btns;
	for (auto& btn : _buttons) {
		bool hover = false;
		for (auto& pos : positions) {
			if (btn.hitTest(pos)) {
				hover = true;
				break;
			}
		}
		if (hover) {
			btn.hover();
			btns.push_back(&btn);
		}
		else {
			btn.noHover();
		}
	}
	// for carousel only check first position that's inside bounds
	for (auto& carousel : _carousels) {
		bool notInside = true;
		for (auto& pos : positions) {
			if (carousel.inside(pos)) {
				Button* hitItem = carousel.hover(pos);
				if (hitItem != nullptr) {
					btns.push_back(hitItem);
				}
				notInside = false;
				break;
			}
		}
		if (notInside) carousel.noHover();
	}
	return btns;
}

void Menu::resetHover() {
	for (auto& btn : _buttons) {
		btn.noHover();
	}
	for (auto& carousel : _carousels) {
		carousel.noHover();
	}
}

void Menu::update() {
	for (auto& btn : _buttons) {
		btn.update();
	}
	for (auto& carousel : _carousels) {
		carousel.update();
	}
}

void Menu::draw() {
	if (_bDrawBg) {
		_bg.draw(_bgBounds);
	}
	for (auto& btn : _buttons) {
		btn.draw();
	}
	for (auto& carousel : _carousels) {
		carousel.draw();
	}
}
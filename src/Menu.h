#pragma once
#include "ofMain.h"
#include "Button.h"
#include "Carousel.h"

class Menu {

public:
	Menu(string name="", float hoverWait = 1.0, float x = 0, float y = 0, float width = 0, float height = 0);

	void setBackground(ofImage bg, float x, float y, float w, float h);
	void addButton(ofImage img, string name, float x, float y, float w, float h, ofImage hoverImg=ofImage());

	bool addCarousel(string name, vector<ofImage>& items, vector<string> names, float x, float y, float w, float itemH, int nItemsDisplay=3, float itemGap=10, ofImage hoverImg=ofImage());

	void setPos(ofVec2f pos);
	void setPos(float x, float y) { setPos(ofVec2f(x, y)); }
	ofVec2f getPos() { return ofVec2f(_x, _y); }
	ofVec2f getOrigPos() { return _origPos; }

	void resetPos() { setPos(_origPos); }
	void setHoverWait(float seconds);

	string getName() {
		return _name;
	}
	void setName(string name) {
		_name = name;
	}

	bool carouselUp(int index) {
		if (index >= 0 && index <= _carousels.size()) {
			_carousels[0].prevPage();
			return true;
		}
		return false;
	}
	bool carouselDown(int index) {
		if (index >= 0 && index <= _carousels.size()) {
			_carousels[0].nextPage();
			return true;
		}
		return false;
	}
	vector<Carousel>& getCarousels() {
		return _carousels;
	}
	vector<Button>& getButtons() {
		return _buttons;
	}

	vector<Button*> hover(vector<ofVec2f> positions);
	// returns button(s)/item(s) hovered
	void resetHover();

	void update();

	void draw();

protected:
	string _name;
	float _x, _y, _w, _h; ofVec2f _origPos;
	float _hoverWait = 1.0; // 1 sec default

	ofImage _bg;
	ofRectangle _bgBounds;
	bool _bDrawBg = false;

	vector<Button> _buttons;
	vector<Carousel> _carousels;
};
#pragma once
#include "ofMain.h"

class Button {
public:

	Button() {};
	Button(ofImage img, string name, ofVec2f pos, float w, float h, float hoverWait = 1.0) :
		_img(img), _name(name), _hoverWait(hoverWait)
	{
		_bounds.set(pos, w, h);
	}

	void setHoverImg(ofImage img) {
		_hoverImg = img;
	}

	bool hover(ofVec2f pos) {
		if (!hidden && hitTest(pos)) {
			hover();
			return true;
		}
		noHover();
		return false;
	}

	void hover() {
		if (_hoverStart == 0)
			_hoverStart = ofGetElapsedTimef(); // start hover
	}

	void noHover() {
		_hoverStart = 0;
	}

	bool hitTest(ofVec2f pos) {
		return _bounds.inside(pos);
	}

	float getHoverTime() {
		if (_hoverStart != 0)
			return ofGetElapsedTimef() - _hoverStart;
		return 0;
	}

	bool isSelected() {
		if (_hoverStart != 0)
			return ofGetElapsedTimef() - _hoverStart >= _hoverWait;
		return false;
	}

	ofVec2f getPos() {
		return _bounds.position;
	}

	void setPos(ofVec2f pos) {
		_bounds.setPosition(pos);
	}

	void translate(ofVec2f d) {
		_bounds.translate(d);
	}

	float getWidth() {
		return _bounds.width;
	}
	float getHeight() {
		return _bounds.height;
	}
	void setWidth(float w) {
		_bounds.setWidth(w);
	}
	void setHeight(float h) {
		_bounds.setHeight(h);
	}
	ofVec2f getSize() {
		return ofVec2f(_bounds.width, _bounds.height);
	}
	void setSize(float w, float h) {
		_bounds.setSize(w, h);
	}

	ofRectangle& getBounds() { return _bounds; }

	void setHoverWait(float wait) {
		_hoverWait = wait;
	}

	string getName() {
		return _name;
	}
	void setName(string name) {
		_name = name;
	}

	void update() {}

	void hide(bool y = true) {
		hidden = y;
	}
	void unhide() { hidden = false; }
	bool isHidden() {
		return hidden;
	}

	void draw(bool drawHover = true) {
		if (hidden) return;
		_img.draw(_bounds);
		if (drawHover && _hoverStart != 0) { // highlight if hover
			ofPushStyle();
			float alpha = ofMap(ofGetElapsedTimef() - _hoverStart, 0, _hoverWait, 0, 255);
			ofSetColor(255, alpha);
			_hoverImg.draw(_bounds);
			ofPopStyle();
		}
	}

protected:
	ofImage _img, _hoverImg;
	string _name;
	ofRectangle _bounds;

	float _hoverStart = 0;
	float _hoverWait = 1.0;

	bool hidden = false;
};
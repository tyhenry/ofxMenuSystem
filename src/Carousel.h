#pragma once
#include "ofMain.h"
#include "Button.h"

class Carousel {
public:
	Carousel(string name =""): _name(name) {}
	bool setup(vector<ofImage>& items, vector<string> names,
		float x, float y, float w, float itemH,
		int nItemsDisplay = 3, float itemGap = 10, ofImage hoverImg = ofImage())
	{
		if (items.size() != names.size() || items.size() == 0 || nItemsDisplay <= 0)
			return false;

		//dims
		_iW = w; _iH = itemH;
		_nItemsDisp = nItemsDisplay;
		_gap = itemGap;
		float h = _nItemsDisp * _iH + (_nItemsDisp - 1) * _gap;
		_dispBounds.set(x, y, w, h);

		// items
		_items.clear();
		float iX = x, iY = y;
		for (int i = 0; i < items.size(); i++) {
			_items.push_back(Button(items[i], names[i], ofVec2f(iX, iY), _iW, _iH));
			if (hoverImg.isAllocated()) _items.back().setHoverImg(hoverImg);
			iY += _iH + _gap;
		}

		// fbo
		_fbo.allocate(w, h, GL_RGBA);
		_fbo.begin(); ofClear(0); _fbo.end();
		return true;

	}

	Button* hover(ofVec2f pos) {
		Button* hit = nullptr;
		if (_dispBounds.inside(pos)) {
			for (auto& item : _items) {
				if (item.hover(pos)) hit = &item;
				else item.noHover();
			}
		}
		else noHover();
		return hit;
	}

	bool inside(ofVec2f pos) {
		return _dispBounds.inside(pos);
	}

	void noHover() {
		for (auto& item : _items) 
			item.noHover();
	}

	void nextPage() {
		if (_bScrollBackAfterEnd && _topIdx >= _items.size() - _nItemsDisp) {
			scrollTo(0);
			return;
		}
		int newIdx = _topIdx + _nItemsDisp;
		if (newIdx >= _items.size()) {
			newIdx = _items.size() - _nItemsDisp;
		}
		scrollTo(newIdx);
	}
	void prevPage() {
		if (_bScrollEndAfterBack && _topIdx == 0 && _items.size() > _nItemsDisp) {
			scrollTo(_items.size() - _nItemsDisp);
			return;
		}
		int newIdx = _topIdx - _nItemsDisp;
		if (newIdx < 0) newIdx = 0;
		scrollTo(newIdx);
	}

	void setPosition(const ofVec2f& pos) {
		ofVec2f offset = pos - _dispBounds.position;
		_dispBounds.setPosition(pos);
		for (auto& item : _items) {
			item.setPos(item.getPos() + offset);
		}
	}

	void translate(const ofVec2f& amt) {
		_dispBounds.setPosition(_dispBounds.position + amt);
		for (auto& item : _items) {
			item.setPos(item.getPos() + amt);
		}
	}

	void setHoverWait(float hoverWait) {
		for (auto& item : _items) {
			item.setHoverWait(hoverWait);
		}
	}

	string getName() {
		return _name;
	}
	void setName(string name) {
		_name = name;
	}

	void draw() {
		_fbo.begin();
		ofClear(0);
		ofPushMatrix();
		ofTranslate(-_dispBounds.x, -_dispBounds.y); // draw items at 0,0 in fbo
		for (auto& item : _items) {
			item.draw();
		}
		ofPopMatrix();
		_fbo.end();
		_fbo.draw(_dispBounds);
	}
	void endAnimation() {
		if (_animYShift == 0) return;
		scrollToNow(_animTargetIdx); // clamp to target
		// clear animation
		_animYShift = 0; _animStart = 0; _animEnd = 0;
	}
	void update() {
		if (_animYShift != 0) doAnimation();
	}
	void setAnimate(bool on) {
		_bAnimate = on;
	}
	bool getAnimate() {
		return _bAnimate;
	}
	void setAnimationSpeed(float speed) {
		_animSpeed = speed;
	}
	float getAnimationSpeed() {
		return _animSpeed;
	}
	void setScrollWrap(bool afterEnd, bool beforeFront) {
		_bScrollBackAfterEnd = afterEnd;
		_bScrollEndAfterBack = beforeFront;
	}

protected:
	string _name;
	bool scrollTo(int topIdx) {
		if (_bAnimate) return scrollToAnimate(topIdx);
		else return scrollToNow(topIdx);
	}
	bool scrollToNow(int topIdx) {
		if (!isIndexValid(topIdx)) {
			ofLogError("carousel") << "couldn't shift to index out of bounds: " << topIdx;
			return false;
		}

		float yShift = _dispBounds.y - _items[topIdx].getPos().y;
		shift(yShift);
		_topIdx = topIdx;
		ofLogVerbose("carousel") << "shifted to index: " << topIdx;
		return true;
	}
	bool scrollToAnimate(int topIdx) {
		if (_animYShift != 0) return false; // complete current animation first
		if (!isIndexValid(topIdx)) {
			ofLogError("carousel") << "couldn't shift to index out of bounds: " << topIdx;
			return false;
		}
		// start new animation
		_animYShift = _dispBounds.y - _items[topIdx].getPos().y;
		_animStart = ofGetElapsedTimef();
		float duration = abs(_animYShift) / (_iH + _gap) * _animSpeed;
		duration = min(duration, _maxAnimTime); // clamp at max
		_animEnd = _animStart + duration;
		_animTargetIdx = topIdx;
		return true;
	}
	void doAnimation() {
		if (_animYShift == 0) return; // no animation
		float curTime = ofGetElapsedTimef();
		if (curTime >= _animEnd) { // clamp
			scrollToNow(_animTargetIdx); // clamp to target
			// clear animation
			_animYShift = 0; _animStart = 0; _animEnd = 0;
			return;
		}
		// calc new y position for target index
		float pct = ofMap(curTime, _animStart, _animEnd, 0, 1, true);
		// ease
		pct = standardEase(pct);
		float newY = (_dispBounds.y - _animYShift) + _animYShift * pct;
		shiftIndexTo(_animTargetIdx, newY);
	}
	void shift(float yAmt) { // positive = move items down, negative = move items up
		for (auto& item : _items) {
			item.setPos(item.getPos() + ofVec2f(0, yAmt));
		}
	}
	void shiftIndexTo(int idx, float y) {
		if (!isIndexValid(idx)) return;
		float yShift = y - _items[idx].getPos().y;
		shift(yShift);
	}
	bool isIndexValid(int idx) {
		if (idx >= 0 && idx < _items.size()) return true;
		return false;
	}

	// easing

	float easeInOut(const float p) {
		if (p < 0.5) return 4 * p * p * p; // cubic // quadratic: return 2 * p * p;
		return (-2 * p * p) + (4 * p) - 1; // quadratic
	}
	float standardEase(const float p) {
		return cubicBezier(p, 0.4, 0, 0.2, 1); // material design standard motion curve
	}


	float _iW, _iH, _gap;
	int _topIdx = 0;
	ofRectangle _dispBounds;
	ofFbo _fbo;
	int _nItemsDisp;
	vector<Button> _items;

	bool _bScrollBackAfterEnd = true; // whether the carousel should scroll back up after last page
	bool _bScrollEndAfterBack = true; // whether the caroulse should scroll to last page if prevPage() on first
	bool _bAnimate = true;
	float _animYShift = 0; // total distance of current animation
	float _animStart = 0; // time animation started
	float _animEnd = 0; // time animation will end
	int _animTargetIdx = 0; // index of item animation will end on
	float _animSpeed = 0.3; // time it takes to move one index (_iH+_gap pixels)
	float _maxAnimTime = 0.7; // maximum animation time


	// cubic bezier function for material design standard curve:

	float cubicBezier(float p, float x1, float y1, float x2, float y2) {
		// from http://www.flong.com/texts/code/shapers_bez/

		float y0a = 0.00; // initial y
		float x0a = 0.00; // initial x 
		float y1a = y1;    // 1st influence y   
		float x1a = x1;    // 1st influence x 
		float y2a = y2;    // 2nd influence y
		float x2a = x2;    // 2nd influence x
		float y3a = 1.00; // final y 
		float x3a = 1.00; // final x 

		float A = x3a - 3 * x2a + 3 * x1a - x0a;
		float B = 3 * x2a - 6 * x1a + 3 * x0a;
		float C = 3 * x1a - 3 * x0a;
		float D = x0a;

		float E = y3a - 3 * y2a + 3 * y1a - y0a;
		float F = 3 * y2a - 6 * y1a + 3 * y0a;
		float G = 3 * y1a - 3 * y0a;
		float H = y0a;

		// Solve for t given x (using Newton-Raphelson), then solve for y given t.
		// Assume for the first guess that t = x.
		float currentt = p;
		int nRefinementIterations = 5;
		for (int i = 0; i < nRefinementIterations; i++) {
			float currentx = xFromT(currentt, A, B, C, D);
			float currentslope = slopeFromT(currentt, A, B, C);
			currentt -= (currentx - p)*(currentslope);
			currentt = constrain(currentt, 0, 1);
		}

		float y = yFromT(currentt, E, F, G, H);
		return y;
	}
	// Helper functions:
	float slopeFromT(float t, float A, float B, float C) {
		float dtdx = 1.0 / (3.0*A*t*t + 2.0*B*t + C);
		return dtdx;
	}

	float xFromT(float t, float A, float B, float C, float D) {
		float x = A*(t*t*t) + B*(t*t) + C*t + D;
		return x;
	}

	float yFromT(float t, float E, float F, float G, float H) {
		float y = E*(t*t*t) + F*(t*t) + G*t + H;
		return y;
	}

	float constrain(float v, float min, float max) {
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}
};
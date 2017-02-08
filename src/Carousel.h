#pragma once
#include "ofMain.h"
#include "Button.h"

class Carousel {
public:

	Carousel(string name ="")
	: _name(name) 
	{}

	// setup

	bool setup
	(
		vector<ofImage>& items,			// button images
		vector<string> names,			// button names
		float x,						// carousel x pos
		float y,						// carousel y pos
		float itemW,					// button width
		float itemH,					// button height
		int nItemsDisplay = 3,			// num buttons to display at once
		float itemGap = 10,				// gap between buttons
		ofImage hoverImg = ofImage()	// button master hover image
	);

	// loop

	void update() {						// update only needed for animation
		if (_animYShift != 0) 
			doAnimation();
	}
	void draw();						// draw

	// set/get name

	void setName(string name) { _name = name;  }
	string getName() { return _name; }

	// change position

	void setPosition(const ofVec2f& pos);
	void translate(const ofVec2f& amt);
	ofRectangle getBounds() { return _dispBounds; }

	// hover logic

	Button* hover(ofVec2f pos);						// return hovered button (if any)
	bool inside(ofVec2f pos) {						// test if pos is inside carousel
		return _dispBounds.inside(pos); 
	}
	void noHover() {								// reset hover states for all buttons
		for (auto& item : _items) 
		item.noHover(); 
	}
	void setHoverWait(float hoverWait) {			// set time to wait before hover -> select
		for (auto& item : _items) 
		item.setHoverWait(hoverWait); 
	}

	// paging

	void nextPage();
	void prevPage();
	bool isAtTop() {								// is carousel at start
		return (_topIdx == 0); 
	}
	bool isAtBottom() {								// is carousel at end
		return (
		_topIdx >= _items.size() - _nItemsDisp);	
	}

	// animation

	void endAnimation() {
		if (_animYShift == 0) return;
		scrollToNow(_animTargetIdx); // clamp to target
		// clear animation
		_animYShift = 0; _animStart = 0; _animEnd = 0;
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
	string _name;						// carousel name

	// items
	vector<Button> _items;				// all items/buttons in carousel
	float _iW, _iH, _gap;				// item width, item height, gap between items
	ofRectangle _dispBounds;			// display area
	int _topIdx	= 0;	// current button index at top of display area
	int _nItemsDisp;					// num items to display at once
	ofFbo _fbo;							// fbo for pre-draw overflow

	// scroll
	bool _bScrollBackAfterEnd = true;	// whether the carousel should scroll back up after last page
	bool _bScrollEndAfterBack = true;	// whether the caroulse should scroll to last page if prevPage() on first

	// scroll animation
	bool _bAnimate = true;
	float _animYShift = 0;				// total distance of current animation
	float _animStart = 0;				// time in seconds animation started
	float _animEnd = 0;					// time in seconds animation will end
	int _animTargetIdx = 0;				// index of item animation will end on
	float _animSpeed = 0.3;				// time it takes to move one index (_iH+_gap pixels)
	float _maxAnimTime = 0.7;			// maximum animation time

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
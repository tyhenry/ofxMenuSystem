#include "Carousel.h"

bool Carousel::setup(vector<ofImage>& items, vector<string> names, float x, float y, float itemW, float itemH, int nItemsDisplay, float itemGap, ofImage hoverImg)
{
	if (items.size() != names.size() || items.size() == 0 || nItemsDisplay <= 0)
		return false;

	// setup display bounds
	_iW = itemW; 
	_iH = itemH;
	_nItemsDisp = nItemsDisplay;
	_gap = itemGap;
	float w = _iW;
	float h = _nItemsDisp * _iH + (_nItemsDisp - 1) * _gap;
	_dispBounds.set(x, y, w, h);

	// setup items/buttons
	_items.clear();
	float iX = x, iY = y;
	for (int i = 0; i < items.size(); i++) {
		_items.push_back(Button(items[i], names[i], ofVec2f(iX, iY), _iW, _iH));
		if (hoverImg.isAllocated())
			_items.back().setHoverImg(hoverImg);
		iY += _iH + _gap;
	}

	// setup fbo for pre-draw
	_fbo.allocate(w, h, GL_RGBA);
	_fbo.begin(); ofClear(0); _fbo.end();
	return true;

}

Button * Carousel::hover(ofVec2f pos)
{
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

void Carousel::nextPage() {
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
void Carousel::prevPage() {
	if (_bScrollEndAfterBack && _topIdx == 0 && _items.size() > _nItemsDisp) {
		scrollTo(_items.size() - _nItemsDisp);
		return;
	}
	int newIdx = _topIdx - _nItemsDisp;
	if (newIdx < 0) newIdx = 0;
	scrollTo(newIdx);
}

void Carousel::setPosition(const ofVec2f& pos) {
	ofVec2f offset = pos - _dispBounds.position;
	_dispBounds.setPosition(pos);
	for (auto& item : _items) {
		item.setPos(item.getPos() + offset);
	}
}

void Carousel::translate(const ofVec2f& amt) {
	_dispBounds.setPosition(_dispBounds.position + amt);
	for (auto& item : _items) {
		item.setPos(item.getPos() + amt);
	}
}

void Carousel::draw() {
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


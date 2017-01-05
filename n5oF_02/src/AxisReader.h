#pragma once

#include "ofThread.h"
class ofApp;
class Axis;

class AxisReader : public ofThread {

public:

	AxisReader();
	void init(ofApp * _app);
	void start(int sleep_ms);
	void stop();
	void threadedFunction();

	int sleep_ms;
	ofApp * app;
};
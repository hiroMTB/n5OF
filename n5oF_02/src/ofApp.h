#pragma once

#include "ofMain.h"
#include "ofxLtcReader.h"
#include "ofxOsc.h"
#include "ofxSvg.h"
#include "AdsCommander.h"
#include "Axis.h"
#include "AxisReader.h"

class ofApp : public ofBaseApp {
public:

	static ofApp * app;
	static void init() { app = new ofApp(); }

	void setup();
	void update();
	void exit();

	void draw();
	void draw_dump();
	void draw_target();
	void draw_actual();
	void draw_info();
	void draw_rail_num();

	void keyPressed(int key);
	void sync_osc();
	void sync_standalone();
	int normalize_frame(int frame);

public:

	bool bStop;
	bool bOscReceived;
	bool bDrawVid;
	bool bFlip;
	int sync_mode;			// 0:standalone, 1:osc
	int current_frame;
	int max_frame;

	ofxOscReceiver oscR;
	int OSC_PORT;
	int sync_offset_frame;

	ofxSVG svg;
	float svg_scale;
	vector<ofVec2f> home_pos;

	ofVideoPlayer vid;
	vector<Axis> axis;

	AdsCommander adsCmd;
	AxisReader axisReader;

	vector<float> carib_min = { 250, 120, 210, 120, 200, 140, 150, 180, 90, 110 };
	vector<float> carib_max = { 4900, 4800, 4910, 4800, 4900, 4780, 4750, 4870, 4700, 4800 };
};

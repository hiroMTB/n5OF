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
	//void audioIn(float * input, int bufferSize, int nChannels);
	void audioIn(ofSoundBuffer& buffer);
	void sync_osc();
	void sync_smpte();
	void sync_standalone();

	int normalize_frame(int frame);

public:

	bool bStop;
	bool bDrawVid;
	int sync_mode;			// 0:standalone, 1:osc, 2:smpte
	float screen_scale;
	int current_frame;
	int max_frame;

	ofxLtcReader ltc;
	ofSoundStream snd;
	ofxOscReceiver oscR;
	int OSC_PORT;
	int sync_offset_frame;

	ofxSVG svg;
	float svg_scale;
	vector<ofVec2f> home_pos;

	ofVideoPlayer vid;
	AdsCommander adsCmd;
	vector<Axis> axis;

	int round(float val) {
		return (int)(std::floor(val + 0.5));
	}

	float ltcAmp;
	int smpteCh;

	AxisReader axisReader;
};

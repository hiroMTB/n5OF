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

	enum N5OF_MODE {
		N5OF_SYNC = 0,
		N5OF_TEST,
	};

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
	void audioIn(ofSoundBuffer& buffer);
	void sync_osc();
	void sync_smpte();
	void sync_standalone();
	int normalize_frame(int frame);
	int round(float val) { return (int)(std::floor(val + 0.5));	}

public:

	bool bStop;
	bool bDrawVid;
	int sync_mode;			// 0:standalone, 1:osc, 2:smpte
	float screen_scale;
	int current_frame;
	int max_frame;

	N5OF_MODE mode;
	ofxLtcReader ltc;
	ofSoundStream snd;
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

	float ltcAmp;
	int smpteCh;

};

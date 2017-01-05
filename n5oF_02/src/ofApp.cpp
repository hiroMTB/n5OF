#include "ofApp.h"

using namespace std;
#include <iostream>
#include <fstream>

extern double pix2mm;
extern double mm2pix;

ofApp * ofApp::app = NULL;

void ofApp::setup() {

	ofSetFrameRate(120);
	ofSetWindowShape(1500, 1000);
	ofSetWindowPosition(0, 0);

	svg.load("n5_guide.svg");
	svg_scale = 0.3333333;

	screen_scale = 1; //17500.0/18000.0;
	current_frame = 0;
	max_frame = 25 * 60 * 8;
	sync_offset_frame = 0;
	sync_mode = 0;

	axis.assign(10, Axis());

	axis[0].init(1);
	axis[1].init(0);

	for (int i = 2; i<axis.size(); i++) {
		axis[i].init(i);
	}

	// SMPTE LTC setup
	snd.listDevices();
	snd.setDeviceID(3);
	snd.setup(this, 0, 2, 48000, 64, 4);
	ltc.setup(&snd, 25);

	// OSC
	OSC_PORT = 9999;
	oscR.setup(OSC_PORT);
	bStop = false;

	home_pos.assign(10, ofVec2f(0, 0));
	home_pos[0] = ofVec2f(480, 192);      // L1
	home_pos[1] = ofVec2f(840, 576);      // L2
	home_pos[2] = ofVec2f(120, 960);      // L3
	home_pos[3] = ofVec2f(840, 1344);     // L4
	home_pos[4] = ofVec2f(480, 1728);     // L5

	home_pos[5] = ofVec2f(2640, 192);     // R1
	home_pos[6] = ofVec2f(2280, 576);     // R2
	home_pos[7] = ofVec2f(3000, 960);     // R3
	home_pos[8] = ofVec2f(2280, 1344);    // R4
	home_pos[9] = ofVec2f(2640, 1728);    // R5

	for (int i = 0; i<5; i++) {
		home_pos[i].x = home_pos[i].x + 1200;
	}

	if (vid.loadMovie("n5_alpha_s_h264.mov"))
		cout << "load movie OK" << endl;
	else 
		cout << "load movie ERROR" << endl;
	

	bDrawVid = true;

	adsCmd.init();
	adsCmd.enableAll();

	axisReader.init(app);
	axisReader.start(4);
}

void ofApp::update() {

	if (!bStop) {
		switch (sync_mode) {
		case 0: sync_standalone(); break;
		case 1: sync_osc();   break;
		case 2: sync_smpte(); break;
		default: break;
	}

		for (int i = 0; i<axis.size(); i++) {
			Axis &a = axis[i];
			a.sendCmd(current_frame);
		}
	}

	if (current_frame != vid.getCurrentFrame() && current_frame<vid.getTotalNumFrames()) {
		vid.setFrame(current_frame);
		vid.update();
	}
}

//void ofApp::audioIn(float *input, int bufferSize, int nChannels) {
void ofApp::audioIn( ofSoundBuffer & buf ){
	float sum = 0;

	if (sync_mode == 2) {
		vector<float> & input = buf.getBuffer();
		int nCh = buf.getNumChannels();

		ltc.readLtc(&input[0], nCh, 1);
		int m = ltc.ltcMinute();
		int s = ltc.ltcSecond();
		int f = ltc.ltcFrame();
		current_frame = (m * 60 + s) * 25 + f;

		//current_frame = normalize_frame(current_frame);

		current_frame %= max_frame;

		
		//int bufferSize = buf.getNumFrames();
		//for(int i=0; i<bufferSize; i++){
		//	sum += abs(input[i*nCh]);
		//}

		//ltcAmp = sum/bufferSize;
		ltcAmp = buf.getRMSAmplitudeChannel(0);
	}
}

void ofApp::sync_standalone() {
	current_frame = normalize_frame(current_frame + 1);
}

void ofApp::sync_osc() {
	while (oscR.hasWaitingMessages()) {
		ofxOscMessage m;
		oscR.getNextMessage(&m);

		if (m.getAddress() == "/frame/1") {
			float frame = m.getArgAsFloat(0);     // ToscA output 0 - 12000
			current_frame = normalize_frame(round(frame));
		}
		else {
			cout << "OSC error, unknown messages" << endl;
			cout << "address : " << m.getAddress() << endl;
		}
	}
}

void ofApp::sync_smpte() {
}

int ofApp::normalize_frame(int frame) {
	frame %= max_frame;
	if (frame<0) {
		frame = max_frame + frame;
	}
	return frame;
}

void ofApp::draw() {

	ofSetLineWidth(1);
	ofBackground(0);
	ofTranslate(30, 30);

	if (bDrawVid) {
		ofPushMatrix(); {
			ofSetColor(255);
			vid.draw(0, 0);
		} ofPopMatrix();
	}

	ofPushMatrix(); {
		ofScale(svg_scale, svg_scale, 1);
		svg.draw();
		draw_target();
		draw_dump();
		draw_actual();
		draw_rail_num();
	} ofPopMatrix();

	ofPushMatrix(); {
		ofTranslate(0, 700);
		draw_info();
	} ofPopMatrix();

}

void ofApp::draw_target() {
	// target, mm
	for (int i = 0; i<axis.size(); i++) {
		const Axis &a = axis[i];
		if (a.tarpos != -123) {
			float pixpos = a.tarpos*mm2pix / a.cmd_scale;
			float x = home_pos[i].x;
			float y = home_pos[i].y;
			if (i<5) x -= pixpos;
			else    x += pixpos;

			ofSetColor(255, 0, 0);
			ofCircle(x, y, 6);
			ofDrawBitmapString(ofToString(a.tarpos, 2), x - 12, y - 18);
		}
	}
}

void ofApp::draw_dump() {

	for (int i = 0; i<axis.size(); i++) {
		const Axis & a = axis[i];
		float x = home_pos[i].x;
		float y = home_pos[i].y;

		float pixpos = a.dump[current_frame].pos;
		if (i<5) {
			x -= pixpos;
		}
		else {
			x += pixpos;
		}

		int onoff = a.dump[current_frame].onoff;
		ofSetColor(0, 255, 0);

		ofFill();
		ofCircle(x, y, 6);
		if (onoff == 255) {
			ofNoFill();
			ofSetColor(255, 0, 0);
			ofCircle(x, y, 75);
		}

		float realpos = pixpos * pix2mm * a.cmd_scale;
		ofDrawBitmapString(ofToString(realpos, 2), x - 12, y + 55);
	}
}

void ofApp::draw_actual() {
	// actual, mm
	for (int i = 0; i<axis.size(); i++) {
		const Axis &a = axis[i];
		if (a.actpos != -123) {
			float pixpos = a.actpos *mm2pix / a.cmd_scale;
			float x = home_pos[i].x;
			float y = home_pos[i].y;
			if (i<5) x -= pixpos;
			else    x += pixpos;

			ofSetColor(255);
			ofCircle(x, y, 6);
			ofDrawBitmapString(ofToString(a.actpos, 2), x - 12, y + 100);
		}
	}
}

void ofApp::draw_info() {

	char mm[255];
	stringstream ss;
	ss << "Fps       : " << ofGetFrameRate() << "\n";
	ss << "Sync Mode : ";
	switch (sync_mode) {
	case 0: ss << "Stand Alone" << "\n"; break;
	case 1: ss << "OSC, port " << OSC_PORT << "\n"; break;
	case 2:
		ss << "SMPTE LTC : amp " << ltcAmp << "\n";
		break;
	}

	ss << "Frame     : " << current_frame << "\n";
	ss << "Time sec  : " << current_frame / 25.0 << "\n" << "\n";


	sprintf(mm, "%-4s  %-5s  %-20s  %-8s  %-8s  %-8s  %-8s", "Axis", "state", "name", "tarpos", "actpos", "tarvel", "actvel");
	ss << mm << "\n";

	for (int i = 0; i<axis.size(); i++) {
		Axis &a = axis[i];
		sprintf(mm, "%4d  %5d  %20s  %8.2f  %8.2f  %8.2f  %8.2f", i + 1, a.state, a.name.c_str(), a.tarpos, a.actpos, a.tarvel, a.actvel);
		ss << mm << "\n";
	}

	ofSetColor(255);
	ofDrawBitmapString(ss.str(), 0, 0);
}

void ofApp::draw_rail_num() {

	for (int i = 0; i<10; i++) {
		float x = home_pos[i].x;
		float y = home_pos[i].y + 10;
		if (i<5) x += 45;
		else    x -= 70;
		ofSetColor(150, 0, 0);
		ofDrawBitmapString(ofToString(i), x, y);
		ofNoFill();
		ofRect(x - 15, y - 40, 50, 50);
	}
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case '0': sync_mode = 0; break;
	case '1': sync_mode = 1; break;
	case '2': sync_mode = 2; break;
	case ' ': bStop = !bStop; break;
	case '>': current_frame = normalize_frame(current_frame + 100);  break;
	case '<': current_frame = normalize_frame(current_frame - 100); break;
	case OF_KEY_RIGHT: current_frame = normalize_frame(current_frame + 1); break;
	case OF_KEY_LEFT: current_frame = normalize_frame(current_frame - 1); break;

	case 'e': adsCmd.enableAll(); 			break;
	case 'r': adsCmd.disableAll();			break;
	case 'v': bDrawVid = !bDrawVid; break;
	}
}

void ofApp::exit() {
	adsCmd.disableAll();
	snd.stop();
	snd.close();
	axisReader.stop();
}
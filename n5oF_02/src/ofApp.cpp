#include "ofApp.h"

extern double pix2mm;
extern double mm2pix;

ofApp * ofApp::app = NULL;

void ofApp::setup() {

	ofSetEscapeQuitsApp(false);
	ofSetFrameRate(120);
	ofSetFullscreen(false);
	ofSetWindowShape(1500, 1080);
	ofSetWindowPosition(0, 30);
	svg_scale = 0.3333333;
	bFlip = true;

	svg.load("n5_guide.svg");

	current_frame = 0;
	max_frame = 25 * 60 * 8;
	sync_offset_frame = -2;
	sync_mode = 1;

	axis.assign(10, Axis());


	for (int i = 0; i<axis.size(); i++) {
		axis[i].init(i, carib_min[i], carib_max[i]);
	}

	// OSC
	OSC_PORT = 64008;
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
	
	vid.play();
	bDrawVid = true;

	adsCmd.init();
	adsCmd.enableAll();

	axisReader.init(app);
	axisReader.start(20);
	
}

void ofApp::update() {

	if (!bStop) {
			switch (sync_mode) {
			case 0: sync_standalone(); break;
			case 1: sync_osc();   break;
			default: break;
			}

			for (int i = 0; i < axis.size(); i++) {
				Axis &a = axis[i];
				a.update(current_frame);
			}
	}

	if (current_frame!=vid.getCurrentFrame() && current_frame<vid.getTotalNumFrames()) {
		vid.setFrame(current_frame);
		vid.update();
	}
}

void ofApp::sync_standalone() {
	current_frame = normalize_frame(current_frame+1);
}

void ofApp::sync_osc() {

	bOscReceived = false;

	while (oscR.hasWaitingMessages()) {

		bOscReceived = true;
		ofxOscMessage m;
		oscR.getNextMessage(&m);

		if (m.getAddress() == "/data") {
			float frame = m.getArgAsFloat(0) * 37500.0;     // ToscA output 0 - 32
			frame += sync_offset_frame;
			current_frame = normalize_frame(floor(frame));
		}
		else {
			cout << "OSC error, unknown messages" << endl;
			cout << "address : " << m.getAddress() << endl;
		}
	}
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

	ofPushMatrix();
	if (bFlip) {
		ofTranslate(1440, 0, 0);
		ofScale(-1, 1, 1);
	}
		if (bDrawVid) {
			ofPushMatrix(); {
				ofSetHexColor(0xFFFFFF);
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

	ofPopMatrix();

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

			ofSetColor( 0, 255, 0);
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

		x += a.cmd_offset*mm2pix*a.cmd_scale;

		int onoff = a.dump[current_frame].onoff;
		ofSetColor(255, 0, 0);

		ofFill();
		ofCircle(x, y, 6);
		if (onoff == 255) {
			ofNoFill();
			ofSetColor(255, 0, 0);
			ofCircle(x, y, 75);
		}

		float realpos = pixpos * pix2mm * a.cmd_scale + a.cmd_offset;
		ofDrawBitmapString(ofToString(realpos, 2), x - 12, y + 55);

		axis[i].dumpPos = realpos;
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
	ss << "Flip      : " << (bFlip ? "ON" : "OFF") << "\n";
	ss << "Fps       : " << ofGetFrameRate() << "\n";
	ss << "Sync Mode : ";
	switch (sync_mode) {
	case 0: ss << "Stand Alone" << "\n"; break;
	case 1: 
		ss << "OSC, port " << OSC_PORT;
		if (bOscReceived) ss << ", message received" << "\n";
		else ss << "\n";
 		break;
	}

	ss << "Frame     : " << current_frame << "\n";
	ss << "Time sec  : " << current_frame / 25.0 << "\n" << "\n";

	sprintf(mm, "%-4s  %-20s  %-8s  %-8s  %-8s  %-8s  %-8s  %-8s  %-8s  %-8s  %-15s  %-3s",
		"Axis", "name", "tarpos", "actpos", "tarvel", "actvel", "Error", "ErrorID", "Disabled", "Homed", "motion state", "prg" );
	ss << mm << "\n";

	for (int i = 0; i<axis.size(); i++) {
		Axis &a = axis[i];
		sprintf(mm, "%4d  %20s  %8.2f  %8.2f  %8.2f  %8.2f  %8d  %8d  %8d  %8d  %15s  %3d",
				i + 1, a.name.c_str(), a.tarpos, a.actpos, a.tarvel, a.actvel, a.Error, a.ErrorID, a.Disabled, a.Homed, Axis::MC_MotionSate[a.motion_state], a.state);
		ss << mm << "\n";
	}

	ofSetColor(255);
	ofDrawBitmapString(ss.str(), 0, 0);
}

void ofApp::draw_rail_num() {

	for (int i = 0; i<10; i++) {
		float x = home_pos[i].x;
		float y = home_pos[i].y-8;
		if (i<5) x += 45;
		else    x -= 70;
		ofSetColor(150, 0, 0);
		ofDrawBitmapString(ofToString(i+1), x, y);
		//ofNoFill();
		//ofRect(x - 15, y - 40, 50, 50);
	}
}

void ofApp::keyPressed(int key) {
	switch (key) {
		case '0': sync_mode = 0; ofSetFrameRate(25);  break;	 // standalone
		case '1': sync_mode = 1; ofSetFrameRate(120); break; // osc
		case ' ': bStop = !bStop; break;
		case '>': current_frame = normalize_frame(current_frame + 100);  break;
		case '<': current_frame = normalize_frame(current_frame - 100); break;
		case OF_KEY_RIGHT: current_frame = normalize_frame(current_frame + 1); break;
		case OF_KEY_LEFT: current_frame = normalize_frame(current_frame - 1); break;

		case 'e': adsCmd.enableAll(); 		break;
		case 'r': adsCmd.disableAll();		break;
		case 'v': bDrawVid = !bDrawVid;		 break;
		case 'h': adsCmd.haltAll();			 break;
		//case 'o': adsCmd.homeAll();			 break;
		case 'g': adsCmd.goToAll(ofRandom(500,4500), 2000); break;
		case 'f': bFlip = !bFlip;			 break;
		case 'd':
			for (int i = 0; i < 10; i++) {
				if (axis[i].Homed && !axis[i].Error) {
					if(50<axis[i].dumpPos && axis[i].dumpPos<4950)
						adsCmd.goTo(axis[i].name, axis[i].dumpPos, 2000);
				}
			}
		break;
	}
}

void ofApp::exit() {
	adsCmd.disableAll();
	if (axisReader.isThreadRunning()) {
		axisReader.stop();
	}
}
#include "AdsCommander.h"
#include "ofApp.h"

ofxBeckhoffADS AdsCommander::ads;

AdsCommander::AdsCommander() {
}

void AdsCommander::init() {

	// TwinCAT 3, normaly start from port 851
	ads.openLocal(851);

}

void AdsCommander::enableAll() {
	for (int i = 0; i<10; i++) {
		const Axis & a = ofApp::app->axis[i];
		ads.writeValue<short>(a.name + ".state", 2);
	}
}

void AdsCommander::disableAll() {
	for (int i = 0; i<10; i++) {
		const Axis & a = ofApp::app->axis[i];
		ads.writeValue<short>(a.name + ".state", 0);
	}
}

void AdsCommander::haltAll() {
	for (int i = 0; i<10; i++) {
		const Axis & a = ofApp::app->axis[i];
		ads.writeValue<bool>(a.name + ".cmd_halt", true);
	}
}

void AdsCommander::goToAll(int pos) {
	for (int i = 0; i<10; i++) {
		const Axis & a = ofApp::app->axis[i];
		ads.writeValue<double>(a.name + ".cmd_pos", pos);
		ads.writeValue<double>(a.name + ".cmd_vel", 500);
		ads.writeValue<bool>(a.name + ".cmd_need_exe", true);
	}
}

void AdsCommander::sendCmd(string name, double tarpos, double tarvel) {
	ads.writeValue<double>(name + ".cmd_pos", tarpos);
	ads.writeValue<double>(name + ".cmd_vel", tarvel);
	ads.writeValue<bool>(name + ".cmd_need_exe", true);
}

void AdsCommander::readStatus(Axis &a) {
	a.actpos = ads.readValue<double>(a.name + ".actual_pos", -123.0);
	a.actvel = ads.readValue<double>(a.name + ".actual_vel", -123);
	//a.actacc = ads.readValue<double>(a.name[i]+".actual_acc", -123);
	a.state = ads.readValue<short>(a.name + ".state", -123);
}

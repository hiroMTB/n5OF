#include "AdsCommander.h"
#include "ofApp.h"

ofxBeckhoffADS AdsCommander::ads;

AdsCommander::AdsCommander() {
}

void AdsCommander::init() {
	// TwinCAT 3, PLC programm normaly start from port 851
	ads.openLocal(851);
}

void AdsCommander::enable( string name){
	ads.writeValue<short>(name + ".state", 2);
}

void AdsCommander::disable( string name) {
	ads.writeValue<short>(name + ".state", 0);
}

void AdsCommander::halt( string name) {
	ads.writeValue<bool>(name + ".cmd_halt", true);
}

void AdsCommander::goTo( string name, int pos, int vel ) {
	ads.writeValue<double>(name + ".cmd_pos", pos);
	ads.writeValue<double>(name + ".cmd_vel", vel);
	ads.writeValue<bool>(name + ".cmd_need_exe", true);
}

void AdsCommander::home( string name) {
	ads.writeValue<bool>(name + ".cmd_need_home", true);
}

void AdsCommander::enableAll() {
	for (int i = 0; i<10; i++) {
		enable(ofApp::app->axis[i].name);
	}
}

void AdsCommander::disableAll() {
	for (int i = 0; i<10; i++) {
		disable(ofApp::app->axis[i].name);
	}
}

void AdsCommander::haltAll() {
	for (int i = 0; i<10; i++) {
		halt(ofApp::app->axis[i].name);
	}
}

void AdsCommander::homeAll() {
	for (int i = 0; i<10; i++) {
		home(ofApp::app->axis[i].name);
	}
}

void AdsCommander::goToAll(int pos, int vel) {
	for (int i = 0; i<10; i++) {
		goTo(ofApp::app->axis[i].name, pos, vel);
	}
}

void AdsCommander::readStatus(Axis &a) {
	a.actpos = ads.readValue<double>(a.name + ".actual_pos", -123);
	a.actvel = ads.readValue<double>(a.name + ".actual_vel", -123);
	//a.actacc = ads.readValue<double>(name[i]+".actual_acc", -123);
	a.state = ads.readValue<short>(a.name + ".state", -123);

	a.Error = ads.readValue<bool>(a.name + ".axis.Status.Error", false);
	a.ErrorID = ads.readValue<int>(a.name + ".axis.Status.ErrorID", -123);
	a.Disabled = ads.readValue<bool>(a.name + ".axis.Status.Disabled", false);
	a.Homed = ads.readValue<bool>(a.name + ".axis.Status.Homed", false);
	a.motion_state= ads.readValue<short>(a.name + ".axis.Status.MotionState", -123);

}

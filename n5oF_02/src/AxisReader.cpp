
#include "AxisReader.h"
#include "ofApp.h"
#include "Axis.h"

AxisReader::AxisReader() {
}

void AxisReader::init(ofApp * _app) {
	app = _app;
}

void AxisReader::start( int _sleep_ms=5) {
	sleep_ms = _sleep_ms;
	startThread();
}

void AxisReader::stop() {
	stopThread();
}

void AxisReader::threadedFunction() {

	while (isThreadRunning()) {

		if (lock()) {

			vector<Axis> & axis = app->axis;
			for (int i = 0; i < axis.size(); i++) {
				ofApp::app->adsCmd.readStatus(axis[i]);
			}

			unlock();
			sleep(sleep_ms);

		}
		else {
			//ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
		}
	}

}

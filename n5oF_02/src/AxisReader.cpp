#pragma once
#include "AxisReader.h"
#include "ofApp.h"
#include "Axis.h"

AxisReader::AxisReader() {
	sleep_ms = 10;
}

void AxisReader::init(ofApp * _app) {
	app = _app;
}

void AxisReader::start( int _sleep_ms) {
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
				axis[i].read();
			}

			unlock();
			sleep(sleep_ms);

		}
		else {
			// If we reach this else statement, it means that we could not
			// lock our mutex, and so we do not need to call unlock().
			// Calling unlock without locking will lead to problems.
			//ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
		}
	}

}

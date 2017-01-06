#pragma once

#include "ofMain.h"
#include "ofxBeckhoffADS.h"
#include "Axis.h"

class AdsCommander {

public:

	AdsCommander();
	static void init();
	static void enableAll();
	static void disableAll();
	static void haltAll();
	static void goToAll(int pos_mm);

	static void sendCmd(string name, double tarpos, double tarvel);
	static void readStatus(Axis & axis);

	static ofxBeckhoffADS ads;

};
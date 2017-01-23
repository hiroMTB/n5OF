#pragma once

#include "ofMain.h"
#include "ofxBeckhoffADS.h"
#include "Axis.h"

class AdsCommander {

public:

	AdsCommander();
	static void init();

	static void enable( string name );
	static void disable( string name );
	static void halt( string name );
	static void goTo(string name, float pos, float vel=500);
	static void home( string name );

	static void enableAll();
	static void disableAll();
	static void haltAll();
	static void goToAll(float pos,float vel=500);
	static void homeAll();
	static void readStatus(Axis & axis);

	static ofxBeckhoffADS ads;

};
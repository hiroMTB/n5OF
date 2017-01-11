#pragma once

#include "ofMain.h"

class CmdData {
public:
	CmdData() {};
	int frame;
	double tarpos_p;	// pix
	double tarvel_p;	// pix
	double taracc_p;	// pix
	double endvel_p;	// pix
};

class DumpData {
public:
	DumpData() {};
	float pos;
	int onoff;
};

class Axis {

public:

	Axis();
	void init(int id);
	void loadCmdCsv();
	void loadDumpCsv();
	void update(int currnet_frame);

	static map<int, string> MC_MotionSate;

public:

	int id;
	int state;
	bool Error;
	int ErrorID;
	bool Disabled;
	bool Homed;
	int motion_state;
	double cmd_scale;
	string name;

	double tarpos;	//	mm
	double tarvel;
	double taracc;
	double endvel;

	double actpos;	// mm
	double actvel;
	double actacc;

	CmdData current_cmd;
	map<int, CmdData> cmdData;
	vector<DumpData> dump;

};
#pragma once

#include "ofMain.h"

class CmdData {
public:
	CmdData() {};
	int frame;
	double tarpos_p;	// pix
	double tarvel_p;	// pix
	double taracc_p;	// pix
	double tardeacc_p;	// pix
	double tarjerk_p;	
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
	void init(int id, float pix20, float pix1180);
	void loadCmdCsv();
	void loadDumpCsv();
	void update(int currnet_frame);

	static map<int, string> MC_MotionSate;

public:

	int id;
	//int state;
	//bool Error;
	int ErrorID;
	bool Disabled;
	bool Homed;
	int motion_state;
	double cmd_scale;
	string name;

	double tarpos;	//	mm
	double tarvel;
	//double taracc;
	//double tardeacc;
	//double tarjerk;

	double actpos;	// mm
	//double actvel;

	double dumpPos;

	CmdData current_cmd;
	map<int, CmdData> cmdData;
	vector<DumpData> dump;

	float cmd_cale;
	float cmd_offset;

};
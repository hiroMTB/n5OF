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
	void sendCmd(int currnet_frame);
	void read();

	int id;
	double cmd_scale;

	string name;

	int state;
	CmdData current_cmd;

	double tarpos;	//	mm
	double tarvel;
	double taracc;
	double endvel;

	double actpos;	// mm
	double actvel;
	double actacc;

	map<int, CmdData> cmdData;
	vector<DumpData> dump;

};
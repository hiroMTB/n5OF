#include "Axis.h"
#include "ofApp.h"

using namespace std;
#include <iostream>
#include <fstream>

extern double pix2mm;
extern double mm2pix;

// https://infosys.beckhoff.com/index.php?content=../content/1031/tcplclib_tc2_mc2/18014398579628043.html&id=
map<int, string> Axis::MC_MotionSate = {
	pair<int, string>(0, "UNDEFINED"),
	pair<int, string>(1, "DISABLED"),
	pair<int, string>(2, "STANDSTILL"),
	pair<int, string>(3, "ERRORSTOP"),
	pair<int, string>(4, "STOPPING"),
	pair<int, string>(5, "HOMING"),
	pair<int, string>(6, "DISCRETEMOTION"),
	pair<int, string>(7, "CONTINOUSMOTION"),
	pair<int, string>(8, "SYNCHRONIZEDMOTION")
};

Axis::Axis() {
	current_cmd.tarpos_p = 3;
	current_cmd.tarvel_p = 10;
	current_cmd.taracc_p = 10;

	tarpos = -123;
	tarvel = -123;
	//taracc = -123;

	actpos = -123;
	//actvel = -123;
	//state = -123;
	name = "AxisNameError";
}

void Axis::init(int _id, float min, float max ) {
	id = _id;

	/*
	float effectiveLen = max - min;
	cmd_scale = effectiveLen / 5000.0;
	cmd_offset = min;
	*/

	float axisLen = (max-min)*(1200.0/1160.0);
	cmd_scale = axisLen / 5000.0;
	cmd_offset = min - (20.0*cmd_scale*pix2mm);
	
	name = "MAIN.axisCtrls[" + ofToString(id + 1) + "]";
	
	printf("Axis %02d : cmd_scale=%5.2f, cmd_offset=%4.2f\n", id+1, cmd_scale, cmd_offset);
	loadCmdCsv();
	loadDumpCsv();
}

void Axis::update(int current_frame) {
	if (cmdData.find(current_frame) != cmdData.end()) {
		CmdData & c = cmdData[current_frame];
		tarpos = cmd_offset + c.tarpos_p * pix2mm * cmd_scale;
		tarvel = c.tarvel_p*cmd_scale*pix2mm * 25.0;
		//taracc = c.taracc_p*cmd_scale*pix2mm *25.0;
		//tarjerk = c.tarjerk_p*cmd_scale*pix2mm *25.0;

		if (Homed && ErrorID==0) {
			if (current_cmd.frame != current_frame) {
				ofApp::app->adsCmd.goTo(name, tarpos, abs(tarvel));
				current_cmd = c;
				printf("cmd f%5d  axis%2d  p%6.0f  v%8.1f\n", c.frame, id+1, c.tarpos_p, abs(c.tarvel_p) );
			}
		}
	}
}

/*

FILE I/O

*/
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

void Axis::loadCmdCsv() {
	string fileName = "cmd/CMD_" + ofToString(id) + ".csv";
	string path = ofToDataPath(fileName, true);

	ifstream file(path.c_str());

	if (!file.is_open()) {
		cout << "Error : Cant open file : " << path << endl;
	}
	else {
		//cout << "open : " << fileName << endl;
		string line;
		int cnt = 0;
		while (std::getline(file, line)) {
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			if (cnt++<2) continue;
			vector<string> cmds = split(line, ',');
			int frame = ofToInt(cmds[0]);
			float pos_pix = ofToFloat(cmds[1]);    // pix
			float spd_pix = ofToFloat(cmds[2]);   // pix/s
			//float acc_pix = spd_pix * 9.0;		 // pix/s2
			//float jerk_pix = spd_pix * 90.0;	 // pix/s3

			CmdData d;
			d.frame = frame;
			d.tarpos_p = pos_pix;
			d.tarvel_p = spd_pix;
			//d.taracc_p = acc_pix;
			//d.tarjerk_p = jerk_pix;
			cmdData.insert(pair<int, CmdData>(frame, d));
		}
	}
}

void Axis::loadDumpCsv() {
	string fileName = "dump/DUMP_" + ofToString(id) + ".csv";
	string path = ofToDataPath(fileName, true);

	ifstream file(path.c_str());

	if (!file.is_open()) {
		cout << "Error : Cant open file : " << path << endl;
	}
	else {
		//cout << "open : " << fileName << endl;
		string line;
		int cnt = 0;
		while (std::getline(file, line)) {
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			if (cnt++<2) continue;
			vector<string> cmds = split(line, ',');
			int frame = ofToInt(cmds[0]);
			float pix = ofToFloat(cmds[1]);     // pix absolute
			int onoff = ofToInt(cmds[2]);       // 0 or 255

			DumpData d;
			d.pos = pix;
			d.onoff = onoff;
			dump.push_back(d);
		}
	}
}
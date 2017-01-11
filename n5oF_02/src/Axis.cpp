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
	current_cmd.tarpos_p = -123;
	current_cmd.tarvel_p = -123;
	current_cmd.taracc_p = -123;
	current_cmd.endvel_p = 0;

	cmd_scale = 0.2;

	tarpos = -123;
	tarvel = -123;
	taracc = -123;
	endvel = -123;

	actpos = -123;
	actvel = -123;
	actacc = -123;
	state = -123;
	name = "AxisNameError";
}

void Axis::init(int _id) {
	id = _id;
	name = "MAIN.axisCtrls[" + ofToString(id + 1) + "]";
	loadCmdCsv();
	loadDumpCsv();
}

void Axis::update(int current_frame) {
	if (cmdData.find(current_frame) != cmdData.end()) {
		CmdData & c = cmdData[current_frame];
		tarpos = c.tarpos_p*cmd_scale*pix2mm;
		tarvel = c.tarvel_p*cmd_scale*pix2mm * 25.0;

		ofApp::app->adsCmd.goTo(name, tarpos, abs(tarvel));
		current_cmd = c;
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
		cout << "open : " << fileName << endl;
		string line;
		int cnt = 0;
		while (std::getline(file, line)) {
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			if (cnt++<2) continue;
			vector<string> cmds = split(line, ',');
			int frame = ofToInt(cmds[0]);
			float tPos_pix = ofToFloat(cmds[1]);     // pix
			float sSpd_pix = ofToFloat(cmds[2]);     // pix/s
													 //float eSpd_pix = ofToFloat(cmds[3]);   // pix/s2
			CmdData d;
			d.frame = frame;
			d.tarpos_p = tPos_pix;
			d.tarvel_p = sSpd_pix;
			d.endvel_p = 0;
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
		cout << "open : " << fileName << endl;
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
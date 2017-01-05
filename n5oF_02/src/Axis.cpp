#include "Axis.h"
#include "ofApp.h"

extern double pix2mm;
extern double mm2pix;


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

void Axis::sendCmd(int current_frame) {
	if (cmdData.find(current_frame) != cmdData.end()) {
		CmdData & c = cmdData[current_frame];
		tarpos = c.tarpos_p*cmd_scale*pix2mm;
		tarvel = c.tarvel_p*cmd_scale*pix2mm * 25.0;

		ofApp::app->adsCmd.sendCmd(name, tarpos, abs(tarvel));
		current_cmd = c;

		//printf("send cmd : axis%02d, target pos = %7.2f, target vel %7.2f\n", id+1, tarpos, abs(tarvel));

	}
}

void Axis::read() {
	ofApp::app->adsCmd.readStatus(*this);
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
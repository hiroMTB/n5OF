#include "def.h"
#include "ofMain.h"
#include "ofApp.h"

int main() {

	ofSetupOpenGL(500, 800, OF_WINDOW);
	ofApp::init();
	ofRunApp(ofApp::app);

}

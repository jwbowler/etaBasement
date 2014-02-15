#include "ofMain.h"
#include "testApp.h"

#define WINDOW_WIDTH (1024 + 32)
#define WINDOW_HEIGHT 768

int main() {
	ofSetupOpenGL(WINDOW_WIDTH, WINDOW_HEIGHT, OF_WINDOW); // can also be OF_FULLSCREEN
	ofRunApp(new testApp());
}

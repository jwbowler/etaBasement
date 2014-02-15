#pragma once

#include "ofMain.h"
#include "ofxEasyFft.h"

class testApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	// other standard callbacks:

	//void keyPressed(int key);
	//void keyReleased(int key);
	//void mouseMoved(int x, int y);
	//void mouseDragged(int x, int y, int button);
	//void mousePressed(int x, int y, int button);
	//void mouseReleased(int x, int y, int button);
	//void windowResized(int w, int h);
	//void dragEvent(ofDragInfo dragInfo);
	//void gotMessage(ofMessage msg);

private:
	ofxEasyFft fft;

	void plotFft(const vector<float>& buffer, const float scale);

	// constants
	const int fftBufferSize = 4096;
	const int audioBufferSize = 256;
	const int audioSampleRate = 44100;

};

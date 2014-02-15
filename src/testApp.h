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

	float power = 0;
	float bass = 0;
	float mid = 0;
	float treble = 0;

	void plotFft(const vector<float>& buffer, const float scale);

	float integrateFft(const vector<float>& bins, const unsigned int minFreq, const unsigned int maxFreq);

	/*constexpr*/ int getBinFromFreq(const unsigned int freq) {
		return fftBufferSize * freq / audioSampleRate;
	}

	// constants
	const unsigned int fftBufferSize = 4096;
	const unsigned int audioBufferSize = 256;
	const unsigned int audioSampleRate = 44100;

	const unsigned int bassMin = 0;
	const unsigned int bassMax = 100;
	const unsigned int midMin = 100;
	const unsigned int midMax = 1000;
	const unsigned int trebleMin = 1000;
	const unsigned int trebleMax = 2000;

};

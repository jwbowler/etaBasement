#pragma once

#include "ofMain.h"
#include "ofxEasyFft.h"
#include "BMT.h"

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

	//DEBUG FUNCTIONS
	void printHSV(HSV* h);
	void printRGB(RGB* st);

	ofxEasyFft fft;
	ofSerial serial;
	float power = 0;
	float bass = 0;
	float mid = 0;
	float treble = 0;

	void drawRGBRect(int type, int r, int g, int b);

	void plotFft(const vector<float>& buffer, const float scale);

	float integrateFft(const vector<float>& bins, const unsigned int minFreq, const unsigned int maxFreq);

	void pushMusicValues(void);

	void initColor(void);

	void stepColor(void);

	void stepHue();

	void bmtToRGB(void);

	void hsvToRGB(HSV* hsvTriple, RGB* rgbTriple);

	void updateColorVal(HSV* hsvTriple, int type, double bmtVal);

	/*constexpr*/ int getBinFromFreq(const unsigned int freq) {
		return fftBufferSize * freq / audioSampleRate;
	}

	// constants
	const unsigned int fftBufferSize = 4096;
	const unsigned int audioBufferSize = 256;
	const unsigned int audioSampleRate = 44100;

	const unsigned int bassMin = 0;
	const unsigned int bassMax = 200;
	const unsigned int midMin = 100;
	const unsigned int midMax = 1000;
	const unsigned int trebleMin = 1000;
	const unsigned int trebleMax = 2000;
	const int frameBegin = 255;
	const double bassScale = 0.05;
	const double midScale = 1.1;
	const double trebleScale = 1;
	const double hueStep = 3;
	BMT bmt;
};

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

	void keyPressed(int key);
	void keyReleased(int key);
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
	const int frameBegin = 255;

	bool bassBinary;
	float power;
	float bass;
	float mid;
	float treble;
	int bassMin;
	int bassMax;
	int midMin;
	int midMax;
	int trebleMin;
	int trebleMax;
	double bassScale;
	double midScale;
	double trebleScale;
	double hueStep;
	BMT bmt;
};

#include "testApp.h"
#include <iostream>

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	fft.setup(fftBufferSize,
			  OF_FFT_WINDOW_HAMMING,
			  OF_FFT_FFTW,
			  audioBufferSize,
			  audioSampleRate);
	fft.setUseNormalization(false);
}

void testApp::update(){
	fft.update();
}

void testApp::draw(){
	ofBackground(0, 0, 0);
	ofSetColor(255);

	ofPushMatrix();
	ofTranslate(16, 16);
	ofDrawBitmapString("Frequency Domain", 0, 0);
	plotFft(fft.getBins(), 128);
	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(16, 16*2 + 128);
	ofDrawBitmapString("bass", 0, 0);
	ofDrawBitmapString("mid", 64, 0);
	ofDrawBitmapString("treble", 128, 0);
	ofPopMatrix();

	string msg = ofToString((int) ofGetFrameRate()) + " fps";
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);
}

void testApp::plotFft(const vector<float>& buffer, const float scale) {
	ofNoFill();

	int n = MIN(1024, buffer.size());
	ofRect(0, 0, n, scale);

	ofPushMatrix();
	ofTranslate(0, scale);
	ofScale(1, -scale);

	int max = 0;
	float maxVal;

	ofBeginShape();
	for (int i = 0; i < n; i++) {
		if (buffer[i] > maxVal) {
			max = i;
			maxVal = buffer[i];
		}
		ofVertex(i, buffer[i] * 10);
	}
	float total;
	for (int i = 4; i < 20; i++) {
		total += buffer[i];
	}
	ofEndShape();

	ofPopMatrix();

	std::cout << max << std::endl;
}

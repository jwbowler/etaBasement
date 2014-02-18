#include "testApp.h"
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	fft.setup(fftBufferSize,
			  OF_FFT_WINDOW_HAMMING,
			  OF_FFT_BASIC,
			  audioBufferSize,
			  audioSampleRate);
	fft.setUseNormalization(false);
	//John's computer
	if(!serial.setup("/dev/ttyACM0", 9600)) {
	//Amruth's Mac
	//if(!serial.setup("/dev/tty.usbmodem1421", 9600)) {
		std::cout << "Couldn't set up serial connection" << std::endl;
		std::exit(1);
	}

	initColor();
}

void testApp::initColor() {
	bmt.hsvBass = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvBass -> hue = 0; 
	bmt.hsvBass -> sat = 1.0;
	bmt.hsvBass -> val = 1.0;

	bmt.hsvMid = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvMid -> hue = 0;
	bmt.hsvMid -> sat = 0;
	bmt.hsvMid -> val = 0;

	bmt.hsvTreble = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvTreble -> hue = 0;
	bmt.hsvTreble -> sat = 1.0;
	bmt.hsvTreble -> val = 1.0;
}

void testApp::update(){
	fft.update();

	// std::cout << serial.readByte() << std::endl;
	// serial.flush(true, false);

	power = integrateFft(fft.getBins(), 0, fftBufferSize/2);
	bass = integrateFft(fft.getBins(), bassMin, bassMax);
	mid = integrateFft(fft.getBins(), midMin, midMax);
	treble = integrateFft(fft.getBins(), trebleMin, trebleMax);
	pushMusicValues();
}



void testApp::updateColorVal(HSV* hsvTriple, int type, double bmtVal) {
	//type == 0 -> bass
	if (type == 0) {
		hsvTriple -> val = std::min(bmtVal / bassScale, 1.0);
	}
	// mid
	else if (type == 1) {
		hsvTriple -> val = std::min(bmtVal / midScale, 1.0);
	}
	// bass
	else {
		hsvTriple -> val = std::min(bmtVal / trebleScale, 1.0);
	}
}

void testApp::pushMusicValues(void) {
	stepColor();

	// serial.writeByte(power * 100);
	serial.writeByte(frameBegin);
	// serial.writeByte(scaled_bass);
	// serial.writeByte(scaled_mid);
	// serial.writeByte(scaled_treble);

	//Now write the 3 color triplets
	//serial.writeByte(std::min((int) bmt.rgbBass -> r, 254));
	//serial.writeByte(std::min((int) bmt.rgbBass -> g, 254));
	//serial.writeByte(std::min((int) bmt.rgbBass -> b, 254));

	serial.writeByte(std::min((int) bmt.rgbMid -> r, 254));
	serial.writeByte(std::min((int) bmt.rgbMid -> g, 254));
	serial.writeByte(std::min((int) bmt.rgbMid -> b, 254));

	//serial.writeByte(std::min((int) bmt.rgbTreble -> r, 254));
	//serial.writeByte(std::min((int) bmt.rgbTreble -> g, 254));
	//serial.writeByte(std::min((int) bmt.rgbTreble -> b, 254));

	std::cout << std::min((int) bmt.rgbMid -> r, 254) << " " << std::min((int) bmt.rgbMid -> g, 254) << " " << std::min((int) bmt.rgbMid -> b, 254) << std::endl;

	usleep(2000);
}

void testApp::stepHue() {
	bmt.hsvBass -> hue += hueStep;
	bmt.hsvMid -> hue += hueStep;
	bmt.hsvTreble -> hue += hueStep;
}

void testApp::stepColor() {
	stepHue();	
	// char scaled_bass = std::min((int) (bass * bassScale), 254);
	// char scaled_mid = std::min((int) (mid * midScale), 254);
	// char scaled_treble = std::min((int) (treble * trebleScale), 254);
	
	updateColorVal(bmt.hsvBass, 0, bass);
	updateColorVal(bmt.hsvMid, 1, mid);
	updateColorVal(bmt.hsvTreble, 2, treble);

	bmtToRGB();
}

void testApp::bmtToRGB(void) {
	free(bmt.rgbBass);
	free(bmt.rgbMid);
	free(bmt.rgbTreble);

	bmt.rgbBass = hsvToRGB(bmt.hsvBass);
	bmt.rgbMid = hsvToRGB(bmt.hsvMid);
	bmt.rgbTreble = hsvToRGB(bmt.hsvTreble);
}

RGB* testApp::hsvToRGB(HSV* hsvTriple) {
	double r,g,b;
	double h = hsvTriple -> hue;
	double s = hsvTriple -> sat;
	double v = hsvTriple -> val;

  double i = floor(h * 6);
  double f = h * 6 - i;
	double p = v * (1 - s);
	double q = v * (1 - f * s);
  double t = v * (1 - (1 - f) * s);

  switch( (int) i % 6){
      case 0: r = v, g = t, b = p; break;
      case 1: r = q, g = v, b = p; break;
      case 2: r = p, g = v, b = t; break;
      case 3: r = p, g = q, b = v; break;
      case 4: r = t, g = p, b = v; break;
      case 5: r = v, g = p, b = q; break;
  }
  RGB* rgb = (RGB* ) malloc(sizeof(RGB));
  rgb -> r = r * 255;
  rgb -> g = g * 255;
  rgb -> g = b * 255;
  return rgb;
}


/**************************
FFT CODE
***************************/

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
	ofDrawBitmapString("bass", 0, 5);
	ofDrawBitmapString("mid", 64, 5);
	ofDrawBitmapString("treble", 128, 5);
	ofRect(0, 20, 32, bass*100);
	ofRect(64, 20, 32, mid*100);
	ofRect(128, 20, 32, treble*100);
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

	ofBeginShape();
	for (int i = 0; i < n; i++) {
		ofVertex(i, buffer[i] * 10);
	}
	ofEndShape();

	ofPopMatrix();
}

float testApp::integrateFft(const vector<float>& bins, const unsigned int minFreq, const unsigned int maxFreq) {
	assert(getBinFromFreq(maxFreq) < bins.size());
	float total = 0;
	for (int i = getBinFromFreq(minFreq); i < getBinFromFreq(maxFreq); i++) {
		total += bins[i];
	}
	return total;
}

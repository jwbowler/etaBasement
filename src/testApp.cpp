#include "testApp.h"
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <math.h>

void testApp::setup(){
	power = 0;
	bass = 0;
	mid = 0;
	treble = 0;
	bassMin = 0;
	bassMax = 200;
	midMin = 100;
	midMax = 1000;
	trebleMin = 1000;
	trebleMax = 2000;
	bassScale = 0.05;
	midScale = 1.1;
	trebleScale = 1;
	bassOffset = 0.3;
	midOffset = 0.3;
	trebleOffset = 0.3;
	hueStep = 1;
	bassBinary = false;


	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	fft.setup(fftBufferSize,
			  OF_FFT_WINDOW_HAMMING,
			  OF_FFT_BASIC,
			  audioBufferSize,
			  audioSampleRate);
	fft.setUseNormalization(false);
	bool john = 1;
	//John's computer
	// if (john) {
	// if(!serial.setup("/dev/ttyUSB0", 9600)) {
	// 	std::cout << "Couldn't set up serial connection" << std::endl;
	// 	std::exit(1);
	// }
	// }
	// else {
	// 	if(!serial.setup("/dev/tty.usbmodem1421", 9600)) {
	// 		std::cout << "Couldn't set up serial connection" << std::endl;
	// 		std::exit(1);
	// 	}
	// }
	setupGUI();

	initColor();
}

void testApp::initColor() {
	bmt.hsvBass = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvBass -> hue = 0; 
	bmt.hsvBass -> sat = 1.0;
	bmt.hsvBass -> val = 1.0;

	bmt.hsvMid = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvMid -> hue = 0;
	bmt.hsvMid -> sat = 1.0;
	bmt.hsvMid -> val = 1.0;

	bmt.hsvTreble = (HSV* ) malloc(sizeof(HSV));
	bmt.hsvTreble -> hue = 0;
	bmt.hsvTreble -> sat = 1.0;
	bmt.hsvTreble -> val = 1.0;

	bmt.rgbBass = (RGB* ) malloc(sizeof(RGB));
	bmt.rgbMid = (RGB* ) malloc(sizeof(RGB));
	bmt.rgbTreble = (RGB* ) malloc(sizeof(RGB));
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
		double scaled = std::min(bmtVal / bassScale, 1.0);
		hsvTriple -> val = std::max(scaled - bassOffset, 0.0);
	}
	// mid
	else if (type == 1) {
		double scaled = std::min(bmtVal / midScale, 1.0);
		hsvTriple -> val = std::max(scaled - midOffset, 0.0);
	}
	// treble
	else {
		double scaled = std::min(bmtVal / trebleScale, 1.0);
		hsvTriple -> val = std::max(scaled - trebleOffset, 0.0);
	}
}

void testApp::pushMusicValues(void) {
	stepColor();
	
	// serial.writeByte(frameBegin);

	// Now write the 3 color triplets
	// serial.writeByte(std::min( bmt.rgbBass -> r, 254));
	// serial.writeByte(std::min( bmt.rgbBass -> g, 254));
	// serial.writeByte(std::min( bmt.rgbBass -> b, 254));

	// // std::cout << std::min( bmt.rgbMid -> r, 254) << " " << std::min( bmt.rgbMid -> g, 254) << " " << std::min( bmt.rgbMid -> b, 254) << std::endl;

	// serial.writeByte(std::min( bmt.rgbMid -> r, 254));
	// serial.writeByte(std::min( bmt.rgbMid -> g, 254));
	// serial.writeByte(std::min( bmt.rgbMid -> b, 254));

	// serial.writeByte(std::min( bmt.rgbTreble -> r, 254));
	// serial.writeByte(std::min( bmt.rgbTreble -> g, 254));
	// serial.writeByte(std::min( bmt.rgbTreble -> b, 254));

	usleep(10000);
}

void testApp::stepHue() {
	bmt.hsvBass -> hue = fmod(bmt.hsvBass -> hue + hueStep, 360);
	bmt.hsvMid -> hue = fmod(bmt.hsvMid -> hue + hueStep, 360);
	bmt.hsvTreble -> hue = fmod(bmt.hsvTreble -> hue + hueStep ,360);
}

void testApp::stepColor() {
	stepHue();
	
	updateColorVal(bmt.hsvBass, 0, bass);
	updateColorVal(bmt.hsvMid, 1, mid);
	updateColorVal(bmt.hsvTreble, 2, treble);

	bmtToRGB();
}

void testApp::bmtToRGB(void) {
	hsvToRGB(bmt.hsvBass, bmt.rgbBass);
	hsvToRGB(bmt.hsvMid, bmt.rgbMid);
	hsvToRGB(bmt.hsvTreble, bmt.rgbTreble);
}

void testApp::hsvToRGB(HSV* hsvTriple, RGB* rgbTriple) {
	double arr, gee, bee;
	double h = hsvTriple -> hue;
	double s = hsvTriple -> sat;
	double v = hsvTriple -> val;

	double c = v * s;
	double x = c * (1 - std::abs( fmod(h/60.0, 2) - 1) );
	double m = v - c;

	switch ( (int) (h/60) ) {
		case 0: arr = c; gee = x; bee = 0; break;
		case 1: arr = x; gee = c; bee = 0; break;
		case 2: arr = 0; gee = c; bee = x; break;
		case 3: arr = 0; gee = x; bee = c; break;
		case 4: arr = x; gee = 0; bee = c; break;
		case 5: arr = c; gee = 0; bee = x; break;
	}

	arr = (arr+m) * 255;
	gee = (gee+m) * 255;
	bee = (bee+m) * 255;
  rgbTriple->r = (int) round(arr);
  rgbTriple->g = (int) round(gee);
  rgbTriple->b = (int) round(bee);
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


	int r, g, b;

	ofPushMatrix();
	r = bmt.rgbBass -> r;
	g = bmt.rgbBass-> g;
	b = bmt.rgbBass -> b;
	//std::cout << r << " " << g << " " << b << std::endl;
	ofSetColor(r,g,b);
	ofFill();
	ofRect(64 * 4, 20, 50, 50);
	ofPopMatrix();	

	//mid
	ofPushMatrix();
	r = bmt.rgbMid -> r;
	g = bmt.rgbMid-> g;
	b = bmt.rgbMid -> b;
	ofSetColor(r,g,b);
	ofFill();
	ofRect(64 * 5, 20, 50, 50);
	ofPopMatrix();

	//treble
	ofPushMatrix();
	r = bmt.rgbTreble -> r;
	g = bmt.rgbTreble -> g;
	b = bmt.rgbTreble -> b;
	ofSetColor(r,g,b);
	ofFill();
	ofRect(64 * 6, 20, 50, 50);
	ofPopMatrix();
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

void testApp::printHSV(HSV* h) {
		std::cout << "hsv: " << h-> hue << " " << h -> sat << " " << h -> val << ", ";
}

void testApp::printRGB(RGB* st) {
	std::cout << "rgb: " << st-> r << " " << st -> g << " " << st -> b << std::endl;
}

void testApp::keyReleased(int key) {
	//Uses ASCII encoding of letters
	//Was 'escape' so quit
	if (key == 27) {
		std::exit(0);
	}

}
void testApp::setupGUI(void) {
	setupMiscCanvas();
	setupBassCanvas();
	setupMidCanvas();
	setupTrebleCanvas();
}

void testApp::setupMiscCanvas(void) {
	miscCanvas = new ofxUISuperCanvas("Misc Controls: ", 500,200, 500, 200);  
	miscCanvas -> addToggle("Fullscreen", false);
	miscCanvas -> addSpacer();
	miscCanvas -> addSlider("Hue Step",0.0, 10.0, hueStep); 
	ofAddListener(miscCanvas->newGUIEvent, this, &testApp::guiEvent); 
}

void testApp::setupBassCanvas(void) {
	bassCanvas = new ofxUISuperCanvas("Bass Controls: ", 500,300, 500, 200);  
	bassCanvas -> addSlider("Bass Scaling",0.001,4.0,bassScale); 
	bassCanvas -> addSpacer();
	bassCanvas -> addSlider("Bass Offset", 0.0, 1.0, bassOffset); 
	bassCanvas -> addSpacer();
	bassCanvas -> addRangeSlider("Bass Range",0, 300, bassMin, bassMax); 


	bassCanvas->autoSizeToFitWidgets(); 
	ofAddListener(bassCanvas->newGUIEvent, this, &testApp::guiEvent); 
}

void testApp::setupMidCanvas(void) {
	midCanvas = new ofxUISuperCanvas("Mid Controls: ", 500,450, 500, 200);  
	midCanvas -> addSlider("Mid Scaling",0.001,4.0,bassScale); 
	midCanvas -> addSpacer();
	midCanvas -> addSlider("Mid Offset", 0.0, 1.0, midOffset); 
	midCanvas -> addSpacer();
	midCanvas -> addRangeSlider("Mid Range",0,1500, midMin,midMax); 

	midCanvas->autoSizeToFitWidgets(); 
	ofAddListener(midCanvas->newGUIEvent, this, &testApp::guiEvent); 
}

void testApp::setupTrebleCanvas(void) {
	trebleCanvas = new ofxUISuperCanvas("Treble Controls: ", 500,600, 500, 200);  
	trebleCanvas -> addSlider("Treble Scaling",0.001,4.0,bassScale); 
	trebleCanvas -> addSpacer();
	trebleCanvas -> addSlider("Treble Offset", 0.0, 1.0, trebleOffset); 
	trebleCanvas -> addSpacer();
	trebleCanvas -> addRangeSlider("Treble Range", 1000 ,4000,trebleMin, trebleMax); 


	trebleCanvas->autoSizeToFitWidgets(); 
	ofAddListener(trebleCanvas->newGUIEvent, this, &testApp::guiEvent); 
}


void testApp::exit()
{
	delete miscCanvas;
    delete bassCanvas; 
    delete midCanvas;
    delete trebleCanvas;
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    if(e.getName() == "Fullscreen")
    {
        ofxUIToggle *toggle = e.getToggle(); 
        ofSetFullscreen(toggle->getValue()); 
    }
    else if(e.getName() == "Hue Step") {
    	ofxUISlider *slider = e.getSlider();
    	hueStep = slider -> getScaledValue();
    }

    else if(e.getName() == "Bass Scaling")
    {
        ofxUISlider *slider = e.getSlider(); 
        bassScale = slider -> getScaledValue();
    } 
    else if(e.getName() == "Bass Offset") {
    	ofxUISlider *slider = e.getSlider();
    	bassOffset = slider -> getScaledValue();
    }
    else if(e.getName() == "Bass Range") {
    	ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.getSlider();
    	bassMin = slider -> getScaledValueLow();
    	bassMax = slider -> getScaledValueHigh();
    }

    //MID ELEMENTS
    else if(e.getName() == "Mid Scaling")
    {
        ofxUISlider *slider = e.getSlider(); 
        // ofBackground(slider->getScaledValue());
        midScale = slider -> getScaledValue();
    } 
    else if(e.getName() == "Mid Offset") {
    	ofxUISlider *slider = e.getSlider();
    	midOffset = slider -> getScaledValue();
    }
    else if(e.getName() == "Mid Range") {
    	ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.getSlider();
    	midMin = slider -> getScaledValueLow();
    	midMax = slider -> getScaledValueHigh();
    }

    //Treble Elements
    else if(e.getName() == "Treble Scaling")
    {
        ofxUISlider *slider = e.getSlider(); 
        trebleScale = slider -> getScaledValue();
    } 
    else if(e.getName() == "Treble Offset") {
    	ofxUISlider *slider = e.getSlider();
    	trebleOffset = slider -> getScaledValue();
    }
    else if(e.getName() == "Treble Range") {
    	ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.getSlider();
    	trebleMin = slider -> getScaledValueLow();
    	trebleMax = slider -> getScaledValueHigh();
    }
    
    
}

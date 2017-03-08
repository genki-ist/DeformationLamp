#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    pix.allocate(WIDTH, HEIGHT, OF_PIXELS_GRAY);
    vidPix.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_PIXELS_GRAY);
    
    pos = ofVec2f(0,0);
    target = ofVec2f(700,0);
    
    initTime = 0.0;
    
    flag = false;
}

//--------------------------------------------------------------
void ofApp::update(){

    pix = gui->_grayPixels;
    vidPix = gui->_grayVidPixels;
    
    //---get difference texture
    if(gui->_imgOrMov == true)
    {
        tex.loadData(pix.getData(), WIDTH, HEIGHT, GL_LUMINANCE);
    }
    else
    {
        tex.loadData(vidPix.getData(), VIDEO_WIDTH, VIDEO_HEIGHT, GL_LUMINANCE);
    }
    
    //---move texture position with easing
    if(gui->_slide != flag)
    {
        initTime = ofGetElapsedTimef();
        flag = gui->_slide;
    }
    
    endTime = initTime + duration;
    
    float now = ofGetElapsedTimef();
    easing = ofxeasing::map_clamp(now, initTime, endTime, 0.0, 1.0, &ofxeasing::quad::easeIn);
    
    target.x = gui->_projectionSlide;
    
    //---update texture position
    if(flag == true)
    {
        pos.x = target.x * easing + gui->_projectionPos->x;
        pos.y = gui->_projectionPos->y;
    }
    else
    {
        pos.x = target.x * (1.0 - easing) + gui->_projectionPos->x;
        pos.y = gui->_projectionPos->y;
    }
    
//    cout << easing << ", " << pos.x << endl;
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofScale(gui->_projectionScale, gui->_projectionScale);
    tex.draw(pos);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}
//
//  GuiApp.hpp
//  DeformationLamp
//
//  Created by genkitoyama on 2017/03/03.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxCv.h"
#include "ofxImgWarp.h"
#include "ofxGui.h"

#define WIDTH 640
#define HEIGHT 480

class GuiApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void updateTracker();
    
    void keyPressed(int key);
    
    ofxFaceTracker  _imgTracker;
    float           _trackerArea;
    
    ofDirectory _imgDir;
    vector<ofImage> _images;
    
    ofDirectory _vidDir;
    vector<ofVideoPlayer> _videos;
    ofImage         _grayVideo;
    ofImage         _grayVidImg;
    
    int _currentImageNum;
    int _currentImageNumBuf;
    
    ofxImgWarp      _imgWarp;
    bool            _bDrawPoints = false;
    
    ofPixels        _dstPix;
    ofTexture       _tex;
    ofMesh          _imgMesh;
    
    vector<pair<ofPoint,ofPoint> > _ctrl;   //image warpingの制御点(firstが固定、secondを動かして歪ませる)
    int             _ctrlPosID[15] = {0,3,8,13,16,20,24,31,35,36,45,40,47,48,54};  //facetrackerの顔の制御点ID
    
    int             _movingCtrlPosID;
    
    ofImage         _grayImg;
    ofImage         _warpedImg;
    ofImage         _grayWarpedImg;
    ofPixels        _grayPixels;
    ofTexture       _grayDiff;
    
    //---gui
    ofxPanel        _gui;
    ofParameter<bool> _imgOrMov;
    ofParameter<float> _movingScale;
    ofParameter<float> _scaleFactor;
    ofParameter<float> _movingFreq;
    ofParameter<float> _faceAreaParam;
    
    ofParameter<float> _chinParam;
    ofParameter<float> _leftEyebrowParam;
    ofParameter<float> _rightEyebrowParam;
    ofParameter<float> _leftEyeParam;
    ofParameter<float> _rightEyeParam;
    ofParameter<float> _leftMouseParam;
    ofParameter<float> _rightMouseParam;
    ofParameter<bool> _slide;
    ofParameter<ofVec2f> _projectionPos;
    ofParameter<float> _projectionScale;
    ofParameter<int> _projectionSlide;
    

};
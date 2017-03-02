#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxCv.h"
#include "ofxImgWarp.h"
#include "ofxGui.h"

#define WIDTH 640
#define HEIGHT 480

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void initTracker();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxFaceTracker  _imgTracker;
    ofImage         _img;
    float           _trackerArea;
    
    ofDirectory _dir;
    vector<ofImage> _images;
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
    
    ofxPanel        _gui;
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
    
};

#pragma once

#include "ofMain.h"
#include "ofxEasing.h"
#include "GuiApp.hpp"

#define WIDTH 640
#define HEIGHT 480

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
        
    void keyPressed(int key);
    
    shared_ptr<GuiApp> gui;
    
    ofTexture tex;
    ofPixels pix;
    
    ofVec2f pos;
    ofVec2f target;
    
    bool flag;
    float easing;
    
    float initTime;
    float endTime;
    float duration = 1.5;
};

#include "ofMain.h"
#include "ofApp.h"
#include "GuiApp.hpp"
#include "ofAppGLFWWindow.h"

// 1: 本番モード(projector), 2: デバッグモード
#define DEBUG_MODE 2

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    
#if DEBUG_MODE == 1
    settings.width = 1200;
    settings.height = 800;
    settings.setPosition(ofVec2f(1920,0));
#elif DEBUG_MODE == 2
    settings.width = 960;
    settings.height = 540;
    settings.setPosition(ofVec2f(1200,0));
#endif
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    
    settings.width = 960;
    settings.height = 1080;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = false;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    
    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<GuiApp> guiApp(new GuiApp);
    mainApp->gui = guiApp;
    
    ofRunApp(guiWindow, guiApp);
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}

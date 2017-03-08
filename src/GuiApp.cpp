//
//  GuiApp.cpp
//  DeformationLamp
//
//  Created by genkitoyama on 2017/03/03.
//
//

#include "GuiApp.hpp"

//--------------------------------------------------------------
void GuiApp::setup(){
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    //---load images
    _imgDir.listDir("images_2/");
    _imgDir.sort();
    
    if(_imgDir.size()){
        _images.assign(_imgDir.size(), ofImage());
    }
    
    for(int i = 0; i < (int)_imgDir.size(); i++){
        _images[i].load(_imgDir.getPath(i));
    }
    
    //---load videos
    _vidDir.listDir("videos/");
    _vidDir.sort();
    
    if(_vidDir.size()){
        _videos.assign(_vidDir.size(), ofVideoPlayer());
    }
    
//    for(int i = 0; i < (int)_vidDir.size(); i++){
    for(int i = 0; i < 1; i++){
        _videos[i].load(_vidDir.getPath(i));
        _videos[i].play();
    }

    _vidImage.load("out1.jpg");

    _currentImageNum = 0;
    _currentImageNumBuf = 0;
    
    //---facetracker setup
    _imgTracker.setup();
    
    //---imagewarp setup
    _dstPix = _images[_currentImageNum].getPixels();
    _imgWarp.setup(_dstPix.getWidth(), _dstPix.getHeight(), 50);
    
    _tex.allocate(_dstPix);
    _tex.loadData(_dstPix);
    
    //---facertacker&imagewarp update
    updateTracker();
    
    //---grayscale setup
    _grayImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    _grayVidImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    _grayWarpedImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    _warpedImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    ofxCv::convertColor(_images[_currentImageNum], _grayImg, CV_RGB2GRAY);
    _grayImg.update();
    
    ofxCv::convertColor(_vidImage, _grayVidImg, CV_RGB2GRAY);
    _grayVidImg.update();
    
    _grayPixels.allocate(WIDTH, HEIGHT, OF_PIXELS_GRAY);
    _grayVidPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_PIXELS_GRAY);
    
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<HEIGHT;j++)
        {
            _grayPixels[j*WIDTH+i] = _grayImg.getPixels()[j*WIDTH + i];
        }
    }
    _grayDiff.allocate(_grayPixels);
    
    for(int i=0;i<VIDEO_WIDTH;i++)
    {
        for(int j=0;j<VIDEO_HEIGHT;j++)
        {
            _grayVidPixels[j*VIDEO_WIDTH+i] = _grayVidImg.getPixels()[j*VIDEO_WIDTH + i];
        }
    }
    _grayVidDiff.allocate(_grayVidPixels);
    
    //---gui setup
    _gui.setup();
    _gui.setPosition(700, 0);
    _gui.add(_imgOrMov.set("imgOrMov",true));
    _gui.add(_movingScale.set("movingScale",0.0,0.0,1.5));
    _gui.add(_scaleFactor.set("scaleFactor",1.3,1.0,2.0));
    _gui.add(_movingFreq.set("freq",1.5,0.1,3.0));
    _gui.add(_faceAreaParam.set("faceAreaParam",1.0,1.0,2.0));
    
    _gui.add(_chinParam.set("chin",0.5,0.0,2.0));
    _gui.add(_leftEyebrowParam.set("leftEyebrow",0.5,0.0,2.0));
    _gui.add(_rightEyebrowParam.set("rightEyebrow",0.5,0.0,2.0));
    _gui.add(_leftEyeParam.set("leftEye",0.5,0.0,2.0));
    _gui.add(_rightEyeParam.set("rightEye",0.5,0.0,2.0));
    _gui.add(_leftMouseParam.set("leftMouse",0.5,0.0,2.0));
    _gui.add(_rightMouseParam.set("rightMouse",0.5,0.0,2.0));
    _gui.add(_slide.set("slide",false));
    _gui.add(_projectionPos.set("pos",ofVec2f(0,0),ofVec2f(0,0),ofVec2f(200,200)));
    _gui.add(_projectionScale.set("scale",1.0,0.8,1.5));
    _gui.add(_projectionSlide.set("slideAmount",700,640,800));
    
    _gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void GuiApp::updateTracker(){
    
    _imgTracker.update(ofxCv::toCv(_images[_currentImageNum]));
    
    if(_imgTracker.getFound())
    {
        _ctrl.clear();
        _imgMesh = _imgTracker.getImageMesh();
        _trackerArea = _imgTracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE).getArea();
        cout << "face area: " << _trackerArea << endl;
        
        //---add control points
        for(auto x: _ctrlPosID)
        {
            ofVec2f ctrlPos = _imgMesh.getVertices()[x];
            _ctrl.push_back(make_pair(ctrlPos,ctrlPos));
        }
        
        ofVec2f pos = ofVec2f((_imgMesh.getVertices()[1]+_imgMesh.getVertices()[31])*0.5);
        _ctrl.push_back(make_pair(pos, pos));
        ofVec2f pos2 = ofVec2f((_imgMesh.getVertices()[15]+_imgMesh.getVertices()[35])*0.5);
        _ctrl.push_back(make_pair(pos2, pos2));
        
        //---add boundary face points
//        ofPolyline outline = _imgTracker.getImageFeature(ofxFaceTracker::ALL_FEATURES);
        ofPolyline outline = _imgTracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
        ofVec2f position = _imgTracker.getPosition();
        for(int i = 0; i < outline.size(); i+=1) {
            ofVec2f point((outline[i] - position) * _scaleFactor + position);
            _ctrl.push_back(make_pair(point, point));
//            ofVec2f point2((outline[i] - position) * (_scaleFactor+0.3) + position);
//            _ctrl.push_back(make_pair(point2, point2));
        }
    }
    
    //---update warped image
    _imgWarp.calcDelta(_ctrl);
    _imgWarp.genNewImg(_images[_currentImageNum].getPixels(),_dstPix,1);
    _tex.loadData(_dstPix);
}

//--------------------------------------------------------------
void GuiApp::update(){
    
    if(_imgOrMov){
        //---move control points
        if(_ctrl.size() > 0)
        {
            //motion全部手打ち
            _movingScale = sin(ofGetElapsedTimef()*2*_movingFreq);
            _movingScale = ofMap(_movingScale, -1.0, 1.0, 0.0, _scaleFactor);
        
            _movingScale *= _trackerArea/(48000/_faceAreaParam);
            
            _ctrl[2].second.y = _ctrl[2].first.y + _chinParam*10 * _movingScale;
            _ctrl[5].second.y = _ctrl[5].first.y - _leftEyebrowParam*10 * _movingScale;
            _ctrl[6].second.y = _ctrl[6].first.y - _rightEyebrowParam*10 * _movingScale;
            _ctrl[9].second.x = _ctrl[9].first.x - _leftEyeParam*10 * _movingScale;
            _ctrl[9].second.y = _ctrl[9].first.y + _leftEyeParam*10 * _movingScale;
            _ctrl[10].second.x = _ctrl[10].first.x + _rightEyeParam*10 * _movingScale;
            _ctrl[10].second.y = _ctrl[10].first.y + _rightEyeParam*10 * _movingScale;
            _ctrl[13].second.x = _ctrl[13].first.x - _leftMouseParam*10 * _movingScale;
            _ctrl[13].second.y = _ctrl[13].first.y - _leftMouseParam*10 * _movingScale;
            _ctrl[14].second.x = _ctrl[14].first.x +  _rightMouseParam*10 * _movingScale;
            _ctrl[14].second.y = _ctrl[14].first.y - _rightMouseParam*10 * _movingScale;
        }
    
        //---image warping
        if(ofGetFrameNum()%3 == 0)
        {
            if(_currentImageNum != _currentImageNumBuf)
            {
                updateTracker();
            
                ofxCv::convertColor(_images[_currentImageNum], _grayImg, CV_RGB2GRAY);
                _grayImg.update();
            
                _currentImageNumBuf = _currentImageNum;
            }
        
            //---get warped image
            _imgWarp.calcDelta(_ctrl);
            _imgWarp.genNewImg(_images[_currentImageNum].getPixels(),_dstPix,1);
            _tex.loadData(_dstPix);
        
            //---convet to ofImage
            ofPixels p;
            _tex.readToPixels(p);
            _warpedImg.setFromPixels(p);
            
            //---convert to grayscale
            ofxCv::convertColor(_warpedImg, _grayWarpedImg, CV_RGB2GRAY);
            _grayWarpedImg.update();
        
            //---extract difference
            for(int i=0;i<WIDTH;i++)
            {
                for(int j=0;j<HEIGHT;j++)
                {
                    int diff = _grayWarpedImg.getPixels()[j*WIDTH + i] - _grayImg.getPixels()[j*WIDTH + i];
                    diff = (int)ofMap(diff, -255, 255, 0, 255);
                    _grayPixels[j*WIDTH+i] = (unsigned char)diff;
                }
            }
            _grayDiff.loadData(_grayPixels.getData(), WIDTH, HEIGHT, GL_LUMINANCE);
        }
    }
    else
    {
        _videos[0].update();
        
        if(_videos[0].isFrameNew())
        {
            ofxCv::convertColor(_videos[0], _grayVideo, CV_RGB2GRAY);
            _grayVideo.update();
            
            for(int i=0;i<VIDEO_WIDTH;i++)
            {
                for(int j=0;j<VIDEO_HEIGHT;j++)
                {
                    int diff = _grayVideo.getPixels()[j*VIDEO_WIDTH + i] - _grayVidImg.getPixels()[j*VIDEO_WIDTH + i];
                    diff = (int)ofMap(diff, -255, 255, 0, 255);
                    _grayVidPixels[j*VIDEO_WIDTH+i] = (unsigned char)diff;
                }
            }
            _grayVidDiff.loadData(_grayVidPixels.getData(), VIDEO_WIDTH, VIDEO_HEIGHT, GL_LUMINANCE);
        }

    }
}

//--------------------------------------------------------------
void GuiApp::draw(){
    ofSetColor(255);
    
    if(_imgOrMov)
    {
        _tex.draw(0,0);
        _grayDiff.draw(0, HEIGHT);
    
        if(_bDrawPoints)
        {
            _imgMesh.drawWireframe();
        
            for(int i=0;i<_ctrl.size();i++)
            {
                ofSetColor(255, 0, 0);
                ofDrawCircle(_ctrl[i].first, 3);
                ofSetColor(255, 255, 0);
                ofDrawCircle(_ctrl[i].second, 3);
            }
        }
    }
    else
    {
        _videos[0].draw(0,0);
        _grayVidDiff.draw(0, HEIGHT);
    }
    
    if(_bDrawGui) _gui.draw();

}

//--------------------------------------------------------------
void GuiApp::keyPressed(int key){
    if(key == ' ')
    {
        _bDrawPoints = !_bDrawPoints;
    }
    
    if(key == 'g')
    {
        _bDrawGui = !_bDrawGui;
    }
    
    if(key == 'c')
    {
        updateTracker();
    }
    
    if(key == OF_KEY_DOWN)
    {
        if (_imgDir.size() > 0){
            _currentImageNum++;
            _currentImageNum %= _imgDir.size();
        }
    }
    
    if(key == OF_KEY_UP)
    {
        if (_imgDir.size() > 0){
            if(_currentImageNum > 0) _currentImageNum--;
            _currentImageNum %= _imgDir.size();
        }
    }

}
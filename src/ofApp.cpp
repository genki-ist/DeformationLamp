#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //load images
    _dir.listDir("images/");
    _dir.sort();
    
    if(_dir.size()){
        _images.assign(_dir.size(), ofImage());
    }
    
    for(int i = 0; i < (int)_dir.size(); i++){
        _images[i].load(_dir.getPath(i));
    }
    _currentImageNum = 0;
    _currentImageNumBuf = 0;
    
    _img.load("obama.jpg");
    
    //facetracker setup
    _imgTracker.setup();
    
    //imagewarp setup
    _dstPix = _images[_currentImageNum].getPixels();
    _imgWarp.setup(_dstPix.getWidth(), _dstPix.getHeight(), 50);
    
    _tex.allocate(_dstPix);
    _tex.loadData(_dstPix);
    
    initTracker();
    
    ofEnableAlphaBlending();
    
    _grayImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    _grayWarpedImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    _warpedImg.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    ofxCv::convertColor(_images[_currentImageNum], _grayImg, CV_RGB2GRAY);
    _grayImg.update();
    
    _grayPixels.allocate(WIDTH, HEIGHT, OF_PIXELS_GRAY);
    
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<HEIGHT;j++)
        {
            _grayPixels[j*WIDTH+i] = _grayImg.getPixels()[j*WIDTH + i];
        }
    }
    _grayDiff.allocate(_grayPixels);
    
    //gui setup
    _gui.setup();
    _gui.setPosition(750, 0);
    _gui.add(_movingScale.set("scale",0.0,0.0,1.5));
    _gui.add(_scaleFactor.set("scaleFactor",1.2,1.0,2.0));
    _gui.add(_movingFreq.set("freq",1.5,0.1,3.0));
    _gui.add(_faceAreaParam.set("faceAreaParam",1.0,1.0,2.0));
    
    _gui.add(_chinParam.set("chin",0.5,0.0,2.0));
    _gui.add(_leftEyebrowParam.set("leftEyebrow",0.5,0.0,2.0));
    _gui.add(_rightEyebrowParam.set("rightEyebrow",0.5,0.0,2.0));
    _gui.add(_leftEyeParam.set("leftEye",0.5,0.0,2.0));
    _gui.add(_rightEyeParam.set("rightEye",0.5,0.0,2.0));
    _gui.add(_leftMouseParam.set("leftMouse",0.5,0.0,2.0));
    _gui.add(_rightMouseParam.set("rightMouse",0.5,0.0,2.0));
    
    //    _gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::initTracker(){
    
    _imgTracker.update(ofxCv::toCv(_images[_currentImageNum]));
    
    if(_imgTracker.getFound())
    {
        _ctrl.clear();
        _imgMesh = _imgTracker.getImageMesh();
        _trackerArea = _imgTracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE).getArea();
        cout << "face area: " << _trackerArea << endl;
        
        for(auto x: _ctrlPosID)
        {
            ofVec2f ctrlPos = _imgMesh.getVertices()[x];
            _ctrl.push_back(make_pair(ctrlPos,ctrlPos));
        }
        
        // add boundary face points
        ofPolyline outline = _imgTracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
        ofVec2f position = _imgTracker.getPosition();
        for(int i = 0; i < outline.size(); i+=1) {
            ofVec2f point((outline[i] - position) * _scaleFactor + position);
            _ctrl.push_back(make_pair(point, point));
        }
        
        //        ofVec2f pos = _imgMesh.getVertices()[4]*0.7 + _imgMesh.getVertices()[31]*0.3;
        //        _ctrl.push_back(make_pair(pos, pos));
        //
        //        ofVec2f pos1 = _imgMesh.getVertices()[3]*0.7 + _imgMesh.getVertices()[29]*0.3;
        //        _ctrl.push_back(make_pair(pos1, pos1));
        //
        //        ofVec2f pos2 = _imgMesh.getVertices()[2]*0.3 + _imgMesh.getVertices()[29]*0.7;
        //        _ctrl.push_back(make_pair(pos2, pos2));
    }
    
    _imgWarp.calcDelta(_ctrl);
    _imgWarp.genNewImg(_images[_currentImageNum].getPixels(),_dstPix,1);
    _tex.loadData(_dstPix);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(_ctrl.size() > 0)
    {
        //motion全部手打ち
        //TODO: magic number へらす
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
    
    if(ofGetFrameNum()%3 == 0)
    {
        if(_currentImageNum != _currentImageNumBuf)
        {
            initTracker();
            
            ofxCv::convertColor(_images[_currentImageNum], _grayImg, CV_RGB2GRAY);
            _grayImg.update();
            
            _currentImageNumBuf = _currentImageNum;
        }
        
        _imgWarp.calcDelta(_ctrl);
        _imgWarp.genNewImg(_images[_currentImageNum].getPixels(),_dstPix,1);
        _tex.loadData(_dstPix);
        
        ofPixels p;
        _tex.readToPixels(p);
        _warpedImg.setFromPixels(p);
        
        ofxCv::convertColor(_warpedImg, _grayWarpedImg, CV_RGB2GRAY);
        _grayWarpedImg.update();
        
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

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
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
    _gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ')
    {
        _bDrawPoints = !_bDrawPoints;
    }
    
    if(key == 'c')
    {
        initTracker();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == OF_KEY_DOWN)
    {
        if (_dir.size() > 0){
            _currentImageNum++;
            _currentImageNum %= _dir.size();
        }
    }
    
    if(key == OF_KEY_UP)
    {
        if (_dir.size() > 0){
            if(_currentImageNum > 0) _currentImageNum--;
            _currentImageNum %= _dir.size();
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(x < _img.getWidth())
    {
        _ctrl[_movingCtrlPosID].second = ofPoint(x,y);
        _imgWarp.calcDelta(_ctrl);
        _imgWarp.genNewImg(_img.getPixels(),_dstPix,1);
        _tex.loadData(_dstPix);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(x < _img.getWidth())
    {
        int minDist = 10000;
        _movingCtrlPosID = 0;
        for(int i=0;i<_ctrl.size();i++)
        {
            float dist = ofDist(x, y, _ctrl[i].second.x, _ctrl[i].second.y);
            if(dist < minDist)
            {
                minDist = dist;
                _movingCtrlPosID = i;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

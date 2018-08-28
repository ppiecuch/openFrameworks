#include "ofQtMediaPlayer.h"

// Qt includes
#include <QUrl>
#include <QList>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMovie>

class QtFrameGrabber : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QtFrameGrabber(QObject *parent = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);
signals:
    void frameAvailable(QImage frame);
public slots:

};


QtFrameGrabber::QtFrameGrabber(QObject *parent) :
    QAbstractVideoSurface(parent)
{
}

QList<QVideoFrame::PixelFormat> QtFrameGrabber::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
        << QVideoFrame::Format_ARGB32
        << QVideoFrame::Format_ARGB32_Premultiplied
        << QVideoFrame::Format_RGB32
        << QVideoFrame::Format_RGB24
        << QVideoFrame::Format_RGB565
        << QVideoFrame::Format_RGB555
        << QVideoFrame::Format_ARGB8565_Premultiplied
        << QVideoFrame::Format_BGRA32
        << QVideoFrame::Format_BGRA32_Premultiplied
        << QVideoFrame::Format_BGR32
        << QVideoFrame::Format_BGR24
        << QVideoFrame::Format_BGR565
        << QVideoFrame::Format_BGR555
        << QVideoFrame::Format_BGRA5658_Premultiplied
        << QVideoFrame::Format_AYUV444
        << QVideoFrame::Format_AYUV444_Premultiplied
        << QVideoFrame::Format_YUV444
        << QVideoFrame::Format_YUV420P
        << QVideoFrame::Format_YV12
        << QVideoFrame::Format_UYVY
        << QVideoFrame::Format_YUYV
        << QVideoFrame::Format_NV12
        << QVideoFrame::Format_NV21
        << QVideoFrame::Format_IMC1
        << QVideoFrame::Format_IMC2
        << QVideoFrame::Format_IMC3
        << QVideoFrame::Format_IMC4
        << QVideoFrame::Format_Y8
        << QVideoFrame::Format_Y16
        << QVideoFrame::Format_Jpeg
        << QVideoFrame::Format_CameraRaw
        << QVideoFrame::Format_AdobeDng;
}

bool QtFrameGrabber::present(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage image(cloneFrame.bits(),
                           cloneFrame.width(),
                           cloneFrame.height(),
                           QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));
        emit frameAvailable(image);
        cloneFrame.unmap();
        return true;
    }
    return false;
}


// https://stackoverflow.com/questions/37724602/how-to-save-a-frame-using-qmediaplayer
ofQtMediaPlayer::ofQtMediaPlayer() {
    player = new QMediaPlayer;
    playlist = new QMediaPlaylist;
    frameGrabber = new QtFrameGrabber;
    player->setVideoOutput(frameGrabber);
    QObject::connect(frameGrabber, &QtFrameGrabber::frameAvailable, [](QImage image) {
        // handle new frame
    });
}

ofQtMediaPlayer::ofQtMediaPlayer(ofQtMediaPlayer && other)
{
    player->setMedia(other.player->media());
}

ofQtMediaPlayer::~ofQtMediaPlayer() {
    player->stop();
    player->deleteLater();
    frameGrabber->deleteLater();
}

ofQtMediaPlayer & ofQtMediaPlayer::operator=(ofQtMediaPlayer && other) {
	if (&other == this) {
		return *this;
	}

    player->setMedia(other.player->media());
	return *this;
}

bool ofQtMediaPlayer::load(string path){

    path = ofToDataPath(path); 

    close();
    playlist->addMedia(QUrl::fromLocalFile(path.c_str()));
    return true;
}

void ofQtMediaPlayer::close(){
	player->setMedia(Q_NULLPTR);
}

void ofQtMediaPlayer::update(){
}

void ofQtMediaPlayer::play(){
    player->play();
}

void ofQtMediaPlayer::stop(){
    player->stop();
}       
    
bool ofQtMediaPlayer::isFrameNew() const{
    return false;
}

const ofPixels & ofQtMediaPlayer::getPixels() const{
    ofPixels px;
    return px;
}

ofPixels & ofQtMediaPlayer::getPixels(){
    ofPixels px;
    return px;
}

float ofQtMediaPlayer::getWidth() const{
    if( isLoaded() ){
        return player->currentMedia().canonicalResource().resolution().width();
    }
    return 0.0;
}

float ofQtMediaPlayer::getHeight() const{
    if( isLoaded() ){
        return player->currentMedia().canonicalResource().resolution().height();
    }
    return 0.0;
}
    
bool ofQtMediaPlayer::isPaused() const{
    return (player->state() ==  QMediaPlayer::PausedState);
}

bool ofQtMediaPlayer::isLoaded() const{
    return player->state() == QMediaPlayer::LoadedMedia;
}

bool ofQtMediaPlayer::isPlaying() const{
    return (player->state() ==  QMediaPlayer::PlayingState);
}

bool ofQtMediaPlayer::setPixelFormat(ofPixelFormat pixelFormat){

    qDebug() << Q_FUNC_INFO << "Unsupported.";

    switch (pixelFormat) {
        case OF_PIXELS_RGB:
        case OF_PIXELS_BGR:
        case OF_PIXELS_BGRA:
        case OF_PIXELS_RGBA:
            frameGrabber->surfaceFormat();
            return true;
        default:
            return false;
	}
}

ofPixelFormat ofQtMediaPlayer::getPixelFormat() const{
    qDebug() << Q_FUNC_INFO << "Unsupported.";
    return OF_PIXELS_RGBA;
}
        
//should implement!
float ofQtMediaPlayer::getPosition() const{
    return player->position();
}

float ofQtMediaPlayer::getSpeed() const{
    return player->playbackRate();
}

float ofQtMediaPlayer::getDuration() const{
    return player->duration() / 1000.0;
}


bool ofQtMediaPlayer::getIsMovieDone() const{
    return player->mediaStatus() == QMediaPlayer::EndOfMedia;
}
    
void ofQtMediaPlayer::setPaused(bool bPause){
    if (bPause)
        player->pause();
    else
        player->play();
}

void ofQtMediaPlayer::setPosition(float pct){
    player->setPosition(pct);
}

void ofQtMediaPlayer::setVolume(float volume){
    player->setVolume(volume);
}

void ofQtMediaPlayer::setLoopState(ofLoopType state){
    if( state == OF_LOOP_NONE ){
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if( state == OF_LOOP_NORMAL ){
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    }else{
        ofLogError("ofQtMediaPlayer") << " cannot set loop of type palindrome " << endl;
    }
}

qreal ofQtMediaPlayer::videoFrameRate(){
    return frameGrabber->surfaceFormat().frameRate();
}

void ofQtMediaPlayer::setSpeed(float speed){
    player->setPlaybackRate(speed);
}
    
int ofQtMediaPlayer::getCurrentFrame() const{
    return player->position();
}

int ofQtMediaPlayer::getTotalNumFrames() const{
    return player->duration();
}

bool ofQtMediaPlayer::isLooping() const{
    return (playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop);
}

ofLoopType ofQtMediaPlayer::getLoopState() const{
    if(isLooping() ){
        return OF_LOOP_NORMAL;
    }
    return OF_LOOP_NONE; 
}

void ofQtMediaPlayer::setFrame(int frame){
    frame = ofClamp(frame, 0, getTotalNumFrames()); 
    return player->setPosition(frame);
}  // frame 0 = first frame...
    
void ofQtMediaPlayer::firstFrame(){
    setPosition(0.0); 
}

void ofQtMediaPlayer::nextFrame(){
    player->setPosition(player->position()+1);
}

void ofQtMediaPlayer::previousFrame(){
    player->setPosition(player->position()-1);
}

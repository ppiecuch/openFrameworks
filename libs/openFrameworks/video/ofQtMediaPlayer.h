
#pragma once 

#include "ofVideoPlayer.h"

class QMediaPlayer;
class QMediaPlaylist;
class QtFrameGrabber;

class ofQtMediaPlayer : public ofBaseVideoPlayer{
    public:
		ofQtMediaPlayer();
		~ofQtMediaPlayer();
		ofQtMediaPlayer(const ofQtMediaPlayer&) = delete;
		ofQtMediaPlayer & operator=(const ofQtMediaPlayer&) = delete;
		ofQtMediaPlayer(ofQtMediaPlayer &&);
		ofQtMediaPlayer & operator=(ofQtMediaPlayer&&);

        bool                load(std::string path);
        void                update();

        void                close();
    
        void                play();
        void                stop();     
    
        bool                isFrameNew() const;

        const ofPixels &    getPixels() const;
        ofPixels &          getPixels();
    
        float               getWidth() const;
        float               getHeight() const;
    
        bool                isPaused() const;
        bool                isLoaded() const;
        bool                isPlaying() const;
    
        bool                setPixelFormat(ofPixelFormat pixelFormat);
        ofPixelFormat       getPixelFormat() const;

        float               getPosition() const;
        float               getSpeed() const;
        float               getDuration() const;
        bool                getIsMovieDone() const;
    
        void                setPaused(bool bPause);
        void                setPosition(float pct);
        void                setVolume(float volume); // 0..1
        void                setLoopState(ofLoopType state);
        bool                isLooping() const;
        void                setSpeed(float speed);
        void                setFrame(int frame);  // frame 0 = first frame...
    
        int                 getCurrentFrame() const;
        int                 getTotalNumFrames() const;
        ofLoopType          getLoopState() const;
    
        qreal               videoFrameRate();
        void                firstFrame();
        void                nextFrame();
        void                previousFrame();

    protected:
        QMediaPlayer  *player;
        QMediaPlaylist *playlist;
        QtFrameGrabber *frameGrabber;
};

#pragma once

#include "ofConstants.h"

#ifdef OF_SOUND_PLAYER_QT
#include "ofSoundBaseTypes.h"
#include "ofFileUtils.h"


// ---------------------------------------------------------------------------- SOUND SYSTEM Qt

// --------------------- global functions:
void ofQtSoundStopAll();
void ofQtSoundSetVolume(float vol);
void ofQtSoundUpdate();
float * ofQtSoundGetSpectrum(int nBands);
void ofQtSetBuffersize(unsigned int bs);


// --------------------- player functions:
class ofQtSoundPlayer : public ofBaseSoundPlayer {

	public:

		ofQtSoundPlayer();
		virtual ~ofQtSoundPlayer();

		bool load(const std::filesystem::path& fileName, bool stream = false);
		void unload();
		void play();
		void stop();

		void setVolume(float vol);
		void setPan(float vol);
		void setSpeed(float spd);
		void setPaused(bool bP);
		void setLoop(bool bLp);
		void setMultiPlay(bool bMp);
		void setPosition(float pct); // 0 = start, 1 = end;
		void setPositionMS(int ms);

		float getPosition() const;
		int getPositionMS() const;
		bool isPlaying() const;
		float getSpeed() const;
		float getPan() const;
		float getVolume() const;
		bool isLoaded() const;

		static void initializeQt();
		static void closeQt();
	
		bool isStreaming;
		bool bMultiPlay;
		bool bLoop;
		bool bLoadedOk;
		bool bPaused;
		float pan; // -1 to 1
		float volume; // 0 - 1
		float internalFreq; // 44100 ?
		float speed; // -n to n, 1 = normal, -1 backwards
		unsigned int length; // in samples;
};

#endif // OF_SOUND_PLAYER_QT

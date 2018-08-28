#include "ofQtSoundPlayer.h"

#ifdef OF_SOUND_PLAYER_QT

// --------------------- player functions:
ofQtSoundPlayer::ofQtSoundPlayer() {
}

ofQtSoundPlayer::~ofQtSoundPlayer() {
}


bool ofQtSoundPlayer::load(const std::filesystem::path& fileName, bool stream) {
}
void ofQtSoundPlayer::unload(){
}
void ofQtSoundPlayer::play(){
}
void ofQtSoundPlayer::stop(){
}

void ofQtSoundPlayer::setVolume(float vol){
}
void ofQtSoundPlayer::setPan(float vol){
}
void ofQtSoundPlayer::setSpeed(float spd){
}
void ofQtSoundPlayer::setPaused(bool bP){
}
void ofQtSoundPlayer::setLoop(bool bLp){
}
void ofQtSoundPlayer::setMultiPlay(bool bMp){
}
void ofQtSoundPlayer::setPosition(float pct){
}
void ofQtSoundPlayer::setPositionMS(int ms){
}

float ofQtSoundPlayer::getPosition() const{
}
int ofQtSoundPlayer::getPositionMS() const{
}
bool ofQtSoundPlayer::isPlaying() const{
}
float ofQtSoundPlayer::getSpeed() const{
}
float ofQtSoundPlayer::getPan() const{
}
float ofQtSoundPlayer::getVolume() const{
}
bool ofQtSoundPlayer::isLoaded() const{
}

void ofQtSoundPlayer::initializeQt(){
}
void ofQtSoundPlayer::closeQt(){
}

#endif // OF_SOUND_PLAYER_QT

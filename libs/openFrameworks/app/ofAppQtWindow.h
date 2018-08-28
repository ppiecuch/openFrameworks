#pragma once

#include "ofConstants.h"

#include "ofAppBaseWindow.h"
#include "ofEvents.h"
#include "ofPixels.h"
#include "ofRectangle.h"

#include <string>
#include <memory>

class Window;
class ofBaseApp;

#ifdef TARGET_OPENGLES
class ofQtWindowSettings: public ofGLESWindowSettings{
#else
class ofQtWindowSettings: public ofGLWindowSettings{
#endif
public:
	ofQtWindowSettings(){}

#ifdef TARGET_OPENGLES
	ofQtWindowSettings(const ofGLESWindowSettings & settings)
	:ofGLESWindowSettings(settings){}
#else
	ofQtWindowSettings(const ofGLWindowSettings & settings)
	:ofGLWindowSettings(settings){}
#endif

	int numSamples = 4;
	bool doubleBuffering = true;
	int redBits = 8;
	int greenBits = 8;
	int blueBits = 8;
	int alphaBits = 8;
	int depthBits = 24;
	int stencilBits = 0;
	bool stereo = false;
	bool visible = true;
	bool iconified = false;
	bool decorated = true;
	bool resizable = true;
	int monitor = 0;
	bool multiMonitorFullScreen = false;
	std::shared_ptr<ofAppBaseWindow> shareContextWith;
};

#ifdef TARGET_OPENGLES
class ofAppQtWindow : public ofAppBaseGLESWindow{
#else
class ofAppQtWindow : public ofAppBaseGLWindow{
#endif

public:

    ofAppQtWindow();
	~ofAppQtWindow();

	// Can't be copied, use shared_ptr
	ofAppQtWindow(ofAppQtWindow & w) = delete;
	ofAppQtWindow & operator=(ofAppQtWindow & w) = delete;

	static void loop(){};
	static bool doesLoop(){ return false; }
	static bool allowsMultiWindow(){ return true; }
	static bool needsPolling(){ return true; }
	static void pollEvents();

    // this functions are only meant to be called from inside OF don't call them from your code
    using ofAppBaseWindow::setup;
#ifdef TARGET_OPENGLES
	void setup(const ofGLESWindowSettings & settings);
#else
	void setup(const ofGLWindowSettings & settings);
#endif
	void setup(const ofQtWindowSettings & settings);
	void update();
	void draw();

    bool getWindowShouldClose();
	void setWindowShouldClose();

	void close();

	void hideCursor();
	void showCursor();

	int getHeight();
	int getWidth();

	ofCoreEvents & events();
	std::shared_ptr<ofBaseRenderer> & renderer();
    ofQtWindowSettings getSettings(){ return settings; }

	glm::vec2 getWindowSize();
	glm::vec2 getScreenSize();
	glm::vec2  getWindowPosition();

	void setWindowTitle(std::string title);
	void setWindowPosition(int x, int y);
	void setWindowShape(int w, int h);

	void setOrientation(ofOrientation orientation);
	ofOrientation getOrientation();

	ofWindowMode getWindowMode();

	void setFullscreen(bool fullscreen);
	void toggleFullscreen();

	void enableSetupScreen();
	void disableSetupScreen();

	void setVerticalSync(bool bSync);

    void setClipboardString(const std::string& text);
    std::string getClipboardString();

    float getPixelScreenCoordScale();

    void makeCurrent();
	void swapBuffers();
	void startRender();
	void finishRender();

	static void listVideoModes();
	static void listMonitors();
	bool isWindowIconified();
	bool isWindowActive();
	bool isWindowResizeable();
	void iconify(bool bIconify);

protected:
	static ofAppQtWindow * setCurrent(Window* windowP);

    void * getQtContext();
    void * getQtWindow();

private:
	void setWindowIcon(const std::string & path);
	void setWindowIcon(const ofPixels & iconPixels);

	ofCoreEvents coreEvents;
	std::shared_ptr<ofBaseRenderer> currentRenderer;
	ofQtWindowSettings settings;

	ofWindowMode	windowMode;

	bool bEnableSetupScreen;
    bool bWindowShouldClose;

	ofRectangle windowRect;

	int buttonInUse;
	bool buttonPressed;

	int nFramesSinceWindowResized;

	Window * windowP;

    int getCurrentMonitor();

	ofBaseApp *	ofAppPtr;

    bool iconSet;
    
    friend class Window;
};

#include "ofAppQtWindow.h"
#include "ofEvents.h"

#include "ofBaseApp.h"
#include "ofGLRenderer.h"
#include "ofGLProgrammableRenderer.h"
#include "ofAppRunner.h"
#include "ofFileUtils.h"
#include "ofIcon.h"
#include "ofImage.h"

#include <QDebug>
#include <QClipboard>
#include <QDirIterator>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QWaitCondition>
#include <QPainter>
#include <QScreen>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QApplication>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>


//-------------------------------------------------------
double qtGetTime() {
	static QElapsedTimer timer;
	if (!timer.isValid())
	timer.start();
	return timer.elapsed() / 1000; 
}

void qtDelay(long ms) {
	SleepSimulator s;
	s.sleep(ms);
}

//------------------------------------------------------------
static void rotateMouseXY(ofOrientation orientation, int w, int h, double &x, double &y) {
	int savedY;
	switch(orientation) {
		case OF_ORIENTATION_180:
			x = w - x;
			y = h - y;
			break;

		case OF_ORIENTATION_90_RIGHT:
			savedY = y;
			y = x;
			x = w-savedY;
			break;

		case OF_ORIENTATION_90_LEFT:
			savedY = y;
			y = h - x;
			x = savedY;
			break;

		case OF_ORIENTATION_DEFAULT:
		default:
			break;
	}
}


// == Qt Window ==

#define pixelScreenCoordScale windowP->devicePixelRatio()
#define windowW               windowP->size().width()*pixelScreenCoordScale
#define windowH               windowP->size().height()*pixelScreenCoordScale
#define currentW              windowP->size().width()
#define currentH              windowP->size().height()

class Window : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT
private:
	bool m_needs_initialize, m_done, m_update_pending, m_resize_pending, m_auto_refresh;
	QOpenGLContext *m_context;
	QOpenGLPaintDevice *m_device;
    std::function<void()> m_setup;
    ofAppQtWindow * m_instance;
public:
	QPoint cursorPos;
public:
	Window(ofAppQtWindow *instance, std::function<void()> setup, QWindow *parent = 0) : QWindow(parent)
    , m_needs_initialize(true)
	, m_update_pending(false)
	, m_resize_pending(false)
	, m_auto_refresh(true)
	, m_context(0)
	, m_device(0)
    , m_setup(setup)
    , m_instance(instance)
	, m_done(false) {
		setSurfaceType(QWindow::OpenGLSurface);
	}
	~Window() { delete m_device; }
	void setAutoRefresh(bool a) { m_auto_refresh = a; }

	void initialize() {
		qDebug() << "OpenGL infos with gl functions:";
		qDebug() << "-------------------------------";
		qDebug() << " Renderer:" << (const char*)glGetString(GL_RENDERER);
		qDebug() << " Vendor:" << (const char*)glGetString(GL_VENDOR);
		qDebug() << " OpenGL Version:" << (const char*)glGetString(GL_VERSION);
		qDebug() << " GLSL Version:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

        //default window title
        setTitle(QString("Qt %1 [%2 on %3]")
            .arg(QT_VERSION_STR)
            .arg((const char*)glGetString(GL_RENDERER))
            .arg(QSysInfo::prettyProductName()));

        if (m_setup) m_setup();
	}
	void update() { renderLater(); }
	void render() {
        m_instance->update();
        m_instance->draw();
    }

	Qt::ScreenOrientation getQtOrientation(ofOrientation orientation) {
        switch(orientation) {
            case OF_ORIENTATION_180: return Qt::InvertedPortraitOrientation;
            case OF_ORIENTATION_90_RIGHT: return Qt::InvertedLandscapeOrientation;
            case OF_ORIENTATION_90_LEFT: return Qt::LandscapeOrientation;
            case OF_ORIENTATION_DEFAULT: return Qt::PrimaryOrientation;
        }
    }

    ofOrientation getOfOrientation() {
        switch(contentOrientation()) {
            case Qt::PrimaryOrientation: return OF_ORIENTATION_DEFAULT;
            case Qt::LandscapeOrientation: return OF_ORIENTATION_90_LEFT;
            case Qt::PortraitOrientation: return OF_ORIENTATION_DEFAULT;
            case Qt::InvertedLandscapeOrientation: return OF_ORIENTATION_90_RIGHT;
            case Qt::InvertedPortraitOrientation: return OF_ORIENTATION_180;
        }
    }

	//------------------------------------------------------------
    int QtToOFModifiers(Qt::KeyboardModifiers mods){
        int modifiers = 0;
        if(mods & Qt::ShiftModifier){
            modifiers |= OF_KEY_SHIFT;
        }

        if(mods & Qt::AltModifier){
            modifiers |= OF_KEY_ALT;
        }
        if(mods & Qt::ControlModifier){
            modifiers |= OF_KEY_CONTROL;
        }
        if(mods & Qt::MetaModifier){
            modifiers |= OF_KEY_SUPER;
        }
        return modifiers;
    }

	//------------------------------------------------------------
    void keyboard_cb(Qt::Key keycode, int scancode, const QString &text, bool press, int mods) {
        ofAppQtWindow * instance = m_instance;
        int key = 0;
        uint32_t codepoint = 0;
        switch (keycode) {
            case Qt::Key_Escape:
                key = OF_KEY_ESC;
                break;
            case Qt::Key_F1:
                key = OF_KEY_F1;
                break;
            case Qt::Key_F2:
                key = OF_KEY_F2;
                break;
            case Qt::Key_F3:
                key = OF_KEY_F3;
                break;
            case Qt::Key_F4:
                key = OF_KEY_F4;
                break;
            case Qt::Key_F5:
                key = OF_KEY_F5;
                break;
            case Qt::Key_F6:
                key = OF_KEY_F6;
                break;
            case Qt::Key_F7:
                key = OF_KEY_F7;
                break;
            case Qt::Key_F8:
                key = OF_KEY_F8;
                break;
            case Qt::Key_F9:
                key = OF_KEY_F9;
                break;
            case Qt::Key_F10:
                key = OF_KEY_F10;
                break;
            case Qt::Key_F11:
                key = OF_KEY_F11;
                break;
            case Qt::Key_F12:
                key = OF_KEY_F12;
                break;
            case Qt::Key_Left:
                key = OF_KEY_LEFT;
                break;
            case Qt::Key_Right:
                key = OF_KEY_RIGHT;
                break;
            case Qt::Key_Up:
                key = OF_KEY_UP;
                break;
            case Qt::Key_Down:
                key = OF_KEY_DOWN;
                break;
            case Qt::Key_PageUp:
                key = OF_KEY_PAGE_UP;
                break;
            case Qt::Key_PageDown:
                key = OF_KEY_PAGE_DOWN;
                break;
            case Qt::Key_Home:
                key = OF_KEY_HOME;
                break;
            case Qt::Key_End:
                key = OF_KEY_END;
                break;
            case Qt::Key_Insert:
                key = OF_KEY_INSERT;
                break;
            case Qt::Key_Shift:
                key = OF_KEY_LEFT_SHIFT;
                break;
            case Qt::Key_Control:
                key = OF_KEY_LEFT_CONTROL;
                break;
            case Qt::Key_Alt:
                key = OF_KEY_LEFT_ALT;
                break;
            case Qt::Key_Super_L:
                key = OF_KEY_LEFT_SUPER;
                break;
            case Qt::Key_AltGr:
                key = OF_KEY_RIGHT_ALT;
                break;
            case Qt::Key_Super_R:
                key = OF_KEY_RIGHT_SUPER;
                break;
            case Qt::Key_Backspace:
                key = OF_KEY_BACKSPACE;
                break;
            case Qt::Key_Delete:
                key = OF_KEY_DEL;
                break;
            case Qt::Key_Enter:
                key = OF_KEY_RETURN;
                break;
            case Qt::Key_Return:
                key = OF_KEY_RETURN;
                break;
            case Qt::Key_Tab:
                key = OF_KEY_TAB;
                break;   
            default:
                if (text.length())
                    codepoint = text[0].unicode();
                key = codepoint;
                break;
        }

        if(press){
            instance->events().notifyKeyPressed(key,keycode,scancode,codepoint);
        }else{
            instance->events().notifyKeyReleased(key,keycode,scancode,codepoint);
        }
    }

    void mouse_cb( Qt::MouseButton button, bool pressed, int modifiers) {
        ofAppQtWindow * instance = m_instance;
        int ofbutton = 0; switch(button){
        case Qt::LeftButton:
            ofbutton = OF_MOUSE_BUTTON_LEFT;
            break;
        case Qt::RightButton:
            ofbutton = OF_MOUSE_BUTTON_RIGHT;
            break;
        case Qt::MiddleButton:
            ofbutton = OF_MOUSE_BUTTON_MIDDLE;
            break;
        }
        ofMouseEventArgs::Type action = pressed?ofMouseEventArgs::Pressed:ofMouseEventArgs::Released;
        instance->buttonPressed = pressed;
        instance->buttonInUse = ofbutton;

        ofMouseEventArgs args(action, instance->events().getMouseX(), instance->events().getMouseY(), ofbutton, modifiers);
        instance->events().notifyMouseEvent(args);
    }

    void motion_cb(double x, double y) {
        ofAppQtWindow * instance = m_instance;
        rotateMouseXY(getOfOrientation(), instance->getWidth(), instance->getHeight(), x, y);

        ofMouseEventArgs::Type action = instance->buttonPressed?ofMouseEventArgs::Dragged:ofMouseEventArgs::Moved;
        ofMouseEventArgs args(action, x, y, instance->buttonInUse, instance->events().getModifiers());
        instance->events().notifyMouseEvent(args);
    }

    void entry_cb(int entered) {
        ofAppQtWindow * instance = m_instance;
        ofMouseEventArgs args(entered?ofMouseEventArgs::Entered:ofMouseEventArgs::Exited,
            instance->events().getMouseX(),
            instance->events().getMouseY(),
            instance->buttonInUse,
            instance->events().getModifiers());
        instance->events().notifyMouseEvent(args);
    }

    void scroll_cb(double x, double y) {
        ofAppQtWindow * instance = m_instance;
        rotateMouseXY(getOfOrientation(), instance->getWidth(), instance->getHeight(), x, y);
        instance->events().notifyMouseScrolled(instance->events().getMouseX(), instance->events().getMouseY(), x, y);
    }

    void drop_cb(int numFiles, const char** dropString) {
        ofAppQtWindow * instance = m_instance;
        ofDragInfo drag;
        drag.position = {instance->events().getMouseX(), instance->events().getMouseY()};
        drag.files.resize(numFiles);
        for(int i=0; i<(int)drag.files.size(); i++){
            drag.files[i] = std::filesystem::path(dropString[i]).string();
        }
        instance->events().notifyDragEvent(drag);
    }

    void error_cb(int errorCode, const char* errorDescription){
        ofLogError("ofAppQtWindow") << errorCode << ": " << errorDescription;
    }

    void resize_cb(int w, int h) {
        ofAppQtWindow * instance = m_instance;
        instance->events().notifyWindowResized(w, h);
        instance->nFramesSinceWindowResized = 0;
    }

    //------------------------------------------------------------
    void mousePressEvent(QMouseEvent *event) {
		cursorPos = QPoint(event->x(), event->y());
		const Qt::KeyboardModifiers modifiers = event->modifiers();
		if (event->buttons() & Qt::LeftButton) mouse_cb(Qt::LeftButton, true, QtToOFModifiers(modifiers));
		if (event->buttons() & Qt::RightButton) mouse_cb(Qt::RightButton, true, QtToOFModifiers(modifiers));
		if (event->buttons() & Qt::MiddleButton) mouse_cb(Qt::MiddleButton, true, QtToOFModifiers(modifiers));
	}
	void mouseReleaseEvent(QMouseEvent *event) {
		const Qt::KeyboardModifiers modifiers = event->modifiers();
        // event->x(),y() returns 0 - use last saved position
		mouse_cb(event->button(), false, QtToOFModifiers(modifiers));
	}
	void mouseMoveEvent(QMouseEvent *event) {
		cursorPos = QPoint(event->x(), event->y());
        motion_cb(event->x(), event->y());
	}
#ifndef QT_NO_WHEELEVENT
    void wheelEvent (QWheelEvent *e)
    {
        QPoint numPixels = e->pixelDelta() / 10; // resoultion
        QPoint numDegrees = e->angleDelta() / 8;

        if (!numPixels.isNull()) {
            scroll_cb(numPixels.x(), numPixels.y());
        } else if (!numDegrees.isNull()) {
            QPoint numSteps = numDegrees / 15;
            scroll_cb(numSteps.x(), numSteps.y());
        }      
    }
#endif
    void keyPressEvent(QKeyEvent* event) {
		switch(event->key()) {
			case Qt::Key_Escape: quit(); break;
			default: event->ignore();
			break;
		}
	}
	void quit() { m_done = true; close(); }
	bool done() const { return m_done; }
	
	QOpenGLContext *context() const { return m_context; }
	
	ofAppQtWindow* getAppInstance(){ return m_instance; }


protected:
	void closeEvent(QCloseEvent *event) { quit(); }
	bool event(QEvent *event) {
		switch (event->type()) {
			case QEvent::UpdateRequest:
				m_update_pending = false;
				renderNow();
				return true;
			default:
				return QWindow::event(event);
		}
	}
	void exposeEvent(QExposeEvent *event) {
		Q_UNUSED(event);
		if (isExposed()) renderNow();
	}
	void resizeEvent(QResizeEvent *event)
	{
		Q_UNUSED(event);
		renderLater();
	}
public slots:
	void renderLater() {
		if (!m_update_pending) {
			m_update_pending = true;
			QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
		}
	}
	void renderNow() {
		if (!isExposed() || m_done) return;
        bool needsInitialize = false;
		if (!m_context) {
			m_context = new QOpenGLContext(this);
			m_context->setFormat(requestedFormat());
			m_context->create();
			needsInitialize = true;
		}
		m_context->makeCurrent(this);
		if (needsInitialize) {
			initializeOpenGLFunctions();
			initialize();
		}
		render();
		if (m_auto_refresh) renderLater();
	}
	void makeCurrent() {
        assert(m_context);
		m_context->makeCurrent(this);
    }
	void swapBuffers() {
        assert(m_context);
		if (isExposed()) m_context->swapBuffers(this);
    }
};


//-------------------------------------------------------
ofAppQtWindow::ofAppQtWindow(){
    
	bEnableSetupScreen	= true;
    bWindowShouldClose  = false;
	buttonInUse			= 0;
	buttonPressed		= false;

	windowMode			= OF_WINDOW;

	ofAppPtr			= nullptr;

	nFramesSinceWindowResized = 0;
	iconSet = false;
	windowP = nullptr;
}

ofAppQtWindow::~ofAppQtWindow(){
	close();
}

void ofAppQtWindow::close(){
	if(windowP){
        delete windowP;
    }
	windowP = nullptr;
}

//------------------------------------------------------------
#ifdef TARGET_OPENGLES
void ofAppQtWindow::setup(const ofGLESWindowSettings & settings){
#else
void ofAppQtWindow::setup(const ofGLWindowSettings & settings){
#endif
	const ofQtWindowSettings * glSettings = dynamic_cast<const ofQtWindowSettings*>(&settings);
	if(glSettings){
		setup(*glSettings);
	}else{
		setup(ofQtWindowSettings(settings));
	}
}

void ofAppQtWindow::setup(const ofQtWindowSettings & _settings){
	if(windowP){
		ofLogError() << "window already setup, probably you are mixing old and new style setup";
		ofLogError() << "call only ofCreateWindow(settings) or ofSetupOpenGL(...)";
		ofLogError() << "calling window->setup() after ofCreateWindow() is not necesary and won't do anything";
		return;
	}
	settings = _settings;

	QSurfaceFormat surface_format = QSurfaceFormat::defaultFormat();
	surface_format.setAlphaBufferSize( settings.alphaBits );
	surface_format.setBlueBufferSize( settings.blueBits );
	surface_format.setGreenBufferSize( settings.greenBits );
	surface_format.setRedBufferSize( settings.redBits );
	surface_format.setDepthBufferSize( settings.depthBits );
	surface_format.setStencilBufferSize( settings.stencilBits );
	surface_format.setSamples( settings.numSamples );
# ifdef DEBUG
    surface_format.setOption( QSurfaceFormat::DebugContext );
# endif
    surface_format.setProfile( QSurfaceFormat::CoreProfile );
	#ifdef TARGET_OPENGLES
        surface_format.setRenderableType( QSurfaceFormat::OpenGLES );
    #else
        surface_format.setRenderableType( QSurfaceFormat::OpenGL );
        if((settings.glVersionMajor==3 && settings.glVersionMinor>=2) || settings.glVersionMajor>=4){
            surface_format.setProfile( QSurfaceFormat::CoreProfile );
        }else if(settings.glVersionMajor>=3){
            surface_format.setProfile( QSurfaceFormat::CompatibilityProfile );
        }else{
            surface_format.setProfile( QSurfaceFormat::NoProfile );
        }
        surface_format.setVersion( settings.glVersionMajor, settings.glVersionMinor );
    #endif
#ifndef TARGET_OSX
	surface_format.setSwapBehavior( settings.doubleBuffering?QSurfaceFormat::DoubleBuffer:QSurfaceFormat::DefaultSwapBehavior );
#endif
	// surface_format.setSwapInterval( 1 );
    surface_format.setStereo(settings.stereo);

	if (settings.resizable)
    {
        
    }
	if (settings.decorated)
    {
        
    }

	#ifdef TARGET_OPENGLES
		if(settings.glesVersion>=2){
			currentRenderer = shared_ptr<ofBaseRenderer>(new ofGLProgrammableRenderer(this));
		}else{
			currentRenderer = shared_ptr<ofBaseRenderer>(new ofGLRenderer(this));
		}
	#else
		if(settings.glVersionMajor>=3){
			currentRenderer = shared_ptr<ofBaseRenderer>(new ofGLProgrammableRenderer(this));
		}else{
			currentRenderer = shared_ptr<ofBaseRenderer>(new ofGLRenderer(this));
		}
	#endif

    windowP = new Window(this, [=](){
        // setup code
        if(currentRenderer->getType()==ofGLProgrammableRenderer::TYPE){
    #ifndef TARGET_OPENGLES
            static_cast<ofGLProgrammableRenderer*>(currentRenderer.get())->setup(settings.glVersionMajor,settings.glVersionMinor);
    #else
            static_cast<ofGLProgrammableRenderer*>(currentRenderer.get())->setup(settings.glesVersion,0);
    #endif
        }else{
            static_cast<ofGLRenderer*>(currentRenderer.get())->setup();
        }
        events().notifySetup();
    });
    if(!windowP){
        ofLogError("ofAppQtWindow") << "couldn't create Qt window";
        return;
    }
    windowP->setFormat(surface_format);
    windowP->resize(settings.getWidth(), settings.getHeight());
    windowP->show();
    if(settings.windowMode==OF_FULLSCREEN){
        setFullscreen(true);
    }else{
        if (settings.isPositionSet()) {
            windowP->setPosition(settings.getPosition().x,settings.getPosition().y);
        }
    }
    if(!iconSet){
        ofPixels iconPixels;
        #ifdef DEBUG
            iconPixels.allocate(ofIconDebug.width,ofIconDebug.height,ofIconDebug.bytes_per_pixel);
            GIMP_IMAGE_RUN_LENGTH_DECODE(iconPixels.getData(),ofIconDebug.rle_pixel_data,iconPixels.getWidth()*iconPixels.getHeight(),ofIconDebug.bytes_per_pixel);
        #else
            iconPixels.allocate(ofIcon.width,ofIcon.height,ofIcon.bytes_per_pixel);
            GIMP_IMAGE_RUN_LENGTH_DECODE(iconPixels.getData(),ofIcon.rle_pixel_data,iconPixels.getWidth()*iconPixels.getHeight(),ofIcon.bytes_per_pixel);
        #endif
        setWindowIcon(iconPixels);
    }
    if(settings.iconified){
        iconify(true);
    }
	
    setVerticalSync(true);
}

//------------------------------------------------------------
void ofAppQtWindow::setWindowIcon(const string & path){
    ofPixels iconPixels;
	ofLoadImage(iconPixels,path);
	setWindowIcon(iconPixels);
}

//------------------------------------------------------------
void ofAppQtWindow::setWindowIcon(const ofPixels & iconPixels){
    windowP->setIcon(QIcon(QPixmap::fromImage(getQImageFromPixels(iconPixels))));
}

//--------------------------------------------
void ofAppQtWindow::pollEvents(){ 
    qApp->processEvents();
}

//--------------------------------------------
ofCoreEvents & ofAppQtWindow::events(){
	return coreEvents;
}

//--------------------------------------------
shared_ptr<ofBaseRenderer> & ofAppQtWindow::renderer(){
	return currentRenderer;
}

//--------------------------------------------
void ofAppQtWindow::update(){
	events().notifyUpdate();
}

//--------------------------------------------
void ofAppQtWindow::draw(){
	currentRenderer->startRender();
	if( bEnableSetupScreen ) currentRenderer->setupScreen();

	events().notifyDraw();

	#if defined(TARGET_WIN32) || defined(Q_OS_WIN)
	if (currentRenderer->getBackgroundAuto() == false){
		// on a PC resizing a window with this method of accumulation (essentially single buffering)
		// is BAD, so we clear on resize events.
		if (nFramesSinceWindowResized < 3){
			currentRenderer->clear();
		} else {
			if ( (events().getFrameNum() < 3 || nFramesSinceWindowResized < 3) && settings.doubleBuffering){
                windowP->swapBuffers();
			}else{
				glFlush();
			}
		}
	} else {
		if(settings.doubleBuffering){
            windowP->swapBuffers();
		} else {
			glFlush();
		}
	}
	#else
		if (currentRenderer->getBackgroundAuto() == false){
			// in accum mode resizing a window is BAD, so we clear on resize events.
			if (nFramesSinceWindowResized < 3){
				currentRenderer->clear();
			}
		}
		if(settings.doubleBuffering){
            windowP->swapBuffers();
		} else{
			glFlush();
		}
	#endif

	currentRenderer->finishRender();

	nFramesSinceWindowResized++;
}


//--------------------------------------------
void ofAppQtWindow::swapBuffers() {
    windowP->swapBuffers();
}

//--------------------------------------------
void ofAppQtWindow::startRender() {
	renderer()->startRender();
}

//--------------------------------------------
void ofAppQtWindow::finishRender() {
	renderer()->finishRender();
}

//--------------------------------------------
bool ofAppQtWindow::getWindowShouldClose(){
	return windowP->done();
}

//--------------------------------------------
void ofAppQtWindow::setWindowShouldClose(){
	windowP->quit();
}

//------------------------------------------------------------
void ofAppQtWindow::setWindowTitle(string title){
	windowP->setTitle(title.c_str());
}

//------------------------------------------------------------
float ofAppQtWindow::getPixelScreenCoordScale(){
    return pixelScreenCoordScale;
}

//------------------------------------------------------------
glm::vec2 ofAppQtWindow::getWindowSize(){
    return {currentW, currentH};
}

//------------------------------------------------------------
glm::vec2 ofAppQtWindow::getWindowPosition(){
    return {windowP->position().x(), windowP->position().y()};
}

//------------------------------------------------------------
int ofAppQtWindow::getCurrentMonitor(){
    int index = qApp->screens().indexOf(qApp->primaryScreen());
    return (index == -1)?0:index;
}


//------------------------------------------------------------
glm::vec2 ofAppQtWindow::getScreenSize(){
    QScreen *sc = qApp->primaryScreen();
    ofOrientation orientation = windowP->getOfOrientation();
    if( orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180 ){
        return {sc->size().width(), sc->size().height()};
    }else{
        return {sc->size().height(), sc->size().width()};
    }
}

//------------------------------------------------------------
int ofAppQtWindow::getWidth(){
    ofOrientation orientation = windowP->getOfOrientation();
	if( orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180 ){
		return currentW;
	}else{
		return currentH;
	}
}

//------------------------------------------------------------
int ofAppQtWindow::getHeight(){
    ofOrientation orientation = windowP->getOfOrientation();
	if( orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180 ){
		return currentH;
	}else{
		return currentW;
	}
}

//------------------------------------------------------------
ofWindowMode ofAppQtWindow::getWindowMode(){
	return windowMode;
}

//------------------------------------------------------------
void ofAppQtWindow::setWindowPosition(int x, int y){
    windowP->setPosition(QPoint(x,y));
}

//------------------------------------------------------------
void ofAppQtWindow::setWindowShape(int w, int h){
    if(windowMode == OF_WINDOW){
		windowP->resize(w/pixelScreenCoordScale, h/pixelScreenCoordScale);
	}
}

//------------------------------------------------------------
void ofAppQtWindow::hideCursor(){
    windowP->setCursor(Qt::BlankCursor);
};

//------------------------------------------------------------
void ofAppQtWindow::showCursor(){
    windowP->unsetCursor();
};

//------------------------------------------------------------
void ofAppQtWindow::enableSetupScreen(){
	bEnableSetupScreen = true;
};

//------------------------------------------------------------
void ofAppQtWindow::disableSetupScreen(){
	bEnableSetupScreen = false;
};

//------------------------------------------------------------
void ofAppQtWindow::setFullscreen(bool fullscreen){
 
	ofWindowMode curWindowMode = windowMode;
 
	if (fullscreen){
		windowMode = OF_FULLSCREEN;
	}else{
		windowMode = OF_WINDOW;
    }

    //we only want to change window mode if the requested window is different to the current one.
    bool bChanged = windowMode != curWindowMode;
    if( !bChanged ){
        return;
	}

	if (windowMode = OF_FULLSCREEN){
        windowP->showFullScreen();
	}else{
        windowP->showNormal();
    }
}

//------------------------------------------------------------
void ofAppQtWindow::toggleFullscreen(){
	if (windowMode == OF_GAME_MODE) return;


	if (windowMode == OF_WINDOW){
		setFullscreen(true);
	} else {
		setFullscreen(false);
	}
}

//------------------------------------------------------------
void ofAppQtWindow::setOrientation(ofOrientation orientation){
	windowP->reportContentOrientationChange(windowP->getQtOrientation(orientation));
}

//------------------------------------------------------------
ofOrientation ofAppQtWindow::getOrientation(){
	return windowP->getOfOrientation();
}

//------------------------------------------------------------
ofAppQtWindow * ofAppQtWindow::setCurrent(Window* windowP){
	ofAppQtWindow * instance = windowP->getAppInstance();
	shared_ptr<ofMainLoop> mainLoop = ofGetMainLoop();
	if(mainLoop){
		mainLoop->setCurrentWindow(instance);
	}
	instance->makeCurrent();
	return instance;
}

//------------------------------------------------------------
void * ofAppQtWindow::getQtContext() { return windowP->context(); }
void * ofAppQtWindow::getQtWindow() { return windowP; }

//------------------------------------------------------------
void ofAppQtWindow::setVerticalSync(bool bVerticalSync){
    assert(windowP);
    QSurfaceFormat fmt = windowP->format();
	if(bVerticalSync){
		fmt.setSwapInterval( 1);
	}else{
		fmt.setSwapInterval(0);
	}
	windowP->setFormat(fmt);
}

//------------------------------------------------------------
void ofAppQtWindow::setClipboardString(const string& text) {
    QGuiApplication::clipboard()->setText(text.c_str());
}

//------------------------------------------------------------
string ofAppQtWindow::getClipboardString() {
	const char* clipboard =  QGuiApplication::clipboard()->text().toUtf8().constData();

	if (clipboard) {
		return clipboard;
	} else {
		return "";
	}
}

//------------------------------------------------------------
bool ofAppQtWindow::isWindowIconified(){
	return (windowP->windowState()&Qt::WindowMinimized)!=0;
}

//------------------------------------------------------------
bool ofAppQtWindow::isWindowActive(){
	return true;
}

//------------------------------------------------------------
bool ofAppQtWindow::isWindowResizeable(){
	return true;
}

//------------------------------------------------------------
void ofAppQtWindow::iconify(bool bIconify){
    assert(windowP);
	if(bIconify)
		windowP->showMinimized();
	else
		windowP->showNormal();
}

//------------------------------------------------------------
void ofAppQtWindow::makeCurrent(){
    assert(windowP);
	windowP->makeCurrent();
}

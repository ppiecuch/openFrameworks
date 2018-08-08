#pragma once
#include <stdint.h>

//-------------------------------
#define OF_VERSION_MAJOR 0
#define OF_VERSION_MINOR 11
#define OF_VERSION_PATCH 0
#define OF_VERSION_PRE_RELEASE "master"

// Set to 1 for compatibility with old projects using ofVec instead of glm
#ifndef OF_USE_LEGACY_VECTOR_MATH
	#define OF_USE_LEGACY_VECTOR_MATH 0
#endif

// Set to 1 to use std filesystem instead of boost's
#ifndef OF_USING_STD_FS
#define OF_USING_STD_FS 0
#endif

//-------------------------------

/// \brief This enumerates the targeted operating systems or platforms.
enum ofTargetPlatform{
	/// \brief 32- and 64-bit x86 architecture on Mac OSX.
	OF_TARGET_OSX,
	/// \brief 32- and 64-bit x86 architecture using MinGW on Windows OS.
    OF_TARGET_MINGW,
	/// \brief 32- and 64-bit x86 architecture using Visual Studio on Windows OS.
	OF_TARGET_WINVS,
	/// \brief 32- and 64-bit armv7, arm64, x86 (simulator) architecture Mac iOS.
	OF_TARGET_IOS,
	/// \brief 32- and 64-bit armeabi-v7a and x86 Android OS.
	OF_TARGET_ANDROID,
	/// \brief 32-bit x86 architecture on Linux OS.
	OF_TARGET_LINUX,
	/// \brief 64-bit x86 architecture on Linux OS.
	OF_TARGET_LINUX64,
	/// \brief 32-bit armv6 little endian architecture on Linux OS.
	OF_TARGET_LINUXARMV6L,
	/// \brief 32-bit armv7 little endian architecture on Linux OS.
	OF_TARGET_LINUXARMV7L,
	/// \brief Compiled to javascript using Emscripten.
	/// \sa https://github.com/kripken/emscripten
	OF_TARGET_EMSCRIPTEN,
	/// \brief Qt based build.
	OF_TARGET_QT,
};


#ifndef OF_TARGET_IPHONE
    #define OF_TARGET_IPHONE OF_TARGET_IOS
#endif


// Cross-platform deprecation warning
#ifdef __GNUC__
	// clang also has this defined. deprecated(message) is only for gcc>=4.5
	#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || __GNUC__ > 4
        #define OF_DEPRECATED_MSG(message, ...) __VA_ARGS__ __attribute__ ((deprecated(message)))
    #else
        #define OF_DEPRECATED_MSG(message, ...) __VA_ARGS__ __attribute__ ((deprecated))
    #endif
	#define OF_DEPRECATED(...) __VA_ARGS__ __attribute__ ((deprecated))
	#define OF_INTERNAL_DEPRECATED(...) __VA_ARGS__ __attribute__ ((deprecated("OF core deprecated")))
#elif defined(_MSC_VER)
	#define OF_DEPRECATED_MSG(message, ...) __declspec(deprecated(message)) __VA_ARGS__
	#define OF_DEPRECATED(...) __declspec(deprecated) __VA_ARGS__
	#define OF_INTERNAL_DEPRECATED(...) __declspec(deprecated("OF core deprecated")) __VA_ARGS__
#else
	#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
	#define OF_DEPRECATED_MSG(message, ...) __VA_ARGS__
	#define OF_DEPRECATED(...) __VA_ARGS__
#endif

//-------------------------------
//  find the system type --------
//-------------------------------

// 		helpful:
// 		http://www.ogre3d.org/docs/api/html/OgrePlatform_8h-source.html

#if defined(QT_GUI_LIB) || defined(QT_OPENGL_LIB)
	#define TARGET_QT
    #ifdef QT_OPENGL_ES_2
        #define TARGET_OPENGLES
    #endif
	#define TARGET_IMPLEMENTS_URL_LOADER
    #include <QDateTime>
    #include <QFile>
    #include <QFileInfo>
    #include <QDirIterator>
    #include <QMutex>
    #include <QWaitCondition>
#elif defined( __WIN32__ ) || defined( _WIN32 )
	#define TARGET_WIN32
#elif defined( __APPLE_CC__)
    #define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
    #include <TargetConditionals.h>
	#if (TARGET_OS_IPHONE || TARGET_OS_IOS || TARGET_OS_SIMULATOR || TARGET_OS_IPHONE_SIMULATOR) && !TARGET_OS_TV && !TARGET_OS_WATCH
        #define TARGET_OF_IPHONE
        #define TARGET_OF_IOS
        #define TARGET_OPENGLES
        #include <unistd.h>
    #elif TARGET_OS_TV
        #define TARGET_OF_IOS
        #define TARGET_OF_TVOS
        #define TARGET_OPENGLES
        #include <unistd.h>
    #elif TARGET_OS_WATCH
        #define TARGET_OF_IOS
        #define TARGET_OF_WATCHOS
        #define TARGET_OPENGLES
        #include <unistd.h>
	#else
		#define TARGET_OSX
	#endif
#elif defined (__ANDROID__)
	#define TARGET_ANDROID
	#define TARGET_OPENGLES
#elif defined(__ARMEL__)
	#define TARGET_LINUX
	#define TARGET_OPENGLES
	#define TARGET_LINUX_ARM
#elif defined(__EMSCRIPTEN__)
	#define TARGET_EMSCRIPTEN
	#define TARGET_OPENGLES
	#define TARGET_NO_THREADS
	#define TARGET_PROGRAMMABLE_GL
	#define TARGET_IMPLEMENTS_URL_LOADER
#else
	#define TARGET_LINUX
#endif
//-------------------------------


// use Qt implementation insted of external dependencies:
#ifdef TARGET_QT
class SleepSimulator {
	QMutex localMutex;
	QWaitCondition sleepSimulator;
public:
	SleepSimulator() { localMutex.lock(); }
	void sleep(unsigned long sleepMS) { sleepSimulator.wait(&localMutex, sleepMS); }
	void CancelSleep() { sleepSimulator.wakeAll(); }
};

namespace std { namespace filesystem {
    struct path : public std::string {
        path() : std::string() { }
        path(const std::string s) : std::string(s) { }
        path(const char *p) : std::string(p) { }
        std::string string() const { return *this; }
        std::string generic_string() const { return string(); }
        const char* c_str() const { return this->c_str(); }
        path extension() const { return path(QFileInfo(string().c_str()).completeSuffix().toStdString()); }
        path filename() const { return path(QFileInfo(string().c_str()).fileName().toStdString()); }
        path stem() const { return path(QFileInfo(string().c_str()).baseName().toStdString()); }
        path& replace_extension( const path& replacement = path() ) {
            QFileInfo info(string().c_str());
            QString newName = info.path() + info.baseName() + replacement.string().c_str();
            *this = newName.toStdString();
            return *this;
        }
        path & operator /= (const path & p) {
            if ( ! p.empty() ) {
                if (this->empty()) {
                    *this = p;
                } else {
                    const auto sep = QDir::separator().toLatin1();
                    if (*this->string().rbegin() != sep)
                        *this += sep;
                    if (*p.string().begin() == sep)
                        *this += p.substr(1);
                    else
                        *this += p;
                }
            }
            return *this;
        }
        path make_preferred() const {  return path( QDir::toNativeSeparators(string().c_str()).toStdString() ); }
        path parent_path() { path(QFileInfo(string().c_str()).dir().path().toStdString()); }
        bool is_absolute() const { return QFileInfo(string().c_str()).isAbsolute(); }
    };
    inline qint64 last_write_time(const path &p) { return QFileInfo(string().c_str()).lastModified().toSecsSinceEpoch(); }
    inline size_t file_size(const path &p) { return QFile::exists(p.string().c_str()); }
    inline bool exists(const path &p) { return QFile::exists(p.string().c_str()); }
    inline path current_path() { return path( QDir::currentPath().toStdString() ); }
    inline void current_path(const path &p) { QDir::setCurrent(p.string().c_str()); }
    inline bool copy_file(const path& from, const path& to) {}
    inline bool rename(const path& old_p, const path& new_p) {}
    inline bool remove(const path& p) { QFile::remove(p.string().c_str()); }
    inline bool remove_all(const path& p) { return QDir().rmpath(p.string().c_str()); }
    inline bool create_directories(const path& p) { return QDir().mkpath(p.string().c_str()); }
    inline bool create_directory(const path& p) { return QDir().mkdir(p.string().c_str()); }
    inline path canonical(const path& p) { return QFileInfo(p.string().c_str()). canonicalFilePath().toStdString(); }
    inline path absolute(const path& p) { return QFileInfo(p.string().c_str()).absoluteFilePath().toStdString(); }
    inline bool is_regular_file(const path &p) { QFileInfo(p.string().c_str()).isFile(); }
	inline bool is_symlink(const path &p) { QFileInfo(p.string().c_str()).isSymLink(); }
	inline bool is_directory(const path &p) { return QFileInfo(p.string().c_str()).isDir(); }
    struct status_info {
        status_info(long uid, long gid, int perm) : st_uid(uid), st_gid(gid), st_perm(perm) {}
        QFile::Permissions permissions() const { return st_perm; };
        long st_uid, st_gid;
        QFile::Permissions st_perm;
    };
    inline status_info status(const path &p) {
        QFile::Permissions pm = QFileInfo(p.string().c_str()).permissions();
        return status_info(0, 0, pm);
    };
    struct directory_entry {
        directory_entry() {}
        directory_entry(const std::string &p) : entry(p) {}
        const std::filesystem::path path() const { return entry; }
        operator const std::filesystem::path& () const { return entry; }
        std::filesystem::path entry;
    };
    struct directory_iterator : public QDirIterator {
        directory_iterator() : QDirIterator("") {}
        directory_iterator(const std::filesystem::path &p, IteratorFlags flags = NoIteratorFlags) : QDirIterator(p.c_str(), flags), entry(filePath().toStdString()) {}
        const directory_entry& operator*() const { return entry; }
        const directory_entry *operator ->() { return &entry; }
        bool operator == (const directory_iterator & other) const {
            return (entry.path().empty() && other.entry.path().empty());
        }
        bool operator != (const directory_iterator & other) const { return !(*this == other); }
        void operator ++() { entry = next().toStdString(); }
        directory_entry entry;
    };
    struct recursive_directory_iterator : public directory_iterator {
        recursive_directory_iterator() : directory_iterator() {}
        recursive_directory_iterator(const std::filesystem::path &p) : directory_iterator(p.c_str(), Subdirectories) {}
    };
    inline path operator / (const path & lhs, const path & rhs)
    {
        path p(lhs);
        p /= rhs;
        return p;
    }
} }
#endif

// then the the platform specific includes:
#ifdef TARGET_QT
    #include <qopengl.h>
#endif

#ifdef TARGET_WIN32
	#define GLEW_STATIC
	#define GLEW_NO_GLU
	#include "GL/glew.h"
    #include "GL/wglew.h"
	#define __WINDOWS_DS__
	#define __WINDOWS_MM__
	#if (_MSC_VER)       // microsoft visual studio
		//TODO: Fix this in the code instead of disabling the warnings
		#define _CRT_SECURE_NO_WARNINGS
		#define _WINSOCK_DEPRECATED_NO_WARNINGS

		#include <stdint.h>
		#include <functional>
		#pragma warning(disable : 4068)		// unknown pragmas
		#pragma warning(disable : 4756)		// overflow in constant arithmetic
		#pragma warning(disable : 4800)		// 'Boolean' : forcing value to bool 'true' or 'false'

		// make microsoft visual studio complain less about double / float conversion and
		// truncation
		#pragma warning(disable : 4244)
		#pragma warning(disable : 4305)
		// warnings: http://msdn.microsoft.com/library/2c8f766e.aspx

		// NOMINMAX doesn't seem to work anymore in vs2015 so let's just remove them
		#undef min
		#undef max
	#endif

	#define TARGET_LITTLE_ENDIAN			// intel cpu

	// some gl.h files, like dev-c++, are old - this is pretty universal
	#ifndef GL_BGR_EXT
	#define GL_BGR_EXT 0x80E0
	#endif

	#define WIN32_HIGH_RES_TIMING

	// note: this is experimental!
	// uncomment to turn this on (only for windows machines)
	// if you want to try setting the timer to be high resolution
	// this could make camera grabbing and other low level
	// operations quicker, but you must quit the app normally,
	// ie, using "esc", rather than killing the process or closing
	// the console window in order to set the timer resolution back
	// to normal (since the high res timer might give the OS
	// problems)
	// info: http://www.geisswerks.com/ryan/FAQS/timing.html

#endif

#ifdef TARGET_OSX
	#ifndef __MACOSX_CORE__
		#define __MACOSX_CORE__
	#endif
	#include <unistd.h>
	#include "GL/glew.h"
	#include <OpenGL/gl.h>
	#include <ApplicationServices/ApplicationServices.h>

	#if defined(__LITTLE_ENDIAN__)
		#define TARGET_LITTLE_ENDIAN		// intel cpu
	#endif
#endif

#ifdef TARGET_LINUX

	#include <unistd.h>

	#ifdef TARGET_LINUX_ARM
		#ifdef TARGET_RASPBERRY_PI
			#include "bcm_host.h"
			// rpi firmware headers define countof
			// which messes up other libraries like glm
			#undef countof
		#endif

		#include "GLES/gl.h"
		#include "GLES/glext.h"
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"

		#define EGL_EGLEXT_PROTOTYPES
		#include "EGL/egl.h"
		#include "EGL/eglext.h"
	#else // normal linux
		#define GL_GLEXT_PROTOTYPES
		#include <GL/glew.h>
		#include <GL/gl.h>
		#include <GL/glext.h>
	#endif

	// for some reason, this isn't defined at compile time,
	// so this hack let's us work
	// for 99% of the linux folks that are on intel
	// everyone one else will have RGB / BGR issues.
	//#if defined(__LITTLE_ENDIAN__)
	#define TARGET_LITTLE_ENDIAN		// intel cpu
	//#endif

	// some things for serial compilation:
	#define B14400	14400
	#define B28800	28800

#endif


#ifdef TARGET_OF_IOS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>

	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>


	#define TARGET_LITTLE_ENDIAN		// arm cpu
#endif

#ifdef TARGET_ANDROID
	#include <typeinfo>
	#include <unistd.h>
	#include <GLES/gl.h>
	#define GL_GLEXT_PROTOTYPES
	#include <GLES/glext.h>

	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

	#define TARGET_LITTLE_ENDIAN
#endif

#ifdef TARGET_EMSCRIPTEN
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include "EGL/egl.h"
	#include "EGL/eglext.h"

	#define TARGET_LITTLE_ENDIAN
#endif

#include "tesselator.h"
typedef TESSindex ofIndexType;



#define OF_EXIT_APP(val)		std::exit(val);


// core: ---------------------------
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>


//------------------------------------------------ capture
// check if any video capture system is already defined from the compiler
#ifdef QT_MULTIMEDIA_LIB
  // use Qt multimedia framework
  #define OF_VIDEO_CAPTURE_QT
#elif !defined(OF_VIDEO_CAPTURE_GSTREAMER) && !defined(OF_VIDEO_CAPTURE_QUICKTIME) && !defined(OF_VIDEO_CAPTURE_DIRECTSHOW) && !defined(OF_VIDEO_CAPTURE_ANDROID) && !defined(OF_VIDEO_CAPTURE_IOS)
	#ifdef TARGET_LINUX

		#define OF_VIDEO_CAPTURE_GSTREAMER

	#elif defined(TARGET_OSX)
		//on 10.6 and below we can use the old grabber
		#ifndef MAC_OS_X_VERSION_10_7
			#define OF_VIDEO_CAPTURE_QUICKTIME
		//if we are below 10.12 or targeting below 10.12 we use QTKit
		#elif !defined(MAC_OS_X_VERSION_10_12) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12
			#define OF_VIDEO_CAPTURE_QTKIT
		#else
			#define OF_VIDEO_CAPTURE_AVF
        #endif

	#elif defined (TARGET_WIN32)

		// comment out this following line, if you'd like to use the
		// quicktime capture interface on windows
		// if not, we default to videoInput library for
		// direct show capture...

		#define OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP

		#ifdef OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP
			#define OF_VIDEO_CAPTURE_DIRECTSHOW
		#else
			#define OF_VIDEO_CAPTURE_QUICKTIME
		#endif

	#elif defined(TARGET_ANDROID)

		#define OF_VIDEO_CAPTURE_ANDROID

	#elif defined(TARGET_EMSCRIPTEN)

		#define OF_VIDEO_CAPTURE_EMSCRIPTEN

	#elif defined(TARGET_OF_IOS)

		#define OF_VIDEO_CAPTURE_IOS

	#endif
#endif

//------------------------------------------------  video player
// check if any video player system is already defined from the compiler
#ifdef QT_MULTIMEDIA_LIB
  // use Qt multimedia framework
  #define OF_VIDEO_PLAYER_QT
#elif !defined(OF_VIDEO_PLAYER_GSTREAMER) && !defined(OF_VIDEO_PLAYER_IOS) && !defined(OF_VIDEO_PLAYER_DIRECTSHOW) && !defined(OF_VIDEO_PLAYER_QUICKTIME) && !defined(OF_VIDEO_PLAYER_AVFOUNDATION) && !defined(OF_VIDEO_PLAYER_EMSCRIPTEN)
    #ifdef TARGET_LINUX
        #define OF_VIDEO_PLAYER_GSTREAMER
    #elif defined(TARGET_ANDROID)
        #define OF_VIDEO_PLAYER_ANDROID
    #elif defined(TARGET_OF_IOS)
        #define OF_VIDEO_PLAYER_IOS
	#elif defined(TARGET_WIN32)
        #define OF_VIDEO_PLAYER_DIRECTSHOW
    #elif defined(TARGET_OSX)
        //for 10.8 and 10.9 users we use AVFoundation, for 10.7 we use QTKit, for 10.6 users we use QuickTime
        #ifndef MAC_OS_X_VERSION_10_7
            #define OF_VIDEO_PLAYER_QUICKTIME
        #elif !defined(MAC_OS_X_VERSION_10_8)
            #define OF_VIDEO_PLAYER_QTKIT
        #else
            #define OF_VIDEO_PLAYER_AVFOUNDATION
        #endif
    #elif defined(TARGET_EMSCRIPTEN)
        #define OF_VIDEO_PLAYER_EMSCRIPTEN
    #else
        #define OF_VIDEO_PLAYER_QUICKTIME
    #endif
#endif

//------------------------------------------------ soundstream
// check if any soundstream api is defined from the compiler
#ifdef QT_MULTIMEDIA_LIB
  // use Qt multimedia framework
  #define OF_SOUNDSTREAM_QT
#elif !defined(OF_SOUNDSTREAM_RTAUDIO) && !defined(OF_SOUNDSTREAM_ANDROID) && !defined(OF_SOUNDSTREAM_IOS) && !defined(OF_SOUNDSTREAM_EMSCRIPTEN)
	#if defined(TARGET_LINUX) || defined(TARGET_WIN32) || defined(TARGET_OSX)
		#define OF_SOUNDSTREAM_RTAUDIO
	#elif defined(TARGET_ANDROID)
		#define OF_SOUNDSTREAM_ANDROID
	#elif defined(TARGET_OF_IOS)
		#define OF_SOUNDSTREAM_IOS
	#elif defined(TARGET_EMSCRIPTEN)
		#define OF_SOUNDSTREAM_EMSCRIPTEN
	#endif
#endif

//------------------------------------------------ soundplayer
// check if any soundplayer api is defined from the compiler
#ifdef QT_MULTIMEDIA_LIB
  // use Qt multimedia framework
  #define OF_SOUND_PLAYER_QT
#elif !defined(OF_SOUND_PLAYER_QUICKTIME) && !defined(OF_SOUND_PLAYER_FMOD) && !defined(OF_SOUND_PLAYER_OPENAL) && !defined(OF_SOUND_PLAYER_EMSCRIPTEN)
  #ifdef TARGET_OF_IOS
  	#define OF_SOUND_PLAYER_IPHONE
  #elif defined(TARGET_LINUX)
  	#define OF_SOUND_PLAYER_OPENAL
  #elif defined(TARGET_EMSCRIPTEN)
	#define OF_SOUND_PLAYER_EMSCRIPTEN
  #elif !defined(TARGET_ANDROID) && (!defined(USE_FMOD) || USE_FMOD)
  	#define OF_SOUND_PLAYER_FMOD
  #endif
#endif

//------------------------------------------------ c++11
// check if the compiler supports c++11. vs hasn't updated the value
// of __cplusplus so we need to check for vs >= 2012 (1700)
#if __cplusplus>=201103 || _MSC_VER >= 1700
#define HAS_CPP11 1
#endif

//------------------------------------------------ thread local storage
// clang has a bug where it won't support tls on some versions even
// on c++11, this is a workaround that bug
#ifndef HAS_TLS
	#if defined(__clang__) && __clang__
		#if __has_feature(cxx_thread_local) && !defined(__MINGW64__) && !defined(__MINGW32__) && !defined(__ANDROID__) && !defined(TARGET_OF_IOS)
			#define HAS_TLS 1
		#endif
    #elif !defined(TARGET_WIN32) || _MSC_VER
		#define HAS_TLS 1
	#endif
#endif

//------------------------------------------------ make_unique
// This is a helper method for make unique on platforms that support C++11, but not C++14.
#if !defined(NO_OF_MAKE_UNIQUE) && (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && __cplusplus <= 201103L)

// Implementation for C++11 platforms that do not yet have std::make_unique.
// Implementation from http://stackoverflow.com/a/13512344/1518329
namespace std {


template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
	static_assert(std::extent<T>::value == 0,
				  "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

	typedef typename std::remove_extent<T>::type U;
	return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}


} // namespace std

#endif

//------------------------------------------------ forward declaration for std::filesystem::path
// Remove from here once everything is using std::filesystem::path
#ifndef TARGET_QT
# if OF_USING_STD_FS
#	if __cplusplus < 201703L

		namespace std {
			namespace experimental{
				namespace filesystem {
					namespace v1 {
						namespace __cxx11 {
							class path;
						}
					}

					using v1::__cxx11::path;
				}
			}
			namespace filesystem = experimental::filesystem;
		}
#	else

	namespace std {
		namespace filesystem {
			class path;
		}
	}
#	endif
# else
#	if !_MSC_VER
#		define BOOST_NO_CXX11_SCOPED_ENUMS
#		define BOOST_NO_SCOPED_ENUMS
#	endif
	namespace boost {
		namespace filesystem {
			class path;
		}
	}
	namespace std {
		namespace filesystem = boost::filesystem;
	}
# endif
#endif // TARGET_QT

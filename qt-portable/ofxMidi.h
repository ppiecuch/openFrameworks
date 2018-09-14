/* Thu Sep 13 21:26:22 CEST 2018 */
#pragma once
#define TARGET_QT
#include "ofMain.h"

#line 0 "../addons-all/ofxMidi/libs/rtmidi/RtMidi.h"
/**********************************************************************/
/*! \class RtMidi
    \brief An abstract base class for realtime MIDI input/output.

    This class implements some common functionality for the realtime
    MIDI input/output subclasses RtMidiIn and RtMidiOut.

    RtMidi WWW site: http://music.mcgill.ca/~gary/rtmidi/

    RtMidi: realtime MIDI i/o C++ classes
    Copyright (c) 2003-2017 Gary P. Scavone

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**********************************************************************/

/*!
  \file RtMidi.h
 */

#ifndef RTMIDI_H
#define RTMIDI_H

#define RTMIDI_VERSION "3.0.0"

#include <exception>
#include <iostream>
#include <string>
#include <vector>
/* #include "ofxMidiConstants.h" */

/************************************************************************/
/*! \class RtMidiError
    \brief Exception handling class for RtMidi.

    The RtMidiError class is quite simple but it does allow errors to be
    "caught" by RtMidiError::Type. See the RtMidi documentation to know
    which methods can throw an RtMidiError.
*/
/************************************************************************/

class RtMidiError : public std::exception
{
 public:
  //! Defined RtMidiError types.
  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    MEMORY_ERROR,      /*!< An error occured during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    INVALID_USE,       /*!< The function was called incorrectly. */
    DRIVER_ERROR,      /*!< A system driver error occured. */
    SYSTEM_ERROR,      /*!< A system error occured. */
    THREAD_ERROR       /*!< A thread error occured. */
  };

  //! The constructor.
  RtMidiError( const std::string& message, Type type = RtMidiError::UNSPECIFIED ) throw() : message_(message), type_(type) {}
 
  //! The destructor.
  virtual ~RtMidiError( void ) throw() {}

  //! Prints thrown error message to stderr.
  virtual void printMessage( void ) const throw() { std::cerr << '\n' << message_ << "\n\n"; }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) const throw() { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) const throw() { return message_; }

  //! Returns the thrown error message as a c-style string.
  virtual const char* what( void ) const throw() { return message_.c_str(); }

 protected:
  std::string message_;
  Type type_;
};

//! RtMidi error callback function prototype.
/*!
    \param type Type of error.
    \param errorText Error description.

    Note that class behaviour is undefined after a critical error (not
    a warning) is reported.
 */
typedef void (*RtMidiErrorCallback)( RtMidiError::Type type, const std::string &errorText, void *userData );

class MidiApi;

class RtMidi
{
 public:

  //! MIDI API specifier arguments.
  enum Api {
    UNSPECIFIED,    /*!< Search for a working compiled API. */
    MACOSX_CORE,    /*!< Macintosh OS-X Core Midi API. */
    LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
    UNIX_JACK,      /*!< The JACK Low-Latency MIDI Server API. */
    WINDOWS_MM,     /*!< The Microsoft Multimedia MIDI API. */
    RTMIDI_DUMMY    /*!< A compilable but non-functional API. */
  };

  //! A static function to determine the current RtMidi version.
  static std::string getVersion( void ) throw();

  //! A static function to determine the available compiled MIDI APIs.
  /*!
    The values returned in the std::vector can be compared against
    the enumerated list values.  Note that there can be more than one
    API compiled for certain operating systems.
  */
  static void getCompiledApi( std::vector<RtMidi::Api> &apis ) throw();

  //! Pure virtual openPort() function.
  virtual void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi" ) ) = 0;

  //! Pure virtual openVirtualPort() function.
  virtual void openVirtualPort( const std::string &portName = std::string( "RtMidi" ) ) = 0;

  //! Pure virtual getPortCount() function.
  virtual unsigned int getPortCount() = 0;

  //! Pure virtual getPortName() function.
  virtual std::string getPortName( unsigned int portNumber = 0 ) = 0;

  //! Pure virtual closePort() function.
  virtual void closePort( void ) = 0;

  //! Returns true if a port is open and false if not.
  /*!
      Note that this only applies to connections made with the openPort()
      function, not to virtual ports.
  */
  virtual bool isPortOpen( void ) const = 0;

  //! Set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 ) = 0;

 protected:

  RtMidi();
  virtual ~RtMidi();

  MidiApi *rtapi_;
};

/**********************************************************************/
/*! \class RtMidiIn
    \brief A realtime MIDI input class.

    This class provides a common, platform-independent API for
    realtime MIDI input.  It allows access to a single MIDI input
    port.  Incoming MIDI messages are either saved to a queue for
    retrieval using the getMessage() function or immediately passed to
    a user-specified callback function.  Create multiple instances of
    this class to connect to more than one MIDI device at the same
    time.  With the OS-X, Linux ALSA, and JACK MIDI APIs, it is also
    possible to open a virtual input port to which other MIDI software
    clients can connect.

    by Gary P. Scavone, 2003-2017.
*/
/**********************************************************************/

// **************************************************************** //
//
// RtMidiIn and RtMidiOut class declarations.
//
// RtMidiIn / RtMidiOut are "controllers" used to select an available
// MIDI input or output interface.  They present common APIs for the
// user to call but all functionality is implemented by the classes
// MidiInApi, MidiOutApi and their subclasses.  RtMidiIn and RtMidiOut
// each create an instance of a MidiInApi or MidiOutApi subclass based
// on the user's API choice.  If no choice is made, they attempt to
// make a "logical" API selection.
//
// **************************************************************** //

class RtMidiIn : public RtMidi
{
 public:

  //! User callback function type definition.
  typedef void (*RtMidiCallback)( double timeStamp, std::vector<unsigned char> *message, void *userData);

  //! Default constructor that allows an optional api, client name and queue size.
  /*!
    An exception will be thrown if a MIDI system initialization
    error occurs.  The queue size defines the maximum number of
    messages that can be held in the MIDI queue (when not using a
    callback function).  If the queue size limit is reached,
    incoming messages will be ignored.

    If no API argument is specified and multiple API support has been
    compiled, the default order of use is ALSA, JACK (Linux) and CORE,
    JACK (OS-X).

    \param api        An optional API id can be specified.
    \param clientName An optional client name can be specified. This
                      will be used to group the ports that are created
                      by the application.
    \param queueSizeLimit An optional size of the MIDI input queue can be specified.
  */
  RtMidiIn( RtMidi::Api api=UNSPECIFIED,
            const std::string& clientName = "RtMidi Input Client",
            unsigned int queueSizeLimit = 100 );

  //! If a MIDI connection is still open, it will be closed by the destructor.
  ~RtMidiIn ( void ) throw();

  //! Returns the MIDI API specifier for the current instance of RtMidiIn.
  RtMidi::Api getCurrentApi( void ) throw();

  //! Open a MIDI input connection given by enumeration number.
  /*!
    \param portNumber An optional port number greater than 0 can be specified.
                      Otherwise, the default or first port found is opened.
    \param portName An optional name for the application port that is used to connect to portId can be specified.
  */
  void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi Input" ) );

  //! Create a virtual input port, with optional name, to allow software connections (OS X, JACK and ALSA only).
  /*!
    This function creates a virtual MIDI input port to which other
    software applications can connect.  This type of functionality
    is currently only supported by the Macintosh OS-X, any JACK,
    and Linux ALSA APIs (the function returns an error for the other APIs).

    \param portName An optional name for the application port that is
                    used to connect to portId can be specified.
  */
  void openVirtualPort( const std::string &portName = std::string( "RtMidi Input" ) );

  //! Set a callback function to be invoked for incoming MIDI messages.
  /*!
    The callback function will be called whenever an incoming MIDI
    message is received.  While not absolutely necessary, it is best
    to set the callback function before opening a MIDI port to avoid
    leaving some messages in the queue.

    \param callback A callback function must be given.
    \param userData Optionally, a pointer to additional data can be
                    passed to the callback function whenever it is called.
  */
  void setCallback( RtMidiCallback callback, void *userData = 0 );

  //! Cancel use of the current callback function (if one exists).
  /*!
    Subsequent incoming MIDI messages will be written to the queue
    and can be retrieved with the \e getMessage function.
  */
  void cancelCallback();

  //! Close an open MIDI connection (if one exists).
  void closePort( void );

  //! Returns true if a port is open and false if not.
  /*!
      Note that this only applies to connections made with the openPort()
      function, not to virtual ports.
  */
  virtual bool isPortOpen() const;

  //! Return the number of available MIDI input ports.
  /*!
    \return This function returns the number of MIDI ports of the selected API.
  */
  unsigned int getPortCount();

  //! Return a string identifier for the specified MIDI input port number.
  /*!
    \return The name of the port with the given Id is returned.
    \retval An empty string is returned if an invalid port specifier
            is provided. User code should assume a UTF-8 encoding.
  */
  std::string getPortName( unsigned int portNumber = 0 );

  //! Specify whether certain MIDI message types should be queued or ignored during input.
  /*!
    By default, MIDI timing and active sensing messages are ignored
    during message input because of their relative high data rates.
    MIDI sysex messages are ignored by default as well.  Variable
    values of "true" imply that the respective message type will be
    ignored.
  */
  void ignoreTypes( bool midiSysex = true, bool midiTime = true, bool midiSense = true );

  //! Fill the user-provided vector with the data bytes for the next available MIDI message in the input queue and return the event delta-time in seconds.
  /*!
    This function returns immediately whether a new message is
    available or not.  A valid message is indicated by a non-zero
    vector size.  An exception is thrown if an error occurs during
    message retrieval or an input connection was not previously
    established.
  */
  double getMessage( std::vector<unsigned char> *message );

  //! Set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 );

 protected:
  void openMidiApi( RtMidi::Api api, const std::string &clientName, unsigned int queueSizeLimit );

};

/**********************************************************************/
/*! \class RtMidiOut
    \brief A realtime MIDI output class.

    This class provides a common, platform-independent API for MIDI
    output.  It allows one to probe available MIDI output ports, to
    connect to one such port, and to send MIDI bytes immediately over
    the connection.  Create multiple instances of this class to
    connect to more than one MIDI device at the same time.  With the
    OS-X, Linux ALSA and JACK MIDI APIs, it is also possible to open a
    virtual port to which other MIDI software clients can connect.

    by Gary P. Scavone, 2003-2017.
*/
/**********************************************************************/

class RtMidiOut : public RtMidi
{
 public:

  //! Default constructor that allows an optional client name.
  /*!
    An exception will be thrown if a MIDI system initialization error occurs.

    If no API argument is specified and multiple API support has been
    compiled, the default order of use is ALSA, JACK (Linux) and CORE,
    JACK (OS-X).
  */
  RtMidiOut( RtMidi::Api api=UNSPECIFIED,
             const std::string& clientName = "RtMidi Output Client" );

  //! The destructor closes any open MIDI connections.
  ~RtMidiOut( void ) throw();

  //! Returns the MIDI API specifier for the current instance of RtMidiOut.
  RtMidi::Api getCurrentApi( void ) throw();

  //! Open a MIDI output connection.
  /*!
      An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.  An
      exception is thrown if an error occurs while attempting to make
      the port connection.
  */
  void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi Output" ) );

  //! Close an open MIDI connection (if one exists).
  void closePort( void );

  //! Returns true if a port is open and false if not.
  /*!
      Note that this only applies to connections made with the openPort()
      function, not to virtual ports.
  */
  virtual bool isPortOpen() const;

  //! Create a virtual output port, with optional name, to allow software connections (OS X, JACK and ALSA only).
  /*!
      This function creates a virtual MIDI output port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X, Linux ALSA
      and JACK APIs (the function does nothing with the other APIs).
      An exception is thrown if an error occurs while attempting to
      create the virtual port.
  */
  void openVirtualPort( const std::string &portName = std::string( "RtMidi Output" ) );

  //! Return the number of available MIDI output ports.
  unsigned int getPortCount( void );

  //! Return a string identifier for the specified MIDI port type and number.
  /*!
    \return The name of the port with the given Id is returned.
    \retval An empty string is returned if an invalid port specifier
            is provided. User code should assume a UTF-8 encoding.
  */
  std::string getPortName( unsigned int portNumber = 0 );

  //! Immediately send a single message out an open MIDI output port.
  /*!
      An exception is thrown if an error occurs during output or an
      output connection was not previously established.
  */
  void sendMessage( const std::vector<unsigned char> *message );

  //! Immediately send a single message out an open MIDI output port.
  /*!
      An exception is thrown if an error occurs during output or an
      output connection was not previously established.

      \param message A pointer to the MIDI message as raw bytes
      \param size    Length of the MIDI message in bytes
  */
  void sendMessage( const unsigned char *message, size_t size );

  //! Set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 );

 protected:
  void openMidiApi( RtMidi::Api api, const std::string &clientName );
};


// **************************************************************** //
//
// MidiInApi / MidiOutApi class declarations.
//
// Subclasses of MidiInApi and MidiOutApi contain all API- and
// OS-specific code necessary to fully implement the RtMidi API.
//
// Note that MidiInApi and MidiOutApi are abstract base classes and
// cannot be explicitly instantiated.  RtMidiIn and RtMidiOut will
// create instances of a MidiInApi or MidiOutApi subclass.
//
// **************************************************************** //

class MidiApi
{
 public:

  MidiApi();
  virtual ~MidiApi();
  virtual RtMidi::Api getCurrentApi( void ) = 0;
  virtual void openPort( unsigned int portNumber, const std::string &portName ) = 0;
  virtual void openVirtualPort( const std::string &portName ) = 0;
  virtual void closePort( void ) = 0;

  virtual unsigned int getPortCount( void ) = 0;
  virtual std::string getPortName( unsigned int portNumber ) = 0;

  inline bool isPortOpen() const { return connected_; }
  void setErrorCallback( RtMidiErrorCallback errorCallback, void *userData );

  //! A basic error reporting function for RtMidi classes.
  void error( RtMidiError::Type type, std::string errorString );

protected:
  virtual void initialize( const std::string& clientName ) = 0;

  void *apiData_;
  bool connected_;
  std::string errorString_;
  RtMidiErrorCallback errorCallback_;
  bool firstErrorOccurred_;
  void *errorCallbackUserData_;
};

class MidiInApi : public MidiApi
{
 public:

  MidiInApi( unsigned int queueSizeLimit );
  virtual ~MidiInApi( void );
  void setCallback( RtMidiIn::RtMidiCallback callback, void *userData );
  void cancelCallback( void );
  virtual void ignoreTypes( bool midiSysex, bool midiTime, bool midiSense );
  double getMessage( std::vector<unsigned char> *message );

  // A MIDI structure used internally by the class to store incoming
  // messages.  Each message represents one and only one MIDI message.
  struct MidiMessage { 
    std::vector<unsigned char> bytes; 

    //! Time in seconds elapsed since the previous message
    double timeStamp;

    // Default constructor.
  MidiMessage()
  :bytes(0), timeStamp(0.0) {}
  };

  struct MidiQueue {
    unsigned int front;
    unsigned int back;
    unsigned int ringSize;
    MidiMessage *ring;

    // Default constructor.
  MidiQueue()
  :front(0), back(0), ringSize(0), ring(0) {}
    bool push(const MidiMessage&);
    bool pop(std::vector<unsigned char>*, double*);
    unsigned int size(unsigned int *back=0,
		      unsigned int *front=0);
  };

  // The RtMidiInData structure is used to pass private class data to
  // the MIDI input handling function or thread.
  struct RtMidiInData {
    MidiQueue queue;
    MidiMessage message;
    unsigned char ignoreFlags;
    bool doInput;
    bool firstMessage;
    void *apiData;
    bool usingCallback;
    RtMidiIn::RtMidiCallback userCallback;
    void *userData;
    bool continueSysex;

    // Default constructor.
  RtMidiInData()
  : ignoreFlags(7), doInput(false), firstMessage(true),
      apiData(0), usingCallback(false), userCallback(0), userData(0),
      continueSysex(false) {}
  };

 protected:
  RtMidiInData inputData_;
};

class MidiOutApi : public MidiApi
{
 public:

  MidiOutApi( void );
  virtual ~MidiOutApi( void );
  virtual void sendMessage( const unsigned char *message, size_t size ) = 0;
};

// **************************************************************** //
//
// Inline RtMidiIn and RtMidiOut definitions.
//
// **************************************************************** //

inline RtMidi::Api RtMidiIn :: getCurrentApi( void ) throw() { return rtapi_->getCurrentApi(); }
inline void RtMidiIn :: openPort( unsigned int portNumber, const std::string &portName ) { rtapi_->openPort( portNumber, portName ); }
inline void RtMidiIn :: openVirtualPort( const std::string &portName ) { rtapi_->openVirtualPort( portName ); }
inline void RtMidiIn :: closePort( void ) { rtapi_->closePort(); }
inline bool RtMidiIn :: isPortOpen() const { return rtapi_->isPortOpen(); }
inline void RtMidiIn :: setCallback( RtMidiCallback callback, void *userData ) { ((MidiInApi *)rtapi_)->setCallback( callback, userData ); }
inline void RtMidiIn :: cancelCallback( void ) { ((MidiInApi *)rtapi_)->cancelCallback(); }
inline unsigned int RtMidiIn :: getPortCount( void ) { return rtapi_->getPortCount(); }
inline std::string RtMidiIn :: getPortName( unsigned int portNumber ) { return rtapi_->getPortName( portNumber ); }
inline void RtMidiIn :: ignoreTypes( bool midiSysex, bool midiTime, bool midiSense ) { ((MidiInApi *)rtapi_)->ignoreTypes( midiSysex, midiTime, midiSense ); }
inline double RtMidiIn :: getMessage( std::vector<unsigned char> *message ) { return ((MidiInApi *)rtapi_)->getMessage( message ); }
inline void RtMidiIn :: setErrorCallback( RtMidiErrorCallback errorCallback, void *userData ) { rtapi_->setErrorCallback(errorCallback, userData); }

inline RtMidi::Api RtMidiOut :: getCurrentApi( void ) throw() { return rtapi_->getCurrentApi(); }
inline void RtMidiOut :: openPort( unsigned int portNumber, const std::string &portName ) { rtapi_->openPort( portNumber, portName ); }
inline void RtMidiOut :: openVirtualPort( const std::string &portName ) { rtapi_->openVirtualPort( portName ); }
inline void RtMidiOut :: closePort( void ) { rtapi_->closePort(); }
inline bool RtMidiOut :: isPortOpen() const { return rtapi_->isPortOpen(); }
inline unsigned int RtMidiOut :: getPortCount( void ) { return rtapi_->getPortCount(); }
inline std::string RtMidiOut :: getPortName( unsigned int portNumber ) { return rtapi_->getPortName( portNumber ); }
inline void RtMidiOut :: sendMessage( const std::vector<unsigned char> *message ) { ((MidiOutApi *)rtapi_)->sendMessage( &message->at(0), message->size() ); }
inline void RtMidiOut :: sendMessage( const unsigned char *message, size_t size ) { ((MidiOutApi *)rtapi_)->sendMessage( message, size ); }
inline void RtMidiOut :: setErrorCallback( RtMidiErrorCallback errorCallback, void *userData ) { rtapi_->setErrorCallback(errorCallback, userData); }

// **************************************************************** //
//
// MidiInApi and MidiOutApi subclass prototypes.
//
// **************************************************************** //

#if !defined(__LINUX_ALSA__) && !defined(__UNIX_JACK__) && !defined(__MACOSX_CORE__) && !defined(__WINDOWS_MM__)
  #define __RTMIDI_DUMMY__
#endif

#if defined(__MACOSX_CORE__)

class MidiInCore: public MidiInApi
{
 public:
  MidiInCore( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInCore( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::MACOSX_CORE; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  void initialize( const std::string& clientName );
};

class MidiOutCore: public MidiOutApi
{
 public:
  MidiOutCore( const std::string &clientName );
  ~MidiOutCore( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::MACOSX_CORE; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__UNIX_JACK__)

class MidiInJack: public MidiInApi
{
 public:
  MidiInJack( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInJack( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::UNIX_JACK; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  std::string clientName;

  void connect( void );
  void initialize( const std::string& clientName );
};

class MidiOutJack: public MidiOutApi
{
 public:
  MidiOutJack( const std::string &clientName );
  ~MidiOutJack( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::UNIX_JACK; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  std::string clientName;

  void connect( void );
  void initialize( const std::string& clientName );
};

#endif

#if defined(__LINUX_ALSA__)

class MidiInAlsa: public MidiInApi
{
 public:
  MidiInAlsa( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInAlsa( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::LINUX_ALSA; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  void initialize( const std::string& clientName );
};

class MidiOutAlsa: public MidiOutApi
{
 public:
  MidiOutAlsa( const std::string &clientName );
  ~MidiOutAlsa( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::LINUX_ALSA; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__WINDOWS_MM__)

class MidiInWinMM: public MidiInApi
{
 public:
  MidiInWinMM( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInWinMM( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::WINDOWS_MM; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  void initialize( const std::string& clientName );
};

class MidiOutWinMM: public MidiOutApi
{
 public:
  MidiOutWinMM( const std::string &clientName );
  ~MidiOutWinMM( void );
  RtMidi::Api getCurrentApi( void ) { return RtMidi::WINDOWS_MM; };
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__RTMIDI_DUMMY__)

class MidiInDummy: public MidiInApi
{
 public:
 MidiInDummy( const std::string &/*clientName*/, unsigned int queueSizeLimit ) : MidiInApi( queueSizeLimit ) { errorString_ = "MidiInDummy: This class provides no functionality."; error( RtMidiError::WARNING, errorString_ ); }
  RtMidi::Api getCurrentApi( void ) { return RtMidi::RTMIDI_DUMMY; }
  void openPort( unsigned int /*portNumber*/, const std::string &/*portName*/ ) {}
  void openVirtualPort( const std::string &/*portName*/ ) {}
  void closePort( void ) {}
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int /*portNumber*/ ) { return ""; }

 protected:
  void initialize( const std::string& /*clientName*/ ) {}
};

class MidiOutDummy: public MidiOutApi
{
 public:
  MidiOutDummy( const std::string &/*clientName*/ ) { errorString_ = "MidiOutDummy: This class provides no functionality."; error( RtMidiError::WARNING, errorString_ ); }
  RtMidi::Api getCurrentApi( void ) { return RtMidi::RTMIDI_DUMMY; }
  void openPort( unsigned int /*portNumber*/, const std::string &/*portName*/ ) {}
  void openVirtualPort( const std::string &/*portName*/ ) {}
  void closePort( void ) {}
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int /*portNumber*/ ) { return ""; }
  void sendMessage( const unsigned char * /*message*/, size_t /*size*/ ) {}

 protected:
  void initialize( const std::string& /*clientName*/ ) {}
};

#endif

#endif

#line 0 "../addons-all/ofxMidi/src/ofxMidiConstants.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 * references:
 *	http://www.srm.com/qtma/davidsmidispec.html
 */
#pragma once

/* #include "ofConstants.h" */

// for RtMidi
#if defined(TARGET_LINUX)
    #ifndef __LINUX_ALSA__
        #define __LINUX_ALSA__
    #endif
#elif defined(TARGET_WIN32)
    #ifndef __WINDOWS_MM__
        #define __WINDOWS_MM__
    #endif
#elif defined(TARGET_MACOSX)
    #ifndef __MACOSX_CORE__
        #define __MACOSX_CORE__
    #endif
#endif

// api types, most of these match RtMidi::Api enums
enum ofxMidiApi {
	MIDI_API_DEFAULT,      // choose platform default
	MIDI_API_COREMIDI,     // CoreMidi macOS or iOS
	MIDI_API_ALSA,         // ALSA Linux
	MIDI_API_JACK,         // JACK
	MIDI_API_WINDOWS_MM    // Windows Multimedia MIDI
};

// MIDI status bytes
enum MidiStatus {

    MIDI_UNKNOWN            = 0x00,

    // channel voice messages
    MIDI_NOTE_OFF           = 0x80,
    MIDI_NOTE_ON            = 0x90,
    MIDI_CONTROL_CHANGE     = 0xB0,
    MIDI_PROGRAM_CHANGE     = 0xC0,
    MIDI_PITCH_BEND         = 0xE0,
    MIDI_AFTERTOUCH         = 0xD0, // aka channel pressure
    MIDI_POLY_AFTERTOUCH    = 0xA0, // aka key pressure

    // system messages
    MIDI_SYSEX              = 0xF0,
    MIDI_TIME_CODE          = 0xF1,
    MIDI_SONG_POS_POINTER   = 0xF2,
    MIDI_SONG_SELECT        = 0xF3,
    MIDI_TUNE_REQUEST       = 0xF6,
    MIDI_SYSEX_END          = 0xF7,
    MIDI_TIME_CLOCK         = 0xF8,
    MIDI_START              = 0xFA,
    MIDI_CONTINUE           = 0xFB,
    MIDI_STOP               = 0xFC,
    MIDI_ACTIVE_SENSING     = 0xFE,
    MIDI_SYSTEM_RESET       = 0xFF
};

// number range defines
// because it's sometimes hard to remember these  ...
#define MIDI_MIN_BEND       0
#define MIDI_MAX_BEND       16383

#line 0 "../addons-all/ofxMidi/src/ofxMidiTypes.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/// \section  stream interface MIDI objects
/// ref: http://www.gweep.net/~prefect/eng/reference/protocol/midispec.html

/// send a note on event (also set velocity = 0 for noteoff)
struct NoteOn {

	const int channel;  ///< channel 1 - 16
	const int pitch;    ///< pitch 0 - 127
	const int velocity; ///< velocity 0 - 127
	
	explicit NoteOn(const int channel, const int pitch, const int velocity=64) :
		channel(channel), pitch(pitch), velocity(velocity) {}
};

/// send a note off event (velocity is usually ignored)
struct NoteOff {

	const int channel;  ///< channel 1 - 16
	const int pitch;    ///< pitch 0 - 127
	const int velocity; ///< velocity 0 - 127
	
	explicit NoteOff(const int channel, const int pitch, const int velocity=64) :
		channel(channel), pitch(pitch), velocity(velocity) {}
};

/// change a control value aka send a CC message
struct ControlChange {

	const int channel;  ///< channel 1 - 16
	const int control;  ///< control 0 - 127
	const int value;    ///< value 0 - 127
	
	explicit ControlChange(const int channel, const int control, const int value) :
		channel(channel), control(control), value(value) {}
};

/// change a program value (ie an instrument)
struct ProgramChange {

	const int channel;  ///< channel 1 - 16
	const int value;    ///< value 0 - 127
	
	explicit ProgramChange(const int channel, const int value) :
		channel(channel), value(value) {}
};

/// change the pitch bend value
struct PitchBend {

	const int channel;  ///< channel 1 - 16
	const int value;    ///< value 0 - 16383
	
	explicit PitchBend(const int channel, const int value) :
		channel(channel), value(value) {}
};

/// change an aftertouch value
struct Aftertouch {

	const int channel;  ///< channel 1 - 16
	const int value;    ///< value 0 - 127
	
	explicit Aftertouch(const int channel, const int value) :
		channel(channel), value(value) {}
};

/// change a poly aftertouch value
struct PolyAftertouch {

	const int channel;  ///< channel 1 - 16
	const int pitch;    ///< controller 0 - 127
	const int value;    ///< value 0 - 127
	
	explicit PolyAftertouch(const int channel, const int pitch, const int value) :
		channel(channel), pitch(pitch), value(value) {}
};

/// start a raw MIDI byte stream
struct StartMidi {
	explicit StartMidi() {}
};

/// finish a MIDI byte stream
struct FinishMidi {
	explicit FinishMidi() {}
};

#line 0 "../addons-all/ofxMidi/src/ofxMidiMessage.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/* #include "ofEvents.h" */
/* #include "ofxMidiConstants.h" */

class ofxMidiMessage;

/// receives MIDI messages
class ofxMidiListener {

public:

	ofxMidiListener() {}
	virtual ~ofxMidiListener() {}

	virtual void newMidiMessage(ofxMidiMessage& message) = 0;
};

/// a single multi byte MIDI message
///
/// check status type and grab data:
/// 
///     if(message.status == MIDI_NOTE_ON) {
///         ofLog() << "note on " << message.channel
///                 << message.note << " " << message.velocity;
///     }
///
/// the message-specific types are only set for the appropriate
/// message types ie pitch is only set for noteon, noteoff, and
/// polyaftertouch messages
///
class ofxMidiMessage: public ofEventArgs {

public:

/// \section Variables

	MidiStatus status;
	int channel;        //< 1 - 16

	/// message-specific values,
	/// converted from raw bytes
	int pitch;          //< 0 - 127
	int velocity;       //< 0 - 127
	int control;        //< 0 - 127
	int value;          //< depends on message status type
	
	/// raw bytes
	std::vector<unsigned char> bytes;
	
	/// delta time since last message in ms
	double deltatime;

	/// the input port we received this message from
	///
	/// note: portNum will be -1 from a virtual port
	///
	int portNum;
	std::string portName;

/// \section Main

	ofxMidiMessage();
	ofxMidiMessage(std::vector<unsigned char>* rawBytes); //< parses
	ofxMidiMessage(const ofxMidiMessage& from);
	ofxMidiMessage& operator=(const ofxMidiMessage& from);
	void copy(const ofxMidiMessage& from);
	
	/// parse message from raw MIDI bytes
	void fromBytes(std::vector<unsigned char> *rawBytes);
	
	/// clear the message contents, also resets status
	void clear();
	
/// \section Util
	
	/// get the raw message as a string in the format:
	///
	/// PortName: status channel [ raw bytes in hex ] deltatime
	///
	std::string toString();

	/// get a MIDI status byte as a string
	/// ie "Note On", "Note Off", "Control Change", etc
	static std::string getStatusString(MidiStatus status);
};

typedef ofEvent<ofxMidiMessage> ofxMidiEvent;

#line 0 "../addons-all/ofxMidi/src/ofxBaseMidi.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/* #include "ofxMidiConstants.h" */
/* #include "ofxMidiMessage.h" */
/* #include "ofxMidiTypes.h" */

/// a base MIDI input port
///
/// see ofxMidiIn for functional documentation
///
class ofxBaseMidiIn {

public:

	ofxBaseMidiIn(const std::string name, ofxMidiApi api);
	virtual ~ofxBaseMidiIn() {}
	
	virtual bool openPort(unsigned int portNumber) = 0;
	virtual bool openPort(std::string deviceName) = 0;
	virtual bool openVirtualPort(std::string portName) = 0;
	virtual void closePort() = 0;

	virtual void listInPorts() = 0;
	virtual std::vector<std::string> getInPortList() = 0;
	virtual int getNumInPorts() = 0;
	virtual std::string getInPortName(unsigned int portNumber) = 0;

	int getPort();
	std::string getName();
	bool isOpen();
	bool isVirtual();
	ofxMidiApi getApi();

	virtual void ignoreTypes(bool midiSysex=true, bool midiTiming=true,
	                         bool midiSense=true) = 0;

	void addListener(ofxMidiListener *listener);
	void removeListener(ofxMidiListener *listener);

	void setVerbose(bool verbose);

protected:

	/// parses and sends received raw messages to listeners
	void manageNewMessage(double deltatime, std::vector<unsigned char> *message);
	
	int portNum;     //< current port num, -1 if not connected
	std::string portName; //< current port name, "" if not connected

	ofEvent<ofxMidiMessage> newMessageEvent; //< current message event
	
	bool bOpen;     //< is the port currently open?
	bool bVerbose;  //< print incoming bytes?
	bool bVirtual;  //< are we connected to a virtual port?
	ofxMidiApi api; //< backend api
};

/// a MIDI output port
///
/// see ofxMidiOut for functional documentation
///
class ofxBaseMidiOut {

public:

	ofxBaseMidiOut(const std::string name, ofxMidiApi api);
	virtual ~ofxBaseMidiOut() {}
	
	virtual bool openPort(unsigned int portNumber=0) = 0;
	virtual bool openPort(std::string deviceName) = 0;
	virtual bool openVirtualPort(std::string portName) = 0;
	virtual void closePort() = 0;

	virtual void listOutPorts() = 0;
	virtual std::vector<std::string> getOutPortList() = 0;
	virtual int getNumOutPorts() = 0;
	virtual std::string getOutPortName(unsigned int portNumber) = 0;
	
	int getPort();
	std::string getName();
	bool isOpen();
	bool isVirtual();
	ofxMidiApi getApi();
	
	void sendNoteOn(int channel, int pitch, int velocity);
	void sendNoteOff(int channel, int pitch, int velocity);
	void sendControlChange(int channel, int control, int value);
	void sendProgramChange(int channel, int value);
	void sendPitchBend(int channel, int value);
	void sendPitchBend(int channel, unsigned char lsb, unsigned char msb);
	void sendAftertouch(int channel, int value);
	void sendPolyAftertouch(int channel, int pitch, int value);
	
	void sendMidiByte(unsigned char byte);
	void sendMidiBytes(std::vector<unsigned char>& bytes);
	
	void startMidiStream();
	void finishMidiStream();
	
protected:

	/// send a raw byte message
	virtual void sendMessage(std::vector<unsigned char> &message) = 0;
	
	int portNum;          //< current port num, -1 if not connected
	std::string portName; //< current port name, "" if not connected

	std::vector<unsigned char> stream; //< byte stream message byte buffer
	
	bool bOpen;             //< is the port currently open?
	bool bStreamInProgress; //< used with byte stream
	bool bVirtual;          //< are we connected to a virtual port?
	ofxMidiApi api;         //< backend api
};

#line 0 "../addons-all/ofxMidi/src/ofxMidiIn.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/* #include "ofxBaseMidi.h" */

// choose the MIDI backend
#ifdef TARGET_OF_IPHONE
	#include "ios/ofxPGMidiIn.h"
	#define OFX_MIDI_IN_TYPE ofxPGMidiIn
#else // OSX, Win, Linux
	// inline desktop/ofxRtMidiIn.h
	#define OFX_MIDI_IN_TYPE ofxRtMidiIn
#endif

///
/// a MIDI input port
///
/// create multiple instances to connect to multiple ports
///
/// *do not* create static instances as this will lead to a crash on Linux,
/// instead create a static std::shared_pttr and initialize it later:
///
/// in .h:
///    class MyClass {
///
///        ...
///
///        static std::shared_ptr<ofxMidiIn> s_midiIn;
///    }
///
/// in .cpp:
///    std::shared_ptr<ofxMidiIn> MyClass::s_midiIn;
///
///    ...
///
///    // initialize somewhere else
///    void MyClass::setup() {
///        if(s_midiIn == NULL) {
///            s_midiIn = std::shared_ptr<ofxMidiIn>(new ofxMidiIn("ofxMidi Client"));
///        }
///    }
///
class ofxMidiIn {

public:

	/// set the input client name (optional) and api (optional)
	ofxMidiIn(const std::string name="ofxMidiIn Client", ofxMidiApi api=MIDI_API_DEFAULT);
	virtual ~ofxMidiIn();
	
/// \section Global Port Info
	
	/// print the connected input ports
	void listInPorts();
	
	/// get a list of input port names
	/// 
	/// the vector index corresponds with the name's port number
	///
	/// note: this order may change when new devices are added/removed
	///       from the system
	///
	std::vector<std::string> getInPortList();
	
	/// get the number of input ports
	int getNumInPorts();
	
	/// get the name of an input port by it's number
	///
	/// returns "" if number is invalid
	///
	std::string getInPortName(unsigned int portNumber);

/// \section Connection
	
	/// connect to an input port
	///
	/// setting port = 0 will open the first available
	///
	bool openPort(unsigned int portNumber=0);
	bool openPort(std::string deviceName);
	
	/// create and connect to a virtual output port (macOS and Linux ALSA only)
	///
	/// allows for connections between software
	///
	/// note: a connected virtual port has a portNum = -1
	/// note: an open virtual port ofxMidiIn object cannot see it's virtual
	///       own virtual port when listing ports
	///
	bool openVirtualPort(std::string portName="ofxMidi Virtual Input");
	
	/// close the port connection
	void closePort();
	
	/// get the port number if connected
	///
	/// returns -1 if not connected or this is a virtual port
	///
	int getPort();
	
	/// get the connected input port name
	///
	/// returns "" if not connected
	///
	std::string getName();
	
	/// returns true if connected
	bool isOpen();
	
	/// returns true if this is a virtual port
	bool isVirtual();
	
/// \section Receiving

	/// specify if certain message types should be ignored
	///
	/// sysex messages are ignored by default
	///
	/// timing and active sensing messages have high data rates
	/// and are ignored by default 
	///
	void ignoreTypes(bool midiSysex=true, bool midiTiming=true, bool midiSense=true);

	/// add/remove listener for incoming MIDI events
	///
	/// listeners receive from *all* incoming MIDI channels
	///
	void addListener(ofxMidiListener* listener);
	void removeListener(ofxMidiListener* listener);

	/// set to verbose = true to print received byte info
	///
	/// warning: this will impact performance with large numbers
	///          of MIDI messages
	///
	void setVerbose(bool verbose);

private:

	std::shared_ptr<ofxBaseMidiIn> midiIn;
};

#line 0 "../addons-all/ofxMidi/src/ofxMidiOut.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/* #include "ofxBaseMidi.h" */

// choose the MIDI backend
#ifdef TARGET_OF_IPHONE
	// inline ios/ofxPGMidiOut.h
	#define OFX_MIDI_OUT_TYPE ofxPGMidiOut
#else // OSX, Win, Linux
	// inline desktop/ofxRtMidiOut.h
	#define OFX_MIDI_OUT_TYPE ofxRtMidiOut
#endif

///
/// a MIDI output port
///
/// create multiple instances to connect to multiple ports
///
/// *do not* create static instances as this will lead to a crash on Linux,
/// instead create a static std::shared_ptr and initialize it later:
///
/// in .h:
///    class MyClass {
///
///        ...
///
///        static std::shared_ptr<ofxMidiOut> s_midiOut;
///    }
///
/// in .cpp:
///    std::shared_ptr<ofxMidiOut> MyClass::s_midiOut;
///
///    ...
///
///    // initialize somewhere else
///    void MyClass::setup() {
///        if(s_midiOut == NULL) {
///            s_midiOut = std::shared_ptr<ofxMidiOut>(new ofxMidiOut("ofxMidi Client"));
///        }
///    }
///
class ofxMidiOut {

public:

	/// set the output client name (optional)
	ofxMidiOut(const std::string name="ofxMidiOut Client", ofxMidiApi api=MIDI_API_DEFAULT);
	virtual ~ofxMidiOut();
	
/// \section Global Port Info
	
	/// print the connected output ports
	void listOutPorts();
	
	/// get a list of output port names
	/// 
	/// the vector index corresponds with the name's port number
	///
	/// note: this order may change when new devices are added/removed
	///       from the system
	///
	std::vector<std::string> getOutPortList();
	
	/// get the number of output ports
	int getNumOutPorts();
	
	/// get the name of an output port by it's number
	///
	/// returns "" if number is invalid
	///
	std::string getOutPortName(unsigned int portNumber);
	
/// \section Connection
	
	/// connect to an output port
	///
	/// setting port = 0 will open the first available
	///
	bool openPort(unsigned int portNumber=0);
	bool openPort(std::string deviceName);
	
	/// create and connect to a virtual output port (macOS and Linux ALSA only)
	///
	/// allows for connections between software
	///
	/// note: a connected virtual port has a portNum = -1
	/// note: an open virtual port ofxMidiOut object cannot see it's virtual
	///       own virtual port when listing ports
	///
	bool openVirtualPort(std::string portName="ofxMidi Virtual Output");
	
	/// close the port connection
	void closePort();
	
	/// get the port number if connected
	///
	/// returns -1 if not connected or this is a virtual port
	///
	int getPort();
	
	/// get the connected output port name
	///
	/// returns "" if not connected
	///
	std::string getName();
	
	/// returns true if connected
	bool isOpen();
	
	/// returns true if this is a virtual port
	bool isVirtual();
	
/// \section Sending
	
	/// MIDI events
	///
	/// number ranges:
	///     channel         1 - 16
	///     pitch           0 - 127
	///     velocity        0 - 127
	///     control value   0 - 127
	///     program value   0 - 127
	///     bend value      0 - 16383
	///     touch value     0 - 127
	///
	/// note:
	///     - a noteon with vel = 0 is equivalent to a noteoff
	///     - send velocity = 64 if not using velocity values
	///     - most synths don't use the velocity value in a noteoff
	///     - the lsb & msb for raw pitch bend bytes are 7 bit
	///
	/// references:
	///     http://www.srm.com/qtma/davidsmidispec.html
	///
	void sendNoteOn(int channel, int pitch, int velocity=64);
	void sendNoteOff(int channel, int pitch, int velocity=64);
	void sendControlChange(int channel, int control, int value);
	void sendProgramChange(int channel, int value);
	void sendPitchBend(int channel, int value);
	void sendPitchBend(int channel, unsigned char lsb, unsigned char msb);
	void sendAftertouch(int channel, int value);
	void sendPolyAftertouch(int channel, int pitch, int value);
	
	/// raw MIDI bytes
	///
	void sendMidiByte(unsigned char byte);
	void sendMidiBytes(std::vector<unsigned char>& bytes);
	
/// \section Sending Stream Interface
	
	/// MIDI events
	///
	/// midiOut << NoteOn(1, 64, 64) << NoteOff(1, 64);
	/// midiOut << ControlChange(1, 100, 64) << ProgramChange(1, 100);
	/// midiOut << << PitchBend(1, 2000);
	/// midiOut << Aftertouch(1, 127) << PolyAftertouch(1, 64, 127);
	///
	ofxMidiOut& operator<<(const NoteOn& var);
	ofxMidiOut& operator<<(const NoteOff& var);
	ofxMidiOut& operator<<(const ControlChange& var);
	ofxMidiOut& operator<<(const ProgramChange& var);
	ofxMidiOut& operator<<(const PitchBend& var);
	ofxMidiOut& operator<<(const Aftertouch& var);
	ofxMidiOut& operator<<(const PolyAftertouch& var);
	
	/// compound raw MIDI byte stream
	///
	/// midiOut << StartMidi() << 0x90 << 0x3C << 0x40 << FinishMidi();
	///
	/// build a raw MIDI byte message and send it with FinishMidi()
	///
	/// note: other MIDI messages (except raw MIDI bytes) cannot be sent while
	///       the stream is in progress
	///
	/// warning: this is not thread safe, use sendMidiBytes() in a shared context
	//
	ofxMidiOut& operator<<(const StartMidi& var);
	ofxMidiOut& operator<<(const FinishMidi& var);
	ofxMidiOut& operator<<(const unsigned char var);
	
private:
	
	std::shared_ptr<ofxBaseMidiOut> midiOut;
};

#line 0 "../addons-all/ofxMidi/src/ofxMidiClock.h"
/*
 * Copyright (c) 2018 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 * Adapted from code written in Swift for ZKM | Karlsruhe
 *
 */
#pragma once

#include <vector>
#include <chrono>
/* #include "ofxMidiConstants.h" */

///
/// MIDI clock message parser
///
/// currently input only, does not send clock ticks
///
class ofxMidiClock {

public:

	ofxMidiClock();
	virtual ~ofxMidiClock() {}

	/// update clock from a raw MIDI message,
	/// returns true if the message was handled
	bool update(std::vector<unsigned char> &message);

	/// manually increment ticks and measure length since last tick
	void tick();

	/// reset timestamp
	void reset();

/// \section Status

	/// get the song position in beats
	unsigned int getBeats();

	/// set the song position in beats
	void setBeats(unsigned int b);

	/// get the song position in seconds
	double getSeconds();

	/// set the song position in seconds
	void setSeconds(double s);

	/// get tempo in beats per minute calculated from clock tick length
	double getBpm();

	/// set clock tick length from tempo in beats per minute
	void setBpm(double bpm);

/// \section Util

	/// get the song position in seconds from a beat position,
    /// 1 beat = 1/16 note = 6 clock ticks
	double beatsToSeconds(unsigned int beats);

	/// get the song position in beats from seconds,
	/// 1 beat = 1/16 note = 6 clock ticks
	unsigned int secondsToBeats(double seconds);

	/// calculate MIDI clock length in ms from a give tempo bpm
	static double bpmToMs(double bpm);

	/// calculate tempo bpm from a give MIDI clock length in ms
	static double msToBpm(double ms);

protected:

	double length = 20.833; //< averaged tick length in ms, default 120 bpm
	unsigned long ticks = 0.0; //< current song pos in ticks (6 ticks = 1 beat)
	std::chrono::steady_clock::time_point timestamp; //< last timestamp
};

#line 0 "../addons-all/ofxMidi/src/ofxMidi.h"
/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

/* #include "ofxMidiIn.h" */
/* #include "ofxMidiOut.h" */
/* #include "ofxMidiClock.h" */
/* #include "ofxMidiTimecode.h" */

/// receives iOS MIDI device (dis)connection events
class ofxMidiConnectionListener {

public:

	ofxMidiConnectionListener() {}
	virtual ~ofxMidiConnectionListener() {}
	
	virtual void midiInputAdded(std::string name, bool isNetwork=false);
	virtual void midiInputRemoved(std::string name, bool isNetwork=false);
	
	virtual void midiOutputAdded(std::string nam, bool isNetwork=false);
	virtual void midiOutputRemoved(std::string name, bool isNetwork=false);
};

// global access
namespace ofxMidi {

/// \section Util

	/// convert MIDI note to frequency in Hz
	/// ala the [mtof] object in Max & Pure Data
	float mtof(float note);

	/// convert a frequency in Hz to a MIDI note
	/// ala the [ftom] object in Max & Pure Data
	float ftom(float frequency);

	/// convert raw MIDI bytes to a printable string, ex. "F0 0C 33"
	std::string bytesToString(std::vector<unsigned char> &bytes);

/// \section iOS Specific
	
	/// set a listener to receieve iOS device (dis)connection events
	///
	/// don't forget to clear before the listener is deallocated!
	///
	/// note: these are noops on Mac, Win, & Linux
	///
	void setConnectionListener(ofxMidiConnectionListener *listener);

	/// clear iOS device event receiver
	void clearConnectionListener();
	
	/// enables the network MIDI session between iOS and macOS on a
	/// local wifi network
	///
	/// in ofxMidi: open the input/outport network ports named "Session 1"
	///
	/// on OSX: use the Audio MIDI Setup Utility to connect to the iOS device
	///
	/// note: this is a noop on Mac, Win, & Linux
	///
	void enableNetworking();
};

#line 0 "../addons-all/ofxMidi/src/ofxMidiTimecode.h"
/*
 * Copyright (c) 2018 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 * Adapted from code written in Swift for the Hertz-Lab @ ZKM | Karlsruhe
 *
 */
#pragma once

#include <vector>
#include <thread>
/* #include "ofEvents.h" */
/* #include "ofxMidiConstants.h" */

/// MTC frame
struct ofxMidiTimecodeFrame {

	int hours = 0;   //< hours 0-23
	int minutes = 0; //< minutes 0-59
	int seconds = 0; //< seconds 0-59
	int frames = 0;  //< frames 0-29 (depending on framerate)
	unsigned char rate = 0x0; //< 0x0: 24, 0x1: 25, 0x2: 29.97, 0x3: 30

	/// get the framerate value in fps
	double getFps() const;

/// \section Util

	/// convert to a string: hh:mm:ss:ff
	std::string toString() const;

	/// convert to time in seconds
	double toSeconds() const;

	/// convert from time in seconds, uses default 24 fps
	void fromSeconds(double s);

	/// convert from time in seconds & framerate value (not fps!)
    void fromSeconds(double s, unsigned char r);
};

///
/// MIDI timecode message parser
///
/// currently input only, does not send timecode
///
class ofxMidiTimecode {

public:

	ofxMidiTimecode() {}
	virtual ~ofxMidiTimecode() {}

	/// update the timecode frame from a raw MIDI message,
	/// returns true if the frame is new
	bool update(std::vector<unsigned char> &message);

	/// reset current frame data
	void reset();

/// \section Status

	/// get the last complete (current) timecode frame
	ofxMidiTimecodeFrame getFrame() {return frame;}

/// \section Util

	/// framerate values
	enum Framerate : unsigned char {
		FRAMERATE_24      = 0x0,
		FRAMERATE_25      = 0x1,
		FRAMERATE_30_DROP = 0x2, // 29.997 drop frame
		FRAMERATE_30      = 0x3
	};

	/// calculate ms from a frame count & framerate value
    static int framesToMs(int frames, unsigned char rate);

	/// calculate frame count from ms & framerate value
    static int msToFrames(int ms, unsigned char rate);

	/// returns the fps for a framerate value, ie. 0x0 -> 24 fps
    static double rateToFps(unsigned char rate);

	/// convert fps to the closest framerate value, ie. 24 fps -> 0x0
    static unsigned char fpsToRate(double fps);

protected:

	/// current frame, ie. last complete Quarter or Full Frame message
	ofxMidiTimecodeFrame frame;

	/// number of bytes in a FF message
	static const int FULLFRAME_LEN = 10;

	/// number of QF messages to make up a full MTC frame
	static const int QUARTERFRAME_LEN = 8;

	/// MTC quarter frame info
    struct QuarterFrame {

        /// detected time direction
        enum Direction {
            BACKWARDS = -1, //< time is moving backwards ie. rewinding
            UNKNOWN   =  0, //< unknown so far
            FORWARDS  =  1  //< time is advancing
        };

        // data
        ofxMidiTimecodeFrame frame;

        // protocol handling
        unsigned int count = 0; //< current received QF message count
        bool receivedFirst = false; //< did we receive the first message? (0x0* frames)
        bool receivedLast = false; //< did we receive the last message? (0x7* hours)
        unsigned int lastDataByte = 0x00; //< last received data byte for direction detection
        Direction direction = UNKNOWN; //< forwards or backwards?
    };

	/// current quarter frame info
	QuarterFrame quarterFrame;

	/// decode a Quarter Frame message, update when we have a full set of 8 messages
    ///
    /// if we receive too many, just overwrite until we have both the first and last
    /// messages (aka 0xF10* & 0xF17*) since a DAW could stop in the middle and start
    /// a new set of Quarter Frame messages based on user input aka start/stop
    ///
    /// also try to detect direction based on last receeved byte,
    /// this should hopefully handle both forwards and backwards playback:
    /// * forwards:  0xF10*, 0xF11*, 0xF12*, 0xF13*, 0xF14*, 0xF15*, 0xF16*, 0xF17*
    /// * backwards: 0xF17*, 0xF16*, 0xF15*, 0xF14*, 0xF13*, 0xF12*, 0xF11*, 0xF10*
    ///
    /// note: Quarter Frame state is cleared when a Full Frame message is received
    ///
    /// returns true if a complete quarter frame was decoded
    bool decodeQuarterFrame(std::vector<unsigned char> &message);

	/// decode a FF message: F0 7F 7F 01 01 hh mm ss ff F7
	/// returns true if a full frame was decoded
    bool decodeFullFrame(std::vector<unsigned char> &message);

	/// check if all bytes are correct
    bool isFullFrame(std::vector<unsigned char> &message);
};

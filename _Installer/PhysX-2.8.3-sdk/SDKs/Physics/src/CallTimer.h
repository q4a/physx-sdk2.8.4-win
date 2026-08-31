#ifndef CALLTIMER_H
#define CALLTIMER_H

#include <Foundation/src/include/Time.h>

#include <stdio.h>

#define BUILD_DLL

// N.B. - This should be in a separate header.
#if defined(WIN32)
#  if defined(BUILD_DLL)
#    define Timer_Export __declspec(dllexport)
#  else // defined(BUILD_DLL)
#    define Timer_Export __declspec(dllimport)
#  endif // defined(BUILD_DLL)
#else // defined(WIN32)
#    define Timer_Export
#endif // defined(WIN32)

#undef Timer_Export
#define Timer_Export


/**
 * Profile SDK API method calls.
 *
 * This class is intended to simplify the mechanics of instrumenting API
 * calls to determine the time spent in each call.  One instance of this
 * class should be instantiated for each method that can be called
 * simultaneously.  Open output files can be shared among a group of
 * objects with the written data including an instance stamp to
 * distinguish the source of any given data element.
 *
 * The output file will have the format:
 *
 * <start of file>
 *   <byte order mark>
 *   <buffer value> ...
 * <end of file>
 *
 * where the <byte order mark> is 4 bytes written in the source byte
 * ordering.  The data is intended to be "reader makes right", which it
 * can do by comparing the integer '1' value of the mark with its read
 * value to determine if a byte swap is required.
 *
 * The buffer consists of:
 *   method   - user defined integer number representing the method.
 *   sequence - unsigned integer number of the current call to the method.
 *   elapsed  - double value in microseconds of the call time.
 *
 * Sharing output files amongst more than one object is illustrated in
 * the following example:
 *
 *   enum { METHOD1 = 1, METHOD2};
 *   CallTimer* timer1 = new CallTimer( METHOD1, "filename");
 *   CallTimer* timer2 = new CallTimer( METHOD2, timer1->file());
 *
 *   ...
 *
 *   timer1->start();
 *   timer2->start();
 *
 *   timer1->stop();
 *   timer1->write();
 *
 *   timer2->stop();
 *   timer2->write();
 *
 *   ...
 *
 *   delete timer2;
 *   delete timer1;
 *
 * Only the object which was constructed with the filename will actually
 * close the file, so it is important that that object persist until all
 * other objects have completed any writes to the output.
 */

class Timer_Export CallTimer {
  public:
    /// Translate seconds type into our namespace.
    typedef NxFoundation::second Seconds;

    /// Default constructor.
    CallTimer( int method = 0);

    /// Construct with a filename.
    CallTimer( int method, char* filename, bool append = false);

    /// Construct with a file descriptor.
    CallTimer( int method, FILE* file);

    /// Virtual destructor.
    ~CallTimer();

    //
    // Member access.
    //

    const char* filename() const;

    FILE* file() const;
    void  file( FILE* file);

    int  method() const;
    void method( int method);

    size_t sequence() const;

    Seconds seconds();

	Seconds elapsed();

    /// Open a new output file.
    bool open( char* filename, bool append = false);

    /// Close the current output file.
    bool close();

    /// Start the elapsed time interval being measured.
    void start();

    /// End the elapsed time interval being measured.
    Seconds stop();

	/// Write the current buffer contents to the output file.
	void write();

  private:

    /**
     * Type of buffer values.
     *  N.B. This is an int and double value, which will be 12 bytes and
     *       does not need padding for 4 byte alignement.
     */
    struct Timer_Export BufferType {
      int     method;
      size_t  sequence;
      Seconds elapsed;
	  BufferType() : method(0), sequence(0), elapsed(0) {}
    };

    /// Name of owned output file.
    char* filename_;

    /// Handle of output file.
    FILE* file_;

    /// Current method identifier
    int method_;

    /// Current sequence number.
    size_t sequence_;

    /// Value buffer for current interval.
    BufferType buffer_;

    /// Actual timer object.
    NxFoundation::Time timer_;

};

inline
CallTimer::CallTimer( int method)
 : filename_(0), file_(0), method_( method), sequence_(0)
{
}

inline
CallTimer::CallTimer( int method, char* filename, bool append)
 : filename_( 0), file_(0), method_(method), sequence_(0)
{
  this->open( filename, append);
}

inline
CallTimer::CallTimer( int method, FILE* file)
 : filename_(0), file_( file), method_(method), sequence_(0)
{
}

inline
CallTimer::Seconds
CallTimer::seconds()
{
  return this->timer_.PeekElapsedSeconds();
}

inline
CallTimer::Seconds
CallTimer::elapsed()
{
  return this->buffer_.elapsed;
}

inline
void
CallTimer::start()
{
  this->timer_.GetElapsedSeconds();
}

inline
void
CallTimer::file( FILE* file)
{
  this->file_ = file;
}

inline
FILE*
CallTimer::file() const
{
  return this->file_;
}

inline
int
CallTimer::method() const
{
  return this->method_;
}

inline
void
CallTimer::method( int method)
{
  this->method_ = method;
}

inline
size_t
CallTimer::sequence() const
{
  return this->sequence_;
}

#endif // CALLTIMER_H


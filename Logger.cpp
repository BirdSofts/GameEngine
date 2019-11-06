// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,06.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Logger.h"
#include "Shared.h"


using namespace winrt::Windows::Storage;


LogEntity::LogEntity ( void )
{
  id = 0;
  type = logType::info;
  threadId = std::this_thread::get_id ();
  threadName = "";
  message = "";
};


LogEntity LogEntity::operator=( LogEntity& logObj )
{
  LogEntity temp;
  temp.id = logObj.id;
  temp.arrivedAt = logObj.arrivedAt;
  temp.type = logObj.type;
  temp.threadId = logObj.threadId;
  temp.threadName = logObj.threadName;
  temp.message = logObj.message;
  return temp;
};


ToFile::ToFile ( void ) : path ( L"" ), ready ( false )
{
  try
  {

    StorageFolder folder ( ApplicationData::Current ().TemporaryFolder () );
    //StorageFile file = DownloadsFolder::CreateFileAsync ( L"sample.txt" ) ;
    //StorageFile file ( folder.CreateFileAsync ( L"sample.txt" ) );

    path = folder.Path () + L"\\dump.log";

    fileStream.open ( path, std::ofstream::binary );
    if (fileStream.is_open ())
      ready = true;
    else
    {
      //MessageBoxA ( NULL, "The log file could not be opened for writing.", "Error", MB_OK | MB_ICONERROR );
    }

  }
  catch (const std::exception & ex)
  {
    //MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
  }
};


const bool& ToFile::state ( void )
{
  return ready;
}


void ToFile::close ( void )
{
  fileStream.close ();
};


bool ToFile::write ( const std::string& line )
{
  try
  {

    if (ready)
    {
      fileStream << line;
      return true;
    } else
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Failed to output to log file!" );
      return false;
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return false;
  }
};


template<class tType>
unsigned int Logger<tType>::counter { 0 };


template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger ( void ) : theLogRawStr ( "" ), filePolicy (), writeGuard ()
{
  try
  {

    if (filePolicy.state ())
    {
      operating.test_and_set (); // mark the write engine as running
      commit = std::move ( std::thread { loggerEngine<tType>, this } );
    }

    state = 0;

  }
  catch (const std::exception & ex)
  {

    //MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );

  }
};


template<class tType>
void Logger<tType>::push ( const logType& t,
                           const std::thread::id& tId,
                           const std::string& tName,
                           const std::string& msg )
{
  try
  {

    std::stringstream moment;
    SYSTEMTIME cDateT;
    GetLocalTime ( &cDateT );
    // date and time format: xx/xx/xx xx:xx:xx
    moment << cDateT.wDay << '/' << cDateT.wMonth << '/' << cDateT.wYear << ' ';
    if (cDateT.wHour > 9)
      moment << cDateT.wHour << ':';
    else
      moment << "0" << cDateT.wHour << ':';
    if (cDateT.wMinute > 9)
      moment << cDateT.wMinute << ':';
    else
      moment << "0" << cDateT.wMinute << ':';
    if (cDateT.wSecond > 9)
      moment << cDateT.wSecond;
    else
      moment << "0" << cDateT.wSecond;

    counter++;
    theLog.id = counter;
    theLog.arrivedAt = moment.str ();
    theLog.type = t;
    theLog.threadId = tId;
    theLog.threadName = tName;
    theLog.message = msg;

    std::stringstream temp;
    temp << theLog.id << " - " << theLog.arrivedAt << " - ";
    std::stringstream line;
    if (
      (PointerProvider::getVariables ()->running == false) &&
      (PointerProvider::getVariables ()->currentState == "shutting down") &&
      (state == 0))
    {
      line << "\r\n\n";
      state = 1;
    } else
      line << "\r\n";
    if (theLog.id < 1000)
      line << theLog.id << "\t\t";
    else
      line << theLog.id << '\t';

    line << theLog.arrivedAt << '\t';
    switch (theLog.type)
    {
      case 0:
        temp << "INFO: - ";
        line << "INFO:    ";
        break;
      case 1:
        temp << "DEBUG: - ";
        line << "DEBUG:   ";
        break;
      case 2:
        temp << "WARNING: - ";
        line << "WARNING: ";
        break;
      case 3:
        temp << "ERROR: - ";
        line << "ERROR:   ";
        break;
    }
    temp << theLog.threadId << " - " << theLog.threadName << " - " << theLog.message;
    theLogRawStr = temp.str ();
    line << theLog.threadId << '\t' << theLog.threadName << '\t' << theLog.message;

    // after
    if (
      (PointerProvider::getVariables ()->running == true) &&
      (PointerProvider::getVariables ()->currentState == "initialized") &&
      (state == 1))
    {
      line << '\n';
      state = 2;
    }

    std::lock_guard<std::timed_mutex> lock ( writeGuard );
    buffer.push_back ( line.str () );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


template<class tType>
const LogEntity& Logger<tType>::getLog ( void )
{
  return theLog;
};


template<class tType>
const std::string& Logger<tType>::getLogRawStr ( void )
{
  return theLogRawStr;
};


template<class tType>
void Logger<tType>::shutdown ( void )
{
  buffer.clear ();
  filePolicy.close ();
  operating.clear ();
  commit.join ();
};


template<class tType>
void loggerEngine ( Logger<tType>* engine )
{
  try
  {

    // dump engine: write the present logs' data
    std::this_thread::sleep_for ( std::chrono::milliseconds { 20 } );

    PointerProvider::getFileLogger ()->push (
      logType::info, std::this_thread::get_id (), "logThread",
      "Logging engine is successfully started:\n\nFull-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );

    // Todo robust lock
    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
      if (engine->buffer.size ())
      {
        if (!lock.try_lock_for ( std::chrono::milliseconds { 50 } ))
          continue;
        for (auto& element : engine->buffer)
        {
          if (!engine->filePolicy.write ( element ))
          {
            PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "logThread",
                                                      "Writing to file wasn't possible." );
          }
        }

        engine->buffer.clear ();
        lock.unlock ();
      }
    } while (engine->operating.test_and_set () || engine->buffer.size ());

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "logThread",
                                              ex.what () );
  }
};


void LoggerClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  Logger<ToFile> tempObj;
  tempObj.push ( logType::error, std::this_thread::get_id (), "mainThread", "The problem solver... :)" );
  tempObj.getLog ();
  tempObj.getLogRawStr ();
  tempObj.shutdown ();

}

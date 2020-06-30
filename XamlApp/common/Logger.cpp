// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Logger.h"
#include "Shared.h"


using namespace winrt::Windows::Storage;


LogEntity::LogEntity ( void )
{

  m_id = 0;
  m_type = logType::info;
  m_threadId = std::this_thread::get_id ();
  m_threadName = "";
  m_message = "";

};


//LogEntity::~LogEntity ( void )
//{
//
//};


LogEntity LogEntity::operator=( LogEntity& logObj )
{

  LogEntity temp;

  temp.m_id = logObj.m_id;
  temp.m_arrivedAt = logObj.m_arrivedAt;
  temp.m_type = logObj.m_type;
  temp.m_threadId = logObj.m_threadId;
  temp.m_threadName = logObj.m_threadName;
  temp.m_message = logObj.m_message;

  return temp;

};


ToFile::ToFile ( void ) :
  m_path ( L"" ), m_ready ( false )
{
  try
  {

    StorageFolder folder ( ApplicationData::Current ().TemporaryFolder () );
    //StorageFile file = DownloadsFolder::CreateFileAsync ( L"sample.txt" ) ;
    //StorageFile file ( folder.CreateFileAsync ( L"sample.txt" ) );

    m_path = folder.Path () + L"\\dump.log";

    m_fileStream.open ( m_path, std::ofstream::binary );
    if (m_fileStream.is_open ())
      m_ready = true;
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


//ToFile::~ToFile ( void )
//{
//
//};


const bool& ToFile::m_isReady ( void )
{
  return m_ready;
}


void ToFile::m_close ( void )
{
  m_fileStream.close ();
};


bool ToFile::m_write ( const std::string& line )
{
  try
  {

    if (m_ready)
    {
      m_fileStream << line;
      return true;
    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Failed to output to log file!" );
      return false;
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


template<class tType>
unsigned int Logger<tType>::m_counter { 0 };


template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger ( void ) : m_theLogRawStr ( "" ), m_filePolicy (), m_writeGuard ()
{
  try
  {

    if (m_filePolicy.m_isReady ())
    {
      m_operating.test_and_set (); // mark the write engine as running
      m_commit = std::move ( std::thread { loggerEngine<tType>, this } );
    }

    m_state = 0;

  }
  catch (const std::exception & ex)
  {
    //MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
  }
};


//template<class tType>
//Logger<tType>::~Logger ( void )
//{
//
//};


template<class tType>
void Logger<tType>::m_push ( const logType& t, const std::thread::id& tId,
                             const std::string& tName, const std::string& msg )
{
  try
  {

    std::stringstream moment;

    SYSTEMTIME cDateT;
    GetLocalTime ( &cDateT ); // date and time format: xx/xx/xx xx:xx:xx

    moment
      << cDateT.wDay << '/'
      << cDateT.wMonth << '/'
      << cDateT.wYear << ' ';

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

    m_counter++;
    m_theLog.m_id = m_counter;
    m_theLog.m_arrivedAt = moment.str ();
    m_theLog.m_type = t;
    m_theLog.m_threadId = tId;
    m_theLog.m_threadName = tName;
    m_theLog.m_message = msg;

    std::stringstream lineRaw;
    lineRaw << m_theLog.m_id << " - " << m_theLog.m_arrivedAt << " - ";

    std::stringstream lineFormatted;

    // line formatting: add some space to recognize the different application state in log file
    if (
      (PointerProvider::getVariables ()->running == false) &&
      (PointerProvider::getVariables ()->currentState == "suspending") &&
      (m_state == 0))
    {
      lineFormatted << "\r\n\n";
      m_state = 1;
    } else
      lineFormatted << "\r\n";

    if (m_theLog.m_id < 1000)
      lineFormatted << m_theLog.m_id << "\t\t";
    else
      lineFormatted << m_theLog.m_id << '\t';

    lineFormatted << m_theLog.m_arrivedAt << '\t';

    switch (m_theLog.m_type)
    {
      case 0:
        lineRaw << "INFO: - ";
        lineFormatted << "INFO:    ";
        break;
      case 1:
        lineRaw << "DEBUG: - ";
        lineFormatted << "DEBUG:   ";
        break;
      case 2:
        lineRaw << "WARNING: - ";
        lineFormatted << "WARNING: ";
        break;
      case 3:
        lineRaw << "ERROR: - ";
        lineFormatted << "ERROR:   ";
        break;
    }

    lineRaw << m_theLog.m_threadId << " - " << m_theLog.m_threadName << " - " << m_theLog.m_message;
    m_theLogRawStr = lineRaw.str ();

    lineFormatted << m_theLog.m_threadId << '\t' << m_theLog.m_threadName << '\t' << m_theLog.m_message;

    // line formatting: add some space to recognize the different application state in log file
    if (
      (PointerProvider::getVariables ()->running == true) &&
      (PointerProvider::getVariables ()->currentState == "initialized") &&
      (m_state == 1))
    {
      lineFormatted << '\n';
      m_state = 2;
    }

    std::lock_guard<std::timed_mutex> lock ( m_writeGuard );
    m_buffer.push_back ( lineFormatted.str () );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


template<class tType>
const LogEntity& Logger<tType>::m_getLog ( void )
{
  return m_theLog;
};


template<class tType>
const std::string& Logger<tType>::m_getLogRawStr ( void )
{
  return m_theLogRawStr;
};


template<class tType>
void Logger<tType>::m_shutdown ( void )
{

  m_buffer.clear ();
  m_filePolicy.m_close ();
  m_operating.clear ();
  m_commit.join ();

};


template<class tType>
void loggerEngine ( Logger<tType>* engine )
{
  try
  {

    // dump engine: write any present logs' data
    std::this_thread::sleep_for ( std::chrono::milliseconds { 20 } );

    PointerProvider::getFileLogger ()->m_push (
      logType::info, std::this_thread::get_id (), "logThread",
      "Logging engine is successfully started:\n\nFull-featured surveillance \
      is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );

    // Todo robust lock
    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->m_writeGuard, std::defer_lock );

    do
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );

      if (engine->m_buffer.size ())
      {
        if (!lock.try_lock_for ( std::chrono::milliseconds { 50 } ))
          continue;

        for (auto& element : engine->m_buffer)
        {
          if (!engine->m_filePolicy.m_write ( element ))
          {
            PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (),
                                                        "logThread", "Writing to file wasn't possible." );
          }
        }

        engine->m_buffer.clear ();
        lock.unlock ();
      }

    } while (engine->m_operating.test_and_set () || engine->m_buffer.size ());

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "logThread",
                                                ex.what () );
  }
};


void LoggerClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  Logger<ToFile> tempObj;
  tempObj.m_push ( logType::error, std::this_thread::get_id (), "mainThread", "The problem solver... :)" );
  tempObj.m_getLog ();
  tempObj.m_getLogRawStr ();
  tempObj.m_shutdown ();

}

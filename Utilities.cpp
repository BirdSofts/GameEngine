// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Utilities.h"
#include "Shared.h"


theException::theException ( void ) : expected ( "null" ) {};
void theException::set ( const char* prm )
{
  expected = prm;
};
const char* theException::what ( void ) const throw()
{
  return expected;
};


LogEntity::LogEntity ( void )
{
  id = 0;
  type = logType::info;
  threadId = std::this_thread::get_id ();
  threadName = L"";
  message = L"";
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


toFile::toFile ( void ) : ready ( false )
{
  try
  {

    fileStream.open ( "dump.log", std::ofstream::binary );
    if (fileStream.is_open ())
      ready = true;
    else
    {
      MessageBoxA ( NULL, "The log file could not be opened for writing.", "Error", MB_OK | MB_ICONERROR );
    }

  }
  catch (const std::exception& ex)
  {
    MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
  }
};


toFile::~toFile ( void )
{
  ready = false;
  fileStream.close ();
};


const bool& toFile::state ( void )
{
  return ready;
}


bool toFile::write ( const std::wstring& line )
{
  try
  {

    if (ready)
    {
      fileStream << Converter::strConverter ( line );
      return true;
    } else
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Failed to output to log file!" );
      return false;
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return false;
  }
};


template<class tType>
unsigned int Logger<tType>::counter { 0 };


template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger ( void ) : theLogRawStr ( L"" ), filePolicy (), writeGuard ()
{
  try
  {

    if (filePolicy.state ())
    {
      operating.test_and_set (); // mark the write engine as running
      commit = std::move ( std::thread { loggerEngine<tType>, this } );
    }

  }
  catch (const std::exception& ex)
  {

    MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );

  }
};


template<class tType>
Logger<tType>::~Logger ( void )
{
  operating.clear ();
  commit.join ();
  buffer.clear ();
};


template<class tType>
void Logger<tType>::push ( const logType& t,
                           const std::thread::id& tId,
                           const std::wstring& tName,
                           const std::wstring& msg )
{
  try
  {

    std::wstringstream moment;
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

    std::wstringstream temp;
    temp << theLog.id << " - " << theLog.arrivedAt << " - ";
    std::wstringstream line;
    if ((running == false) && (gameState == L"shutting down"))
    {
      line << "\r\n\n";
      gameState = L"uninitialized";
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
    if ((running == true) && (gameState == L"initialized"))
    {
      line << '\n';
      gameState = L"gaming";
    }

    std::lock_guard<std::timed_mutex> lock ( writeGuard );
    buffer.push_back ( line.str () );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


template<class tType>
const LogEntity& Logger<tType>::getLog ( void )
{
  return theLog;
};


template<class tType>
const std::wstring& Logger<tType>::getLogRawStr ( void )
{
  return theLogRawStr;
};


template<class tType>
void loggerEngine ( Logger<tType>* engine )
{
  try
  {

    // dump engine: write the present logs' data
    std::this_thread::sleep_for ( std::chrono::milliseconds { 20 } );

    PointerProvider::getFileLogger ()->push (
      logType::info, std::this_thread::get_id (), L"logThread",
      L"Logging engine is successfully started:\n\nFull-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );

    // Todo robust lock
    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 1000 ) );
      if (engine->buffer.size ())
      {
        if (!lock.try_lock_for ( std::chrono::milliseconds { 50 } ))
          continue;
        for (auto& element : engine->buffer)
        {
          if (!engine->filePolicy.write ( element ))
          {
            PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"logThread",
                                                      L"Writing to file wasn't possible." );
          }
        }

        engine->buffer.clear ();
        lock.unlock ();
      }
    } while (engine->operating.test_and_set () || engine->buffer.size ());

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"logThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void LoggerClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  Logger<toFile> tempObj;
  tempObj.push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The problem solver... :)" );
  tempObj.getLog ();
  tempObj.getLogRawStr ();

}


Configurations::Configurations ( void )
{
  try
  {

    valid = false;
    std::this_thread::sleep_for ( std::chrono::milliseconds { 30 } );

    // defaults initialization:
    defaults.Width = 640;
    defaults.Height = 480;
    defaults.fullscreen = false;

    // currents initialization:
    currents.Width = 0;
    currents.Height = 0;
    currents.fullscreen = false;

    PWSTR docPath { NULL };
    HRESULT hR = SHGetKnownFolderPath ( FOLDERID_Documents, NULL, NULL, &docPath );
    std::wstring path { L"" };
    if (FAILED ( hR ))
    {
      MessageBoxA ( NULL, "The path to document directory is unknown! Please contact your OS support.",
                    "Critical-Error", MB_OK | MB_ICONERROR );

      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Retrieving the path to document directory failed!" );

      pathToMyDocuments = L"";
      path = L"C:\\TheGame";
      hR = SHCreateDirectory ( NULL, path.c_str () );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"The creation of directory failed!" );
        path = L"C:\\settings.lua";
      } else
        path += L"\\settings.lua";
    } else
    {
      pathToMyDocuments = docPath;
      path = pathToMyDocuments + L"\\settings.lua";
    }


    //!? temporary statement: development time path
    path = { L"C:\\Users\\Mehrdad\\Source\\Repos\\LearningDirectX\\settings.lua" };


    pathToSettings = path;
    // Lua accepts a string type as path
    std::string pathStr { "" };
    pathStr = Converter::strConverter ( pathToSettings );

    for (char i = 0; i < 2; i++)
    {
      // read the configuration
      sol::state configs;
      try
      {
        configs.safe_script_file ( pathStr ); // opening the configuration file
        // read the configuration or use the application defaults:
        currents.Width = configs ["configurations"]["resolution"]["width"].get_or ( currents.Width );
        // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
        currents.Height = configs ["configurations"]["resolution"]["height"].get_or ( currents.Height );
        unsigned int temp = configs ["configurations"]["display"]["fullscreen"].get_or ( temp );
        currents.fullscreen = temp;
      }
      catch (const std::exception& ex)
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  Converter::strConverter ( ex.what () ) );
      }

      // validation
      if ((currents.Width != 0) && (!valid))
      {
        valid = true;
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"The configuration file is successfully read:\n\tResolution: (" +
                                                  std::to_wstring ( currents.Width ) + L" x "
                                                  + std::to_wstring ( currents.Height ) + L" )\t\t" +
                                                  L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );
        break;
      } else
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Retrieving the configuration file failed (Non-existent or invalid)!" );

        // rewrite the configuration file with defaults
        if (!apply ( defaults ))
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                    L"Rewriting the Configuration file using default settings failed." );
        }
      }
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Configurations::isValid ( void )
{
  return valid;
};


const ConfigsContainer& Configurations::getDefaults ( void )
{
  return defaults;
};


const ConfigsContainer& Configurations::getSettings ( void )
{
  return currents;
};


void Configurations::apply ( void )
{
  try
  {

    std::ofstream writeStream ( pathToSettings.c_str () );
    if (writeStream.good ())
    {
      std::stringstream settingsLine;
      settingsLine << "configurations =\n\t{\n" <<
        "\t\tresolution = { width = 800 , height = 600 },\n" <<
        "\t\tdisplay = { fullscreen = 0 }\n\t}";
      writeStream << settingsLine.str ();
      writeStream.close ();
    }

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The configuration file is successfully written:\n\tResolution: (" +
                                              std::to_wstring ( currents.Width ) + L" x "
                                              + std::to_wstring ( currents.Height ) + L" )\t\t" +
                                              L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool Configurations::apply ( const ConfigsContainer& object )
{
  try
  {

    std::ofstream writeStream ( pathToSettings.c_str () );
    if (writeStream.good ())
    {
      std::stringstream settingsLine;
      settingsLine << "configurations =\n\t{\n" <<
        "\t\tresolution = { width = " << std::to_string ( object.Width ) <<
        " , height = " << std::to_string ( object.Height ) << " },\n" <<
        "\t\tdisplay = { fullscreen = " << std::to_string ( object.fullscreen ) << " }\n\t}";
      writeStream << settingsLine.str ();
      writeStream.close ();

      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The configuration file is successfully written:\n\tResolution: (" +
                                                std::to_wstring ( object.Width ) + L" x "
                                                + std::to_wstring ( object.Height ) + L" )\t\t" +
                                                L"fullscreen: " + std::to_wstring ( object.fullscreen ) );
      return true;
    } else
      return false;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return false;
  }
};


std::wstring Converter::strConverter ( const std::string& str )
{
  try
  {

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
    return entity.from_bytes ( str );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return L"";
  }
};


std::string Converter::strConverter ( const std::wstring& wstr )
{
  try
  {

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
    return entity.to_bytes ( wstr );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return "";
  }
};

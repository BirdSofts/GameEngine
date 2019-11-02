// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H


#include <string> // C++ standard string class
#include <locale> // C++ standard locals (facets of the below header is declared here)
#include <codecvt> // C++ standard Unicode conversion facets

#include <iostream> // C++ standard input and output streams
#include <sstream> // C++ standard string streams
#include <fstream> // C++ standard file streams

#include <exception> // C++ standard exception

#include <atomic> // C++ standard atomic objects (no data races)
#include <thread> // C++ standard threads
#include <mutex> // C++ standard lockable objects

#include <list> // C++ standard list container

#include <ShlObj.h> // Windows standard control APIs
#include <Shlwapi.h> // Windows standard lightweight utility APIs
//#include <PathCch.h> // Windows standard shell APIs
#pragma comment(lib, "Shlwapi.lib")
//#pragma comment(lib, "PathCch.lib")

// Lua: a powerful, lightweight and embeddable scripting language engine,
// and additionally the most popular general-purpose one used in games nowadays.
// -- more information: http://luabinaries.sourceforge.net/download.html
// -- Lua documentation: https://www.lua.org/docs.html
// -- Sol documentation: https://sol2.readthedocs.io/en/latest/
// purposes: write and read configuration files, run scripts and to write code to drive the gameplay.
// Sol: the go-to framework for high-performance binding between Lua and C++
// -- more information: https://github.com/ThePhD/sol2/releases
//#include <lua.h> // Lua scripting language engine header file
#define SOL_ALL_SAFETIES_ON 1
//#pragma warning( push )
//#pragma warning( disable : 4127 ) // constant warning
//#pragma warning( disable : 4702 ) // unreachable code warning
#include <sol/sol/sol.hpp> // Sol binding framework between Lua and C++
//#pragma warning( pop )
#pragma comment (lib, "lua.lib") // Lua scripting language engine library


// special expansion to exception container
class theException : public std::exception
{
private:
  const char* expected;
public:
  theException ( void );
  void set ( const char* );
  const char* what ( void ) const throw();
};


// log severity
enum logType { info = 0, debug, warning, error };


// log container structure
struct LogEntity
{
  unsigned int id;
  std::wstring arrivedAt;
  logType type;
  std::thread::id threadId;
  std::wstring threadName;
  std::wstring message;
  LogEntity ( void );
  LogEntity operator=( LogEntity& );
};


// file stream policy
class toFile
{
private:
  std::ofstream fileStream;
  bool ready;
public:
  toFile ( void );
  ~toFile ( void );
  const bool& state ( void );
  bool write ( const std::wstring& );
};


// Todo other stream policy
//class toStream {};


// logging engine
template<class tType>
class Logger
{
private:
  LogEntity theLog;
  std::wstring theLogRawStr;
  std::list<std::wstring> buffer; // buffer list container
  tType filePolicy; // output stream policy
  std::timed_mutex writeGuard; // write guard
  std::thread commit; // write engine thread
  // lock-free atomic flag (checking the running state) (standard initialization):
  std::atomic_flag operating { ATOMIC_FLAG_INIT };

  static unsigned int counter;
public:
  Logger ( void );
  ~Logger ( void );
  void push ( const logType&,
              const std::thread::id&,
              const std::wstring&,
              const std::wstring& );
  const LogEntity& getLog ( void );
  const std::wstring& getLogRawStr ( void );

  template<class tType>
  friend void loggerEngine ( Logger<tType>* ); // write engine
};
void LoggerClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


// configurations container
struct ConfigsContainer
{
  unsigned int Width;
  unsigned int Height;
  bool fullscreen;
};


// application configurations class
class Configurations
{
private:
  std::wstring pathToMyDocuments;
  std::wstring pathToSettings;
  bool valid;
  ConfigsContainer defaults;
  ConfigsContainer currents;
public:
  Configurations ( void );
  const bool& isValid ( void );
  const ConfigsContainer& getDefaults ( void );
  const ConfigsContainer& getSettings ( void );
  void apply ( void ); // test
  const bool apply ( const ConfigsContainer& ); // apply method
};// application configurations container


// string converter class
class Converter
{
private:
public:
  // standard string types converters (Lua needs)
  // Windows 10 builds upon Unicode natively, thus using wide strings is wise.
  static std::wstring strConverter ( const std::string& );
  static std::string strConverter ( const std::wstring& );
};


#endif // !UTILITIES_H

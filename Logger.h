// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,06.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#ifndef LOGGER_H
#define LOGGER_H


// log severity
enum logType { info = 0, debug, warning, error };


// log container structure
struct LogEntity
{
  unsigned int id;
  std::string arrivedAt;
  logType type;
  std::thread::id threadId;
  std::string threadName;
  std::string message;
  LogEntity ( void );
  LogEntity operator=( LogEntity& );
};


// file stream policy
class ToFile
{
private:
  std::ofstream fileStream;
  std::wstring path;
  bool ready;
public:
  ToFile ( void );
  const bool& state ( void );
  void close ( void );
  bool write ( const std::string& );
};


// Todo other stream policy
//class toStream {};


// logging engine
template<class tType>
class Logger
{
private:
  LogEntity theLog;
  std::string theLogRawStr;
  std::list<std::string> buffer; // buffer list container
  tType filePolicy; // output stream policy
  std::timed_mutex writeGuard; // write guard
  std::thread commit; // write engine thread
  // lock-free atomic flag (checking the running state) (standard initialization):
  std::atomic_flag operating { ATOMIC_FLAG_INIT };

  int state;
  static unsigned int counter;
public:
  Logger ( void );
  void push ( const logType&,
              const std::thread::id&,
              const std::string&,
              const std::string& );
  const LogEntity& getLog ( void );
  const std::string& getLogRawStr ( void );
  void shutdown ( void );

  template<class tType>
  friend void loggerEngine ( Logger<tType>* ); // write engine
};
void LoggerClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !LOGGER_H

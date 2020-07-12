// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef LOGGER_H
#define LOGGER_H


// log severity
enum logType { info = 0, debug, warning, error };


// log container structure
struct LogEntity
{
  unsigned int m_id;
  std::string m_arrivedAt;
  logType m_type;
  std::thread::id m_threadId;
  std::string m_threadName;
  std::string m_message;

  LogEntity ( void );
  //~LogEntity ( void );

  LogEntity operator=( LogEntity& logObj ); // assignment
};


// file stream policy
class ToFile
{
private:
  std::ofstream m_fileStream;
  std::wstring m_path;

  bool m_ready;
public:
  ToFile ( void );
  //~ToFile ( void );

  void m_close ( void ); // close the resource
  bool m_write ( const std::string& line ); // construction of a log entity

  const bool& ToFile::m_isReady ( void ) { return m_ready; }; // true if ready to write
};


// Todo other stream policy
//class toStream {};


// logging engine
template<class tType>
class Logger
{
private:
  LogEntity m_theLog;
  std::string m_theLogRawStr;
  std::list<std::string> m_buffer; // buffer list container
  tType m_filePolicy; // output stream policy
  std::timed_mutex m_writeGuard; // write guard
  std::thread m_commit; // write engine thread
  // lock-free atomic flag (checking the running state) (standard initialization):
  std::atomic_flag m_operating { ATOMIC_FLAG_INIT };

  int m_state;

  static unsigned int m_counter; // log counter
public:
  Logger ( void );
  //~Logger ( void );

  void m_push ( const logType& t, const std::thread::id& tId,
                const std::string& tName, const std::string& msg ); // push a log entity into list
  void m_shutdown ( void ); // close the resource

  const LogEntity& m_getLog ( void ) { return m_theLog; }; // get a log entity as structure
  const std::string& m_getLogRawStr ( void ) { return m_theLogRawStr; }; // get a log entity as a raw line

  template<class tType>
  friend void loggerEngine ( Logger<tType>* ); // write engine
};
void LoggerClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


template<class tType> unsigned int Logger<tType>::m_counter { 0 };


#endif // !LOGGER_H

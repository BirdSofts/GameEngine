// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,12.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Timer.h"
#include "Shared.h"


Timer::Timer ( void ) :
  m_frameIt ( true ), m_initialized ( false )
{
  try
  {

    if (QueryPerformanceFrequency ( (LARGE_INTEGER*) &m_frequency ))
    {

      // once calculated seconds per count (reciprocal of the number of counts per seconds)
      // TimeValueInSeconds = ActualTimeValue / Frequency
      m_secondsPerCount = double ( 1 ) / m_frequency;


      // timing of the following is measured in QPC units
      m_startTime = 0;
      m_lastStop = 0;
      m_lastTime = 0;
      m_deltaTime = 0;
      m_maxDeltaTime = m_frequency / 10;

      // timing of the following is measured in canonical tick format
      m_totalTimeIdle = 0;
      m_elapsedTicks = 0;
      m_totalTicks = 0;
      m_leftOver = 0;

      //static_cast<uint64_t> ((double ( 1 ) / 60) * m_frequency);

      // frame rate tracking
      m_currentFrameTime = 0;
      m_previousFrameTime = 0;
      m_frameCounter = 60;
      m_FPS = 0;
      m_framesThisSecond = 0;
      m_secondsCounter = 0;

      m_milliSPF = 0.0;
      m_secondsPerCount = 0.0;


      m_initialized = true;
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The high-precision timer is successfully instantiated." );

    } else
    {
      m_initialized = false;
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "The high-precision timer instantiation failed!" );
    }


    long long frequency { 0 };
    // Windows high performance timer: 'QueryPerformanceFrequency' 'QueryPerformanceCounter':
    // note that, if the system doesn't support it, C++ 11 standard chrono is the replacement.
    // -- QueryPerformanceFrequency:
    // current frequency of the performance counter (the counts per second of the performance timer),
    // which is a fixed value at system boot and consistent across all processors.
    // -- QueryPerformanceCounter:
    // the amounts of counts per seconds consisted of a high-precision (<1μs) time stamp,
    // usable in time-interval measurements.
    // note that the function returns zero if an error is occurred.
    // Todo implement C++ standard chrono
    if (QueryPerformanceFrequency ( (LARGE_INTEGER*) &frequency ))
    {
      // once calculated seconds per count (reciprocal of the number of counts per seconds)
      // TimeValueInSeconds = ActualTimeValue / Frequency
      m_secondsPerCount = double ( 1 ) / frequency;

      m_initialized = true;
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The high-precision timer is successfully instantiated." );

    } else
    {
      m_initialized = false;
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "The high-precision timer instantiation failed!" );
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Timer::~Timer ( void )
//{
//
//};


const bool& Timer::m_isInitialized ( void )
{
  return m_initialized;
};


void Timer::m_tick ( void )
{
  try
  {

    // tick and calculate the time between two frames
    uint64_t current;
    if (QueryPerformanceCounter ( (LARGE_INTEGER*) &current ))
    {

      // update the timer
      m_deltaTime = current - m_lastTime;
      m_lastTime = current;
      //m_secondsCounter += m_deltaTime;

      // clamp large deltas from paused states
      if (m_deltaTime > m_maxDeltaTime)
        m_deltaTime = m_maxDeltaTime;

      // small deltas
      //if ((m_deltaTime - m_elapsedTicks) < m_frequency / 4000)
      //{
      //  m_deltaTime = m_elapsedTicks;
      //}

      // keep delta time of current frame untouched
      m_leftOver = m_frequency - m_deltaTime;

      uint64_t temp { m_lastTime };
      uint64_t tempCurrent { m_lastTime };

      // ToDo: implement the timer at extend and clean its class from unneeded stuff
      // for the time being, the algorithm is written dirty, with the purpose of consuming less energy... :)
      // yep, the sake of environment is still streaming in my blood! :)
      while (m_leftOver >= 0)
      {

        QueryPerformanceCounter ( (LARGE_INTEGER*) &tempCurrent );
        temp = tempCurrent - temp;

        m_totalTicks += m_elapsedTicks;
        m_leftOver -= temp;

        std::this_thread::sleep_for ( std::chrono::milliseconds ( 10 ) );
      }

      m_frameCounter++;

    }



    //if (m_paused)
    //  m_deltaTime = 0; // the elapsed time in a stopped state (for calculations in an idle time)
    //else
    if (QueryPerformanceCounter ( (LARGE_INTEGER*) &m_currentFrameTime ))
    {
      m_deltaTime = (m_currentFrameTime - m_previousFrameTime) * m_secondsPerCount; // the elapsed time of one frame
      m_previousFrameTime = m_currentFrameTime; // preparation for the next tick

      // in case, a negative delta means that the processor goes idle. the cause can be an overflow,
      // a power save mode or the movement of the process to another processor.
      if (m_deltaTime < 0)
        m_deltaTime = 0;
    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Timer failed to tick!" );
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Timer::m_event ( const std::string& type )
{
  try
  {

    long long current;

    if (QueryPerformanceCounter ( (LARGE_INTEGER*) &current ))
    {
      // if start is requested as event (invoked at game reactivation)
      //if ((type == "start") && (m_paused))
      //{
      //  // Todo Bug, important: in new hibernation state of the game engine, time jumps on a time machine and rides away,
      //  // and since it is essential to have the correct timing, the timer functionality needs to be rewritten.

      //  m_totalTimeIdle += (current - m_lastStop); // calculate total ideal
      //  m_previousFrameTime = current; // prepare the calculation of this frame
      //  // make ready for next stop:
      //  m_lastStop = 0;
      //  m_paused = false;
      //}

      //// if pause is requested as event (invoked at game deactivation)
      //if ((type == "pause") && (!m_paused))
      //{
      //  m_lastStop = current; // store the time for later use
      //  m_paused = true;
      //}

      // if reset is requested as event (start of the game loop)
      if (type == "reset")
      {
        // prepare the timer:
        m_startTime = current;
        m_previousFrameTime = current;
        m_lastStop = 0;
        //m_paused = false;


        m_lastTime = current;
      }

    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "One timer functionality failed! Requested event: "
                                                  + type );
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//const bool& Timer::m_isPaused ( void )
//{
//  return m_paused;
//};


const double Timer::m_getTotalTime ( void )
{

  // total running time from the start of the game
  double temp;
  //if (m_paused)
  //  temp = (m_lastStop - m_startTime - m_totalTimeIdle) * m_secondsPerCount;
  //else
  temp = (m_currentFrameTime - m_startTime - m_totalTimeIdle) * m_secondsPerCount;
  return temp;

};


const double& Timer::m_getDeltaTime ( void )
{
  return m_deltaTime;
};

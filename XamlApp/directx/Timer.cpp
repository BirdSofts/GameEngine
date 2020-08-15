// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Timer.h"
#include "Shared.h"


Timer::Timer ( void ) :
  m_screenIt ( false ), m_initialized ( false )
{
  try
  {

    m_frequency = 0;
    m_deltaTime = 0;
    m_deltaTimeMax = 0;

    m_secondsPerCount = 0.0;


    m_timeStart = 0;
    m_ticksElapsed = 0;
    m_ticksTotal = 0;
    m_timeLastIdle = 0;
    m_timeTotalIdle = 0;

    m_currentFrame = 0;
    m_previousFrame = 0;

    m_secondsCounter = 0;

    m_ticksLeftOver = 0;

    m_sleepFactor = 0;
    m_split = 0;

    m_framesCounter = 0;
    m_framesThisSecond = 0;
    m_FPS = 0;

    m_milliSPF = 0.0;

    m_ticksProcCpuUsage = 0;
    m_ticksSysCpuUsage = 0;
    m_percentProcCpuUsage = 0;
    m_percentSysCpuUsage = 0;


    LARGE_INTEGER frequency;

    if (QueryPerformanceFrequency ( &frequency ))
    {

      m_frequency = frequency.QuadPart;

      // once calculated seconds per count (reciprocal of the number of counts per seconds)
      // TimeValueInSeconds = ActualTimeValue / Frequency
      m_secondsPerCount = double ( 1 ) / m_frequency;

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


      m_deltaTimeMax = m_frequency / 10;


      // calculate a split of sleep factor
      LARGE_INTEGER tempOne;
      LARGE_INTEGER tempTwo;
      QueryPerformanceCounter ( &tempOne );

      std::this_thread::sleep_for ( std::chrono::microseconds ( 1 ) );

      QueryPerformanceCounter ( &tempTwo );

      m_split = tempTwo.QuadPart - tempOne.QuadPart;


      m_initialized = true;

      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "High-precision timer is successfully instantiated." );

    } else
    {
      m_initialized = false;
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "High-precision timer instantiation failed!" );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Timer::~Timer ( void )
//{
//
//};


void Timer::m_tick ( void )
{
  try
  {

    // tick and calculate the time between two frames
    LARGE_INTEGER current;
    if (QueryPerformanceCounter ( &current ))
    {

      m_currentFrame = current.QuadPart;

      // update timer
      m_deltaTime = m_currentFrame - m_previousFrame; // the elapsed time of one frame
      m_previousFrame = m_currentFrame; // preparation for the next tick

      // clamp large deltas from paused states
      if (m_deltaTime > m_deltaTimeMax)
        m_deltaTime = m_deltaTimeMax;

      //// in case, a negative delta means that the processor goes idle. the cause can be an overflow,
      //// a power save mode or the movement of the process to another processor.
      //if (m_deltaTime < 0)
      //  m_deltaTime = 0;


      // small deltas
      //if ((m_deltaTime - m_elapsedTicks) < m_frequency / 4000)
      //{
      //  m_deltaTime = m_elapsedTicks;
      //}

      // keep delta time of current frame untouched
      m_ticksLeftOver = m_frequency - m_deltaTime;


      //m_sleepFactor = m_ticksLeftOver * m_secondsPerCount * 10000000; // 10, 10000, 10000000
      //// note that each second is 1000 milliseconds, 1000000 microseconds and 1000000000 nanoseconds
      //std::this_thread::sleep_for ( std::chrono::nanoseconds ( long ( m_sleepFactor ) ) );


      m_sleepFactor = 0;
      m_sleepFactor = (m_ticksLeftOver - m_split) / m_split;
      m_sleepFactor = m_sleepFactor * m_secondsPerCount * 10000;

      // ToDo: implement timer class at extend (the factors must be implemented at updating time)
      // for the time being, the algorithm is written dirty, with the purpose of consuming less energy... :)
      // yep, the sake of environment is still streaming in my blood! :)

      // update: suspension of engine thread
      // at first this algorithm was thought to use less CPU, now at extend,
      // it may come to updating the game on other threads and implementing timer there.

      LARGE_INTEGER tempCurrent { 0 };
      uint64_t passedTicks { 0 };
      uint64_t sumOfPassed { 0 };
      int t { 0 };
      while (m_ticksLeftOver >= 0)
      {

        QueryPerformanceCounter ( &tempCurrent );
        passedTicks = tempCurrent.QuadPart - (m_previousFrame - sumOfPassed);

        sumOfPassed += passedTicks;

        m_ticksTotal += m_ticksElapsed;

        m_ticksLeftOver -= passedTicks * 3;

        //if (m_FPS > 0)
        //{

        //  int t { 0 };
        //  if (m_FPS >= 60)
        //  {

        //    t = ((m_FPS % 60) / 3);
        //    if (t == 0)
        //      t = 1;
        //    m_leftOver -= passedTicks * t;

        //  } else
        //    if (m_FPS < 60)
        //    {

        //      t = ((60 % m_FPS) / 2);
        //      if (t == 0)
        //        t = 1;
        //      m_leftOver -= passedTicks * t;

        //    }

        //std::this_thread::sleep_for ( std::chrono::microseconds ( m_sleepFactor - (m_sleepFactor / (60 - m_FPS)) ) );
        std::this_thread::sleep_for ( std::chrono::microseconds ( m_sleepFactor ) );


        //} else
        //{
        //  m_leftOver -= passedTicks;
        //  std::this_thread::sleep_for ( std::chrono::microseconds ( m_sleepFactor ) );
        //}

      }

      m_framesCounter++;
      m_framesThisSecond++;

      m_secondsCounter += m_deltaTime;

      if (m_secondsCounter >= m_frequency)
      {
        m_FPS = m_framesThisSecond;
        m_milliSPF = 1e3 / m_FPS;
        m_screenIt = true;
        m_framesThisSecond = 0;
        m_secondsCounter %= m_frequency;


        //// a static local variable retains its state between the calls:
        //static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
        //static double elapsed; // the elapsed time since the last call
        //frameCounter++;

        //if ((m_timer->m_getTotalTime () - elapsed) >= 1e0)
        //{

        //  // frame calculations:
        //  m_FPS = frameCounter; // the number of counted frames in one second
        //  m_milliSPF = 1e3 / m_FPS; // average taken time by a frame in milliseconds

        //  // reset
        //  frameCounter = 0;
        //  elapsed += 1.0;
        //}


        uint64_t currentProc { 0 }, currentSys { 0 };
        uint64_t temp1 { 0 }, temp2 { 0 };

        currentProc = winrt::Windows::System::Diagnostics::ProcessDiagnosticInfo::GetForCurrentProcess ().CpuUsage ().GetReport ().UserTime ().count ();
        currentSys = winrt::Windows::System::Diagnostics::SystemDiagnosticInfo::GetForCurrentSystem ().CpuUsage ().GetReport ().UserTime ().count ();
        //currentProc = winrt::Windows::System::Diagnostics::ProcessDiagnosticInfo::GetForCurrentProcess ().CpuUsage ().GetReport ().KernelTime ().count ();
        //currentSys = winrt::Windows::System::Diagnostics::SystemDiagnosticInfo::GetForCurrentSystem ().CpuUsage ().GetReport ().KernelTime ().count ();

        temp1 = currentProc - m_ticksProcCpuUsage;
        m_ticksProcCpuUsage = currentProc;
        temp2 = currentSys - m_ticksSysCpuUsage;
        m_ticksSysCpuUsage = currentSys;

        m_percentProcCpuUsage = (100 * temp1) / m_frequency;
        m_percentSysCpuUsage = (100 * temp2) / m_frequency;

      }

    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Timer failed to tick!" );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Timer::m_event ( const typeEvent& type )
{
  try
  {

    LARGE_INTEGER current;

    if (QueryPerformanceCounter ( &current ))
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
      if (type == typeEvent::reset)
      {
        // prepare the timer:
        m_timeStart = current.QuadPart;
        m_previousFrame = current.QuadPart;
        m_timeLastIdle = 0;
      }

    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "One timer functionality failed! Requested event: "
                                                  + type );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


uint64_t& const Timer::m_getTotalTicks ( const typeTiming& type )
{

  uint64_t result { 0 };
  return result;

};


double& const Timer::m_getTotalSeconds ( const typeTiming& type )
{

  // total running time from the start of the game
  double temp;
  //if (m_paused)
  //  temp = (m_lastStop - m_startTime - m_totalTimeIdle) * m_secondsPerCount;
  //else
  temp = (m_currentFrame - m_timeStart - m_timeTotalIdle) * m_secondsPerCount;
  return temp;

};

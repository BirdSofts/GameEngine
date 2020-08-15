// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef TIMER_H
#define TIMER_H


// timer event
enum typeEvent { start = 0, pause, reset };
// timer timing
enum typeTiming { gamed = 0, idle, all };


// high-precision timer container
class Timer
{
private:
  // timing of the following is measured in -QPC units- known as counts
  uint64_t m_frequency; // current frequency (a constant counts per second across all processors)
  uint64_t m_deltaTime; // the automatically updated time between two frames (essential for a robust physics engine)
  uint64_t m_deltaTimeMax; // one tenth of the counts per a second (CPU)

  double m_secondsPerCount; // frequency reciprocal (seconds per count)


  // timing of the following is measured in canonical tick format
  uint64_t m_timeStart; // application start time (queried at the game loop's start)
  uint64_t m_ticksElapsed; // targeted elapsed ticks (frames)
  uint64_t m_ticksTotal; // a total number of ticks (frames)
  uint64_t m_timeLastIdle; // the time of the last pause (user pauses, minimized, inactive)
  uint64_t m_timeTotalIdle; // a total of all idle time (paused, minimized, inactive)

  // frame rate tracking in ticks
  uint64_t m_currentFrame; // the time of the current frame (current tick)
  uint64_t m_previousFrame; // the time of the previous frame (last tick)

  uint64_t m_secondsCounter; // sum of all deltas in a frame
  int64_t m_ticksLeftOver; // leftover ticks of a frame

  // engines hibernation calculations
  uint32_t m_sleepFactor; // amount of time that main engine sleeps measured in seconds
  uint32_t m_split; // calculated split of sleep factor in a frame measured in ticks

  // frame rate tracking in seconds
  uint64_t m_framesCounter; // frames counter (the amount of application frames)
  uint32_t m_framesThisSecond; // frames counter (frames of a second)
  uint32_t m_FPS; // frames per second
  double m_milliSPF; // render time of a frame in milliseconds

  // CPU performance properties (measured in canonical tick format)
  uint64_t m_ticksProcCpuUsage;
  uint64_t m_ticksSysCpuUsage;
  uint16_t m_percentProcCpuUsage;
  uint16_t m_percentSysCpuUsage;

  bool m_screenIt; // screen a frame only once a second
  bool m_initialized; // true if the initialization was successful
public:
  Timer ( void );
  //~Timer ( void );

  void m_tick ( void ); // the time never stops! (the time of every frame i.e. a cycle of game loop)
  void m_event ( const typeEvent& type ); // start, reset, stop
  uint64_t& const m_getTotalTicks ( const typeTiming& type ); // get total time in ticks
  double& const m_getTotalSeconds ( const typeTiming& type ); // get total time in seconds

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  bool& m_isNewFrame ( void ) { return m_screenIt; }; // true if FPS got updated
  const double& m_getDeltaTime ( void ) { return m_deltaTime; }; // get the delta time
  uint32_t& const m_getFPS ( void ) { return m_FPS; }; // get frame per seconds
  double& const m_getMilliSPF ( void ) { return m_milliSPF; }; // get render time of a frame
  uint16_t& const m_getProcCpuUsage ( void ) { return m_percentProcCpuUsage; }; // get process CPU usage
  uint16_t& const m_getSysCpuUsage ( void ) { return m_percentSysCpuUsage; }; // get process CPU usage
};


#endif // !TIMER_H

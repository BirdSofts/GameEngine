// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#ifndef TIMER_H
#define TIMER_H


// high-precision timer container (time is measured in counts)
class Timer
{
private:
  bool initialized; // true if the initialization was successful
  long long timeStart; // application start time (start of the game loop)
  long long timeCurrentFrame; // the time of the current frame
  long long timePreviousFrame; // the time of the previous frame
  long long timeLastStopped; // the time of the last pause (user pauses, minimized, inactive)
  long long timeTotalIdle; // a total of all idle time (paused, minimized, inactive)
  double secondsPerCount; // frequency reciprocal (seconds per count)
  double timeDelta; // the automatically updated time between two frames (essential for a robust physics engine)
  bool paused; // true if the timer is paused
public:
  Timer ( void );
  const bool& isInitialized ( void ); // get the initialized state
  const double getTotal ( void ); // total time minus the idle time
  const double& getDelta ( void ); // get the delta time
  const bool& isPaused ( void ); // get the paused state
  void event ( std::string type ); // start, reset, stop
  void tick ( void ); // the time never stops! (the time of every frame i.e. a cycle of game loop)
};


#endif // !TIMER_H

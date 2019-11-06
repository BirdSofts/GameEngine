// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,05.11.2019</changed>
// ********************************************************************************

#ifndef CORE_H
#define CORE_H


#include "Timer.h"
#include "Direct3D.h"
#include "Direct2D.h"
#include "Game.h"


// Game Framework wrapper
class TheCore
{
  friend class Direct3D;
  friend class Direct2D;
  friend class Game;
private:
  HINSTANCE appInstance; // handle to the application instance

  Timer* timer; // pointer to DirectX high-precision timer application
  int fps; // frames per second
  double mspf; // render time of a frame in milliseconds

  ::IUnknown* appWindow; // pointer to main window

  Direct3D* d3d; // pointer to Direct3D application
  Direct2D* d2d; // pointer to Direct2D application

  Game* game; // pointer to the game wrapper class

  bool debug; // if true FPS to screen (true in debug mode)
  bool initialized; // true if the initialization was successful
  bool paused; // true if the game is paused
  bool resized; // true if the resizing was successful
public:
  TheCore ( ::IUnknown*, Game* ); // constructor and initializer
  const bool& isInitialized ( void ); // get the initialized state
  const bool& isPaused ( void ); // get the paused state
  const ::IUnknown* getWindow ( void ); // get the main window
  Timer* getTimer ( void ); // get the pointer to application timer
  //Direct3D* getd3d ( void ); // get the pointer to application Direct3D
  //Direct2D* getd2d ( void ); // get the pointer to application Direct2D
  const int& getFPS ( void ); // get the FPS
  void frameStatistics ( void ); // frame statistics calculations
  void setResolution ( const bool& ); // resolution changer
  void resizeResources ( const bool& ); // free and resize the resources
  void shutdown ( void ); // destruction preparations
};


#endif // !CORE_H

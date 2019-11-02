// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef WINDOW_H
#define WINDOW_H


#include <windows.h> // Windows standard APIs

#include "Core.h"


// Windows OS window class container
class Window
{
  friend class TheCore;
private:
  HINSTANCE appInstance; // handle to the application instance
  HWND handle; // handle to game window (main window)
  TheCore* core; // pointer to the framework core
  bool initialized; // true if the initialization was successful
  unsigned int clientWidth; // desired window resolution
  unsigned int clientHeight;
  bool minimized; // true if minimized
  bool maximized; // true if maximized
  bool resizing; // true while being dragged around
  bool resized; // true when resized
public:
  Window ( TheCore* ); // constructor and initializer
  const bool& isInitialized ( void ); // get the initialized state
  const HWND& getHandle ( void ); // get the handle to main window
  unsigned int& getWidth ( void ); // get/set current window width
  unsigned int& getHeight ( void ); // get/set current window height
  bool& isResized ( void ); // get/set the resized state
  virtual LRESULT CALLBACK msgProc ( HWND, UINT, WPARAM, LPARAM ); // Windows callback procedure
};


#endif // !WINDOW_H

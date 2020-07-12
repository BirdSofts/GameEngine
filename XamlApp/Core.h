// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef CORE_H
#define CORE_H


#include "directx/Timer.h"
#include "directx/Direct3D.h"
#include "directx/Direct2D.h"
#include "MainPageTypes.h"


// game engine framework wrapper
class TheCore
{
  friend class Direct3D;
  friend class Direct2D;
private:
  winrt::Windows::UI::Xaml::Controls::SwapChainPanel* m_swapChainPanel; // pointer to main window swap chain panel
  MainPageTypes* m_mainPageTypes; // pointer to main page's entities

  Timer* m_timer; // pointer to DirectX high-precision timer application
  int m_FPS; // frames per second
  double m_milliSPF; // render time of a frame in milliseconds

  Direct3D* m_D3D; // pointer to Direct3D application
  Direct2D* m_D2D; // pointer to Direct2D application

  bool m_debug; // if true FPS to screen (true in debug mode)
  bool m_isResizing; // true if the resizing was successful
  bool m_initialized; // true if the initialization was successful
public:
  TheCore ( MainPageTypes* mainPageTypes ); // constructor and initializer
  //~TheCore ( void );

  void m_setSwapChainPanel ( winrt::Windows::UI::Xaml::Controls::SwapChainPanel* swapChainPanel ); // set swap chain panel
  void m_setResolution ( const bool& prm, const int& width = 0, const int& height = 0 ); // resolution changer
  void m_resizeResources ( const bool& displayMode ); // free and resize the resources
  void m_frameStatistics ( void ); // frame statistics calculations
  void m_onSuspending ( void ); // suspension preparations
  void m_onDeviceLost ( void ); // clean and reallocate
  void m_validate ( void ); // validate the correct state of game resources

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  const bool& m_isDebugging ( void ) { return m_debug; }; // get current state of debug flag
  Timer* m_getTimer ( void ) { return m_timer; }; // get the pointer to application timer
  Direct3D* m_getD3D ( void ) { return m_D3D; }; // get the pointer to application Direct3D
  Direct2D* m_getD2D ( void ) { return m_D2D; }; // get the pointer to application Direct2D
  const int& m_getFPS ( void ) { return m_FPS; }; // get the FPS
};


#endif // !CORE_H

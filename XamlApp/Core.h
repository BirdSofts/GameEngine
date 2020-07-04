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


// game engine framework wrapper
class TheCore
{
  friend class Direct3D;
  friend class Direct2D;
private:
  winrt::Windows::UI::Xaml::Controls::SwapChainPanel* m_swapChainPanel; // pointer to application main window
  winrt::Windows::Foundation::Size m_panelSize;
  int m_outputWidth; // current output width
  int m_outputHeight; // current output height
  // rotation

  Timer* m_timer; // pointer to DirectX high-precision timer application
  int m_FPS; // frames per second
  double m_milliSPF; // render time of a frame in milliseconds

  Direct3D* m_D3D; // pointer to Direct3D application
  Direct2D* m_D2D; // pointer to Direct2D application

  bool m_debug; // if true FPS to screen (true in debug mode)
  //bool resized; // true if the resizing was successful
  bool m_initialized; // true if the initialization was successful
public:
  TheCore ( const int&, const int& ); // constructor and initializer
  //~TheCore ( void ) { /**/ };

  void m_frameStatistics ( void ); // frame statistics calculations
  void m_setResolution ( const bool&, const int& = 0, const int& = 0 ); // resolution changer
  void m_setSwapChainPanel ( winrt::Windows::UI::Xaml::Controls::SwapChainPanel* ); // set swap chain panel
  void m_resizeResources ( const bool& ); // free and resize the resources
  void m_onSuspending ( void ); // suspension preparations
  void m_validate ( void ); // validate the correct state of game resources
  void m_onDeviceLost ( void ); // clean and reallocate

  const bool& TheCore::m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  const bool& TheCore::m_isDebugging ( void ) { return m_debug; }; // get current state of debug flag
  const winrt::Windows::UI::Xaml::Controls::SwapChainPanel* TheCore::m_getSwapChainPanel ( void ) { return m_swapChainPanel; }; // pointer to swap chain panel
  Timer* TheCore::m_getTimer ( void ) { return m_timer; }; // get the pointer to application timer
  Direct3D* TheCore::m_getD3D ( void ) { return m_D3D; }; // get the pointer to application Direct3D
  Direct2D* TheCore::m_getD2D ( void ) { return m_D2D; }; // get the pointer to application Direct2D
  const int& TheCore::m_getFPS ( void ) { return m_FPS; }; // get the FPS
};


#endif // !CORE_H

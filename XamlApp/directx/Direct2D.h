// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,28.06.2020</changed>
// ********************************************************************************

#ifndef DIRECT2D_H
#define DIRECT2D_H


#include "Core.h"
#include "Direct3D.h"


// Direct2D wrapper
class Direct2D
{
  friend class TheCore;
  friend class Direct3D;
private:
  TheCore* m_core; // pointer to the framework core

  winrt::com_ptr<IDWriteFactory1> m_writeFactory; // DirectWrite factory
  winrt::com_ptr<ID2D1Factory1> m_factory; // Direct2D factory
  winrt::com_ptr<ID2D1Device> m_device; // Direct2D device
  winrt::com_ptr<ID2D1DeviceContext> m_deviceContext; // Direct2D device context
  winrt::com_ptr<IDXGISurface1> m_dcBuffer; // Direct2D device context buffer
  winrt::com_ptr<ID2D1Bitmap1> m_dcBitmap; // render target bitmap (desired properties)

  winrt::com_ptr<ID2D1SolidColorBrush> m_brushYellow; // brushes
  winrt::com_ptr<ID2D1SolidColorBrush> m_brushWhite;
  winrt::com_ptr<ID2D1SolidColorBrush> m_brushBlack;

  winrt::com_ptr<IDWriteTextFormat> m_textFormatFPS; // text formats
  winrt::com_ptr<IDWriteTextFormat> m_textFormatLogs;
  winrt::com_ptr<IDWriteTextLayout> m_textLayoutFPS; // text layouts
  winrt::com_ptr<IDWriteTextLayout> m_textLayoutLogs;
  bool m_textLayoutsDebug;

  bool m_allocated; // true if resources allocation was successful
  bool m_initialized; // true if initialization was successful

  void m_allocateResources ( void ); // Direct2D resources resize/creation
  void m_initializeTextFormats ( void ); // different formats initialization
  void m_creation ( void ); // Direct2D device creation
  void m_allocation ( void ); // Direct2D resources resize/creation
  void m_onSuspending ( void ); // suspension preparation
  void m_validate ( void ); // validate the correct state of Direct2D resources
  //void m_onDeviceLost ( void ); // clean and reallocate
public:
  Direct2D ( TheCore* ); // creation of the device
  //~Direct2D ( void );
  const bool& m_isInitialized (); // get the initialized state

  void m_debugInfos ( void ); // FPS infos plus logs to screen
};


#endif // !DIRECT2D_H

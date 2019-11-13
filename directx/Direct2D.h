// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,13.11.2019</changed>
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

  Microsoft::WRL::ComPtr<IDWriteFactory1> m_writeFactory; // DirectWrite factory
  Microsoft::WRL::ComPtr<ID2D1Factory1> m_factory; // Direct2D factory
  Microsoft::WRL::ComPtr<ID2D1Device> m_device; // Direct2D device
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_deviceContext; // Direct2D device context
  Microsoft::WRL::ComPtr<IDXGISurface1> m_dcBuffer; // Direct2D device context buffer
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_dcBitmap; // render target bitmap (desired properties)

  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brushYellow; // brushes
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brushWhite;
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brushBlack;

  Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormatFPS; // text formats
  Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormatLogs;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayoutFPS; // text layouts
  Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayoutLogs;
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

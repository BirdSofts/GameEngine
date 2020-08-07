// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
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
  TheCore* m_core; // pointer to the application core

  winrt::com_ptr<ID2D1Factory3> m_factory; // Direct2D factory
  winrt::com_ptr<ID2D1Device2> m_device; // Direct2D device
  winrt::com_ptr<ID2D1DeviceContext2> m_deviceContext; // Direct2D device context
  winrt::com_ptr<IDXGISurface1> m_deviceContextBuffer; // Direct2D device context buffer
  winrt::com_ptr<ID2D1Bitmap1> m_deviceContextBitmap; // render target bitmap (desired properties)

  winrt::com_ptr<IDWriteFactory3> m_directWriteFactory; // DirectWrite factory
  //winrt::com_ptr<IWICImagingFactory> m_wicFactory; // WIC imaging factory

  winrt::com_ptr<ID2D1SolidColorBrush> m_brushYellow; // brushes
  winrt::com_ptr<ID2D1SolidColorBrush> m_brushWhite;
  winrt::com_ptr<ID2D1SolidColorBrush> m_brushBlack;
  winrt::com_ptr<ID2D1SolidColorBrush> m_brushRed;

  winrt::com_ptr<ID2D1DrawingStateBlock1> m_stateBlock; // drawing state block

  winrt::com_ptr<IDWriteTextLayout3> m_textLayoutFPS; // text layouts
  winrt::com_ptr<IDWriteTextLayout3> m_textLayoutLogs;
  winrt::com_ptr<IDWriteTextLayout3> m_textLayoutPointer;
  winrt::com_ptr<IDWriteTextFormat2> m_textFormatFPS; // text formats
  winrt::com_ptr<IDWriteTextFormat2> m_textFormatLogs;
  winrt::com_ptr<IDWriteTextFormat2> m_textFormatPointer;
  DWRITE_TEXT_METRICS1 m_textMetricsFPS; // text metrics
  DWRITE_TEXT_METRICS1 m_textMetricsLogs;
  DWRITE_TEXT_METRICS1 m_textMetricsPointer;

  bool m_textLayoutsDebug; // output prevention while updating

  bool m_allocated; // true if resources are allocated successful
  bool m_initialized; // true in case of successful initialization

  bool m_createResources ( void ); // create Direct2D independent resources
  bool m_createDeviceResources ( void ); // create Direct2D device dependent resources
  bool m_createDeviceContextResources ( void ); // create Direct2D device context dependent resources
  bool m_initializeTextFormats ( void ); // different formats initialization
  void m_release ( void ); // suspension preparation
public:
  Direct2D ( TheCore* coreObj );
  //~Direct2D ( void );

  void m_debugInfos ( void ); // FPS infos plus logs to screen

  const bool& m_isReady () { return m_allocated; }; // get the initialized state
};


#endif // !DIRECT2D_H

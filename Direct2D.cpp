// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Direct2D.h"
#include "Shared.h"


Direct2D::Direct2D ( TheCore* coreObj ) :
  m_core ( coreObj ), m_textLayoutsDebug ( false ),
  m_allocated ( false ), m_initialized ( false )
{
  try
  {

    HRESULT hR;
    unsigned long rC { 0 }; // reference counts

    // creation of DirectWrite factory
    // properties options: --shared (reuse of cached font data, thus better performance), --isolated
    hR = DWriteCreateFactory ( DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_writeFactory );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "The creation of DirectWrite factory failed!" );
      return;
    }

    // creation option flags
    D2D1_FACTORY_OPTIONS options;
#ifndef _NOT_DEBUGGING
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION; // debug (error and warning messages, diagnostic informations)
#else
    options.debugLevel = D2D1_DEBUG_LEVEL_NONE; // shipping build and release
#endif // !_NOT_DEBUGGING

    // creation of Direct2D factory
    // multi threaded flag: safe access to the factory from multiple threads
    hR = D2D1CreateFactory ( D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, &m_factory );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "The creation of Direct2D factory failed!" );
      return;
    }

    // acquiring the underlying DXGI factory used to create the Dirext3D device (resources needs)
    IDXGIDevice1* dxgiDevice;
    hR = m_core->m_getD3D ()->m_getDevice ()->QueryInterface ( __uuidof(IDXGIDevice1), (void**) &dxgiDevice );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Acquiring the DXGI device failed!" );
      return;
    }

    // creation of the Direct2D device
    // threading mode will be inherited from the DXGI device.
    hR = m_factory->CreateDevice ( dxgiDevice, &m_device );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of Direct2D device failed!" );
      return;
    }
    rC = dxgiDevice->Release ();
    dxgiDevice = nullptr;

    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct2D is successfully initialized." );

    m_allocateResources ();

    if (!m_allocated)
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "Allocation of Direct2D resources failed." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Direct2D::~Direct2D ( void )
//{
//
//};


const bool& Direct2D::m_isInitialized ()
{
  return m_initialized;
};


void Direct2D::m_allocateResources ( void )
{
  try
  {

    m_allocated = false;
    HRESULT hR;

    // Direct2D device context creation
    // the option: distribute all the rendering process across multiple threads.
    hR = m_device->CreateDeviceContext ( D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_deviceContext );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of Direct2D device context failed!" );
      return;
    }

    // setting the render target of Direct2D to the same back buffer as Direct3D

    // --retrieving the DXGI version of the Direct3D back buffer (Direct2D needs)
    hR = m_core->m_getD3D ()->m_getSwapChain ()->GetBuffer ( 0, __uuidof(IDXGISurface1), &m_dcBuffer );
    //hR = dcBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dcBuffer );
    //auto rC = dcBuffer->Release ();
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Retrieving the back buffer needed for Direct2D failed!" );
      return;
    }

    // --bitmap (the actual rendering surface) properties (Direct2D needs)
    D2D1_BITMAP_PROPERTIES1 bitMap;
    bitMap.pixelFormat.format = m_core->m_getD3D ()->m_getBackBufferFormat (); // the same as Direct3D back buffer
    bitMap.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bitMap.dpiX = 0.0f; // dots per inch of the bitmap
    bitMap.dpiY = 0.0f;
    bitMap.bitmapOptions =
      D2D1_BITMAP_OPTIONS_TARGET | // usable for the device context target
      D2D1_BITMAP_OPTIONS_CANNOT_DRAW; // not for use as an input
    bitMap.colorContext = nullptr; // a colour context interface

    // --the actual creation of the render target (retrieve the back buffer and set)
    hR = m_deviceContext->CreateBitmapFromDxgiSurface ( m_dcBuffer.Get (), &bitMap, &m_dcBitmap );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of Direct2D bitmap from the DXGI surface failed!" );
      return;
    }

    // --finally set the bitmap as render target (the same back buffer as Direct3D)
    m_deviceContext->SetTarget ( m_dcBitmap.Get () );

    m_initializeTextFormats ();

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct2D::m_initializeTextFormats ( void )
{
  try
  {

    HRESULT hR;

    // creation of standard brushes
    hR = m_deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Yellow ), &m_brushYellow );

    if (SUCCEEDED ( hR ))
      hR = m_deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::White ), &m_brushWhite );

    if (SUCCEEDED ( hR ))
      hR = m_deviceContext->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Black ), &m_brushBlack );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of one or more brushes failed!" );
      return;
    }

    // text formats creation
    // second parameter: nullptr: use system font collection
    hR = m_writeFactory.Get ()->CreateTextFormat ( L"Lucida Console", nullptr,
                                                   DWRITE_FONT_WEIGHT_EXTRA_LIGHT, DWRITE_FONT_STYLE_NORMAL,
                                                   DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-GB", &m_textFormatFPS );

    if (SUCCEEDED ( hR ))
      hR = m_writeFactory.Get ()->CreateTextFormat ( L"Lucida Console", nullptr,
                                                     DWRITE_FONT_WEIGHT_EXTRA_LIGHT, DWRITE_FONT_STYLE_NORMAL,
                                                     DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-GB", &m_textFormatLogs );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of one or more text formats failed!" );
      return;
    }

    // text alignment
    hR = m_textFormatFPS->SetTextAlignment ( DWRITE_TEXT_ALIGNMENT_LEADING );
    if (SUCCEEDED ( hR ))
      hR = m_textFormatLogs->SetTextAlignment ( DWRITE_TEXT_ALIGNMENT_LEADING );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Alignment of one or more text formats failed!" );
      return;
    }

    // paragraph alignment
    hR = m_textFormatFPS->SetParagraphAlignment ( DWRITE_PARAGRAPH_ALIGNMENT_NEAR );
    if (SUCCEEDED ( hR ))
      hR = m_textFormatLogs->SetParagraphAlignment ( DWRITE_PARAGRAPH_ALIGNMENT_NEAR );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Alignment of one or more text paragraphs failed!" );
      return;
    }

    m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct2D::m_debugInfos ( void )
{
  try
  {

    if (m_core->m_debug /*&& (!core->appWindow->isResized ())*/ && m_textLayoutsDebug)
    {
      // drawing operations must be issued between a BeginDraw and EndDraw calls
      m_deviceContext->BeginDraw ();
      // drawing a fully formatted text
      if (m_textLayoutFPS)
        m_deviceContext->DrawTextLayout ( D2D1::Point2F ( 3.0f, 3.0f ), m_textLayoutFPS.Get (),
                                          m_brushYellow.Get (), D2D1_DRAW_TEXT_OPTIONS_NONE );
      if (m_textLayoutLogs)
        m_deviceContext->DrawTextLayout ( D2D1::Point2F ( 3.0f, 40.0f ), m_textLayoutLogs.Get (),
                                          m_brushYellow.Get (), D2D1_DRAW_TEXT_OPTIONS_NONE );
      if (FAILED ( m_deviceContext->EndDraw () ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Drawing the FPS information on screen failed!" );
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

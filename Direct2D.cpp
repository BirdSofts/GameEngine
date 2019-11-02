// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Direct2D.h"
#include "Shared.h"


Direct2D::Direct2D ( TheCore* coreObj ) :
  core ( coreObj ), textLayoutsDebug ( false ),
  initialized ( false ), allocated ( false )
{
  try
  {

    HRESULT hR;
    unsigned long rC { 0 }; // reference counts

    // creation of DirectWrite factory
    // properties options: --shared (reuse of cached font data, thus better performance), --isolated
    hR = DWriteCreateFactory ( DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFac );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of DirectWrite factory failed!" );
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
    hR = D2D1CreateFactory ( D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, &factory );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of Direct2D factory failed!" );
      return;
    }

    // acquiring the underlying DXGI factory used to create the Dirext3D device (resources needs)
    IDXGIDevice1* dxgiDevice;
    hR = core->d3d->device->QueryInterface ( __uuidof(IDXGIDevice1), ( void**) & dxgiDevice );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Acquiring the DXGI device failed!" );
      return;
    }

    // creation of the Direct2D device
    // threading mode will be inherited from the DXGI device.
    hR = factory->CreateDevice ( dxgiDevice, &device );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of Direct2D device failed!" );
      return;
    }
    rC = dxgiDevice->Release ();
    dxgiDevice = nullptr;

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"Direct2D is successfully initialized." );

    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Allocation of Direct2D resources failed." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct2D::allocateResources ( void )
{
  try
  {

    allocated = false;
    HRESULT hR;

    // Direct2D device context creation
    // the option: distribute all the rendering process across multiple threads.
    hR = device->CreateDeviceContext ( D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &deviceCon );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of Direct2D device context failed!" );
      return;
    }

    // setting the render target of Direct2D to the same back buffer as Direct3D

    // --retrieving the DXGI version of the Direct3D back buffer (Direct2D needs)
    hR = core->d3d->swapChain->GetBuffer ( 0, __uuidof(IDXGISurface1), &dcBuffer );
    //hR = dcBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dcBuffer );
    //auto rC = dcBuffer->Release ();
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Retrieving the back buffer needed for Direct2D failed!" );
      return;
    }

    // --bitmap (the actual rendering surface) properties (Direct2D needs)
    D2D1_BITMAP_PROPERTIES1 bitMap;
    bitMap.pixelFormat.format = core->d3d->colourFormat; // the same as Direct3D back buffer
    bitMap.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bitMap.dpiX = 0.0f; // dots per inch of the bitmap
    bitMap.dpiY = 0.0f;
    bitMap.bitmapOptions =
      D2D1_BITMAP_OPTIONS_TARGET | // usable for the device context target
      D2D1_BITMAP_OPTIONS_CANNOT_DRAW; // not for use as an input
    bitMap.colorContext = nullptr; // a colour context interface

    // --the actual creation of the render target (retrieve the back buffer and set)
    hR = deviceCon->CreateBitmapFromDxgiSurface ( dcBuffer.Get (), &bitMap, &dcBitmap );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of Direct2D bitmap from the DXGI surface failed!" );
      return;
    }

    // --finally set the bitmap as render target (the same back buffer as Direct3D)
    deviceCon->SetTarget ( dcBitmap.Get () );

    initializeTextFormats ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Direct2D::initializeTextFormats ( void )
{
  try
  {

    HRESULT hR;

    // creation of standard brushes
    hR = deviceCon->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Yellow ), &brushYellow );

    if (SUCCEEDED ( hR ))
      hR = deviceCon->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::White ), &brushWhite );

    if (SUCCEEDED ( hR ))
      hR = deviceCon->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Black ), &brushBlack );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of one or more brushes failed!" );
      return;
    }

    // text formats creation
    // second parameter: nullptr: use system font collection
    hR = writeFac.Get ()->CreateTextFormat ( L"Lucida Console", nullptr,
                                             DWRITE_FONT_WEIGHT_EXTRA_LIGHT, DWRITE_FONT_STYLE_NORMAL,
                                             DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-GB", &textFormatFPS );

    if (SUCCEEDED ( hR ))
      hR = writeFac.Get ()->CreateTextFormat ( L"Lucida Console", nullptr,
                                               DWRITE_FONT_WEIGHT_EXTRA_LIGHT, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-GB", &textFormatLogs );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of one or more text formats failed!" );
      return;
    }

    // text alignment
    hR = textFormatFPS->SetTextAlignment ( DWRITE_TEXT_ALIGNMENT_LEADING );
    if (SUCCEEDED ( hR ))
      hR = textFormatLogs->SetTextAlignment ( DWRITE_TEXT_ALIGNMENT_LEADING );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Alignment of one or more text formats failed!" );
      return;
    }

    // paragraph alignment
    hR = textFormatFPS->SetParagraphAlignment ( DWRITE_PARAGRAPH_ALIGNMENT_NEAR );
    if (SUCCEEDED ( hR ))
      hR = textFormatLogs->SetParagraphAlignment ( DWRITE_PARAGRAPH_ALIGNMENT_NEAR );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Alignment of one or more text paragraphs failed!" );
      return;
    }

    allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Direct2D::isInitialized ()
{
  return initialized;
};


void Direct2D::debugInfos ( void )
{
  try
  {

    if (core->debug && (!core->appWindow->isResized ()) && textLayoutsDebug)
    {
      // drawing operations must be issued between a BeginDraw and EndDraw calls
      deviceCon->BeginDraw ();
      // drawing a fully formatted text
      if (textLayoutFPS)
        deviceCon->DrawTextLayout ( D2D1::Point2F ( 3.0f, 3.0f ), textLayoutFPS.Get (),
                                    brushYellow.Get (), D2D1_DRAW_TEXT_OPTIONS_NONE );
      if (textLayoutLogs)
        deviceCon->DrawTextLayout ( D2D1::Point2F ( 3.0f, 40.0f ), textLayoutLogs.Get (),
                                    brushYellow.Get (), D2D1_DRAW_TEXT_OPTIONS_NONE );
      if (FAILED ( deviceCon->EndDraw () ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                  L"Drawing the FPS information on screen failed!" );
      }
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};

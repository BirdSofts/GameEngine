// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Core.h"
#include "Shared.h"


TheCore::TheCore ( ::IUnknown* window, const int& width, const int& height ) :
  m_timer ( nullptr ), m_FPS ( 0 ), m_milliSPF ( 0 ),
  m_appWindow ( window ), m_outputWidth ( width ), m_outputHeight ( height ),
  m_D3D ( nullptr ), m_D2D ( nullptr ),
  m_debug ( false ), m_initialized ( false )
{
  try
  {

    // timer instantiation
    m_timer = new (std::nothrow) Timer;
    if (!m_timer->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Timer initialization failed!" );
      return;
    }

    //// application window instantiation
    //appWindow = new (std::nothrow) Window ( this );
    //if (!appWindow->isInitialized ())
    //{
    //  PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
    //                                            "Window initialization failed!" );
    //  return;
    //}
    //appHandle = appWindow->getHandle (); // handle to the instantiated window

    // Direct3D 10 instantiation
    m_D3D = new (std::nothrow) Direct3D ( this );
    if (!m_D3D->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Direct3D initialization failed!" );
      return;
    }

    // Direct2D 10 instantiation
    m_D2D = new (std::nothrow) Direct2D ( this );
    if (!m_D2D->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Direct2D initialization failed!" );
      return;
    }

    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The framework is successfully initialized." );

    m_debug = true; // Todo must be switched from within the application

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//TheCore::~TheCore ( void )
//{
//
//};


const bool& TheCore::m_isInitialized ( void )
{
  return m_initialized;
};


const bool& TheCore::m_isDebugging ( void )
{
  return m_debug;
};


const ::IUnknown* TheCore::m_getWindow ( void )
{
  return m_appWindow;
};


Timer* TheCore::m_getTimer ( void )
{
  return m_timer;
};


Direct3D* TheCore::m_getD3D ( void )
{
  return m_D3D;
};


Direct2D* TheCore::m_getD2D ( void )
{
  return m_D2D;
};


const int& TheCore::m_getFPS ( void )
{
  return m_FPS;
};


void TheCore::m_frameStatistics ( void )
{
  try
  {

    HRESULT hR;

    // a static local variable retains its state between the calls:
    static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
    static double elapsed; // the elapsed time since the last call
    frameCounter++;

    if ((m_timer->m_getTotalTime () - elapsed) >= 1e0)
    {

      // frame calculations:
      m_FPS = frameCounter; // the number of counted frames in one second
      m_milliSPF = 1e3 / m_FPS; // average taken time by a frame in milliseconds

      if (m_debug && m_D2D)
      {
        m_D2D->m_textLayoutsDebug = false;
        //// results to window caption
        //std::wstring caption = L"The Game ^,^ - FPS: " + std::to_wstring ( fps ) +
        //  L" - mSPF: " + std::to_wstring ( mspf );
        //SetWindowTextW ( appHandle, caption.c_str () );

        // results to client window area
        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << "Resolution: " << m_D3D->m_displayMode.Width << " x " << m_D3D->m_displayMode.Height
          << " - Display mode #" << m_D3D->m_displayModeIndex + 1 << " of " << m_D3D->m_displayModesCount << " @ "
          << m_D3D->m_displayMode.RefreshRate.Numerator / m_D3D->m_displayMode.RefreshRate.Denominator << " Hz" << std::endl
          << "Display Adapter: " << m_D3D->m_videoCardDescription
          << " - Dedicated memory: " << m_D3D->m_videoCardMemory << "MB" << std::endl
          << "^_^ - FPS: " << m_FPS << L" - mSPF: " << m_milliSPF << std::endl;

        // before rendering a text to a bitmap: the creation of the text layout
        hR = m_D2D->m_writeFactory->CreateTextLayout ( outFPS.str ().c_str (), (UINT32) outFPS.str ().size (),
                                                       m_D2D->m_textFormatFPS.get (), (float) m_outputWidth,
                                                       (float) m_outputHeight, m_D2D->m_textLayoutFPS.put () );
        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "The Creation of text layout for FPS information failed!" );
          return;
        }

        std::wstring out { L"Last event: " };
        out += Converter::strConverter ( PointerProvider::getFileLogger ()->m_getLogRawStr () );
        hR = m_D2D->m_writeFactory->CreateTextLayout ( out.c_str (), (UINT32) (UINT32) out.size (),
                                                       m_D2D->m_textFormatLogs.get (), (float) m_outputWidth,
                                                       (float) m_outputHeight, m_D2D->m_textLayoutLogs.put () );
        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "The Creation of text layout for Logs failed!" );
          return;
        }
        m_D2D->m_textLayoutsDebug = true;

      }

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_setResolution ( const bool& prm, const int& width, const int& height )
{
  try
  {

    if (width == 0 && height == 0)
    {

      if (prm)
      {
        // highest supported resolution + fullscreen
        m_D3D->m_displayModeIndex = m_D3D->m_displayModesCount - 1;
        m_D3D->m_displayMode = m_D3D->m_displayModes [m_D3D->m_displayModeIndex];
      } else
      {
        // lowest supported resolution
        m_D3D->m_displayModeIndex = 0;
        m_D3D->m_displayMode = m_D3D->m_displayModes [m_D3D->m_displayModeIndex];
      }

    } else
    {

      // user enjoys windowed mode at arbitrary resolutions
      m_outputWidth = width;
      m_D3D->m_displayMode.Width = width;
      m_outputHeight = height;
      m_D3D->m_displayMode.Height = height;

      m_resizeResources ( false );

    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_resizeResources ( const bool& displayMode )
{
  try
  {

    if (m_initialized)
    {

      unsigned long rC { 0 };
      //HRESULT hR;

      // free game resources
      //if ()
      //{
      //}

      // free Direct2D resources
      if (m_D2D && !rC)
      {

        //rC = m_D2D->m_dcBitmap.Reset ();
        //rC = m_D2D->m_deviceContext.Reset ();
        //rC = m_D2D->m_dcBuffer.Reset ();
        //if (rC)
        //{
        //  rC = 0; // HACK debug
        //  PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
        //                                            "Problem while releasing one or more resources!" );
        //}

      }

      // free Direct3D resources
      if (m_D3D->m_depthSview && m_D3D->m_renderTview && !rC)
      {

        m_D3D->m_deviceContext->ClearState ();
        //rC = m_D3D->m_rasterizerState.Reset ();
        m_D3D->m_deviceContext->OMSetRenderTargets ( 0, nullptr, nullptr );
        //rC = m_D3D->m_depthSview.Reset ();
        //rC = m_D3D->m_depthSstate.Reset ();
        //rC = m_D3D->m_depthSbuffer.Reset ();
        //rC = m_D3D->m_renderTview.Reset ();
        //if (rC)
        //{
        //  rC = 0; // HACK debug
        //  PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
        //                                            "Problem while releasing one or more resources!" );
        //}

      }

      // reallocation procedures
      if (!rC)
      {

        if (displayMode)
        {
          m_D3D->m_setDisplayMode ();
        }

        m_D3D->m_allocation ();

        if (m_D2D)
        {
          m_D2D->m_allocateResources ();
        }

        //game->allocateResources ();

      } else
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Resources' deallocation failed!" );
      }

    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_onSuspending ( void )
{
  try
  {

    m_initialized = false;

    if (m_D2D)
    {
      m_D2D->m_onSuspending ();
    }

    if (m_D3D)
    {
      m_D3D->m_onSuspending ();
    }

    if (m_appWindow)
    {
      m_appWindow = nullptr;
    }

    // timer application destruction
    if (m_timer)
      delete m_timer;

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The Application Core is successfully suspended." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_validate ( void )
{
  m_D3D->m_validate ();
  m_D2D->m_validate ();
};


void TheCore::m_onDeviceLost ( void )
{

};

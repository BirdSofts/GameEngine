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


TheCore::TheCore ( MainPageTypes* mainPageTypes ) :
  m_mainPageTypes ( mainPageTypes ),
  m_timer ( nullptr ), m_FPS ( 0 ), m_milliSPF ( 0 ),
  m_D3D ( nullptr ), m_D2D ( nullptr ),
  m_debug ( false ), m_isResizing ( false ), m_initialized ( false )
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

    // Direct3D instantiation
    m_D3D = new (std::nothrow) Direct3D ( this );
    if (!m_D3D->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Direct3D initialization failed!" );
      return;
    }

    // Direct2D instantiation
    m_D2D = new (std::nothrow) Direct2D ( this );
    if (!m_D2D->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Direct2D initialization failed!" );
      return;
    }

    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Application framework is successfully initialized." );

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


void TheCore::m_setSwapChainPanel ( winrt::Windows::UI::Xaml::Controls::SwapChainPanel* swapChainPanel )
{
  try
  {

    HRESULT hR;

    m_swapChainPanel = swapChainPanel;

    winrt::Windows::Graphics::Display::DisplayInformation currrent =
      winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView ();

    winrt::com_ptr<ISwapChainPanelNative> panelNative;
    panelNative = m_swapChainPanel->as<ISwapChainPanelNative> ();
    hR = panelNative->SetSwapChain ( m_D3D->m_swapChain.get () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Acquiring backing native interface of swap chain panel failed!" );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
}


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
      m_D3D->m_displayMode.Width = width;
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

      m_isResizing = true;

      unsigned long rC { 0 };
      HRESULT hR;

      // free game resources
      //if ()
      //{
      //}

      // free Direct2D resources
      if (m_D2D && !rC)
      {

        rC = m_D2D->m_textFormatLogs->Release ();
        rC = m_D2D->m_textFormatFPS->Release ();
        rC = m_D2D->m_textFormatPointer->Release ();
        rC = m_D2D->m_brushBlack->Release ();
        rC = m_D2D->m_brushWhite->Release ();
        rC = m_D2D->m_brushYellow->Release ();
        rC = m_D2D->m_brushRed->Release ();
        m_D2D->m_deviceContext->SetTarget ( nullptr );
        rC = m_D2D->m_dcBitmap->Release ();
        rC = m_D2D->m_dcBuffer->Release ();

        m_D2D->m_textLayoutFPS.detach ();
        m_D2D->m_textLayoutLogs.detach ();
        m_D2D->m_textLayoutPointer.detach ();
        m_D2D->m_textFormatLogs.detach ();
        m_D2D->m_textFormatFPS.detach ();
        m_D2D->m_textFormatPointer.detach ();
        m_D2D->m_brushBlack.detach ();
        m_D2D->m_brushWhite.detach ();
        m_D2D->m_brushYellow.detach ();
        m_D2D->m_brushRed.detach ();
        m_D2D->m_dcBitmap.detach ();
        m_D2D->m_dcBuffer.detach ();
        m_D2D->m_deviceContext.detach ();
        if (rC)
        {
          rC = 0; // HACK debug
          PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                      "Problem while releasing one or more resources!" );
        }

      }

      // free Direct3D resources
      if (m_D3D->m_depthSview && m_D3D->m_renderTview && !rC)
      {

        m_D3D->m_deviceContext->ClearState ();
        rC = m_D3D->m_rasterizerState->Release ();
        m_D3D->m_deviceContext->OMSetRenderTargets ( 0, nullptr, nullptr );
        rC = m_D3D->m_depthSview->Release ();
        rC = m_D3D->m_depthSstate->Release ();
        rC = m_D3D->m_depthSbuffer->Release ();
        rC = m_D3D->m_renderTview->Release ();
        ID3D11RenderTargetView* nullViews [] = { nullptr };
        m_D3D->m_deviceContext->OMSetRenderTargets ( _countof ( nullViews ), nullViews, nullptr );
        m_D3D->m_rasterizerState.detach ();
        m_D3D->m_depthSbuffer.detach ();
        m_D3D->m_depthSstate.detach ();
        m_D3D->m_depthSview.detach ();
        m_D3D->m_renderTview.detach ();
        m_D3D->m_deviceContext->Flush ();
        if (rC)
        {
          rC = 0; // HACK debug
          PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                      "Problem while releasing one or more resources!" );
        }

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

        m_isResizing = false;

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

      if (m_debug && m_D2D && !m_isResizing)
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
        outFPS << "Resolution: " << m_mainPageTypes->m_getDisplay ()->panelWidthPixels << " x " << m_mainPageTypes->m_getDisplay ()->panelHeightPixels
          << " - Display mode #" << m_D3D->m_displayModeIndex + 1 << " of " << m_D3D->m_displayModesCount << " @ "
          << m_D3D->m_displayMode.RefreshRate.Numerator / m_D3D->m_displayMode.RefreshRate.Denominator << " Hz" << std::endl
          << "Display Adapter: " << m_D3D->m_videoCardDescription
          << " - Dedicated memory: " << m_D3D->m_videoCardMemory << "MB" << std::endl
          << "^_^ - FPS: " << m_FPS << L" - mSPF: " << m_milliSPF << std::endl;

        if (m_D2D->m_textLayoutFPS)
        {
          m_D2D->m_textLayoutFPS->Release ();
          m_D2D->m_textLayoutFPS.detach ();
        }

        // before rendering a text to a bitmap: the creation of the text layout
        hR = m_D2D->m_writeFactory->CreateTextLayout ( outFPS.str ().c_str (), (UINT32) outFPS.str ().size (),
                                                       m_D2D->m_textFormatFPS.get (), m_mainPageTypes->m_getDisplay ()->panelWidthDips,
                                                       m_mainPageTypes->m_getDisplay ()->panelHeightDips, m_D2D->m_textLayoutFPS.put () );
        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Creation of text layout for FPS information failed!" );
          return;
        }

        m_D2D->m_textLayoutsDebug = true;

      }

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }

    if (m_D2D->m_textLayoutLogs)
    {

      m_D2D->m_textLayoutLogs->Release ();
      m_D2D->m_textLayoutLogs.detach ();
    }

    std::wostringstream outLastlog;
    outLastlog << L"Last event: ";
    outLastlog << Converter::strConverter ( PointerProvider::getFileLogger ()->m_getLogRawStr () ) << std::endl;
    hR = m_D2D->m_writeFactory->CreateTextLayout ( outLastlog.str ().c_str (), (UINT32) (UINT32) outLastlog.str ().size (),
                                                   m_D2D->m_textFormatLogs.get (), m_mainPageTypes->m_getDisplay ()->panelWidthDips,
                                                   m_mainPageTypes->m_getDisplay ()->panelHeightDips, m_D2D->m_textLayoutLogs.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of text layout for Logs failed!" );
      return;
    }

    if (m_D2D->m_textLayoutPointer)
    {

      m_D2D->m_textLayoutPointer->Release ();
      m_D2D->m_textLayoutPointer.detach ();
    }

    std::wostringstream outPointer;
    outPointer << m_mainPageTypes->m_getPointer ().c_str () << std::endl;
    hR = m_D2D->m_writeFactory->CreateTextLayout ( outPointer.str ().c_str (), (UINT32) (UINT32) outPointer.str ().size (),
                                                   m_D2D->m_textFormatPointer.get (), m_mainPageTypes->m_getDisplay ()->panelWidthDips,
                                                   m_mainPageTypes->m_getDisplay ()->panelHeightDips, m_D2D->m_textLayoutPointer.put () );

    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of text layout for Logs failed!" );
      return;
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

    // timer application destruction
    if (m_timer)
      delete m_timer;

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Application Framework is successfully suspended." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_onDeviceLost ( void )
{

};


void TheCore::m_validate ( void )
{
  m_D3D->m_validate ();
  //m_D2D->m_validate ();
};

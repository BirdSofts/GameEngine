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
  m_mainPageTypes ( mainPageTypes ), m_timer ( nullptr ),
  m_D3D ( nullptr ), m_D2D ( nullptr ),
  m_deviceRestored ( true ), m_debug ( false ), m_isResizing ( false ), m_initialized ( false )
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
    if (!m_D3D->m_isReady ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "A test on Direct3D section failed!" );
      return;
    }

    // Direct2D instantiation
    m_D2D = new (std::nothrow) Direct2D ( this );
    if (!m_D2D->m_isReady ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "A test on Direct2D section failed!" );
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


void TheCore::m_registerDeviceNotify ( IDeviceNotify* deviceNotify )
{
  m_deviceNotify = deviceNotify;
};


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


void TheCore::m_updateDisplay ( const bool& setResolution )
{
  try
  {

    if (setResolution)
    {
      // highest supported resolution
      m_D3D->m_displayModeIndex = m_D3D->m_displayModesCount - 1;
      m_D3D->m_displayMode = m_D3D->m_displayModes [m_D3D->m_displayModeIndex];
    } else
    {
      // lowest supported resolution
      m_D3D->m_displayModeIndex = 0;
      m_D3D->m_displayMode = m_D3D->m_displayModes [m_D3D->m_displayModeIndex];
    }

    m_resizeResources ( false );


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

        m_D2D->m_allocated = false;

        rC = m_D2D->m_stateBlock->Release ();

        rC = m_D2D->m_textFormatLogs->Release ();
        rC = m_D2D->m_textFormatFPS->Release ();
        rC = m_D2D->m_textFormatPointer->Release ();
        rC = m_D2D->m_brushBlack->Release ();
        rC = m_D2D->m_brushWhite->Release ();
        rC = m_D2D->m_brushYellow->Release ();
        rC = m_D2D->m_brushRed->Release ();
        m_D2D->m_deviceContext->SetTarget ( nullptr );
        rC = m_D2D->m_deviceContextBitmap->Release ();
        rC = m_D2D->m_deviceContextBuffer->Release ();
        rC = m_D2D->m_deviceContext->Release ();

        m_D2D->m_stateBlock.detach ();

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
        m_D2D->m_deviceContextBitmap.detach ();
        m_D2D->m_deviceContextBuffer.detach ();
        m_D2D->m_deviceContext.detach ();
        if (rC)
        {
          rC = 0; // HACK debug
          PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                      "Releasing one or more Direct2D resources failed!" );
        }

      }


      // free Direct3D resources
      if (m_D3D->m_depthStencilView && m_D3D->m_renderTargetView && !rC)
      {

        m_D3D->m_allocated = false;

        m_D3D->m_deviceContext->ClearState ();
        rC = m_D3D->m_rasterizerState->Release ();
        m_D3D->m_deviceContext->OMSetRenderTargets ( 0, nullptr, nullptr );
        rC = m_D3D->m_depthStencilView->Release ();
        rC = m_D3D->m_depthStencilState->Release ();
        rC = m_D3D->m_depthStencilBuffer->Release ();
        rC = m_D3D->m_renderTargetView->Release ();
        ID3D11RenderTargetView* nullViews [] = { nullptr };
        m_D3D->m_deviceContext->OMSetRenderTargets ( _countof ( nullViews ), nullViews, nullptr );
        m_D3D->m_rasterizerState.detach ();
        m_D3D->m_depthStencilBuffer.detach ();
        m_D3D->m_depthStencilState.detach ();
        m_D3D->m_depthStencilView.detach ();
        m_D3D->m_renderTargetView.detach ();
        m_D3D->m_deviceContext->Flush ();
        if (rC)
        {
          rC = 0; // HACK debug
          PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                      "Releasing one or more Direct3D resources failed!" );
        }

      }

      // reallocation procedures

      if (displayMode)
      {
        m_D3D->m_setDisplayMode ();
      }

      if (m_D3D)
      {
        if (m_D3D->m_createDeviceResources ())
        {
          if (m_D2D)
          {
            if (m_D2D->m_createDeviceContextResources ())
              PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                          "Resources are successfully reallocated!" );
            //game->allocateResources ();

          }
        }

      }
      m_isResizing = false;

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
    unsigned long rC;

    if (m_debug && m_D2D && !m_isResizing)
    {

      if (m_timer->m_isNewFrame ())
      {

        m_timer->m_isNewFrame () = false;
        m_D2D->m_textLayoutsDebug = false;

        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << "Resolution: " << m_mainPageTypes->m_getDisplay ()->panelWidthPixels << " x " << m_mainPageTypes->m_getDisplay ()->panelHeightPixels
          << " - Display mode #" << m_D3D->m_displayModeIndex + 1 << " of " << m_D3D->m_displayModesCount << " @ "
          << m_D3D->m_displayMode.RefreshRate.Numerator / m_D3D->m_displayMode.RefreshRate.Denominator << " Hz" << std::endl
          << "Display Adapter: " << m_D3D->m_videoCardDescription
          << " - Dedicated memory: " << m_D3D->m_videoCardMemory << "MB" << std::endl
          << "^_^ - FPS: " << m_timer->m_getFPS () << L" - mSPF: " << m_timer->m_getMilliSPF () << std::endl;

        if (m_D2D->m_textLayoutFPS)
        {
          rC = m_D2D->m_textLayoutFPS->Release ();
          m_D2D->m_textLayoutFPS.detach ();
        }

        // before rendering a text to a bitmap: the creation of the text layout
        hR = m_D2D->m_directWriteFactory->CreateTextLayout ( outFPS.str ().c_str (), (UINT32) outFPS.str ().size (),
                                                             m_D2D->m_textFormatFPS.get (), m_mainPageTypes->m_getDisplay ()->panelWidthDips, 50.f,
                                                             reinterpret_cast<IDWriteTextLayout**>(m_D2D->m_textLayoutFPS.put ()) );
        if (SUCCEEDED ( hR ))
          hR = m_D2D->m_textLayoutFPS->GetMetrics ( &m_D2D->m_textMetricsFPS );
        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Creation of text layout for FPS information failed!" );
          return;
        }

        m_D2D->m_textLayoutsDebug = true;
      }

    }

    // last log entity text layout
    std::wostringstream outLastlog;
    outLastlog << L"Last event: ";
    outLastlog << Converter::strConverter ( PointerProvider::getFileLogger ()->m_getLogRawStr () ) << std::endl;

    if (m_D2D->m_textLayoutLogs)
    {

      rC = m_D2D->m_textLayoutLogs->Release ();
      m_D2D->m_textLayoutLogs.detach ();
    }

    hR = m_D2D->m_directWriteFactory->CreateTextLayout ( outLastlog.str ().c_str (), (UINT32) (UINT32) outLastlog.str ().size (),
                                                         m_D2D->m_textFormatLogs.get (), m_mainPageTypes->m_getDisplay ()->panelWidthDips, 50.f,
                                                         reinterpret_cast<IDWriteTextLayout**>(m_D2D->m_textLayoutLogs.put ()) );
    if (SUCCEEDED ( hR ))
      hR = m_D2D->m_textLayoutLogs->GetMetrics ( &m_D2D->m_textMetricsLogs );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of text layout for Logs failed!" );
      return;
    }

    // pointer position text layout
    std::wostringstream outPointer;
    outPointer << m_mainPageTypes->m_getPointer ().c_str () << std::endl;

    if (m_D2D->m_textLayoutPointer)
    {

      rC = m_D2D->m_textLayoutPointer->Release ();
      m_D2D->m_textLayoutPointer.detach ();
    }

    hR = m_D2D->m_directWriteFactory->CreateTextLayout ( outPointer.str ().c_str (), (UINT32) (UINT32) outPointer.str ().size (),
                                                         m_D2D->m_textFormatPointer.get (), 300.f, 30.f,
                                                         reinterpret_cast<IDWriteTextLayout**>(m_D2D->m_textLayoutPointer.put ()) );
    if (SUCCEEDED ( hR ))
      hR = m_D2D->m_textLayoutPointer->GetMetrics ( &m_D2D->m_textMetricsPointer );
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


void TheCore::m_release ( void )
{
  try
  {

    if (m_D2D)
    {
      m_D2D->m_release ();
    }

    if (m_D3D)
    {
      m_D3D->m_release ();
    }

    if (m_deviceRestored)
    {
      // timer application destruction
      if (m_timer)
      {
        delete m_timer;
        m_timer = nullptr;
      }

      m_initialized = false;

      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "Application Framework is successfully suspended." );
    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "main/gameThread",
                                                  "Application Framework is successfully released." );
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TheCore::m_onDeviceLost ( void )
{

  m_deviceRestored = false;

  m_deviceNotify->OnDeviceEvents ();

  m_release ();

  if (m_D3D->m_createResources ())
    if (m_D2D->m_createResources ())
      m_deviceRestored = true;

  m_deviceNotify->OnDeviceEvents ();

};


void TheCore::m_validate ( void )
{
  m_D3D->m_validate ();
  //m_D2D->m_validate ();
};

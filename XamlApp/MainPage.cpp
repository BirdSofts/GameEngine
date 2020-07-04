// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"


using namespace winrt;
using namespace Windows::UI::Xaml;


namespace winrt::GameEngine::implementation
{

  MainPage::MainPage () :
    m_appWindow ( nullptr ), m_visible ( false ), m_inResizeMove ( false ),
    m_core ( nullptr ), m_game ( nullptr ),
    m_inputLoop ( nullptr ), m_inputCore ( nullptr ),
    m_initialized ( false )
  {
    InitializeComponent ();

    // Register event handlers for page life cycle and keep a pointer to it.
    m_appWindow = Window::Current ().CoreWindow ();

    m_appWindow.get ().PointerCursor ( winrt::Windows::UI::Core::CoreCursor
                                       { winrt::Windows::UI::Core::CoreCursorType::Arrow, 0 } );

    m_appWindow.get ().VisibilityChanged ( { this, &MainPage::m_onVisibilityChanged } );

    auto currentDisplayInfos = Windows::Graphics::Display::DisplayInformation::GetForCurrentView ();
    currentDisplayInfos.DpiChanged ( { this, &MainPage::m_onDpiChanged } );

    currentDisplayInfos.OrientationChanged ( { this, &MainPage::m_onOrientationChanged } );

    Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated ( { this, &MainPage::m_onDisplayContentInvalidated } );

    // current display DPI
    m_display.Dpi = Windows::Graphics::Display::DisplayInformation::GetForCurrentView ().LogicalDpi ();

    // Todo 
    // window/panel size
    m_display.windowWidthDips = static_cast<float> (PointerProvider::getConfiguration ()->m_getSettings ().Width);
    m_display.windowHeightDips = static_cast<float> (PointerProvider::getConfiguration ()->m_getSettings ().Height);
    auto size = winrt::Windows::Foundation::Size ( m_display.windowWidthDips, m_display.windowHeightDips );
    m_display.panelWidthDips = swapchainpanel ().RenderSize ().Width;
    m_display.panelHeightDips = swapchainpanel ().RenderSize ().Height;
    m_display.updatePixels ();
    m_display.fullscreen = PointerProvider::getConfiguration ()->m_getSettings ().fullscreen;

    swapchainpanel ().CompositionScaleChanged ( { this, &MainPage::m_onCompositionScaleChanged } );

    swapchainpanel ().SizeChanged ( { this, &MainPage::m_onSwapChainPanelSizeChanged } );

    m_initialized = true;

    PointerProvider::getVariables ()->running = true;


    // application core instantiation
    m_core = new (std::nothrow) TheCore ( m_display.windowWidthPixels, m_display.windowHeightPixels );

    if (!m_core->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Initialization of application core failed!" );
    } else
    {

      // game instantiation
      m_game = std::make_unique<Game> ( m_core );

      if (!m_game->m_isReady ())
      {
        m_game->m_onSuspending (); // failure, shut the application down properly.

        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "The game initialization failed!" );

        //MessageBoxA ( NULL, "The Game functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );

        //failure = true;
      } else
      {
        PointerProvider::getVariables ()->currentState = "initialized";

        PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                    "Entering the game loop..." );
        // Todo 
        // reconstruction:
        // add window dependent concepts (note: pass to app core)
        //
        // set swap chain panel independently (adjusting the logical size)
        // run game on thread
        // if no errors adaptation of Microsoft template
      }

    }

  }

  //MainPage::~MainPage ()
  //{
  //  // tear down
  //}


  int32_t MainPage::MyProperty ()
  {
    throw hresult_not_implemented ();
  }

  void MainPage::MyProperty ( int32_t /* value */ )
  {
    throw hresult_not_implemented ();
  }


  MainPage::Display::Display ()
  {
    Dpi = 96.f;
    windowWidthDips = 0.0f;
    windowHeightDips = 0.0f;
    windowWidthPixels = 0;
    windowHeightPixels = 0;
    panelWidthDips = 0.0f;
    panelHeightDips = 0.0f;
    panelWidthPixels = 0;
    panelHeightPixels = 0;
    fullscreen = false;
  };
  void MainPage::Display::updatePixels ()
  {
    // to pixels ( dips * c_DPI / 96.f + 0.5f )
    windowWidthPixels = static_cast<int>(windowWidthDips * Dpi / 96.f + 0.5f);
    windowHeightPixels = static_cast<int>(windowHeightDips * Dpi / 96.f + 0.5f);
    panelWidthPixels = static_cast<int>(panelWidthDips * Dpi / 96.f + 0.5f);
    panelHeightPixels = static_cast<int>(panelHeightDips * Dpi / 96.f + 0.5f);
  };
  void MainPage::Display::updateDips ()
  {
    // to dips ( float (pixels) * 96.f / c_DPI )
    windowWidthDips = (static_cast<float> (windowWidthPixels) * 96.f / Dpi);
    windowHeightDips = (static_cast<float> (windowHeightPixels) * 96.f / Dpi);
    panelWidthDips = (static_cast<float> (panelWidthPixels) * 96.f / Dpi);
    panelHeightDips = (static_cast<float> (panelHeightPixels) * 96.f / Dpi);
  };



  void MainPage::m_onRendering ( Windows::Foundation::IInspectable const& sender,
                                 Windows::Foundation::IInspectable const& args )
  {

  };


  //void MainPage::m_onActivated ( Windows::UI::Core::CoreWindow const& sender,
  //                               Windows::UI::Core::WindowActivatedEventArgs const& args )
  //{
  //  Window::Current ().Activate ();
  //};


  //void MainPage::m_onFocused ( Windows::UI::Core::CoreWindow const& sender,
  //                             Windows::UI::Core::WindowActivatedEventArgs const& args )
  //{
  //  if (Window::Current ().Visible ())
  //  {
  //    m_game->m_isPaused () = false;
  //    m_game->m_getCore ()->m_getTimer ()->m_event ( "start" );
  //  } else
  //  {
  //    m_game->m_isPaused () = true;
  //    m_game->m_getCore ()->m_getTimer ()->m_event ( "pause" );
  //  }
  //};


  void MainPage::m_onWindowResized ( Windows::Foundation::IInspectable const& sender,
                                     Windows::UI::Core::WindowSizeChangedEventArgs const& args )
  {
    m_display.windowWidthDips = Window::Current ().Bounds ().Width;
    m_display.windowHeightDips = Window::Current ().Bounds ().Height;
    m_display.updatePixels ();

    // Todo: drag and drop resizing (resolution needs to be sent)
    //if (args.Handled () && m_initialized)
    //{
    //  m_game->m_getCore ()->m_setResolution ( false,
    //                                          m_resolution.m_currentWidth,
    //                                          m_resolution.m_currentHeight );
    //}
  };


  void MainPage::m_onVisibilityChanged ( Windows::Foundation::IInspectable const& sender,
                                         Windows::UI::Core::VisibilityChangedEventArgs const& args )
  {
    m_visible = Window::Current ().Visible ();

    //if (m_visible)
    //{
    //  m_game->m_isPaused () = false;
    //  m_game->m_getCore ()->m_getTimer ()->m_event ( "start" );
    //} else
    //{
    //  m_game->m_isPaused () = true;
    //  m_game->m_getCore ()->m_getTimer ()->m_event ( "pause" );
    //}
  };


  void MainPage::m_onDpiChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                                  Windows::Foundation::IInspectable const& args )
  {
    m_display.Dpi = sender.LogicalDpi ();
  };


  void MainPage::m_onOrientationChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                                          Windows::Foundation::IInspectable const& args )
  {

  };


  void MainPage::m_onDisplayContentInvalidated ( Windows::Graphics::Display::DisplayInformation const& sender,
                                                 Windows::Foundation::IInspectable const& args )
  {
    m_game->m_validate ();
  };


  void MainPage::m_onCompositionScaleChanged ( Windows::UI::Xaml::Controls::SwapChainPanel const& sender,
                                               Windows::Foundation::IInspectable const& args )
  {

  };


  void MainPage::m_onSwapChainPanelSizeChanged ( Windows::Foundation::IInspectable const& sender,
                                                 Windows::UI::Xaml::SizeChangedEventArgs const& e )
  {

  };


  void MainPage::m_onAcceleratorKeyActivated ( winrt::Windows::UI::Core::CoreDispatcher const& sender,
                                               winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& e )
  {

  };


  void MainPage::m_onPointerPressed ( Windows::Foundation::IInspectable const& sender,
                                      winrt::Windows::UI::Core::PointerEventArgs const& e )
  {

  };


  void MainPage::m_onPointerMoved ( Windows::Foundation::IInspectable const& sender,
                                    winrt::Windows::UI::Core::PointerEventArgs const& e )
  {

  };


  void MainPage::m_onPointerReleased ( Windows::Foundation::IInspectable const& sender,
                                       winrt::Windows::UI::Core::PointerEventArgs const& e )
  {

  };


  //void MainPage::ClickHandler ( IInspectable const& sender,
  //                              RoutedEventArgs const& e )
  //{

  //};


  void MainPage::AppBarButton_Click ( Windows::Foundation::IInspectable const& sender,
                                      Windows::UI::Xaml::RoutedEventArgs const& e )
  {

  };


  void MainPage::Button_Click ( winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e )
  {
    //textBlock ().Text ( L"test" );
    winrt::Windows::UI::Text::FontWeight a;
    a.Weight = 1;
    textBlock ().FontWeight ( a );
    m_game->m_getCore ()->m_setSwapChainPanel ( &swapchainpanel () );

    if (!m_game->m_run ())
    {
      //failure = true;
      PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                  "One or more errors occurred while running the game!" );
    }
  }
}


namespace winrt::GameEngine::factory_implementation
{

}

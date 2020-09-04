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
    m_pointerX ( 0.0f ), m_pointerY ( 0.0f ), m_isPointerPressed ( false ),
    m_initialized ( false ), m_allocated ( false )
  {

    InitializeComponent ();

    //m_test = L"Test"; // at initialization process, UI thread gets and sets this value on button

    // Register event handlers for page life cycle and keep a pointer to it.
    m_appWindow = Window::Current ().CoreWindow ();

    m_appWindow.get ().PointerCursor ( winrt::Windows::UI::Core::CoreCursor
                                       { winrt::Windows::UI::Core::CoreCursorType::Arrow, 0 } );

    // response to window size events
    m_appWindow.get ().SizeChanged ( { this, &MainPage::m_onWindowResized } );

#if defined (NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    try
    {
      m_appWindow.get ().ResizeStarted ( [this]( auto&&, auto&& ) { m_inResizeMove = true; } );
      m_appWindow.get ().ResizeCompleted ( [this]( auto&&, auto&& ) { m_inResizeMove = false; } );
    }
    catch (...) {}
#endif // requires Windows 10 Creators Update (10.0.15063) or later

    // response to window focus events
    m_appWindow.get ().Activated ( { this, &MainPage::m_onFocused } );

    // response to window visibility events
    m_appWindow.get ().VisibilityChanged ( { this, &MainPage::m_onVisibilityChanged } );

    //auto dispatcher = CoreWindow
    m_appWindow.get ().Dispatcher ().AcceleratorKeyActivated ( { this, &MainPage::m_onAcceleratorKeyActivated } );

    m_appWindow.get ().KeyDown ( { this, &MainPage::m_onKeyDown } );
    m_appWindow.get ().KeyUp ( { this, &MainPage::m_onKeyUp } );

    // response to window close events // because of suspension procedure, following event rarely occurs.
    //m_appWindow.get ().Closed ( [this]( auto&&, auto&& ) { PointerProvider::getVariables ()->running = false; } );

    // response to client display DPI events
    auto currentDisplayInfos = Windows::Graphics::Display::DisplayInformation::GetForCurrentView ();
    currentDisplayInfos.DpiChanged ( { this, &MainPage::m_onDpiChanged } );

    currentDisplayInfos.OrientationChanged ( { this, &MainPage::m_onOrientationChanged } );

    Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated ( { this, &MainPage::m_onDisplayContentInvalidated } );

    //// pointer movement on entire window (swap chain panel has independent input)
    //m_appWindow.get ().PointerMoved ( { this, &MainPage::m_onPointerMoved } );

    // current display properties
    m_types.m_getDisplay ()->Dpi = currentDisplayInfos.LogicalDpi ();
    m_types.m_getDisplay ()->orientationNative = currentDisplayInfos.NativeOrientation ();
    m_types.m_getDisplay ()->orientationCurrent = currentDisplayInfos.CurrentOrientation ();
    m_types.m_getDisplay ()->compositionScaleX = swapChainPanel ().CompositionScaleX ();
    m_types.m_getDisplay ()->compositionScaleY = swapChainPanel ().CompositionScaleY ();
    m_types.m_getDisplay ()->computeRotation ();


    // Todo 
    // user settings (load state)
    // resize the window to user settings
    m_types.m_getDisplay ()->windowWidthPixels = static_cast<float> (PointerProvider::getConfiguration ()->m_getSettings ().Width);
    m_types.m_getDisplay ()->windowHeightPixels = static_cast<float> (PointerProvider::getConfiguration ()->m_getSettings ().Height);
    m_types.m_getDisplay ()->updateDips ();
    auto size = winrt::Windows::Foundation::Size ( m_types.m_getDisplay ()->windowWidthDips, m_types.m_getDisplay ()->windowHeightDips );

    // run on full screen
    m_types.m_getDisplay ()->fullscreen = PointerProvider::getConfiguration ()->m_getSettings ().fullscreen;

    auto view = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView ();
    // game engine itself never needs first-run in full screen
    //if (m_types.m_getDisplay ()->fullscreen)
    //{
    //  if (view.IsFullScreen ())
    //    view.ExitFullScreenMode ();
    //  else
    //    view.TryEnterFullScreenMode ();
    //}

    view.PreferredLaunchWindowingMode ( winrt::Windows::UI::ViewManagement::ApplicationViewWindowingMode::PreferredLaunchViewSize );
    view.PreferredLaunchViewSize ( size );

    // minimum client window size
    size.Width = 640.0f; size.Height = 480.0f;
    view.SetPreferredMinSize ( size );

    // set and save
    view.FullScreenSystemOverlayMode ( winrt::Windows::UI::ViewManagement::FullScreenSystemOverlayMode::Minimal );
    view.TryResizeView ( view.PreferredLaunchViewSize () );

    swapChainPanel ().CompositionScaleChanged ( { this, &MainPage::m_onCompositionScaleChanged } );

    swapChainPanel ().SizeChanged ( { this, &MainPage::m_onSwapChainPanelSizeChanged } );

    m_initialized = true;

    // Todo 
          // reconstruction:
          // add window dependent concepts/types/properties (note: accessible from app cote)
          //
          // adaptation of Microsoft template

  };


  //MainPage::~MainPage ()
  //{
  //  // tear down
  //};


  int32_t MainPage::MyProperty ()
  {
    throw hresult_not_implemented ();
  };
  void MainPage::MyProperty ( int32_t /* value */ )
  {
    throw hresult_not_implemented ();
  };

  hstring MainPage::Test ()
  {
    return m_test;
  };
  void MainPage::Test ( hstring const& value )
  {
    if (m_test != value)
    {
      m_test = value;
      m_propertyChanged ( *this, Windows::UI::Xaml::Data::PropertyChangedEventArgs { L"Test" } );
    }
  };


  winrt::event_token MainPage::PropertyChanged ( Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler )
  {
    return m_propertyChanged.add ( handler );
  };


  void MainPage::PropertyChanged ( winrt::event_token const& token )
  {
    m_propertyChanged.remove ( token );
  };


  void MainPage::allocateResources ( void )
  {

    try
    {

      // current swap chain panel size
      m_types.m_getDisplay ()->panelWidthDips = swapChainPanel ().ActualSize ().x;
      m_types.m_getDisplay ()->panelHeightDips = swapChainPanel ().ActualSize ().y;
      m_types.m_getDisplay ()->updatePixels ();

      // application core instantiation
      m_core = new (std::nothrow) TheCore ( &m_types );

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
          m_game->m_release (); // failure, shut the application down properly.

          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "The Game's initialization failed!" );

          //MessageBoxA ( NULL, "The Game's functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );

          //failure = true;
        } else
        {
          PointerProvider::getVariables ()->currentState = "initialized";

          PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                      "Entering the game loop...\n" );
        }

      }



      // register swap chain panel to receive independent input events on pointer device
      auto workItemHandler =
        winrt::Windows::System::Threading::WorkItemHandler
        ( [this]( winrt::Windows::Foundation::IAsyncAction )
          {

            // procedure will raise pointer events for specified devices on eider thread it is created on
            m_inputCore = swapChainPanel ().CreateCoreIndependentInputSource (
              winrt::Windows::UI::Core::CoreInputDeviceTypes::Mouse |
              winrt::Windows::UI::Core::CoreInputDeviceTypes::Touch |
              winrt::Windows::UI::Core::CoreInputDeviceTypes::Pen
            );

            // pointer events registration, raised on background thread
            m_inputCore.PointerPressed ( winrt::Windows::Foundation::TypedEventHandler
                                         <Windows::Foundation::IInspectable, winrt::Windows::UI::Core::PointerEventArgs>
                                         ( this, &MainPage::m_onPointerPressed ) );
            m_inputCore.PointerMoved ( winrt::Windows::Foundation::TypedEventHandler
                                       <Windows::Foundation::IInspectable, winrt::Windows::UI::Core::PointerEventArgs>
                                       ( this, &MainPage::m_onPointerMoved ) ); // pointer movement on swap chain panel
            m_inputCore.PointerReleased ( winrt::Windows::Foundation::TypedEventHandler
                                          <Windows::Foundation::IInspectable, winrt::Windows::UI::Core::PointerEventArgs>
                                          ( this, &MainPage::m_onPointerReleased ) );

            // process input messages upon their deliverances
            m_inputCore.Dispatcher ().ProcessEvents ( Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit );

          } );
      // run the task on a high priority background thread
      m_inputLoop = winrt::Windows::System::Threading::ThreadPool::RunAsync (
        workItemHandler,
        winrt::Windows::System::Threading::WorkItemPriority::High,
        winrt::Windows::System::Threading::WorkItemOptions::TimeSliced );


      m_game->m_getCore ()->m_setSwapChainPanel ( &swapChainPanel () );

      winrt::Windows::System::Threading::ThreadPool::RunAsync (
        winrt::Windows::System::Threading::WorkItemHandler
        ( [this]( winrt::Windows::Foundation::IAsyncAction )
          {

            if (!m_game->m_run ())
            {
              //failure = true;
              PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "gameThread",
                                                          "One or more errors occurred while running the game!" );
            }

          } ),
        winrt::Windows::System::Threading::WorkItemPriority::High,
            winrt::Windows::System::Threading::WorkItemOptions::TimeSliced );



      PointerProvider::getVariables ()->running = true;

      m_allocated = true;

    }
    catch (const std::exception& ex)
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  ex.what () );
    }

  };


  void MainPage::m_onRendering ( Windows::Foundation::IInspectable const& sender,
                                 Windows::Foundation::IInspectable const& args )
  {
    //
  };


  void MainPage::m_onActivated ( Windows::UI::Core::CoreWindow const& sender,
                                 Windows::UI::Core::WindowActivatedEventArgs const& args )
  {
    Window::Current ().Activate ();
  };


  void MainPage::m_onFocused ( Windows::UI::Core::CoreWindow const& sender,
                               Windows::UI::Core::WindowActivatedEventArgs const& args )
  {

    if (!m_allocated)
      allocateResources ();

    if (m_core != nullptr) ///
    {
      //if (sender.ActivationMode () == Windows::UI::Core::CoreWindowActivationMode::ActivatedInForeground)
      //{
      //  m_game->m_isPaused () = false;
      //  m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
      //} else
      //{
      //  m_game->m_isPaused () = true;
      //  m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      //}
    }
  };


  void MainPage::m_onWindowResized ( Windows::Foundation::IInspectable const& sender,
                                     Windows::UI::Core::WindowSizeChangedEventArgs const& args )
  {

    m_types.m_getDisplay ()->windowWidthDips = args.Size ().Width;
    m_types.m_getDisplay ()->windowHeightDips = args.Size ().Height;
    m_types.m_getDisplay ()->updatePixels ();

  };


  void MainPage::m_onVisibilityChanged ( Windows::Foundation::IInspectable const& sender,
                                         Windows::UI::Core::VisibilityChangedEventArgs const& args )
  {
    m_visible = args.Visible ();

    if (m_visible)
    {

      if (m_allocated)
      {
        m_game->m_isPaused () = false;
        m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
      }


    } else
    {

      if (m_allocated)
      {
        m_game->m_isPaused () = true;
        m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      }


    }
  };


  void MainPage::m_onDpiChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                                  Windows::Foundation::IInspectable const& args )
  {
    //concurrency::critical_section::scoped_lock lock ( m_core->getCriticalSection () );
    // note that below acquired Dpi may not match the app's effective Dpi, when it is scaled for high resolution devices
    // therefore the effective one is retrievable upon logical one being set on device resources

    m_types.m_getDisplay ()->Dpi = sender.LogicalDpi ();
    m_types.m_getDisplay ()->updateDips ();


    if (m_initialized && m_game != nullptr)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

      m_core->m_updateDisplay ();
      m_game->m_updateDisplay ();

      m_game->m_isPaused () = false;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
    }

  };


  void MainPage::m_onOrientationChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                                          Windows::Foundation::IInspectable const& args )
  {
    //concurrency::critical_section::scoped_lock lock ( m_core->getCriticalSection () );

    m_types.m_getDisplay ()->orientationCurrent = sender.CurrentOrientation ();
    m_types.m_getDisplay ()->computeRotation ();

    if (m_initialized && m_game != nullptr)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

      m_core->m_updateDisplay ();
      m_game->m_updateDisplay ();

      m_game->m_isPaused () = false;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
    }

  };


  void MainPage::m_onDisplayContentInvalidated ( Windows::Graphics::Display::DisplayInformation const& sender,
                                                 Windows::Foundation::IInspectable const& args )
  {
    //concurrency::critical_section::scoped_lock lock ( m_core->getCriticalSection () );

    if (m_initialized && m_game != nullptr)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

      m_core->m_validate ();

      m_game->m_isPaused () = false;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );

    }
  };


  void MainPage::m_onCompositionScaleChanged ( Windows::UI::Xaml::Controls::SwapChainPanel const& sender,
                                               Windows::Foundation::IInspectable const& args )
  {

    m_types.m_getDisplay ()->compositionScaleX = sender.CompositionScaleX ();
    m_types.m_getDisplay ()->compositionScaleY = sender.CompositionScaleY ();

    if (m_initialized && m_game != nullptr)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

      m_core->m_updateDisplay ();
      m_game->m_updateDisplay ();

      m_game->m_isPaused () = false;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );

    }

  };


  void MainPage::m_onSwapChainPanelSizeChanged ( Windows::Foundation::IInspectable const& sender,
                                                 Windows::UI::Xaml::SizeChangedEventArgs const& e )
  {
    //concurrency::critical_section::scoped_lock lock ( m_core->getCriticalSection () );

    m_types.m_getDisplay ()->panelWidthDips = e.NewSize ().Width;
    m_types.m_getDisplay ()->panelHeightDips = e.NewSize ().Height;
    m_types.m_getDisplay ()->updatePixels ();

    // Todo: drag and drop resizing (resolution needs to be sent)
    if (m_initialized && m_game != nullptr)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

      m_core->m_updateDisplay ();
      m_game->m_updateDisplay ();

      PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                  "Main page got resized. Current resolution: " +
                                                  std::to_string ( m_types.m_getDisplay ()->panelWidthPixels ) + " x " +
                                                  std::to_string ( m_types.m_getDisplay ()->panelHeightPixels ) );

      m_game->m_isPaused () = false;
      m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
    }
  };


  void MainPage::m_onAcceleratorKeyActivated ( winrt::Windows::UI::Core::CoreDispatcher const& sender,
                                               winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& e )
  {

    auto eventType = e.EventType ();
    auto virtualKey = e.VirtualKey ();


    // ALT+Enter: fullscreen + highest/lowest resolution switch
    if (eventType == winrt::Windows::UI::Core::CoreAcceleratorKeyEventType::SystemKeyDown)
    {
      auto alt = m_appWindow.get ().GetAsyncKeyState ( winrt::Windows::System::VirtualKey::RightMenu );
      if (alt == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down
           && virtualKey == winrt::Windows::System::VirtualKey::Enter)
      {
        auto view = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView ();

        winrt::Windows::UI::Xaml::Thickness gridPadding { 0 };
        winrt::Windows::UI::Xaml::GridLength gridSize { 0 };

        if (view.IsFullScreen ())
        {
          gridPadding = { 0, 20, 0, 0 };
          gridSize.GridUnitType = winrt::Windows::UI::Xaml::GridUnitType::Auto;
          bottomAppBar ().Visibility ( winrt::Windows::UI::Xaml::Visibility::Visible );
          view.ExitFullScreenMode ();

          if (m_initialized && m_game != nullptr)
          {
            m_game->m_isPaused () = true;
            m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
            std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

            m_core->m_updateDisplay ();
            m_game->m_updateDisplay ();

            m_game->m_isPaused () = false;
            m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
          }
        } else
        {
          gridSize.GridUnitType = winrt::Windows::UI::Xaml::GridUnitType::Pixel;
          bottomAppBar ().Visibility ( winrt::Windows::UI::Xaml::Visibility::Collapsed );
          view.TryEnterFullScreenMode ();

          if (m_initialized && m_game != nullptr)
          {
            m_game->m_isPaused () = true;
            m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
            std::this_thread::sleep_for ( std::chrono::milliseconds ( 40 ) );

            m_core->m_updateDisplay ();
            m_game->m_updateDisplay ();

            m_game->m_isPaused () = false;
            m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
          }
        }
        gridLayout ().Padding ( gridPadding );
        gridRow1 ().Height ( gridSize );
        gridColumn1 ().Width ( gridSize );
        gridColumn3 ().Width ( gridSize );

        e.Handled ( true );
      }
    }


    if (eventType == winrt::Windows::UI::Core::CoreAcceleratorKeyEventType::KeyDown)
    {

      // Todo prevent frame losses

      // key A: go left
      if (virtualKey == winrt::Windows::System::VirtualKey::A)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goLeftRight ( -0.1f );
      }

      // key D: go right
      if (virtualKey == winrt::Windows::System::VirtualKey::D)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goLeftRight ( 0.1f );
      }

      // page up: go up
      if (virtualKey == winrt::Windows::System::VirtualKey::PageUp)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goUpDown ( 0.1f );
      }

      // page down: go down
      if (virtualKey == winrt::Windows::System::VirtualKey::PageDown)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goUpDown ( -0.1f );
      }

      // key W: go forward
      if (virtualKey == winrt::Windows::System::VirtualKey::W)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goForwardBackward ( 0.1f );
      }

      // key S: go backward
      if (virtualKey == winrt::Windows::System::VirtualKey::S)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_goForwardBackward ( -0.1f );
      }

      // key E: go forward
      if (virtualKey == winrt::Windows::System::VirtualKey::E)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_lookSidedLeftRight ( 0.5f );
      }

      // key Q: go backward
      if (virtualKey == winrt::Windows::System::VirtualKey::Q)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_lookSidedLeftRight ( -0.5f );
      }

      // Escape: exit
      if (virtualKey == winrt::Windows::System::VirtualKey::Escape)
      {
        m_game->m_isPaused () = true;
        m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::pause );
        //if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
        //{
        releaseResources ();
        winrt::Windows::UI::Xaml::Application::Current ().Exit ();
        //m_appWindow.get ().Close (); //
        //} else
        //{
        //  m_game->m_getCore ()->m_getTimer ()->m_event ( typeEvent::start );
        //}
      }
    }

  };


  void MainPage::m_onKeyDown ( winrt::Windows::UI::Core::CoreWindow const& sender,
                               winrt::Windows::UI::Core::KeyEventArgs const& e )
  {
    if (true)
    {

    }
  };


  void MainPage::m_onKeyUp ( winrt::Windows::UI::Core::CoreWindow const& sender,
                             winrt::Windows::UI::Core::KeyEventArgs const& e )
  {
    if (true)
    {

    }
  };


  void MainPage::m_onPointerPressed ( Windows::Foundation::IInspectable const& sender,
                                      winrt::Windows::UI::Core::PointerEventArgs const& e )
  {
    // Todo sample: start pointer movement tracking
    // add interactions caused by courser

    m_isPointerPressed = true;

  };


  void MainPage::m_onPointerMoved ( Windows::Foundation::IInspectable const& sender,
                                    winrt::Windows::UI::Core::PointerEventArgs const& e )
  {
    // Todo 
    // add interactions caused by courser
    //trackingUpdate(e.CurrentPoint().Position().X)

    float currentX { e.CurrentPoint ().Position ().X };
    float currentY { e.CurrentPoint ().Position ().Y };

    if ((m_pointerX == 0.0f) && (m_pointerY == 0.0f))
    {
      m_pointerX = currentX;
      m_pointerY = currentY;
    }

    std::string temp { "" };
    temp += "Pointer: " + std::to_string ( currentX );
    temp += ", ";
    temp += std::to_string ( currentY );

    m_types.m_getPointer () = (Converter::strConverter ( temp ));

    winrt::Windows::UI::Core::DispatchedHandler task ( [this]()
                                                       {
                                                         textBlock1 ().Text ( m_types.m_getPointer () );
                                                         //Test ( winrt::to_hstring ( m_types.m_getPointer () ) );
                                                       } );
    textBlock1 ().Dispatcher ().RunAsync ( winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, task );

    if (m_isPointerPressed == true)
    {

      // Undone sided look (z) must be corrected (between -10 und plus 10)

      if (currentX < m_pointerX)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_lookLeftRight ( -static_cast<long>(m_pointerX - currentX) % 3 );
        m_pointerX = currentX;
      } else
        if (currentX > m_pointerX)
        {
          m_game->m_getUniverse ()->m_getCamera ()->m_lookLeftRight ( static_cast<long>(currentX - m_pointerX) % 3 );
          m_pointerX = currentX;
        }

      if (currentY < m_pointerY)
      {
        m_game->m_getUniverse ()->m_getCamera ()->m_lookUpDown ( -static_cast<long>(m_pointerY - currentY) % 3 );
        m_pointerY = currentY;
      } else
        if (currentY > m_pointerY)
        {
          m_game->m_getUniverse ()->m_getCamera ()->m_lookUpDown ( static_cast<long>(currentY - m_pointerY) % 3 );
          m_pointerY = currentY;

        }
    }

  };


  void MainPage::m_onPointerReleased ( Windows::Foundation::IInspectable const& sender,
                                       winrt::Windows::UI::Core::PointerEventArgs const& e )
  {
    // Todo 
    // add interactions caused by courser

    m_isPointerPressed = false;

  };


  void MainPage::SaveInternalState ( winrt::Windows::Foundation::Collections::IPropertySet& state )
  {
    //concurrency::critical_section::scoped_lock lock ( m_core->getCriticalSection () );
    //m_core->m_getD3D ()->Trim ();

    // stopRenderLoop();

    // save app state

    //m_inputCore->Dispatcher ().StopProcessEvents ();

    // release allocated recources
    if (m_game)
    {
      m_game->m_release ();
      m_game.release ();
    }

    if (m_core)
    {
      m_core->m_release ();
      delete m_core;
      m_core = nullptr;
    }

  };


  void MainPage::LoadInternalState ( winrt::Windows::Foundation::Collections::IPropertySet& state )
  {
    // load app state

    // startRenderLoop();
  };


  void MainPage::releaseResources ( void )
  {
    PointerProvider::getVariables ()->currentState = "suspending";

    PointerProvider::getVariables ()->running = false;

    std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );

    // Save application state and stop any background activity
    SaveInternalState ( winrt::Windows::Storage::ApplicationData::Current ().LocalSettings ().Values () );

    PointerProvider::getVariables ()->currentState = "uninitialized";

    if (PointerProvider::getException ())
      PointerProvider::providerException ( nullptr );

    if (PointerProvider::getConfiguration ())
      PointerProvider::providerConfiguration ( nullptr );

    if (PointerProvider::getFileLogger ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "Logger engine is set to shut down..." );

      // failure or success, the logs are somehow to be saved, so give its thread some time
      std::this_thread::sleep_for ( std::chrono::milliseconds { 1000 } );

      PointerProvider::getFileLogger ()->m_shutdown ();
      PointerProvider::providerFileLogger ( nullptr );
    }

    if (PointerProvider::getVariables ())
      PointerProvider::providerVariables ( nullptr );
  };


  //void MainPage::ClickHandler ( IInspectable const& sender,
  //                              RoutedEventArgs const& e )
  //{

  //};


  void MainPage::AppBarButton_Click ( Windows::Foundation::IInspectable const& sender,
                                      Windows::UI::Xaml::RoutedEventArgs const& e )
  {
    // Use the app bar if it is appropriate for your app. Design the app bar, 
    // then fill in event handlers (like this one).
  };


  void MainPage::Button_Click ( winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e )
  {

    winrt::Windows::UI::Text::FontWeight a;
    a.Weight = 1;
    textBlock2 ().FontWeight ( a );

  }
};


namespace winrt::GameEngine::factory_implementation
{

};

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,13.11.2019</changed>
// ********************************************************************************

#include "pch.h"
//#include "GameEngine.h"
#include "common/Utilities.h" // string + s,f streams + exception + threads + list + Windows standards
#include "Shared.h"
#include "Game.h"


using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;


class View : public winrt::implements<View, IFrameworkViewSource, IFrameworkView>
{
private:
  static bool failure; // true if exiting with failure

  winrt::agile_ref<CoreWindow> m_appWindow; // reference to application window
  bool m_visible; // application window visibility state
  float m_Dpi; // current client window DPI
  float m_clientWidth; // current client window width
  float m_clientHeight; // current client window height
  bool m_inResizeMove; // true if client window is set to resize and/or move
  bool m_fullscreen; // current full screen state (additionally fed from configuration)
  // Orientation

  std::unique_ptr <Game> m_game; // unique pointer to game wrapper
  //Game* m_game; // pointer to game wrapper

  bool m_initialized; // true in case of successful initialization
protected:
  void m_onActivated ( CoreApplicationView const& /*applicationView*/,
                       IActivatedEventArgs const& /*args*/ ); // on application window activation

  void m_onFocused ( CoreWindow const& /*sender*/,
                     WindowActivatedEventArgs const& /*args*/ ); // on application window focused/unfocused

  void m_onVisibilityChanged ( CoreWindow const& /*sender*/,
                               VisibilityChangedEventArgs const& /*args*/ ); // on application window resize

  void m_onWindowResized ( CoreWindow const& /*sender*/,
                           WindowSizeChangedEventArgs const& /*args*/ ); // on application window resize

  void m_onDisplayContentInvalidated ( DisplayInformation const& /*sender*/,
                                       IInspectable const& /*args*/ ); // on application window content invalidation

  void m_onDpiChanged ( DisplayInformation const& /*sender*/,
                        IInspectable const& /*args*/ ); // on client display DPI changed

  void m_onSuspending ( IInspectable const& /*sender*/,
                        SuspendingEventArgs const& /*args*/ ); // on application window entering the suspended state

  void m_onResuming ( IInspectable const& /*sender*/,
                      IInspectable const& /*args*/ ); // on application window resuming from suspended state

  void m_onAcceleratorKeyActivated ( CoreDispatcher const& /*sender*/,
                                     AcceleratorKeyEventArgs const& /*args*/ ); // on accelerator key pressed

  void m_suspend (); // release the application resources

  //void m_on Orientation Changed
  //void m_on Display Content Invalidated
  //
public:
  View ( void );
  //~View ( void );

  // implementation documentation:
  // for being able to implement the object which implements IFrameworkView interface,
  // it should made ready by the IFrameworkViewSource factory interface
  // the order in which these methods are called (after defined by a CoreApplication singleton instance):
  // CreateView, Initialize, SetWindow, Load, Run

  // IFrameworkViewSource methods:
  IFrameworkView CreateView ( void ); // creation method overload

  // IFrameworkView methods:
  void Initialize ( CoreApplicationView const& ); // initialization method overload (called on app launch)
  void Load ( winrt::hstring const& /*entryPoint*/ ); // load method overload
  void Run ( void ); // execution method overload (called after load method)
  void SetWindow ( CoreWindow const& ); // window set method overload (after initialization)
  void Uninitialize ( void ); // un-initialization method overload

  static bool exitedWith ( void ); // application exit state provider
};


View::View ( void ) :
  m_appWindow ( nullptr ), m_visible ( false ),
  m_inResizeMove ( false ), m_fullscreen ( false ),
  m_game ( nullptr ), m_initialized ( false )
{

  m_Dpi = 0.0f;
  m_clientWidth = 0.0f;
  m_clientHeight = 0.0f;

};


//View::~View ( void )
//{
//
//};


int WINAPI wWinMain ( _In_ HINSTANCE /*hInstance*/,
                      _In_ HINSTANCE /*hPrevInstance*/,
                      _In_ LPWSTR /*LpCmdLine*/,
                      _In_ int /*nCmdShow*/ )
{

  try
  {

    std::shared_ptr<Variables> areGlobal { new (std::nothrow) Variables () };
    PointerProvider::providerVariables ( areGlobal );

    std::shared_ptr<TheException> anException { new (std::nothrow) TheException () };
    PointerProvider::providerException ( anException );

    std::shared_ptr<Logger<ToFile>> fileLoggerEngine ( new (std::nothrow) Logger<ToFile> () );
    PointerProvider::providerFileLogger ( fileLoggerEngine );

    std::shared_ptr<Configurations> settings ( new (std::nothrow) Configurations () );
    PointerProvider::providerConfiguration ( settings );

    if ((!anException) && (!settings))
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::providerException ( nullptr );
        anException.reset ();
      }
      if (settings)
      {
        PointerProvider::providerConfiguration ( nullptr );
        settings.reset ();
      }

      PointerProvider::getVariables ()->currentState = "services";
      throw;
    }

    if (fileLoggerEngine)
    {
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "Exception, file logger and configuration providers are successfully initialized." );
    } else
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::providerException ( nullptr );
        anException.reset ();
      }
      if (settings)
      {
        PointerProvider::providerConfiguration ( nullptr );
        settings.reset ();
      }

      PointerProvider::getVariables ()->currentState = "appDebug";
      throw;
    }


    if (!DirectX::XMVerifyCPUSupport ())
    {
      PointerProvider::getVariables ()->currentState = "CPU";
      throw;
    }


    //auto view = winrt::make<View> ();
    //CoreApplication::Run ( view );
    CoreApplication::Run ( View () );

    if (View::exitedWith ())
    {
      return EXIT_FAILURE;
    } else
    {
      return EXIT_SUCCESS;
    }

  }
  catch (const std::exception & ex)
  {

    if (PointerProvider::getVariables ()->currentState == "CPU")
    {
      //MessageBoxA ( NULL, "Your CPU isn't supported.", "Error", MB_OK | MB_ICONERROR );
    } else
      if (PointerProvider::getVariables ()->currentState == "services")
      {
        //MessageBoxA ( NULL, "The Game could not be started...", "Error", MB_OK | MB_ICONERROR );
      } else
        if (PointerProvider::getVariables ()->currentState == "appDebug")
        {
          //MessageBoxA ( NULL, "The debug service failed to start.", "Error", MB_OK | MB_ICONERROR );
        } else
        {
          //MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
          if (PointerProvider::getFileLogger ())
          {
            PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                        ex.what () );

            // failure or success, the logs are somehow to be saved, so give its thread some time
            std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
          }
        }
        return EXIT_FAILURE;

  }

}


void View::m_onActivated ( CoreApplicationView const& /*applicationView*/, IActivatedEventArgs const& /*args*/ )
{
  // visible the activated application window and enables it to receive events
  CoreWindow::GetForCurrentThread ().Activate ();
};


void View::m_onFocused ( CoreWindow const& /*sender*/, WindowActivatedEventArgs const& /*args*/ )
{

  if (m_appWindow.get ().ActivationMode () == CoreWindowActivationMode::ActivatedInForeground)
  {
    m_game->m_isPaused () = false;
    m_game->m_getCore ()->m_getTimer ()->m_event ( "start" );
  } else
  {
    m_game->m_isPaused () = true;
    m_game->m_getCore ()->m_getTimer ()->m_event ( "pause" );
  }

};


void View::m_onVisibilityChanged ( CoreWindow const& /*sender*/, VisibilityChangedEventArgs const& args )
{
  m_visible = args.Visible (); ///
  m_game->m_isPaused () = !args.Visible ();
};


void View::m_onWindowResized ( CoreWindow const& /*sender*/, WindowSizeChangedEventArgs const& args )
{

  m_clientWidth = m_appWindow.get ().Bounds ().Width;
  m_clientHeight = m_appWindow.get ().Bounds ().Height;

  // Todo: drag and drop resizing (resolution needs to be sent)
  if (args.Handled () && m_initialized)
  {
    m_game->m_getCore ()->m_setResolution ( false, static_cast<int>(m_clientWidth), static_cast<int>(m_clientHeight) );
  }

};


void View::m_onDisplayContentInvalidated ( DisplayInformation const& /*sender*/, IInspectable const& /*args*/ )
{
  m_game->m_validate ();
};


void View::m_onDpiChanged ( DisplayInformation const& sender, IInspectable const& /*args*/ )
{
  m_Dpi = sender.LogicalDpi ();
};


void View::m_onSuspending ( IInspectable const& /*sender*/, SuspendingEventArgs const& args )
{

  auto deferral = args.SuspendingOperation ().GetDeferral ();
  auto task = std::async (
    std::launch::async, [this, deferral]()
    {
      m_suspend ();
      deferral.Complete ();
    }
  );

};


void View::m_onResuming ( IInspectable const& /*sender*/, IInspectable const& /*args*/ )
{
  //
};


void View::m_onAcceleratorKeyActivated ( CoreDispatcher const& /*sender*/, AcceleratorKeyEventArgs const& args )
{

  // ALT+Enter: fullscreen + highest/lowest resolution switch
  if (args.EventType () == CoreAcceleratorKeyEventType::SystemKeyDown
       && args.VirtualKey () == VirtualKey::Enter
       && args.KeyStatus ().IsMenuKeyDown)
  {
    auto view = ApplicationView::GetForCurrentView ();

    if (view.IsFullScreen ())
    {
      view.ExitFullScreenMode ();
      m_game->m_getCore ()->m_setResolution ( false );
    } else
    {
      view.TryEnterFullScreenMode ();
      m_game->m_getCore ()->m_setResolution ( true );
    }

    args.Handled ( true );
  }


  // Todo: DirectInput
  if (args.EventType () == CoreAcceleratorKeyEventType::KeyDown)
  {

    // Escape: up
    if (args.VirtualKey () == VirtualKey::Up)
    {
      m_game->m_getUniverse ()->m_getCamera ()->forwardBackward ( 0.05f );
    }

    // Escape: down
    if (args.VirtualKey () == VirtualKey::Down)
    {
      m_game->m_getUniverse ()->m_getCamera ()->forwardBackward ( -0.05f );
    }

    // Escape: exit
    if (args.VirtualKey () == VirtualKey::Escape)
    {
      m_game->m_isPaused () = true;
      m_game->m_getCore ()->m_getTimer ()->m_event ( "pause" );
      //if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
      //{
      CoreApplication::Exit (); ///
      //} else
      //{
      //  m_game->m_getCore ()->m_getTimer ()->m_event ( "start" );
      //}
    }
  }

};


void View::m_suspend ( void )
{

  PointerProvider::getVariables ()->currentState = "suspending";

  PointerProvider::getVariables ()->running = false;

  std::this_thread::sleep_for ( std::chrono::milliseconds { 1000 } );

  m_game->m_onSuspending ();

  PointerProvider::getVariables ()->currentState = "uninitialized";

  if (PointerProvider::getException ())
    PointerProvider::providerException ( nullptr );

  if (PointerProvider::getConfiguration ())
    PointerProvider::providerConfiguration ( nullptr );

  if (PointerProvider::getFileLogger ())
  {
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The logging engine is set to shut down..." );

    // failure or success, the logs are somehow to be saved, so give its thread some time
    std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );

    PointerProvider::getFileLogger ()->m_shutdown ();
    PointerProvider::providerFileLogger ( nullptr );
  }

  if (PointerProvider::getVariables ())
    PointerProvider::providerVariables ( nullptr );

};


IFrameworkView View::CreateView ( void )
{
  return *this;
};


void View::Initialize ( CoreApplicationView const& applicationView )
{

  // response to application window activation
  applicationView.Activated ( { this, &View::m_onActivated } );

  // response to application window suspension
  CoreApplication::Suspending ( { this, &View::m_onSuspending } );

  // response to application window resuming from suspension
  CoreApplication::Resuming ( { this, &View::m_onResuming } );

};


void View::Load ( winrt::hstring const& /*entryPoint*/ )
{
  //
};


void View::Run ( void )
{

  m_initialized = true;

  PointerProvider::getVariables ()->running = true;

  // game instantiation
  auto windowPtr = static_cast<::IUnknown*>(winrt::get_abi ( m_appWindow.get () ));
  //m_game = new (std::nothrow) Game ( windowPtr,
  //                                   static_cast<int>(m_clientWidth),
  //                                   static_cast<int>(m_clientHeight) ); ///
  m_game = std::make_unique<Game> ( windowPtr,
                                    static_cast<int>(m_clientWidth),
                                    static_cast<int>(m_clientHeight) );

  if (!m_game->m_isReady ())
  {
    m_game->m_onSuspending (); // failure, shut the application down properly.

    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "The game initialization failed!" );

    //MessageBoxA ( NULL, "The Game functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );

    failure = true;
  } else
  {
    PointerProvider::getVariables ()->currentState = "initialized";

    PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                "Entering the game loop..." );

    if (!m_game->m_run ())
    {
      failure = true;
      PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                  "One or more errors occurred while running the game!" );
    }
  }

};


void View::SetWindow ( CoreWindow const& window )
{

  m_appWindow = window;

  // cursor type: standard arrow cursor resource
  window.PointerCursor ( CoreCursor { CoreCursorType::Arrow, 0 } );

  // response to window size events
  window.SizeChanged ( { this, &View::m_onWindowResized } );

#if defined (NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
  try
  {
    window.ResizeStarted ( [this]( auto&&, auto&& ) { m_inResizeMove = true; } );
    window.ResizeCompleted ( [this]( auto&&, auto&& ) { m_inResizeMove = false; } );
  }
  catch (...) {}
#endif // requires Windows 10 Creators Update (10.0.15063) or later

  // response to window focus events
  window.Activated ( { this, &View::m_onFocused } );

  // response to window close events
  window.VisibilityChanged ( { this, &View::m_onVisibilityChanged } );

  //auto dispatcher = CoreWindow
  window.Dispatcher ().AcceleratorKeyActivated ( { this, &View::m_onAcceleratorKeyActivated } );

  // response to window close events // because of suspension procedure, following event rarely occurs.
  //window.Closed ( [this]( auto&&, auto&& ) { PointerProvider::getVariables ()->running = false; } );

  // response to client display DPI events
  auto currentDisplayInfos = DisplayInformation::GetForCurrentView ();
  currentDisplayInfos.DpiChanged ( { this, &View::m_onDpiChanged } );

  DisplayInformation::DisplayContentsInvalidated ( { this, &View::m_onDisplayContentInvalidated } );

  // DPI of initialized client window area
  m_Dpi = DisplayInformation::GetForCurrentView ().LogicalDpi ();


  // size of initialized client window area (save in case of procedure failure)
  m_clientWidth = float ( PointerProvider::getConfiguration ()->m_getSettings ().Width );
  m_clientHeight = float ( PointerProvider::getConfiguration ()->m_getSettings ().Height );


  // preferred (current configuration) client window size
  auto size = Size ( float ( m_clientWidth ),
                     float ( m_clientHeight ) );
  m_fullscreen = PointerProvider::getConfiguration ()->m_getSettings ().fullscreen;

  if (m_fullscreen)
  {
    auto view = ApplicationView::GetForCurrentView ();

    if (view.IsFullScreen ())
      view.ExitFullScreenMode ();
    else
      view.TryEnterFullScreenMode ();
  }

  ApplicationView::PreferredLaunchViewSize ( size );
  auto view = ApplicationView::GetForCurrentView ();

  // minimum client window size
  size.Width = 480.0f; size.Height = 320.0f;
  view.SetPreferredMinSize ( size );

  // set and save
  view.FullScreenSystemOverlayMode ( FullScreenSystemOverlayMode::Minimal );
  view.TryResizeView ( view.PreferredLaunchViewSize () );

};


void View::Uninitialize ( void )
{
  m_suspend ();
};


bool View::failure = false;
bool View::exitedWith ( void )
{
  return failure;
};

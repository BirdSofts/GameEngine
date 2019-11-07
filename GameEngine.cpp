// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#include "pch.h"
//#include "GameEngine.h"
#include "Utilities.h" // string + s,f streams + exception + threads + list + Windows standards
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
  // Orientation
  //std::unique_ptr <Game> game;
  Game* m_game;
protected:
  void m_onActivated ( CoreApplicationView const& /*applicationView*/,
                       IActivatedEventArgs const& /*args*/ ); // on application window activation

  void m_onVisibilityChanged ( CoreWindow const& /*sender*/,
                               VisibilityChangedEventArgs const& /*args*/ ); // on application window resize

  void m_onWindowResized ( CoreWindow const& /*sender*/,
                           WindowSizeChangedEventArgs const& /*args*/ ); // on application window resize

  void m_onDpiChanged ( DisplayInformation const& /*sender*/,
                        IInspectable const& /*args*/ ); // on client display DPI changed

  void m_onSuspending ( IInspectable const& /*sender*/,
                        SuspendingEventArgs const& /*args*/ ); // on application window entering the suspended state

  void m_onResuming ( IInspectable const& /*sender*/,
                      IInspectable const& /*args*/ ); // on application window resuming from suspended state

  void m_shutdown (); // release the application resources

  //void m_on Key Pressed
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
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
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


    auto view = winrt::make<View> ();
    CoreApplication::Run ( view );
    //CoreApplication::Run ( View () );

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
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      ex.what () );

            // failure or success, the logs are somehow to be saved, so give its thread some time
            std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
          }
        }
        return EXIT_FAILURE;

  }

}


View::View ( void ) :
  m_visible ( false ), m_inResizeMove ( false )
{
  m_Dpi = 96.0f;

  m_clientWidth = 0;
  m_clientHeight = 0;
};


//View::~View ( void )
//{
//
//};


void View::m_onActivated ( CoreApplicationView const& /*applicationView*/, IActivatedEventArgs const& /*args*/ )
{
  // visible the activated application window and enables it to receive events
  CoreWindow::GetForCurrentThread ().Activate ();
};


void View::m_onVisibilityChanged ( CoreWindow const& /*sender*/, VisibilityChangedEventArgs const& args )
{
  m_visible = args.Visible (); ///
  m_game->isPaused () = !args.Visible ();
};


void View::m_onWindowResized ( CoreWindow const& /*sender*/, WindowSizeChangedEventArgs const& /*args*/ )
{

  m_clientWidth = m_appWindow.get ().Bounds ().Width;
  m_clientHeight = m_appWindow.get ().Bounds ().Height;
  if (!m_inResizeMove)
  {
    // Todo: resize the DirectX resources

  }

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
      m_shutdown ();
      deferral.Complete ();
    }
  );

};


void View::m_onResuming ( IInspectable const& /*sender*/, IInspectable const& /*args*/ )
{
  //
};


void View::m_shutdown ( void )
{

  PointerProvider::getVariables ()->currentState = "shutting down";

  PointerProvider::getVariables ()->running = false;

  std::this_thread::sleep_for ( std::chrono::milliseconds { 1000 } );

  m_game->shutdown ();

  PointerProvider::getVariables ()->currentState = "uninitialized";

  if (PointerProvider::getException ())
    PointerProvider::providerException ( nullptr );

  if (PointerProvider::getConfiguration ())
    PointerProvider::providerConfiguration ( nullptr );

  if (PointerProvider::getFileLogger ())
  {
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "The logging engine is set to shut down..." );

    // failure or success, the logs are somehow to be saved, so give its thread some time
    std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );

    PointerProvider::getFileLogger ()->shutdown ();
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

  // Todo: more reasearch how to use Xaml controls when implementing the UWP core

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

  PointerProvider::getVariables ()->running = true;

  // Todo: make unique research
  //CoreDX = std::make_unique<CoreDX> ();

  // game instantiation
  auto windowPtr = static_cast<::IUnknown*>(winrt::get_abi ( m_appWindow.get () ));
  m_game = new (std::nothrow) Game ( windowPtr,
                                     static_cast<int>(m_clientWidth),
                                     static_cast<int>(m_clientHeight) ); ///

  if (!m_game->isReady ())
  {
    m_game->shutdown (); // failure, shut the application down properly.

    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              "The game initialization failed!" );

    //MessageBoxA ( NULL, "The Game functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );

    failure = true;
  } else
  {
    PointerProvider::getVariables ()->currentState = "initialized";

    PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                              "Entering the game loop..." );

    if (!m_game->run ())
    {
      failure = true;
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
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

  // response to window close events
  window.VisibilityChanged ( { this, &View::m_onVisibilityChanged } );

  // response to window close events // because of suspension procedure, following event rarely occurs.
  //window.Closed ( [this]( auto&&, auto&& ) { PointerProvider::getVariables ()->running = false; } );

  // response to client display DPI events
  auto currentDisplayInfos = DisplayInformation::GetForCurrentView ();
  currentDisplayInfos.DpiChanged ( { this, &View::m_onDpiChanged } );

  // size of initialized client window area
  m_clientWidth = window.Bounds ().Width;
  m_clientHeight = window.Bounds ().Height;

  // DPI of initialized client window area
  m_Dpi = DisplayInformation::GetForCurrentView ().LogicalDpi ();

  // preferred and minimum client window size
  auto size = Size ( float ( PointerProvider::getConfiguration ()->getSettings ().Width ),
                     float ( PointerProvider::getConfiguration ()->getSettings ().Height ) );
  ApplicationView::PreferredLaunchViewSize ( size );
  auto view = ApplicationView::GetForCurrentView ();
  size.Width = 480.0f; size.Height = 320.0f;
  view.SetPreferredMinSize ( size );
  // set and save
  view.FullScreenSystemOverlayMode ( FullScreenSystemOverlayMode::Minimal );
  view.TryResizeView ( view.PreferredLaunchViewSize () );
  m_clientWidth = window.Bounds ().Width;
  m_clientHeight = window.Bounds ().Height;

};


void View::Uninitialize ( void )
{
  m_shutdown ();
};


bool View::failure = false;
bool View::exitedWith ( void )
{
  return failure;
};



//switch (msg)
//{
//  case WM_ACTIVATE: // if window activation state changes
//    if (PointerProvider::getVariables ()->currentState == "gaming")
//    {
//      if ((LOWORD ( wPrm ) == WA_INACTIVE)) // activation flag
//      {
//        core->paused = true; // the game is paused
//        core->timer->event ( "pause" );
//      } else
//      {
//        core->timer->event ( "start" );
//        core->paused = false; // the game is running
//      }
//    }
//    break;
//
//  case WM_KEYDOWN: // if a key is pressed
//    if (wPrm == VK_ESCAPE) // the ESC key identification
//    {
//      core->paused = true;
//      core->timer->event ( "pause" );
//      if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
//      {
//        // next expression simply indicates to the system intention to terminate the window,
//        // which puts a WM_QUIT message in the message queue, subsequently causing the main event loop to bail.
//        PostQuitMessage ( 0 ); // send the corresponding quite message
//        PointerProvider::getVariables()->running = false;
//        PointerProvider::getVariables ()->currentState = "shutting down";
//      } else
//      {
//        core->timer->event ( "start" );
//        core->paused = false;
//      }
//      break;
//    }
//
//    if (wPrm == VK_PRIOR) // the page up key identification
//    {
//      if (!core->d3d->isFullscreen ())
//        core->setResolution ( true ); // switch to fullscreen mode and set to highest resolution
//      break;
//    }
//
//    if (wPrm == VK_NEXT) // the page down key identification
//    {
//      if (core->d3d->isFullscreen ())
//        core->setResolution ( false ); // switch to windowed mode and set the lowest resolution
//      break;
//    }
//
//    if (wPrm == VK_UP) // the up arrow key identification
//    {
//      // for the time being till introduction of DirectInput
//      core->game->getUniverse ()->getCamera ()->forwardBackward ( 0.05f );
//      break;
//    }
//
//    if (wPrm == VK_DOWN) // the down arrow key identification
//    {
//      core->game->getUniverse ()->getCamera ()->forwardBackward ( -0.05f );
//      break;
//    }
//
//  case WM_CLOSE: // the user tries to somehow close the application
//  //case WM_DESTROY: // window is flagged to be destroyed (the close button is clicked)
//    core->paused = true;
//    core->timer->event ( "pause" );
//    if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
//    {
//      PostQuitMessage ( 0 );
//      PointerProvider::getVariables()->running = false;
//      PointerProvider::getVariables ()->currentState = "shutting down";
//    } else
//    {
//      core->timer->event ( "start" );
//      core->paused = false;
//    }
//    break;
//
//    //case WM_MENUCHAR: // handling none mnemonic or accelerator key and preventing constant beeping
//    //  // the games don't have a menu, this fact can easily be used to deceive the Windows,
//    //  // binding this not-needed feature to close the non-existent menu.
//    //  return MAKELRESULT ( 0, MNC_CLOSE );
//    //  break;
//
//  case WM_SIZE: // important for games in windowed mode (resizing the client size and game universe)
//    if (PointerProvider::getVariables ()->currentState == "gaming")
//    {
//      if (wPrm == SIZE_MINIMIZED) // window is minimized
//      {
//        minimized = true;
//        core->timer->event ( "pause" );
//        core->paused = true;
//      } else
//        if (wPrm == SIZE_MAXIMIZED) // window is maximized
//        {
//          maximized = true;
//          if (!minimized)
//          {
//            resized = true;
//            core->resizeResources ( false );
//          }
//          minimized = false;
//          core->paused = false;
//          core->timer->event ( "start" );
//        } else
//          if (wPrm == SIZE_RESTORED) // window is restored, find the previous state:
//          {
//            if (minimized)
//            {
//              minimized = false;
//              core->timer->event ( "pause" );
//              core->paused = true;
//            } else
//              if (maximized)
//              {
//                maximized = false;
//                resized = true;
//                core->resizeResources ( false );
//                core->paused = false;
//                core->timer->event ( "start" );
//              } else
//                if (resizing)
//                {
//                  if (PointerProvider::getVariables ()->currentState == "gaming")
//                    if (!core->paused)
//                    {
//                      core->timer->event ( "pause" );
//                      core->paused = true;
//                    }
//                  // a game window get seldom resized or dragged, even when such a case occur,
//                  // constant response to so many WM_SIZE messages while resizing, dragging is pointless.
//                } else // response when resized
//                {
//                  resized = true;
//                  core->resizeResources ( false );
//                  if (PointerProvider::getVariables ()->currentState == "gaming")
//                  {
//                    core->paused = false;
//                    core->timer->event ( "start" );
//                  }
//                }
//          }
//    }
//    break;
//
//  case WM_ENTERSIZEMOVE: // the edge of the window is being dragged around to resize it
//    resizing = true;
//    if (PointerProvider::getVariables ()->currentState == "gaming")
//    {
//      core->timer->event ( "pause" );
//      core->paused = true;
//    }
//    break;
//
//  case WM_EXITSIZEMOVE: // the dragging is finished and the window is now resized
//    resizing = false;
//    resized = true;
//    core->resizeResources ( false );
//    if (PointerProvider::getVariables ()->currentState == "gaming")
//    {
//      core->paused = false;
//      core->timer->event ( "start" );
//    }
//    break;
//
//    // setting the possible minimum size of the window (the message is sent when a window size is about to changed)
//  case WM_GETMINMAXINFO:
//    // a pointer to the 'MINMAXINFO' structure is provided by the message parameter 'lPrm'
//    ((MINMAXINFO*) lPrm)->ptMinTrackSize.x = PointerProvider::getConfiguration ()->getDefaults ().Width;
//    ((MINMAXINFO*) lPrm)->ptMinTrackSize.y = PointerProvider::getConfiguration ()->getDefaults ().Height;
//    break;
//
//}

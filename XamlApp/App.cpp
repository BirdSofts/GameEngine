// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"

#include "App.h"
#include "MainPage.h"


using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace GameEngine;
using namespace GameEngine::implementation;


/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App ()
{
  try
  {

    InitializeComponent ();
    Suspending ( { this, &App::OnSuspending } );
    Resuming ( { this, &App::OnResuming } );

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException ( [this]( IInspectable const&, UnhandledExceptionEventArgs const& e )
                         {
                           if (IsDebuggerPresent ())
                           {
                             auto errorMessage = e.Message ();
                             __debugbreak ();
                           }
                         } );
#endif

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
    //CoreApplication::Run ( View () );

    //if (App::exitedWith ())
    //{
    //  return EXIT_FAILURE;
    //} else
    //{
    //  return EXIT_SUCCESS;
    //}

  }
  catch (const std::exception& ex)
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
        //return EXIT_FAILURE;

  }
}


/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched ( LaunchActivatedEventArgs const& e )
{
#if _DEBUG
  if (IsDebuggerPresent ())
  {

    DebugSettings ().EnableFrameRateCounter ( true );
  }
#endif // _DEBUG

  Frame rootFrame { nullptr };
  auto content = Window::Current ().Content ();
  if (content)
  {
    rootFrame = content.try_as<Frame> ();
  }

  // Do not repeat app initialization when the Window already has content,
  // just ensure that the window is active
  if (rootFrame == nullptr)
  {
    // Create a Frame to act as the navigation context and associate it with
    // a SuspensionManager key
    rootFrame = Frame ();

    rootFrame.NavigationFailed ( { this, &App::OnNavigationFailed } );

    if (e.PreviousExecutionState () == ApplicationExecutionState::Terminated)
    {
      // Restore the saved session state only when appropriate, scheduling the
      // final launch steps after the restore is complete
    }

    if (e.PrelaunchActivated () == false)
    {
      if (rootFrame.Content () == nullptr)
      {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a navigation
        // parameter
        rootFrame.Navigate ( xaml_typename<GameEngine::MainPage> (), box_value ( e.Arguments () ) );
      }
      // Place the frame in the current Window
      Window::Current ().Content ( rootFrame );
      // Ensure the current window is active
      Window::Current ().Activate ();
    }
  } else
  {
    if (e.PrelaunchActivated () == false)
    {
      if (rootFrame.Content () == nullptr)
      {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a navigation
        // parameter
        rootFrame.Navigate ( xaml_typename<GameEngine::MainPage> (), box_value ( e.Arguments () ) );
      }
      // Ensure the current window is active
      Window::Current ().Activate ();
    }
  }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending ( [[maybe_unused]] IInspectable const& sender, [[maybe_unused]] SuspendingEventArgs const& e )
{
  // Save application state and stop any background activity
  auto deferral = e.SuspendingOperation ().GetDeferral ();
  auto task = std::async (
    std::launch::async, [this, deferral]()
    {
      PointerProvider::getVariables ()->currentState = "suspending";

      PointerProvider::getVariables ()->running = false;

      std::this_thread::sleep_for ( std::chrono::milliseconds { 1000 } );

      //m_game->m_onSuspending (); // Todo load and save state

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

      deferral.Complete ();
    }
  );
}


void App::OnResuming ( [[maybe_unused]] IInspectable const& sender, [[maybe_unused]] IInspectable const& args )
{
  // Resume application state and return to background activity
}


/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed ( [[maybe_unused]] IInspectable const& sender, NavigationFailedEventArgs const& e )
{
  throw hresult_error ( E_FAIL, hstring ( L"Failed to load Page " ) + e.SourcePageType ().Name );
}

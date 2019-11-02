// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Utilities.h" // string + s,f streams + exception + threads + list + Windows standards
#include "Shared.h"
#include "Game.h"


// references:
// http://www.rastertek.com/
// https://docs.microsoft.com/
// https://bell0bytes.eu/
// http://www.cplusplus.com/
// https://en.cppreference.com/
// https://www.braynzarsoft.net/


// needed for application's exceptions handling check (simply copy to place to to throw)
//long long* anArray = new long long [1000000000000000000]; // throwing a standard exception on memory allocation


bool running { false };

std::wstring gameState { L"uninitialized" };


// main function (application entry point)
int WINAPI WinMain ( _In_ HINSTANCE hInstance, // generated instance handle by Windows for the program
                     _In_opt_ HINSTANCE hPrevInstance, // obsolete plus backward compatibility (https://bell0bytes.eu/hello-world/)
                     // a long pointer to a string, similar to the command-line parameters of the standard C/C++ main function.
                     _In_ LPSTR lpCmdLine,
                     _In_ int nShowCmd ) // indicates how the main window is to be opened (minimized, maximized)
{
  try
  {

    std::shared_ptr<theException> anException { new (std::nothrow) theException () };
    PointerProvider::exceptionProvider ( anException );

    std::shared_ptr<Logger<toFile>> fileLoggerEngine ( new (std::nothrow) Logger<toFile> () );
    PointerProvider::fileLoggerProvider ( fileLoggerEngine );

    std::shared_ptr<Configurations> settings ( new (std::nothrow) Configurations () );
    PointerProvider::configurationProvider ( settings );

    if ((anException) && (settings))
      running = true;
    else
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::exceptionProvider ( nullptr );
        anException.reset ();
      }

      PointerProvider::getException ()->set ( "appSone" );
      throw* PointerProvider::getException ();
    }

    if (fileLoggerEngine)
    {
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"Exception, file logger and configuration providers are successfully initialized." );
    } else
    {
      // failure, shut down the game properly
      if (anException)
      {
        PointerProvider::exceptionProvider ( nullptr );
        anException.reset ();
      }
      if (settings)
      {
        PointerProvider::configurationProvider ( nullptr );
        settings.reset ();
      }

      PointerProvider::getException ()->set ( "appDebug" );
      throw* PointerProvider::getException ();
    }


    Game game ( hInstance );

    if (!game.isReady ())
    {
      game.shutdown (); // failure, try to save all the logs, while shutting down properly.

      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The game initialization failed!" );

      PointerProvider::getException ()->set ( "appStwo" );
      throw* PointerProvider::getException ();
    }
    gameState = L"initialized";

    PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                              L"Entering the game loop..." );
    bool failure { false };

    if (!game.run ())
    {
      failure = true;
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread",
                                                L"Game failed to run!" );
    }

    game.shutdown (); // failure or success, try to save all the logs, while shutting down properly.

    if (anException)
    {
      PointerProvider::exceptionProvider ( nullptr );
      anException.reset ();
    }

    if (fileLoggerEngine)
    {
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The logging engine is set to shut down..." );
      std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );

      PointerProvider::fileLoggerProvider ( nullptr );
      fileLoggerEngine.reset ();
    }

    if (settings)
    {
      PointerProvider::configurationProvider ( nullptr );
      settings.reset ();
    }

    gameState = L"uninitialized";

    if (failure)
      return EXIT_FAILURE;
    else
      return EXIT_SUCCESS;

  }
  catch (const std::exception& ex)
  {

    if (ex.what () == "appSone")
      MessageBoxA ( NULL, "The Game could not be started...", "Error", MB_OK | MB_ICONERROR );
    else
      if (ex.what () == "appDebug")
        MessageBoxA ( NULL, "The debug service failed to start.", "Error", MB_OK | MB_ICONERROR );
      else
        if (ex.what () == "appStwo")
        {
          MessageBoxA ( NULL, "The Game functionality failed to start...", "Critical-Error", MB_OK | MB_ICONERROR );
        } else
        {
          MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
          if (PointerProvider::getFileLogger ())
          {
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                      Converter::strConverter ( ex.what () ) );
            std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
          }
        }
        return EXIT_FAILURE;
  }
}

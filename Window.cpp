// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Window.h"
#include "Shared.h"


Window* wnd { nullptr };
LRESULT CALLBACK mainWndProc ( HWND handle, UINT msg, WPARAM wPrm, LPARAM lPrm )
{
  // the window procedure, needed since a member function doesn't suite wClass.lpfnWndProc
  // global window procedure forwards messages to member window procedure
  return wnd->msgProc ( handle, msg, wPrm, lPrm );
}


Window::Window ( TheCore* coreObject ) :
  handle ( NULL ), core ( coreObject ), initialized ( false ),
  minimized ( false ), maximized ( false ), resizing ( false ), resized ( false )
{
  try
  {

    wnd = this; // necessary to forward messages

    appInstance = coreObject->getInstance ();

    //if (PointerProvider::getConfiguration ()->getSettings ().Width == 640)
    //  PointerProvider::getConfiguration ()->apply (); // 800*600 resolution to file

    clientWidth = PointerProvider::getConfiguration ()->getSettings ().Width;
    clientHeight = PointerProvider::getConfiguration ()->getSettings ().Height;

    LPCWSTR windowName = { L"The Game" };

    // filling the instantiation of the extended version of window class,
    // a structure that handles properties and actions of a window:
    WNDCLASSEXW wClass;
    wClass.cbClsExtra = 0; // extra class runtime information
    wClass.cbSize = sizeof ( WNDCLASSEX ); // the size of the structure itself (usable when passed as pointer)
    wClass.cbWndExtra = 0; // extra window runtime information
    wClass.hbrBackground = ( HBRUSH) GetStockObject ( DKGRAY_BRUSH ); // a handle to a background brush for window
    wClass.hCursor = LoadCursor ( 0, IDC_ARROW ); // the cursor for the window
    // ('LoadIcon' and 'LoadCursor' functions retrieve handles to standard common icons)
    wClass.hIcon = LoadIcon ( 0, IDI_APPLICATION ); // title bar icon
    wClass.hIconSm = LoadIcon ( 0, IDI_APPLICATION ); // taskbar icon
    wClass.hInstance = appInstance; // handle to current application instance, which has created the window
    // callback function pointer to handler of the window (default window callback procedure)
    wClass.lpfnWndProc = mainWndProc;
    // a long pointer to a constant literal representing the name of the window class itself,
    // useful when the application opens more than one window, so Windows is able to keep track of them
    wClass.lpszClassName = windowName;
    // a long pointer to a constant literal representing the name of the menu to attach
    wClass.lpszMenuName = 0;
    // object style flags: below arguments are usable to redraw the window, when moving it vertically or horizontally,
    // which are not usable for a full-screen application
    wClass.style = CS_HREDRAW | CS_VREDRAW;

    // registering the window class to Windows, the function returns zero in case of failure,
    // and returns a class atom that uniquely identifies the class being registered.
    // note that since all the registered window classes by a program are unregistered,
    // when it terminates, no manual cleaning is necessary.
    if (!RegisterClassExW ( &wClass ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The registration of the client window failed!" );
    }

    // client size: all the raw window size can't be tampered with as working area,
    // therefore the exact dimension needs to be calculated.
    // rectangle structure to store the coordinates (top-left and bottom-right)
    RECT rectangle { 0, 0, long ( clientWidth ), long ( clientHeight ) };
    if (!AdjustWindowRectEx (
      // long pointer to the rectangle structure to be filled with the calculated coordinates
      &rectangle,
      WS_OVERLAPPEDWINDOW, // current window style (is needed to calculated the client size)
      false, // window contains a menu or not
      WS_EX_OVERLAPPEDWINDOW )) // current extended window style (is needed to calculated the client size)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The calculation of the client window size failed!" );
    }

    // after the properties of a window class is known to Windows, it can finally be created.
    // the below function returns a handle to the newly created window, or NULL in case of failure.
    handle = CreateWindowExW (
      WS_EX_OVERLAPPEDWINDOW, // extended window style (for game NULL)
      wClass.lpszClassName, // a long pointer to a constant literal representing the registered window class name
      windowName, // a long pointer to a constant literal representing the name or title of the window
      WS_OVERLAPPEDWINDOW, // window style (an overlapped window has a title bar and a border)
      // note that the below arguments can be set to default if the actual location is of no importance
      CW_USEDEFAULT, // window horizontal position in pixel (x)
      CW_USEDEFAULT, // window vertical position in pixel (y)
      // note that the below arguments can be set to default if the initial dimension is of no importance
      rectangle.right - rectangle.left, rectangle.bottom - rectangle.top, // window size
      NULL, // handle to parent or owner window (Null designates the desktop as parent)
      NULL, // handle to menu, or child-window identifier
      appInstance, // handle to current application instance, which has created the window
      NULL // pointer to window-creation data (advanced)
    );

    if (!handle)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"The creation of the client failed!" );
    }

    // show the window: the second parameter controls how the window is to be shown,
    // it can be passed the last parameter of the main function (nShowCmd) of the program,
    // or SW_SWOW, for example, activates the window and displays it in its current size and position.
    ShowWindow ( handle, SW_SHOW );
    // using the function below, Windows is forced to update the window content,
    // additionally generating a WM_PAINT message that needs to be handled by the event handler.
    UpdateWindow ( handle );
    initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Window::isInitialized ( void )
{
  return initialized;
};


const HWND& Window::getHandle ( void )
{
  return handle;
}


unsigned int& Window::getWidth ( void )
{
  return clientWidth;
};


unsigned int& Window::getHeight ( void )
{
  return clientHeight;
};


bool& Window::isResized ( void )
{
  return resized;
};


// the event handler, a callback function, also known as Windows Procedure,
// is called by Windows from main event loop of the system at the occurrence of an event,
// which the running window must handle, before it continues its normal flow.
// defining callback, it is an executable code, passed as an argument to another code,
// which then at some convenient time is expected to execute (call back) that argument.
// a window can handle as many or as few events as it needs,
// passing all the other ones to default Windows event handler.
// note that the number of events that an application handles,
// is in direct connection to the speed, it returns to continue its purpose.
LRESULT CALLBACK Window::msgProc (
  HWND handle, // current window handle (useful when dealing with multiple windows)
  UINT msg, // the message id that needs to be handled
  WPARAM wPrm, // additional information describing the message
  LPARAM lPrm ) // additional information describing the message
{
  try
  {

    switch (msg)
    {
      case WM_ACTIVATE: // if window activation state changes
        if (gameState == L"gaming")
        {
          if ((LOWORD ( wPrm ) == WA_INACTIVE)) // activation flag
          {
            core->paused = true; // the game is paused
            core->timer->event ( "pause" );
          } else
          {
            core->timer->event ( "start" );
            core->paused = false; // the game is running
          }
        }
        break;

      case WM_KEYDOWN: // if a key is pressed
        if (wPrm == VK_ESCAPE) // the ESC key identification
        {
          core->paused = true;
          core->timer->event ( "pause" );
          if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
          {
            // next expression simply indicates to the system intention to terminate the window,
            // which puts a WM_QUIT message in the message queue, subsequently causing the main event loop to bail.
            PostQuitMessage ( 0 ); // send the corresponding quite message
            running = false;
            gameState = L"shutting down";
          } else
          {
            core->timer->event ( "start" );
            core->paused = false;
          }
          break;
        }

        if (wPrm == VK_PRIOR) // the page up key identification
        {
          if (!core->d3d->isFullscreen ())
            core->setResolution ( true ); // switch to fullscreen mode and set to highest resolution
          break;
        }

        if (wPrm == VK_NEXT) // the page down key identification
        {
          if (core->d3d->isFullscreen ())
            core->setResolution ( false ); // switch to windowed mode and set the lowest resolution
          break;
        }

        if (wPrm == VK_UP) // the up arrow key identification
        {
          // for the time being till introduction of DirectInput
          core->game->getUniverse ()->getCamera ()->forwardBackward ( 0.05f );
          break;
        }

        if (wPrm == VK_DOWN) // the down arrow key identification
        {
          core->game->getUniverse ()->getCamera ()->forwardBackward ( -0.05f );
          break;
        }

      case WM_CLOSE: // the user tries to somehow close the application
      //case WM_DESTROY: // window is flagged to be destroyed (the close button is clicked)
        core->paused = true;
        core->timer->event ( "pause" );
        if (MessageBoxA ( handle, "Exit the Game?", "Exit", MB_YESNO | MB_ICONQUESTION ) == IDYES)
        {
          PostQuitMessage ( 0 );
          running = false;
          gameState = L"shutting down";
        } else
        {
          core->timer->event ( "start" );
          core->paused = false;
        }
        break;

        //case WM_MENUCHAR: // handling none mnemonic or accelerator key and preventing constant beeping
        //  // the games don't have a menu, this fact can easily be used to deceive the Windows,
        //  // binding this not-needed feature to close the non-existent menu.
        //  return MAKELRESULT ( 0, MNC_CLOSE );
        //  break;

      case WM_SIZE: // important for games in windowed mode (resizing the client size and game universe)
        if (gameState == L"gaming")
        {
          if (wPrm == SIZE_MINIMIZED) // window is minimized
          {
            minimized = true;
            core->timer->event ( "pause" );
            core->paused = true;
          } else
            if (wPrm == SIZE_MAXIMIZED) // window is maximized
            {
              maximized = true;
              if (!minimized)
              {
                resized = true;
                core->resizeResources ( false );
              }
              minimized = false;
              core->paused = false;
              core->timer->event ( "start" );
            } else
              if (wPrm == SIZE_RESTORED) // window is restored, find the previous state:
              {
                if (minimized)
                {
                  minimized = false;
                  core->timer->event ( "pause" );
                  core->paused = true;
                } else
                  if (maximized)
                  {
                    maximized = false;
                    resized = true;
                    core->resizeResources ( false );
                    core->paused = false;
                    core->timer->event ( "start" );
                  } else
                    if (resizing)
                    {
                      if (gameState == L"gaming")
                        if (!core->paused)
                        {
                          core->timer->event ( "pause" );
                          core->paused = true;
                        }
                      // a game window get seldom resized or dragged, even when such a case occur,
                      // constant response to so many WM_SIZE messages while resizing, dragging is pointless.
                    } else // response when resized
                    {
                      resized = true;
                      core->resizeResources ( false );
                      if (gameState == L"gaming")
                      {
                        core->paused = false;
                        core->timer->event ( "start" );
                      }
                    }
              }
        }
        break;

      case WM_ENTERSIZEMOVE: // the edge of the window is being dragged around to resize it
        resizing = true;
        if (gameState == L"gaming")
        {
          core->timer->event ( "pause" );
          core->paused = true;
        }
        break;

      case WM_EXITSIZEMOVE: // the dragging is finished and the window is now resized
        resizing = false;
        resized = true;
        core->resizeResources ( false );
        if (gameState == L"gaming")
        {
          core->paused = false;
          core->timer->event ( "start" );
        }
        break;

        // setting the possible minimum size of the window (the message is sent when a window size is about to changed)
      case WM_GETMINMAXINFO:
        // a pointer to the 'MINMAXINFO' structure is provided by the message parameter 'lPrm'
        (( MINMAXINFO*) lPrm)->ptMinTrackSize.x = PointerProvider::getConfiguration ()->getDefaults ().Width;
        (( MINMAXINFO*) lPrm)->ptMinTrackSize.y = PointerProvider::getConfiguration ()->getDefaults ().Height;
        break;

    }

    // it is very important to let Window handle other for the program irrelevant messages,
    // using below pass through function, preventing the Window losing them all. :)
    return DefWindowProc ( handle, msg, wPrm, lPrm );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return DefWindowProc ( handle, msg, wPrm, lPrm );

  }
};

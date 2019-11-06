// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Core.h"
#include "Shared.h"


TheCore::TheCore ( ::IUnknown* window, Game* gameObj ) :
  timer ( nullptr ), fps ( 0 ), mspf ( 0 ),
  appWindow ( window ), d3d ( nullptr ), d2d ( nullptr ), game ( gameObj ),
  debug ( false ), initialized ( false ), paused ( false ), resized ( false )
{
  try
  {

    // timer instantiation
    timer = new (std::nothrow) Timer;
    if (!timer->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Timer initialization failed!" );
      return;
    }

    //// application window instantiation
    //appWindow = new (std::nothrow) Window ( this );
    //if (!appWindow->isInitialized ())
    //{
    //  PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
    //                                            "Window initialization failed!" );
    //  return;
    //}
    //appHandle = appWindow->getHandle (); // handle to the instantiated window

    // Direct3D 10 instantiation
    d3d = new (std::nothrow) Direct3D ( this );
    if (!d3d->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Direct3D initialization failed!" );
      return;
    }

    // Direct2D 10 instantiation
    d2d = new (std::nothrow) Direct2D ( this );
    if (!d2d->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Direct2D initialization failed!" );
      return;
    }

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "The framework is successfully initialized." );

    debug = true; // Todo must be switched from within the application

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


const bool& TheCore::isInitialized ( void )
{
  return initialized;
};


const ::IUnknown* TheCore::getWindow ( void )
{
  return appWindow;
};


const bool& TheCore::isPaused ( void )
{
  return paused;
};


Timer* TheCore::getTimer ( void )
{
  return timer;
};


const int& TheCore::getFPS ( void )
{
  return fps;
};


//Direct3D* TheCore::getd3d ( void )
//{
//  return d3d;
//};


//Direct2D* TheCore::getd2d ( void )
//{
//  return d2d;
//};


void TheCore::frameStatistics ( void )
{
  try
  {

    HRESULT hR;

    // a static local variable retains its state between the calls:
    static int frameCounter; // frame counter (a frame is a full cycle of the game loop)
    static double elapsed; // the elapsed time since the last call
    frameCounter++;

    if ((timer->getTotal () - elapsed) >= 1e0)
    {

      // frame calculations:
      fps = frameCounter; // the number of counted frames in one second
      mspf = 1e3 / fps; // average taken time by a frame in milliseconds

      if (!paused && debug && d2d)
      {
        d2d->textLayoutsDebug = false;
        //// results to window caption
        //std::wstring caption = L"The Game ^,^ - FPS: " + std::to_wstring ( fps ) +
        //  L" - mSPF: " + std::to_wstring ( mspf );
        //SetWindowTextW ( appHandle, caption.c_str () );

        // results to client window area
        // FPS information text layouts
        std::wostringstream outFPS;
        outFPS.precision ( 6 );
        outFPS << "Resolution: " << d3d->displayMode.Width << " x " << d3d->displayMode.Height
          << " - Display mode #" << d3d->displayModeIndex + 1 << " of " << d3d->displayModesCount << " @ "
          << d3d->displayMode.RefreshRate.Numerator / d3d->displayMode.RefreshRate.Denominator << " Hz" << std::endl
          << "Display Adapter: " << d3d->videoCardDescription
          << " - Dedicated memory: " << d3d->videoCardMemory << "MB" << std::endl
          << "^_^ - FPS: " << fps << L" - mSPF: " << mspf << std::endl;

        // before rendering a text to a bitmap: the creation of the text layout
        //hR = d2d->writeFac->CreateTextLayout ( outFPS.str ().c_str (), (UINT32) outFPS.str ().size (),
        //                                       d2d->textFormatFPS.Get (), (float) appWindow->clientWidth,
        //                                       (float) appWindow->clientHeight, &d2d->textLayoutFPS );
        //if (FAILED ( hR ))
        //{
        //  PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
        //                                            "The Creation of text layout for FPS information failed!" );
        //  return;
        //}

        std::string out { "Last event: " };
        out += PointerProvider::getFileLogger ()->getLogRawStr ();
        //hR = d2d->writeFac->CreateTextLayout ( out.c_str (), (UINT32) (UINT32) out.size (),
        //                                       d2d->textFormatLogs.Get (), (float) appWindow->clientWidth,
        //                                       (float) appWindow->clientHeight, &d2d->textLayoutLogs );
        //if (FAILED ( hR ))
        //{
        //  PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
        //                                            "The Creation of text layout for Logs failed!" );
        //  return;
        //}
        //d2d->textLayoutsDebug = true;

      }

      // reset
      frameCounter = 0;
      elapsed += 1.0;
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void TheCore::setResolution ( const bool& prm )
{
  try
  {

    if (prm)
    {
      if (!d3d->fullscreen)
      {
        d3d->displayModeIndex = d3d->displayModesCount - 1;
        d3d->displayMode = d3d->displayModes [d3d->displayModeIndex];
        d3d->fullscreen = true;
      }
    } else
    {
      if (d3d->fullscreen)
      {
        d3d->displayModeIndex = 0;
        d3d->displayMode = d3d->displayModes [d3d->displayModeIndex];
        d3d->fullscreen = false;
      }
    }

    resizeResources ( true );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void TheCore::resizeResources ( const bool& displayMode )
{
  try
  {

    if (initialized)
    {
      unsigned long rC { 0 };
      //HRESULT hR;

      // free game resources
      //if (game->vertexBuffer [0] && game->indexBuffer [1])
      //{
      //}

      // free Direct2D resources
      if (d2d && !rC)
      {
        rC = d2d->dcBitmap.Reset ();
        rC = d2d->deviceCon.Reset ();
        rC = d2d->dcBuffer.Reset ();
        //if (rC)
        //{
        //  rC = 0; // HACK debug
        //  PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
        //                                            "Problem while releasing one or more resources!" );
        //}
      }

      // free Direct3D resources
      if (d3d->dSview && d3d->rTview && !rC)
      {
        d3d->devCon->ClearState ();
        rC = d3d->rasterizerState.Reset ();
        d3d->devCon->OMSetRenderTargets ( 0, nullptr, nullptr );
        rC = d3d->dSview.Reset ();
        rC = d3d->dSstate.Reset ();
        rC = d3d->dSbuffer.Reset ();
        rC = d3d->rTview.Reset ();
        //if (rC)
        //{
        //  rC = 0; // HACK debug
        //  PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
        //                                            "Problem while releasing one or more resources!" );
        //}

      }

      // reallocation procedures
      if (!rC)
      {
        if (displayMode)
        {
          d3d->displayModeSetter ();
          std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );
        }
        d3d->allocateResources ();
        if (d2d)
        {
          d2d->allocateResources ();
        }
        //game->allocateResources ();
        //appWindow->isResized () = false;
        resized = true;

      } else
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Resources' deallocation failed!" );
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void TheCore::shutdown ( void )
{
  try
  {

    unsigned long rC { 0 };
    //HRESULT hR;

    initialized = false;

    if (d3d)
      // to prevent exceptions, switch back to windowed mode
      d3d->swapChain->SetFullscreenState ( false, nullptr );

    // Direct2D application destruction
    if (d2d)
    {
      d2d->initialized = false;
      rC = d2d->dcBitmap.Reset ();
      rC = d2d->deviceCon.Reset ();
      //rC = d2d->dcBuffer.Reset ();
      rC = d2d->device.Reset ();
      rC = d2d->factory.Reset ();
      //rC = d2d->writeFac.Reset ();
      d2d->core = nullptr;
      delete d2d;
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct2D is successfully destructed." );
    }

    // Direct3D application destruction
    if (d3d)
    {
      d3d->initialized = false;
      d3d->devCon->ClearState ();
      rC = d3d->rasterizerState.Reset ();
      d3d->devCon->OMSetRenderTargets ( 0, nullptr, nullptr );
      rC = d3d->dSview.Reset ();
      rC = d3d->dSstate.Reset ();
      rC = d3d->dSbuffer.Reset ();
      rC = d3d->rTview.Reset ();
      rC = d3d->swapChain.Reset ();

#ifndef _NOT_DEBUGGING
      rC = d3d->debug.Reset ();
#endif // !_NOT_DEBUGGING

      rC = d3d->device.Reset ();
      d3d->core = nullptr;

      delete d3d;
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct3D is successfully destructed." );
    }

    // application main window destruction
    //if (appWindow)
    //{
    //  appWindow->initialized = false;
    //  appWindow->handle = NULL;
    //  appWindow->appInstance = NULL;
    //  appWindow->core = nullptr;
    //  delete appWindow;
    //  PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
    //                                            "Application main window class is successfully destructed." );
    //}

    // timer application destruction
    if (timer)
      delete timer;

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "The Application Core is successfully shut down." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};

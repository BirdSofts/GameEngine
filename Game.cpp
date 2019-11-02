// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Game.h"
#include "Shared.h"


Game::Game ( HINSTANCE& h_instance ) :
  core ( nullptr ), universe ( nullptr ),
  initialized ( false ), allocated ( false )
{
  try
  {

    shaderColour = nullptr;
    shaderTexture = nullptr;
    texture = nullptr;
    shaderDiffuseLight = nullptr;

    _2Dtriangles = nullptr;
    _2Dline = nullptr;
    _2DtexturedTriangles = nullptr;
    _2DlightedTriangle = nullptr;

    // the game framework instantiation
    core = new (std::nothrow) TheCore ( h_instance, this );

    if (!core->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Initialization of framework failed!" );
    }

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The game is successfully initialized." );

    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The game resources is successfully allocated." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Game::allocateResources ( void )
{
  try
  {

    allocated = false;

    // the game framework instantiation
    universe = new (std::nothrow) Universe ( core->d3d->device.Get () );

    if (!universe->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Initialization of game universe failed!" );
    }

    shaderColour = new (std::nothrow) ShaderColour ( core->d3d->device.Get () );

    shaderTexture = new (std::nothrow) ShaderTexture ( core->d3d->device.Get () );

    _2Dtriangles = new (std::nothrow) Triangles ( core->d3d->device.Get () );

    _2Dline = new (std::nothrow) Line ( core->d3d->device.Get () );

    texture = new (std::nothrow) Texture<TargaHeader>
      ( core->d3d->device.Get (), "./textures/clouds.tga" ); // a texture file
    if (!texture)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Instantiation of texture failed!" );
      return;
    }
    _2DtexturedTriangles = new (std::nothrow) TexturedTriangles ( core->d3d->device.Get () );

    shaderDiffuseLight = new (std::nothrow) ShaderDiffuseLight ( core->d3d->device.Get () );

    _2DlightedTriangle = new (std::nothrow) LightedTriangle ( core->d3d->device.Get () );

    _3Dcube = new (std::nothrow) Cube ( core->d3d->device.Get () );

    allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Game::isReady ( void )
{
  return initialized;
};


const bool Game::run ( void )
{
  try
  {

    MSG msg { 0 }; // a new message structure
    unsigned short counter { 0 };

    // setting the needed starting points
    core->timer->event ( "reset" ); // reset (start)

    universe->getCamera ()->setPosition ( 0.0f, 0.0f, -2.0f ); // set the start view

    universe->getDiffuseLight ()->setColour ( 0.2f, 0.6f, 0.6f, 1.0f ); // diffuse light colour
    universe->getDiffuseLight ()->setDirection ( 0.0f, 0.0f, 1.0f ); // light direction: point down the positive Z axis



    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

#pragma region peekLoop
      if ((counter % 10) == 0)
      {
        // check and peek (get) window messages already placed in an event queue.
        // note the difference between the below two functions:
        // the get function, once called, actually waits for a message,
        // while peek function allows the normal flow, if there is no message in the message queue.
        while ( // peek loop for messages (empting the message queue)
                PeekMessage (
                  // pointer to a message structure to receive message information
                  &msg,
                  // handle to the window, whose messages is intended,
                  // and NULL as argument allow the retrieve of all messages for any window, which belongs to the current thread.
                  core->appHandle,
                  // to retrieve messages filtered through the introduced range. (both zero retunes all available messages)
                  // first message to last message
                  0, 0,
                  // removal flags specify how the messages are to be handled:
                  // additionally to below introduced argument, PM_NOREMOVE prevents the removal of messages in the queue,
                  // therefore after it is passed, the get function is additionally needed to actually retrieve the messages.
                  PM_REMOVE ))
          //while ( GetMessage ( &msg, NULL, 0, 0 ) ) // not a good one for a game, which needs to deliver 30 F/S
        {
          // translation of the virtual-key messages into character messages
          TranslateMessage ( &msg );
          // dispatching the message to the window procedure function, so the event could be handled appropriately.
          DispatchMessage ( &msg );
          // the evaluated value: exit (using intellisense or MSDN, the possible and obvious messages could be seen)
        }
        counter = 0;
      }
#pragma endregion



      // tick the timer to calculate a frame
      core->timer->tick ();
      // -- fps calculation
      core->frameStatistics ();

      if (!core->paused)
      {

        // -----------------------------------------------------------------------------------------------------------
        // a game loop purpose:
        // -- process inputted data
        // Todo add input processes so far

        render ();


        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        core->d3d->present ();


        // -----------------------------------------------------------------------------------------------------------
        // -- update the game universe/logic
        // Note the needed delta time
        update ();

      } else
      {
        // Todo add additional suitable procedure can be done in paused state

        // if there is no suitable processing for paused state, set the engine at hibernation:
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
      }

      counter++;
    } while (running == true);
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    return true;
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return false;
  }
};


void Game::render ( void )
{
  try
  {

    core->d3d->clearBuffers ();

    universe->renderResources ();
    universe->getCamera ()->renderCamera ();

    if (core->d2d && core->debug)
      core->d2d->debugInfos (); // -- fps on screen representation



    // setting the active vertex/pixel shaders (active shader technique)
    core->d3d->device->VSSetShader ( shaderColour->getVertexShader () );
    core->d3d->device->PSSetShader ( shaderColour->getPixelShader () );

    // setting the active input layout
    core->d3d->device->IASetInputLayout ( shaderColour->getInputLayout () );

    // set the active vertex and index buffers (binds an array of vertex/index buffers to input-assembler stage)
    // basically, which vertices to put to graphics pipeline when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    core->d3d->device->IASetVertexBuffers ( 0, 1, _2Dtriangles->getVertexBuffer (), &strides, &offset );
    // set the active corresponding index buffer in the input assembler
    core->d3d->device->IASetIndexBuffer ( _2Dtriangles->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw indexed vertices, starting from vertex 0
    core->d3d->device->DrawIndexed ( _2Dtriangles->verticesCount, 0, 0 );



    core->d3d->device->IASetVertexBuffers ( 0, 1, _2Dline->getVertexBuffer (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( _2Dline->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    core->d3d->device->DrawIndexed ( _2Dline->verticesCount, 0, 0 );



    // setting the active texture
    core->d3d->device->PSSetShaderResources ( 0, 1, texture->getTexture () );

    core->d3d->device->VSSetShader ( shaderTexture->getVertexShader () );
    core->d3d->device->PSSetShader ( shaderTexture->getPixelShader () );

    core->d3d->device->IASetInputLayout ( shaderTexture->getInputLayout () );

    // setting the active sampler
    core->d3d->device->PSSetSamplers ( 0, 1, shaderTexture->getSamplerState () );

    strides = sizeof ( VertexT );
    core->d3d->device->IASetVertexBuffers ( 0, 1, _2DtexturedTriangles->getVertexBuffer (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( _2DtexturedTriangles->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->device->DrawIndexed ( _2DtexturedTriangles->verticesCount, 0, 0 );



    core->d3d->device->VSSetShader ( shaderDiffuseLight->getVertexShader () );
    core->d3d->device->PSSetShader ( shaderDiffuseLight->getPixelShader () );
    core->d3d->device->IASetInputLayout ( shaderDiffuseLight->getInputLayout () );
    core->d3d->device->PSSetSamplers ( 0, 1, shaderDiffuseLight->getSamplerState () );
    strides = sizeof ( VertexL );
    core->d3d->device->IASetVertexBuffers ( 0, 1, _2DlightedTriangle->getVertexBuffer (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( _2DlightedTriangle->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->device->DrawIndexed ( _2DlightedTriangle->verticesCount, 0, 0 );



    core->d3d->device->IASetVertexBuffers ( 0, 1, _3Dcube->getVertexBuffer (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( _3Dcube->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->device->DrawIndexed ( _3Dcube->verticesCount, 0, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Game::update ( void )
{
  try
  {

    _2Dline->update ();
    universe->update ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


Universe* Game::getUniverse ( void )
{
  return universe;
};


void Game::shutdown ( void )
{
  try
  {

    //unsigned long rC { 0 };
    //HRESULT hR;

    initialized = false;

    if (_3Dcube)
    {
      _3Dcube->release ();
      delete _3Dcube;
      _3Dcube = nullptr;
    }
    if (_2Dtriangles)
    {
      _2Dtriangles->release ();
      delete _2Dtriangles;
      _2Dtriangles = nullptr;
    }
    if (_2Dline)
    {
      _2Dline->release ();
      delete _2Dline;
      _2Dline = nullptr;
    }
    if (_2DtexturedTriangles)
    {
      _2DtexturedTriangles->release ();
      delete _2DtexturedTriangles;
      _2DtexturedTriangles = nullptr;
    }
    if (_2DlightedTriangle)
    {
      _2DlightedTriangle->release ();
      delete _2DlightedTriangle;
      _2DlightedTriangle = nullptr;
    }

    if (shaderTexture)
    {
      shaderTexture->release ();
      delete shaderTexture;
      shaderTexture = nullptr;
    }
    if (texture)
    {
      texture->release ();
      delete texture;
      texture = nullptr;
    }
    if (shaderColour)
    {
      shaderColour->release ();
      delete shaderColour;
      shaderColour = nullptr;
    }
    if (shaderDiffuseLight)
    {
      shaderDiffuseLight;
      delete shaderDiffuseLight;
      shaderDiffuseLight = nullptr;
    }

    if (universe)
    {
      universe->release ();
      delete universe;
      universe = nullptr;
    }
    if (core)
    {
      core->shutdown ();
      delete core;
      core = nullptr;
    }

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Game is successfully shut down." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};

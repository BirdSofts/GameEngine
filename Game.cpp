// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Game.h"
#include "Shared.h"


using namespace winrt::Windows::UI::Core;


Game::Game ( ::IUnknown* window, const int& width, const int& height ) :
  core ( nullptr ), universe ( nullptr ),
  initialized ( false ), allocated ( false ), paused ( false )
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
    core = new (std::nothrow) TheCore ( window, this, width, height );

    if (!core->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Initialization of framework failed!" );
    }

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "The game is successfully initialized." );

    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The game resources is successfully allocated." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Game::allocateResources ( void )
{
  try
  {

    allocated = false;

    // the game framework instantiation
    universe = new (std::nothrow) Universe ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    if (!universe->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Initialization of game universe failed!" );
    }

    shaderColour = new (std::nothrow) ShaderColour ( core->d3d->m_device.Get () );

    shaderTexture = new (std::nothrow) ShaderTexture ( core->d3d->m_device.Get () );

    _2Dtriangles = new (std::nothrow) Triangles ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    _2Dline = new (std::nothrow) Line ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    texture = new (std::nothrow) Texture<TargaHeader>
      ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get (), "./textures/clouds.tga" ); // a texture file
    if (!texture)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Instantiation of texture failed!" );
      return;
    }
    _2DtexturedTriangles = new (std::nothrow) TexturedTriangles ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    shaderDiffuseLight = new (std::nothrow) ShaderDiffuseLight ( core->d3d->m_device.Get () );

    _2DlightedTriangle = new (std::nothrow) LightedTriangle ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    _3Dcube = new (std::nothrow) Cube ( core->d3d->m_device.Get (), core->d3d->m_devCon.Get () );

    allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
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


    PointerProvider::getVariables ()->currentState = "gaming";


    MSG msg { 0 }; // a new message structure
    unsigned short counter { 1 };

    // setting the needed starting points
    core->timer->event ( "reset" ); // reset (start)

    universe->getCamera ()->setPosition ( 0.0f, 0.0f, -2.0f ); // set the start view

    universe->getDiffuseLight ()->setColour ( 0.2f, 0.6f, 0.6f, 1.0f ); // diffuse light colour
    universe->getDiffuseLight ()->setDirection ( 0.0f, 0.0f, 1.0f ); // light direction: point down the positive Z axis



    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

#pragma region peekLoop
      if ((counter % 10) == 0)
      {

        counter = 0;

        CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
          CoreProcessEventsOption::ProcessAllIfPresent );

      }
#pragma endregion



      // tick the timer to calculate a frame
      core->timer->tick ();
      // -- fps calculation
      core->frameStatistics ();

      if (!paused)
      {

        // -----------------------------------------------------------------------------------------------------------
        // a game loop purpose:
        // -- process inputted data
        // Todo add input processes so far

        render ();


        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        core->d3d->m_present ();


        // -----------------------------------------------------------------------------------------------------------
        // -- update the game universe/logic
        // Note the needed delta time
        update ();

      } else
      {

        // Todo add additional suitable procedure can be done in paused state



        CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
          CoreProcessEventsOption::ProcessOneIfPresent );
        // if no task go to hibernation
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );

        // Microsoft's resource (suspension)
        //CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
        //  CoreProcessEventsOption::ProcessOneAndAllPending );

      }

      counter++;
    } while (PointerProvider::getVariables ()->running == true);
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    return true;
  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return false;
  }
};


void Game::render ( void )
{
  try
  {

    core->d3d->m_clearBuffers ();

    universe->renderResources ();
    universe->getCamera ()->renderCamera ();

    if (core->d2d && core->debug)
      core->d2d->debugInfos (); // -- fps on screen representation



    // setting the active vertex/pixel shaders (active shader technique)
    core->d3d->m_devCon->VSSetShader ( shaderColour->getVertexShader (), nullptr, 0 );
    core->d3d->m_devCon->PSSetShader ( shaderColour->getPixelShader (), nullptr, 0 );

    // setting the active input layout
    core->d3d->m_devCon->IASetInputLayout ( shaderColour->getInputLayout () );

    // set the active vertex and index buffers (binds an array of vertex/index buffers to input-assembler stage)
    // basically, which vertices to put to graphics pipeline when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    core->d3d->m_devCon->IASetVertexBuffers ( 0, 1, _2Dtriangles->getVertexBuffer (), &strides, &offset );
    // set the active corresponding index buffer in the input assembler
    core->d3d->m_devCon->IASetIndexBuffer ( _2Dtriangles->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    core->d3d->m_devCon->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw indexed vertices, starting from vertex 0
    core->d3d->m_devCon->DrawIndexed ( _2Dtriangles->verticesCount, 0, 0 );



    core->d3d->m_devCon->IASetVertexBuffers ( 0, 1, _2Dline->getVertexBuffer (), &strides, &offset );
    core->d3d->m_devCon->IASetIndexBuffer ( _2Dline->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->m_devCon->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    core->d3d->m_devCon->DrawIndexed ( _2Dline->verticesCount, 0, 0 );



    // setting the active texture
    core->d3d->m_devCon->PSSetShaderResources ( 0, 1, texture->getTexture () );

    core->d3d->m_devCon->VSSetShader ( shaderTexture->getVertexShader (), nullptr, 0 );
    core->d3d->m_devCon->PSSetShader ( shaderTexture->getPixelShader (), nullptr, 0 );

    core->d3d->m_devCon->IASetInputLayout ( shaderTexture->getInputLayout () );

    // setting the active sampler
    core->d3d->m_devCon->PSSetSamplers ( 0, 1, shaderTexture->getSamplerState () );

    strides = sizeof ( VertexT );
    core->d3d->m_devCon->IASetVertexBuffers ( 0, 1, _2DtexturedTriangles->getVertexBuffer (), &strides, &offset );
    core->d3d->m_devCon->IASetIndexBuffer ( _2DtexturedTriangles->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    core->d3d->m_devCon->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->m_devCon->DrawIndexed ( _2DtexturedTriangles->verticesCount, 0, 0 );



    core->d3d->m_devCon->VSSetShader ( shaderDiffuseLight->getVertexShader (), nullptr, 0 );
    core->d3d->m_devCon->PSSetShader ( shaderDiffuseLight->getPixelShader (), nullptr, 0 );
    core->d3d->m_devCon->IASetInputLayout ( shaderDiffuseLight->getInputLayout () );
    core->d3d->m_devCon->PSSetSamplers ( 0, 1, shaderDiffuseLight->getSamplerState () );
    strides = sizeof ( VertexL );
    core->d3d->m_devCon->IASetVertexBuffers ( 0, 1, _2DlightedTriangle->getVertexBuffer (), &strides, &offset );
    core->d3d->m_devCon->IASetIndexBuffer ( _2DlightedTriangle->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->m_devCon->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->m_devCon->DrawIndexed ( _2DlightedTriangle->verticesCount, 0, 0 );



    core->d3d->m_devCon->IASetVertexBuffers ( 0, 1, _3Dcube->getVertexBuffer (), &strides, &offset );
    core->d3d->m_devCon->IASetIndexBuffer ( _3Dcube->getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->m_devCon->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    core->d3d->m_devCon->DrawIndexed ( _3Dcube->verticesCount, 0, 0 );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Game::update ( void )
{
  try
  {

    _2Dline->update ();
    universe->update ();

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


bool& Game::isPaused ( void )
{
  return paused;
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

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "The Game is successfully shut down." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};

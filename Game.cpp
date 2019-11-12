// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,12.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Game.h"
#include "Shared.h"


using namespace winrt::Windows::UI::Core;


Game::Game ( ::IUnknown* window, const int& width, const int& height ) :
  m_core ( nullptr ), m_universe ( nullptr ),
  m_allocated ( false ), m_paused ( false ), m_initialized ( false )
{
  try
  {

    m_shaderColour = nullptr;
    m_shaderTexture = nullptr;
    m_texture = nullptr;
    m_shaderDiffuseLight = nullptr;

    _2d_triangles = nullptr;
    _2d_line = nullptr;
    _2d_texturedTriangles = nullptr;
    _2d_lightedTriangle = nullptr;

    // the game framework instantiation
    m_core = new (std::nothrow) TheCore ( window, width, height );

    if (!m_core->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Initialization of framework failed!" );
    }

    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The game is successfully initialized." );

    m_allocateResources ();

    if (!m_allocated)
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The game resources is successfully allocated." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Game::~Game ( void )
//{
//
//};


void Game::m_allocateResources ( void )
{
  try
  {

    m_allocated = false;

    // the game framework instantiation
    m_universe = new (std::nothrow) Universe ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    if (!m_universe->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Initialization of game universe failed!" );
    }

    m_shaderColour = new (std::nothrow) ShaderColour ( m_core->m_getD3D ()->m_getDevice ().Get () );

    m_shaderTexture = new (std::nothrow) ShaderTexture ( m_core->m_getD3D ()->m_getDevice ().Get () );

    _2d_triangles = new (std::nothrow) Triangles ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    _2d_line = new (std::nothrow) Line ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    m_texture = new (std::nothrow) Texture<TargaHeader>
      ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get (), "./textures/clouds.tga" ); // a texture file
    if (!m_texture)
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Instantiation of texture failed!" );
      return;
    }
    _2d_texturedTriangles = new (std::nothrow) TexturedTriangles ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    m_shaderDiffuseLight = new (std::nothrow) ShaderDiffuseLight ( m_core->m_getD3D ()->m_getDevice ().Get () );

    _2d_lightedTriangle = new (std::nothrow) LightedTriangle ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    _3d_cube = new (std::nothrow) Cube ( m_core->m_getD3D ()->m_getDevice ().Get (), m_core->m_getD3D ()->m_getDevCon ().Get () );

    m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


const bool& Game::m_isReady ( void )
{
  return m_initialized;
};


const bool Game::m_run ( void )
{
  try
  {

    PointerProvider::getVariables ()->currentState = "gaming";


    MSG msg { 0 }; // a new message structure
    unsigned short counter { 1 };

    // setting the needed starting points
    m_core->m_getTimer ()->m_event ( "reset" ); // reset (start)

    m_universe->m_getCamera ()->setPosition ( 0.0f, 0.0f, -2.2f ); // set the start view

    const float colour [] { 0.2f, 0.6f, 0.6f, 1.0f };
    m_universe->m_getDiffuseLight ()->m_setColour ( colour ); // diffuse light colour

    const float direction [] { 0.0f, 0.0f, 1.0f };
    m_universe->m_getDiffuseLight ()->m_setDirection ( direction ); // light direction: point down the positive Z axis



    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

      if ((counter % 10) == 0)
      {

        counter = 0;

        CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
          CoreProcessEventsOption::ProcessAllIfPresent );

      }


      // tick the timer to calculate a frame
      m_core->m_getTimer ()->m_tick ();
      // -- fps calculation
      m_core->m_frameStatistics ();

      if (!m_paused)
      {

        // -----------------------------------------------------------------------------------------------------------
        // a game loop purpose:
        // -- process inputted data
        // Todo add input processes so far

        m_render ();


        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        m_core->m_getD3D ()->m_present ();


        // -----------------------------------------------------------------------------------------------------------
        // -- update the game universe/logic
        // Note the needed delta time
        m_update ();

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
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


void Game::m_render ( void )
{
  try
  {

    m_core->m_getD3D ()->m_clearBuffers ();

    m_universe->m_renderResources ();
    m_universe->m_getCamera ()->renderCamera ();

    if (m_core->m_getD2D () && m_core->m_isDebugging ())
      m_core->m_getD2D ()->m_debugInfos (); // -- fps on screen representation



    // setting the active vertex/pixel shaders (active shader technique)
    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shaderColour->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shaderColour->m_getPixelShader (), nullptr, 0 );

    // setting the active input layout
    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shaderColour->m_getInputLayout () );

    // set the active vertex and index buffers (binds an array of vertex/index buffers to input-assembler stage)
    // basically, which vertices to put to graphics pipeline when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_triangles->m_getVertexBuffer (), &strides, &offset );
    // set the active corresponding index buffer in the input assembler
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_triangles->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw indexed vertices, starting from vertex 0
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_triangles->m_getVerticesCount (), 0, 0 );



    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_line->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_line->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_line->m_getVerticesCount (), 0, 0 );



    // setting the active texture
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShaderResources ( 0, 1, m_texture->m_getTexture () );

    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shaderTexture->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shaderTexture->m_getPixelShader (), nullptr, 0 );

    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shaderTexture->m_getInputLayout () );

    // setting the active sampler
    m_core->m_getD3D ()->m_getDevCon ()->PSSetSamplers ( 0, 1, m_shaderTexture->m_getSamplerState () );

    strides = sizeof ( VertexT );
    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_texturedTriangles->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_texturedTriangles->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );

    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_texturedTriangles->m_getVerticesCount (), 0, 0 );



    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shaderDiffuseLight->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shaderDiffuseLight->m_getPixelShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shaderDiffuseLight->m_getInputLayout () );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetSamplers ( 0, 1, m_shaderDiffuseLight->m_getSamplerState () );
    strides = sizeof ( VertexL );
    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_lightedTriangle->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_lightedTriangle->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_lightedTriangle->m_getVerticesCount (), 0, 0 );



    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _3d_cube->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _3d_cube->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _3d_cube->m_getVerticesCount (), 0, 0 );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Game::m_update ( void )
{
  try
  {

    _2d_line->m_update ();
    m_universe->m_update ();

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


bool& Game::m_isPaused ( void )
{
  return m_paused;
};



TheCore* Game::m_getCore ( void )
{
  return m_core;
};


Universe* Game::m_getUniverse ( void )
{
  return m_universe;
};


void Game::m_release ( void )
{
  try
  {

    //unsigned long rC { 0 };
    //HRESULT hR;

    m_initialized = false;


    if (_3d_cube)
    {
      _3d_cube->m_release ();
      delete _3d_cube;
      _3d_cube = nullptr;
    }
    if (_2d_triangles)
    {
      _2d_triangles->m_release ();
      delete _2d_triangles;
      _2d_triangles = nullptr;
    }
    if (_2d_line)
    {
      _2d_line->m_release ();
      delete _2d_line;
      _2d_line = nullptr;
    }
    if (_2d_texturedTriangles)
    {
      _2d_texturedTriangles->m_release ();
      delete _2d_texturedTriangles;
      _2d_texturedTriangles = nullptr;
    }
    if (_2d_lightedTriangle)
    {
      _2d_lightedTriangle->m_release ();
      delete _2d_lightedTriangle;
      _2d_lightedTriangle = nullptr;
    }


    if (m_shaderTexture)
    {
      m_shaderTexture->m_release ();
      delete m_shaderTexture;
      m_shaderTexture = nullptr;
    }
    if (m_texture)
    {
      m_texture->m_release ();
      delete m_texture;
      m_texture = nullptr;
    }
    if (m_shaderColour)
    {
      m_shaderColour->m_release ();
      delete m_shaderColour;
      m_shaderColour = nullptr;
    }
    if (m_shaderDiffuseLight)
    {
      m_shaderDiffuseLight;
      delete m_shaderDiffuseLight;
      m_shaderDiffuseLight = nullptr;
    }


    if (m_universe)
    {
      m_universe->m_release ();
      delete m_universe;
      m_universe = nullptr;
    }
    if (m_core)
    {
      m_core->m_release ();
      delete m_core;
      m_core = nullptr;
    }

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The Game is successfully shut down." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

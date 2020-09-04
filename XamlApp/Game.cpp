// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Game.h"
#include "Shared.h"


Game::Game ( TheCore* coreObj ) :
  m_core ( coreObj ), m_universe ( nullptr ),
  m_allocated ( false ), m_paused ( false ), m_initialized ( false )
{
  try
  {

    m_shaderColour = nullptr;
    m_shaderTexture = nullptr;
    m_texture = nullptr;
    m_shaderTexDiffuseLight = nullptr;

    _2d_triangles = nullptr;
    _2d_line = nullptr;
    _2d_texturedTriangles = nullptr;
    _2d_lightedTriangle = nullptr;

    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "The Game is successfully initialized." );

    m_core->m_registerDeviceNotify ( this );

    m_allocateResources ();

    if (!m_allocated)
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The Game's resources is successfully allocated." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Game::~Game ( void )
//{
//
//};


void Game::OnDeviceEvents ( void )
{
  try
  {

    if (!m_core->m_isDeviceRestored ())
    {
      m_paused = true;

      // deallocate invalid resources
      m_release ();
    } else
    {
      // reallocate resources
      m_allocateResources ();

      m_paused = false;
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "main/gameThread",
                                                ex.what () );
  }
};


void Game::m_allocateResources ( void )
{
  try
  {

    m_allocated = false;

    // the game framework instantiation
    m_universe = new (std::nothrow) Universe ( m_core );
    if (!m_universe->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Initialization of game universe failed!" );
    }

    m_shaderColour = new (std::nothrow) ShaderColour ( m_core );

    m_shaderTexture = new (std::nothrow) ShaderTexture ( m_core );

    _2d_triangles = new (std::nothrow) Triangles ( m_core->m_getD3D ()->m_getDevice ().get (), m_core->m_getD3D ()->m_getDevCon ().get () );

    _2d_line = new (std::nothrow) Line ( m_core->m_getD3D ()->m_getDevice ().get (), m_core->m_getD3D ()->m_getDevCon ().get () );

    m_texture = new (std::nothrow) Texture ( m_core, "./textures/clouds.tga" ); // a texture file
    if (!m_texture)
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Instantiation of texture failed!" );
      return;
    }
    _2d_texturedTriangles = new (std::nothrow) TexturedTriangles ( m_core->m_getD3D ()->m_getDevice ().get (), m_core->m_getD3D ()->m_getDevCon ().get () );

    m_shaderColDiffAmbiLight = new (std::nothrow) ShaderColDiffAmbiLight ( m_core );

    m_shaderTexDiffuseLight = new (std::nothrow) ShaderTexDiffLight ( m_core );

    m_terrain = new (std::nothrow) Terrain ( m_core, dynamic_cast<Shader*>(m_shaderColDiffAmbiLight) );
    if (!m_terrain->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Initialization of terrain failed!" );
    }

    _2d_lightedTriangle = new (std::nothrow) LightedTriangle ( m_core->m_getD3D ()->m_getDevice ().get (), m_core->m_getD3D ()->m_getDevCon ().get () );

    _3d_cube = new (std::nothrow) Cube ( m_core->m_getD3D ()->m_getDevice ().get (), m_core->m_getD3D ()->m_getDevCon ().get () );

    _3d_meshCube = new (std::nothrow) MeshCube ( m_core, dynamic_cast<Shader*>(m_shaderColour), m_texture );

    m_allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


const bool Game::m_run ( void )
{
  try
  {

    PointerProvider::getVariables ()->currentState = "gaming";

    unsigned short counter { 1 };

    // setting the needed starting points
    m_core->m_getTimer ()->m_event ( typeEvent::reset ); // reset (start)

    m_universe->m_getCamera ()->m_setPosition ( 0.0f, 0.2f, -5.0f ); // set the start view

    const float colourA [] { 0.2f, 0.2f, 0.2f, 1.0f };
    m_universe->m_getDiffuseLight ()->m_setAmbientColour ( colourA ); // ambient light colour

    const float colourD [] { 1.0f, 1.0f, 1.0f, 1.0f };
    m_universe->m_getDiffuseLight ()->m_setDiffuseColour ( colourD ); // diffuse light colour

    const float direction [] { 0.0f, -1.0f, 0.0f };
    m_universe->m_getDiffuseLight ()->m_setDirection ( direction ); // light direction: point down the positive Y axis



    //auto workItemHandler =
    //  winrt::Windows::System::Threading::WorkItemHandler
    //  ( [this]( winrt::Windows::Foundation::IAsyncAction )
    //    {
    //      do
    //      {

    //        if (!m_paused)
    //        {
    //          m_update ();
    //        }

    //        std::this_thread::sleep_for ( std::chrono::milliseconds ( 10 ) );

    //      } while (PointerProvider::getVariables ()->running == true);

    //    } );
    //// run the task on a high priority background thread
    //winrt::Windows::Foundation::IAsyncAction loop;
    //loop = winrt::Windows::System::Threading::ThreadPool::RunAsync (
    //  workItemHandler,
    //  winrt::Windows::System::Threading::WorkItemPriority::High,
    //  winrt::Windows::System::Threading::WorkItemOptions::TimeSliced );



    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

      if ((counter % 60) == 0)
      {

        counter = 0;

        // additional in-between processes if any need to execute
        // like messages of a core window

      }


      // tick the timer to calculate a frame
      m_core->m_getTimer ()->m_tick ();






      if (!m_paused)
      {

        // -- fps calculation
        m_core->m_frameStatistics ();

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

        if (counter == 59)
          PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "gameThread",
                                                      "Logger service test: (each log represent 60 delivered frames!) ^_^" );

      } else
      {

        // additional suitable procedure can be done in paused state

        // if no task go to hibernation
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );

      }


      counter++;
    } while ((PointerProvider::getVariables ()->running == true));
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "gameThread",
                                                ex.what () );
    return false;
  }
};


void Game::m_render ( void )
{
  try
  {

    // engine test
    //int temp { 0 };
    //do
    //{
    //  temp++;
    //} while (temp < 10000000);

    m_core->m_getD3D ()->m_clearBuffers ();

    m_universe->m_renderResources ();
    m_universe->m_getCamera ()->m_renderCamera ();

    if (m_core->m_getD2D () && m_core->m_isDebugging ())
      m_core->m_getD2D ()->m_debugInfos (); // -- fps on screen representation

    m_terrain->m_render ();


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


    //if (!_2d_line->m_getMapped ())
    //{
    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_line->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_line->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_line->m_getVerticesCount (), 0, 0 );
    //}



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



    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shaderTexDiffuseLight->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shaderTexDiffuseLight->m_getPixelShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shaderTexDiffuseLight->m_getInputLayout () );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetSamplers ( 0, 1, m_shaderTexDiffuseLight->m_getSamplerState () );
    strides = sizeof ( VertexTL );
    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _2d_lightedTriangle->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _2d_lightedTriangle->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _2d_lightedTriangle->m_getVerticesCount (), 0, 0 );



    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, _3d_cube->m_getVertexBuffer (), &strides, &offset );
    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( _3d_cube->m_getIndexBuffer (), DXGI_FORMAT_R32_UINT, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( _3d_cube->m_getVerticesCount (), 0, 0 );


    _3d_meshCube->m_render ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "gameThread",
                                                ex.what () );
  }
};


void Game::m_update ( void )
{
  try
  {

    _2d_line->m_update ();
    _3d_meshCube->m_update ();

    //m_universe->m_update ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "gameThread",
                                                ex.what () );
  }
};


void Game::m_updateDisplay ( void )
{
  m_universe->m_createResources ();
};


void Game::m_release ( void )
{
  try
  {

    //Todo:
    // procedure description:
    // save state of game objects and release them all
    // save the game process so user doesn't get angry
    //xx trim resources and prepare power suspension

    //unsigned long rC { 0 };
    //HRESULT hR;

    m_initialized = false;


    // object models
    if (_3d_meshCube)
    {
      _3d_meshCube->m_releaseData ();
      delete _3d_cube;
      _3d_cube = nullptr;
    }
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


    // game's resources
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
    if (m_shaderTexDiffuseLight)
    {
      m_shaderTexDiffuseLight;
      delete m_shaderTexDiffuseLight;
      m_shaderTexDiffuseLight = nullptr;
    }
    if (m_universe)
    {
      m_universe->m_release ();
      delete m_universe;
      m_universe = nullptr;
    }

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Game's resources are successfully released." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

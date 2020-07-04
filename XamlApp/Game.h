// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef GAME_H
#define GAME_H


#include "Core.h"
#include "directx/Universe.h"
#include "resources/Shader.h"
#include "resources/Texture.h"
#include "objects/Polygons.h"


class Game
{
  friend class TheCore;
private:
  TheCore* m_core; // pointer to the application core

  Universe* m_universe; // world, camera, lights...

  ShaderColour* m_shaderColour;
  ShaderTexture* m_shaderTexture;
  Texture<TargaHeader>* m_texture; // texture resource
  ShaderDiffuseLight* m_shaderDiffuseLight;

  // 2D models:
  Triangles* _2d_triangles; // three triangles
  Line* _2d_line; // a line (clockwise turn, dynamic rewrite)
  TexturedTriangles* _2d_texturedTriangles; // two textured triangles
  LightedTriangle* _2d_lightedTriangle; // one triangle illuminated by diffuse light

  // 3D models
  Cube* _3d_cube;

  bool m_allocated; // true if resources allocation was successful
  bool m_paused; // true if the game is paused
  bool m_initialized; // true if initialization was successful

  void m_allocateResources ( void ); // resources creation
public:
  Game ( TheCore* ); // game initialization
  ~Game ( void ) { /**/ };

  const bool m_run ( void ); // game engine loop
  void m_render ( void ); // render the scene
  void m_update ( void ); // updating the game world
  void m_onSuspending ( void ); // suspension preparation

  void m_validate ( void ) { m_core->m_validate (); }; // game's resources/objects validation/reallocation

  const bool& m_isReady ( void ) { return m_initialized; }; // get the initialized state
  bool& m_isPaused ( void ) { return m_paused; }; // get the paused state
  TheCore* m_getCore ( void ) { return m_core; }; // get the pointer to D3D core
  Universe* m_getUniverse ( void ) { return m_universe; }; // get the pointer to game universe

};


#endif // !GAME_H

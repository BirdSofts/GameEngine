// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef TERRAIN_H
#define TERRAIN_H


#include "../Core.h"
#include "../objects/ModelFormats.h"
#include "../resources/Shader.h"


struct TerrainMap
{
  float x, y, z;
  float nX, nY, nZ;
};


// Terrain wrapper
class Terrain
{
private:
  TheCore* m_core; // pointer to application core
  Shader* m_shader; // pointer to shader base class

  bool m_dynamic; // true: dynamic usage + write access for CPU
  unsigned short m_density; // how many lines within one tenth space
  DirectX::XMFLOAT3 m_startPosition; // drawing starts from this position
  //unsigned short m_id; // when more than one terrain, index them in an imaginary matrix

  ID3D11Buffer* m_vertexBuffer; // models' vertex buffer
  ID3D11Buffer* m_indexBuffer; // models' index buffer
  unsigned long m_entitiesCount; // vertices/indices count

  bool m_initialized; // true if initialization was successful

  bool m_allocate ( void ); // resource allocation process
  bool m_allocate2 ( void ); // resource allocation process
public:
  Terrain ( TheCore* coreObj, Shader* shadObj );
  //~Terrain ( void );

  void m_render ( void ); // resource render process
  void m_release ( void ); // release allocated resources

  bool& const m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  ID3D11Buffer** const m_getVertexBuffer ( void ) { return &m_vertexBuffer; }; // get pointer to vertex buffer
  ID3D11Buffer* const m_getIndexBuffer ( void ) { return m_indexBuffer; }; // get pointer to index buffer
};


#endif // !TERRAIN_H

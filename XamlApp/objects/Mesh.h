// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef MESH_H
#define MESH_H


#include "Core.h"
#include "ModelFormats.h"
#include "resources/Shader.h"
#include "resources/Texture.h"


// Todo reconstruction to objects and their properties
// a mesh object model:
// mesh creation algorithm (vertices, texture, normal)
// mesh properties: inherit texture class (+ changes), inherit shader class


// mesh models base class
template <class tType>
class Mesh
{
private:
  std::string m_entryPoint;
  bool m_dynamic; // true: dynamic usage + write access for CPU
protected:
  TheCore* m_core;
  Shader* m_shader;
  Texture* m_texture;

  DirectX::XMFLOAT3 m_position;
  float m_scale;

  D3D11_BUFFER_DESC m_vertexBufferDesc;
  // 2D/3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  ID3D11Buffer* m_vertexBuffer; // models' vertex buffer

  D3D11_SUBRESOURCE_DATA m_vertexBufferDate; // to interface with the object model vertices as resources

  D3D11_BUFFER_DESC m_indexBufferDesc;
  ID3D11Buffer* m_indexBuffer; // models' index buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.

  bool m_allocate ( tType*, unsigned long*, unsigned long& ); // object model resources allocation
public:
  Mesh ( TheCore* coreObj, Shader* shadObj, Texture* texObj, std::string entry, bool rewrite );
  //~Model ( void );

  void m_render ( void ); // render object model
  void m_updateBuffer ( tType* ); // update object model
  void m_release ( void ); // release object model

  DirectX::XMFLOAT3& m_getPosition ( void ) { return m_position; }; // get/set object position
  float& m_getScale ( void ) { return m_scale; }; // get/set object scale
  ID3D11Buffer** const m_getVertexBuffer ( void ) { return &m_vertexBuffer; }; // get pointer to vertex buffer
  ID3D11Buffer* const m_getIndexBuffer ( void ) { return m_indexBuffer; }; // get pointer to index buffer
};
void MeshClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


class MeshCube : public Mesh<Vertex>
{
private:
  Vertex* m_verticesData; // object model vertices data
  unsigned long* m_verticesIndex; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  bool m_allocated; // true after successful resource allocation
public:
  MeshCube ( TheCore* coreObj, Shader* shadObj, Texture* texObj );
  //~MeshCube ( void );

  void m_update ( void );
  void m_releaseData ( void ); // release object model data

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


#endif // !MESH_H

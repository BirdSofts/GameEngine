// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef POLYGONS_H
#define POLYGONS_H


#include "ModelFormats.h"


// object models base class
template <class tType>
class Model
{
private:
  std::string m_entryPoint;
  bool m_dynamic; // true: dynamic usage + write access for CPU
protected:
  ID3D11Device3* m_device; // pointer to DirectX device
  ID3D11DeviceContext3* m_deviceContext; // pointer to DirectX device context

  D3D11_BUFFER_DESC m_vertexBufferDesc;
  // 2D/3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  ID3D11Buffer* m_vertexBuffer; // models' vertex buffer

  D3D11_SUBRESOURCE_DATA m_subResourceDate; // to interface with the object model vertices as resources

  D3D11_BUFFER_DESC m_indexBufferDesc;
  ID3D11Buffer* m_indexBuffer; // models' index buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.

  bool m_allocate ( tType* data, unsigned long* index, unsigned long& count ); // object model resources allocation
public:
  Model ( ID3D11Device3* dev, ID3D11DeviceContext3* devC, std::string entry, bool rewrite );
  //~Model ( void );

  void m_release ( void ); // release the object model

  ID3D11Buffer** const m_getVertexBuffer ( void ) { return &m_vertexBuffer; }; // get pointer to vertex buffer
  ID3D11Buffer* const m_getIndexBuffer ( void ) { return m_indexBuffer; }; // get pointer to index buffer
};
void ModelClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


class Triangles : public Model<Vertex>
{
private:
  Vertex m_verticesData [9]; // object model vertices data
  unsigned long m_verticesIndices [9]; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  bool m_allocated; // true after successful resource allocation
public:
  Triangles ( ID3D11Device3* dev, ID3D11DeviceContext3* devC );
  //~Triangles ( void );

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


class Line : public Model<Vertex>
{
private:
  Vertex m_verticesData [2]; // object model vertices data
  unsigned long m_verticesIndex [2]; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  D3D11_MAPPED_SUBRESOURCE m_mappedRes; // updating the resource
  // note DirectX 11: D3D11_MAPPED_SUBRESOURCE

  bool m_allocated; // true after successful resource allocation
public:
  Line ( ID3D11Device3* dev, ID3D11DeviceContext3* devC );
  //~Line ( void );

  void m_update ( void );

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


class TexturedTriangles : public Model<VertexT>
{
private:
  VertexT m_verticesData [6]; // object model vertices data
  unsigned long m_verticesIndex [6]; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  bool m_allocated; // true after successful resource allocation
public:
  TexturedTriangles ( ID3D11Device3* dev, ID3D11DeviceContext3* devC );
  //~TexturedTriangles ( void );

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


class LightedTriangle : public Model<VertexL>
{
private:
  VertexL m_verticesData [3]; // object model vertices data
  unsigned long m_verticesIndex [3]; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  bool m_allocated; // true after successful resource allocation
public:
  LightedTriangle ( ID3D11Device3* dev, ID3D11DeviceContext3* devC );
  //~LightedTriangle ( void );

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


class Cube : public Model<VertexL>
{
private:
  VertexL* m_verticesData; // object model vertices data
  unsigned long* m_verticesIndex; // object model vertices indices
  unsigned long m_verticesCount; // object model vertices count

  bool m_allocated; // true after successful resource allocation
public:
  Cube ( ID3D11Device3* dev, ID3D11DeviceContext3* devC );
  //~Cube ( void );

  const unsigned long& m_getVerticesCount ( void ) { return m_verticesCount; }; // get vertices count
};


#endif // !POLYGONS_H

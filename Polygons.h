// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#ifndef POLYGONS_H
#define POLYGONS_H


#include "ModelFormats.h"


// object models base class
template <class tType>
class Model
{
private:
  std::string entryPoint;
  bool dynamic; // true: dynamic usage + write access for CPU
protected:
  ID3D11Device* device; // pointer to DirectX device
  ID3D11DeviceContext* devCon; // pointer to DirectX device context

  D3D11_BUFFER_DESC vertexBufferDesc;
  // 2D/3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  ID3D11Buffer* vertexBuffer; // models' vertex buffer

  D3D11_SUBRESOURCE_DATA subResourceDate; // to interface with the object model vertices as resources

  D3D11_BUFFER_DESC indexBufferDesc;
  ID3D11Buffer* indexBuffer; // models' index buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.

  bool allocate ( tType*, unsigned long*, unsigned long& ); // object model resources allocation
public:
  Model ( ID3D11Device*, ID3D11DeviceContext*, std::string, bool );
  ID3D11Buffer** const getVertexBuffer ( void ); // vertex buffer
  ID3D11Buffer* const getIndexBuffer ( void ); // index buffer
  void release ( void ); // release the object model
};
void O2DmodelClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


class Triangles : public Model<Vertex>
{
private:
  Vertex verticesData [9]; // object model vertices dataobject model vertices data
  unsigned long verticesIndices [9]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  Triangles ( ID3D11Device*, ID3D11DeviceContext* );
};


class Line : public Model<Vertex>
{
private:
  Vertex verticesData [2]; // object model vertices dataobject model vertices data
  unsigned long verticesIndex [2]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  D3D11_MAPPED_SUBRESOURCE mappedRes; // updating the resource
  // note DirectX 11: D3D11_MAPPED_SUBRESOURCE

  Line ( ID3D11Device*, ID3D11DeviceContext* );
  void update ( void );
};


class TexturedTriangles : public Model<VertexT>
{
private:
  VertexT verticesData [6]; // object model vertices dataobject model vertices data
  unsigned long verticesIndex [6]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  TexturedTriangles ( ID3D11Device*, ID3D11DeviceContext* );
};


class LightedTriangle : public Model<VertexL>
{
private:
  VertexL verticesData [3]; // object model vertices dataobject model vertices data
  unsigned long verticesIndex [3]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  LightedTriangle ( ID3D11Device*, ID3D11DeviceContext* );
};


class Cube : public Model<VertexL>
{
private:
  VertexL* verticesData; // object model vertices dataobject model vertices data
  unsigned long* verticesIndex; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  Cube ( ID3D11Device*, ID3D11DeviceContext* );
};


#endif // !POLYGONS_H

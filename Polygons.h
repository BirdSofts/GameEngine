// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef POLYGONS_H
#define POLYGONS_H


#include <d3d10_1.h>
#include <DirectXMath.h>
#include <string>

#include "ModelFormats.h"


// object models base class
template <class tType>
class Model
{
private:
  std::wstring entryPoint;
  bool dynamic; // true: dynamic usage + write access for CPU
protected:
  ID3D10Device1* device; // pointer to Direct3D device

  D3D10_BUFFER_DESC vertexBufferDesc;
  // 2D/3D models buffer containers, drawn by invoked shaders that are compiled into vertex/pixel shaders
  ID3D10Buffer* vertexBuffer; // models' vertex buffer

  D3D10_SUBRESOURCE_DATA subResourceDate; // to interface with the object model vertices as resources

  D3D10_BUFFER_DESC indexBufferDesc;
  ID3D10Buffer* indexBuffer; // models' index buffer
  // Note index buffers purposes: record the location of each vertex introduced in vertex buffer,
  // achieving much hider speed, and helps to cache the vertices data in faster locations of video memory.

  bool allocate ( tType*, unsigned long*, unsigned long& ); // object model resources allocation
public:
  Model ( ID3D10Device1*, std::wstring, bool );
  ID3D10Buffer** const getVertexBuffer ( void ); // vertex buffer
  ID3D10Buffer* const getIndexBuffer ( void ); // index buffer
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

  Triangles ( ID3D10Device1* );
};


class Line : public Model<Vertex>
{
private:
  Vertex verticesData [2]; // object model vertices dataobject model vertices data
  unsigned long verticesIndex [2]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  D3D10_MAPPED_TEXTURE2D mappedRes; // updating the resource
  // note DirectX 11: D3D11_MAPPED_SUBRESOURCE

  Line ( ID3D10Device1* );
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

  TexturedTriangles ( ID3D10Device1* );
};


class LightedTriangle : public Model<VertexL>
{
private:
  VertexL verticesData [3]; // object model vertices dataobject model vertices data
  unsigned long verticesIndex [3]; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  LightedTriangle ( ID3D10Device1* );
};


class Cube : public Model<VertexL>
{
private:
  VertexL* verticesData; // object model vertices dataobject model vertices data
  unsigned long* verticesIndex; // object model vertices dataobject model vertices indices
public:
  unsigned long verticesCount; // object model vertices dataobject model vertices count
  bool allocated; // true after successful resource allocation

  Cube ( ID3D10Device1* );
};


#endif // !POLYGONS_H

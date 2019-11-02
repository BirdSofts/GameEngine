// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef MODELFORMATS_H
#define MODELFORMATS_H


#include <DirectXMath.h>


struct Vertex
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT4 color;
};


struct VertexT
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT2 texture;
};


struct VertexL
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT2 texture;
  DirectX::XMFLOAT3 normal; // normal light
};


// Format: vertex containing the texture and diffuse light properties
// each triangle is read and written in one line
class VertexTexDiffuseL
{
public:
  static unsigned int read ( const char*, VertexL** ); // 
  static void write ( VertexL*, unsigned int& );
};


#endif // MODELFORMATS_H

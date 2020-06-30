// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,28.06.2020</changed>
// ********************************************************************************

#ifndef MODELFORMATS_H
#define MODELFORMATS_H


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

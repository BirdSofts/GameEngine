// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,05.11.2019</changed>
// ********************************************************************************

#ifndef TEXTURE_H
#define TEXTURE_H


// targa texture file type
struct TargaHeader
{
  unsigned char dataOne [12];
  unsigned short width;
  unsigned short height;
  unsigned char bpp; // bit per pixel
  unsigned short dataTwo;
};


// png texture file typev
struct PngHeader {};


// operations on textures wrapper
template <typename fileType>
class Texture
{
private:
  ID3D11Device* device; // pointer to DirecX device
  ID3D11DeviceContext* devCon; // pointer to DirectX device context

  fileType file; // different file formats
  unsigned char* data; // raw file data holder
  ID3D11Texture2D* texture; // structured texture data, rendered by DirectX
  ID3D11ShaderResourceView* textureView; // resource view, holder of texture data drawn by shader

  bool Load ( const char* ); // specialized defined file loader
public:
  Texture ( ID3D11Device*, ID3D11DeviceContext*, const char* );
  ID3D11ShaderResourceView** const getTexture ( void ); // get texture data (unsuccessful texture file loading: nullptr)
  void release ( void ); // resource releaser
};
void TextureClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !TEXTURE_H

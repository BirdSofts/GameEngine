// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,28.06.2020</changed>
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
  ID3D11Device* m_device; // pointer to DirecX device
  ID3D11DeviceContext* m_deviceContext; // pointer to DirectX device context

  fileType m_file; // different file formats
  unsigned char* m_data; // raw file data holder
  ID3D11Texture2D* m_texture; // structured texture data, rendered by DirectX
  ID3D11ShaderResourceView* m_textureView; // resource view, holder of texture data drawn by shader

  bool m_initialized; // true in case of a successful procedure

  bool m_load ( const char* ); // specialized defined file loader
public:
  Texture ( ID3D11Device*, ID3D11DeviceContext*, const char* );
  //~Texture ( void );
  const bool& m_isInitialized ( void ); // get the initialized state

  ID3D11ShaderResourceView** const m_getTexture ( void ); // get texture data (unsuccessful texture file loading: nullptr)
  void m_release ( void ); // resource releaser
};
void TextureClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !TEXTURE_H

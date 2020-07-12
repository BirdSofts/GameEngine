// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
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


// png texture file type
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

  bool m_load ( const char* path ); // specialized file loader
public:
  Texture ( ID3D11Device* dev, ID3D11DeviceContext* devC, const char* path );
  //~Texture ( void );

  void m_release ( void ); // resource releaser

  const bool& Texture<fileType>::m_isInitialized ( void ) { return m_initialized; } // get the initialized state
  ID3D11ShaderResourceView** const Texture<fileType>::m_getTexture ( void ) { return &m_textureView; }; // get texture data
};
void TextureClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !TEXTURE_H

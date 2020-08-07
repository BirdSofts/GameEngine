// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef TEXTURE_H
#define TEXTURE_H


#include <Core.h>


// targa (tga) texture type
struct TgaHeader
{
  unsigned char dataOne [12];
  unsigned short width;
  unsigned short height;
  unsigned char bpp; // bit per pixel
  unsigned short dataTwo;
};


// png texture type
struct PngHeader {};


// operations on textures wrapper
class Texture
{
private:
  TheCore* m_core;

  TgaHeader* m_fileTga; // different file formats
  PngHeader* m_filePng;
  unsigned char* m_data; // raw data of file
  ID3D11Texture2D* m_texture; // structured texture data, rendered by DirectX
  ID3D11ShaderResourceView* m_textureView; // resource view, holder of texture data drawn by shader

  bool m_initialized; // true in case of a successful procedure

  bool m_loadTga ( const char* path ); // specialized file loader
  bool m_loadPng ( const char* path ); // specialized file loader
public:
  Texture ( TheCore* coreObj, const char* path );
  //~Texture ( void );

  void m_release ( void ); // resource releaser

  const bool& Texture::m_isInitialized ( void ) { return m_initialized; } // get the initialized state
  ID3D11ShaderResourceView** const Texture::m_getTexture ( void ) { return &m_textureView; }; // get texture data
};
//void TextureClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !TEXTURE_H

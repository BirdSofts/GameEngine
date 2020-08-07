// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef SHADER_H
#define SHADER_H


#include "Core.h"
#include "../directx/Universe.h"


struct Buffer // shader buffer (compiled .cso files holder)
{
  byte* buffer;
  long long size;

  Buffer ( void );
  ~Buffer ( void );

  void release ( void );
};


// shader base class wrapper
class Shader
{
private:
  TheCore* m_core; // pointer to Direct device

  ID3D10Blob* m_vertexBuffer; // texture buffer
  ID3D10Blob* m_pixelBuffer; // pixel buffer
  ID3D10Blob* m_errorMsg; // HLSL compilation errors

  ID3D11VertexShader* m_vertexShader; // standard vertex shader
  ID3D11PixelShader* m_pixelShader; // standard pixel shader
  ID3D11InputLayout* m_inputLayout; // standard input layout
  ID3D11SamplerState* m_samplerState; // standard sampler state (textured shaders)

  std::string m_entryPoint;
public:
  Shader ( TheCore* coreObj, std::string entry );
  //~Shader ( void );

  void m_loadCompiled ( std::string& fileName, Buffer* csoBuffer ); // read shader data (compiled .cso files)
  bool m_initializeCompiled ( std::string* filePaths, D3D11_INPUT_ELEMENT_DESC* polygonLayout,
                              unsigned int elmCount ); // rendering pipeline (GPU initialization)
  bool m_compile ( LPCWSTR* files ); // compile HLSL using DirectX APIs
  bool m_initialize ( D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int elmCount,
                      D3D11_SAMPLER_DESC* sampler ); // rendering pipeline (GPU initialization)
  void m_release ( void ); // release the shaders resources

  ID3D11VertexShader* const m_getVertexShader ( void ) { return m_vertexShader; };
  ID3D11PixelShader* const m_getPixelShader ( void ) { return m_pixelShader; };
  ID3D11InputLayout* const m_getInputLayout ( void ) { return m_inputLayout; };
  ID3D11SamplerState** const m_getSamplerState ( void ) { return &m_samplerState; };
};


class ShaderColour : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC m_polygonLayoutDesc [2]; // input layout description
  unsigned int m_elementsCount;
  std::string m_files [2];
  //unsigned short filesCount;

  bool m_initialized; // true if initialization was successful
public:
  ShaderColour ( TheCore* coreObj );
  //~ShaderColour ( void );

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
};


class ShaderTexture : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC m_polygonLayoutDesc [2];
  unsigned int m_elementsCount;
  D3D11_SAMPLER_DESC m_samplerDesc; // tecture sampler state description
  LPCWSTR m_files [2];
  //unsigned short filesCount;

  bool m_initialized; // true if initialization was successful
public:
  ShaderTexture ( TheCore* coreObj );
  //~ShaderTexture ( void );

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
};


class ShaderColDiffLight : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC m_polygonLayoutDesc [3];
  unsigned int m_elementsCount;
  LPCWSTR m_files [2];
  //unsigned short filesCount;

  bool m_initialized; // true if initialization was successful
public:
  ShaderColDiffLight ( TheCore* coreObj );
  //~ShaderDiffuseLight ( void );

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
};


class ShaderTexDiffLight : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC m_polygonLayoutDesc [3];
  unsigned int m_elementsCount;
  D3D11_SAMPLER_DESC m_samplerDesc;
  LPCWSTR m_files [2];
  //unsigned short filesCount;

  bool m_initialized; // true if initialization was successful
public:
  ShaderTexDiffLight ( TheCore* coreObj );
  //~ShaderDiffuseLight ( void );

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
};


#endif // !SHADER_H

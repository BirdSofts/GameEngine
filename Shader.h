// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#ifndef SHADER_H
#define SHADER_H


#include "Universe.h"


// shader buffer
class Shader
{
private:
  struct Buffer // shader buffer (compiled .cso files holder)
  {
    byte* buffer;
    long long size;
    Buffer ( void );
    ~Buffer ( void );
    void release ( void );
  };
protected:
  ID3D11Device* m_device; // pointer to Direct device

  ID3D10Blob* m_vertexBuffer; // texture buffer
  ID3D10Blob* m_pixelBuffer; // pixel buffer
  ID3D10Blob* m_errorMsg; // HLSL compilation errors

  ID3D11VertexShader* m_vertexShader; // standard vertex shader
  ID3D11PixelShader* m_pixelShader; // standard pixel shader
  ID3D11InputLayout* m_inputLayout; // standard input layout
  ID3D11SamplerState* m_samplerState; // standard sampler state (textured shaders)

  std::string m_entryPoint;
public:
  Shader ( ID3D11Device*, std::string );
  //~Shader ( void );

  void m_loadCompiled ( std::string&, Buffer* ); // read shader data (compiled .cso files)
  bool m_initializeCompiled ( std::string*, D3D11_INPUT_ELEMENT_DESC*,
                              unsigned int ); // rendering pipeline (GPU initialization)

  bool m_compile ( LPCWSTR* ); // compile HLSL using DirectX APIs
  bool m_initialize ( D3D11_INPUT_ELEMENT_DESC*, unsigned int,
                      D3D11_SAMPLER_DESC* ); // rendering pipeline (GPU initialization)

  ID3D11VertexShader* const m_getVertexShader ( void );
  ID3D11PixelShader* const m_getPixelShader ( void );
  ID3D11InputLayout* const m_getInputLayout ( void );
  ID3D11SamplerState** const m_getSamplerState ( void );
  void m_release ( void ); // release the shaders resources
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
  ShaderColour ( ID3D11Device* );
  //~ShaderColour ( void );
  const bool& m_isInitialized ( void ); // get the initialized state
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
  ShaderTexture ( ID3D11Device* );
  //~ShaderTexture ( void );
  const bool& m_isInitialized ( void ); // get the initialized state
};


class ShaderDiffuseLight : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC m_polygonLayoutDesc [3];
  unsigned int m_elementsCount;
  D3D11_SAMPLER_DESC m_samplerDesc;
  LPCWSTR m_files [2];
  //unsigned short filesCount;

  bool m_initialized; // true if initialization was successful
public:
  ShaderDiffuseLight ( ID3D11Device* );
  //~ShaderDiffuseLight ( void );
  const bool& m_isInitialized ( void ); // get the initialized state
};


#endif // !SHADER_H

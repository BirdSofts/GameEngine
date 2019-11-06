// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
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
  ID3D11Device* device; // pointer to Direct device

  ID3D10Blob* vertexBuffer; // texture buffer
  ID3D10Blob* pixelBuffer; // pixel buffer
  ID3D10Blob* errorMsg; // HLSL compilation errors

  ID3D11VertexShader* vertexShader; // standard vertex shader
  ID3D11PixelShader* pixelShader; // standard pixel shader
  ID3D11InputLayout* inputLayout; // standard input layout
  ID3D11SamplerState* samplerState; // standard sampler state (textured shaders)

  std::string entryPoint;
public:
  Shader ( ID3D11Device*, std::string );
  void loadCompiled ( std::string&, Buffer* ); // read shader data (compiled .cso files)
  bool initializeCompiled ( std::string*,
                            D3D11_INPUT_ELEMENT_DESC*, unsigned short ); // rendering pipeline (GPU initialization)
  bool compile ( LPCWSTR* ); // compile HLSL using DirectX APIs
  bool initialize ( D3D11_INPUT_ELEMENT_DESC*, unsigned short,
                    D3D11_SAMPLER_DESC* ); // rendering pipeline (GPU initialization)
  ID3D11VertexShader* const getVertexShader ( void );
  ID3D11PixelShader* const getPixelShader ( void );
  ID3D11InputLayout* const getInputLayout ( void );
  ID3D11SamplerState** const getSamplerState ( void );
  void release ( void ); // release the shaders resources
};


class ShaderColour : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC polygonLayoutDesc [2]; // input layout description
  unsigned short elementsCount;
  std::string files [2];
  //unsigned short filesCount;

  bool initialized; // true if initialization was successful
public:
  ShaderColour ( ID3D11Device* );
  const bool& isInitialized ( void ); // get the initialized state
};


class ShaderTexture : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC polygonLayoutDesc [2];
  unsigned short elementsCount;
  D3D11_SAMPLER_DESC samplerDesc; // tecture sampler state description
  LPCWSTR files [2];
  //unsigned short filesCount;

  bool initialized; // true if initialization was successful
public:
  ShaderTexture ( ID3D11Device* );
  const bool& isInitialized ( void ); // get the initialized state
};


class ShaderDiffuseLight : public Shader
{
private:
  D3D11_INPUT_ELEMENT_DESC polygonLayoutDesc [3];
  unsigned int elementsCount;
  D3D11_SAMPLER_DESC samplerDesc;
  LPCWSTR files [2];
  //unsigned short filesCount;

  bool initialized; // true if initialization was successful
public:
  ShaderDiffuseLight ( ID3D11Device* );
  const bool& isInitialized ( void ); // get the initialized state
};


#endif // !SHADER_H

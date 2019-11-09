// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Shader.h"
#include "Shared.h"


Shader::Shader ( ID3D11Device* dev, std::string entry ) :
  m_device ( dev ), m_entryPoint ( " Entry Point: " + entry )
{

  // shaders, introduced in several different types,
  // are used to actually render vertices or pixels to the screen in the render process,
  // controlled by the graphics rendering pipeline, which is programmed by these shaders,
  // and each of them is a small program that controls one step of the pipeline.
  // the process in nature takes vertices as input and results in fully rendered images,
  // in which each type of shader is run many times based on its different nature.
  // note that shaders are written in HLSL (High Level Shader Language),
  // Visual Studio is able to create HLSL programs, and after compiling,
  // the HLSL file is compiled into CSO (Compiled Shader Objects), usable by the running program.
  // note that it is essential to write high efficient vertex shaders: http://www.rastertek.com/dx10s2tut04.html

  m_vertexBuffer = nullptr;
  m_pixelBuffer = nullptr;
  m_errorMsg = nullptr;

  m_vertexShader = nullptr;
  m_pixelShader = nullptr;
  m_inputLayout = nullptr;
  m_samplerState = nullptr;

};


//Shader::~Shader ( void )
//{
//
//};


Shader::Buffer::Buffer ( void )
{
  buffer = nullptr;
  size = 0;
};


Shader::Buffer::~Buffer ( void )
{
  if (buffer)
    release ();
};


void Shader::Buffer::release ( void )
{

  if (buffer)
  {
    delete buffer;
    buffer = nullptr;
  }

};


void Shader::m_loadCompiled ( std::string& fileName, Buffer* csoBuffer )
{
  try
  {

    // load shaders from .cso compiled by Visual Studio files

    std::string path { "./" };

    //#ifndef _NOT_DEBUGGING
    //    path = { "./x64/Debug/GameEngine/" };
    //#else
    //    path = { "./x64/Release/GameEngine/" };
    //#endif // !_NOT_DEBUGGING

    path += fileName;
    std::ifstream csoFile ( path, std::ios::binary | std::ios::ate );

    if (csoFile.is_open ())
    {
      csoBuffer->size = csoFile.tellg (); // shader object size
      csoBuffer->buffer = new (std::nothrow) byte [csoBuffer->size];

      if (csoBuffer->buffer)
      {
        csoFile.seekg ( 0, std::ios::beg );
        csoFile.read ( reinterpret_cast<char*>(csoBuffer->buffer), csoBuffer->size );
      } else
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Shader buffer allocation for compiled file failed!" + m_entryPoint );
      }

      csoFile.close ();

    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Loading shader form compiled file failed!" + m_entryPoint );
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
  }
};


bool Shader::m_initializeCompiled ( std::string* filePaths, D3D11_INPUT_ELEMENT_DESC* polygonLayout,
                                    unsigned int elmCount )
{
  try
  {

    HRESULT hR;

    // load and encapsulate .cso shaders (compiled by VisualStudio) into usable shader objects
    Buffer vertexBuf; // vertex shader buffer
    m_loadCompiled ( filePaths [0], &vertexBuf ); // load process

    Buffer pixelBuf; // pixel shader buffer
    m_loadCompiled ( filePaths [1], &pixelBuf );

    if (vertexBuf.buffer && pixelBuf.buffer)
    {

      // Direct3D interface for vertex shaders: vertex shader creation
      // purpose: invoking the HLSL shaders for drawing the 3D models already on the GPU
      hR = m_device->CreateVertexShader ( vertexBuf.buffer,
                                          vertexBuf.size, nullptr, &m_vertexShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of vertex shader failed!" + m_entryPoint );
        return false;
      }


      // Direct3D interface for pixel shaders: pixel shader creation
      hR = m_device->CreatePixelShader ( pixelBuf.buffer,
                                         pixelBuf.size, nullptr, &m_pixelShader );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of pixel shader failed!" + m_entryPoint );
        return false;
      }


      // input layout creation (how to handle the defined vertices)
      // the interface holds definition of how to feed vertex data into the input-assembler stage of the graphics rendering pipeline
      hR = m_device->CreateInputLayout ( polygonLayout, elmCount,
                                         vertexBuf.buffer,
                                         vertexBuf.size, &m_inputLayout );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of input layout failed!" + m_entryPoint );
        return false;
      }

    }

    vertexBuf.release ();
    pixelBuf.release ();

    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
    return false;
  }
};


bool Shader::m_compile ( LPCWSTR* files )
{
  try
  {

    HRESULT hR;

    std::string errorStr; // HLSL compilation errors in string


    // directly compile the shader into buffer (DirectX APIs)
    hR = D3DCompileFromFile ( files [0], nullptr, nullptr, "main", "vs_4_1", D3DCOMPILE_DEBUG,
                              D3DCOMPILE_EFFECT_CHILD_EFFECT, &m_vertexBuffer, &m_errorMsg );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Compilation of texture vertex shader file failed!" + m_entryPoint );

      if (m_errorMsg)
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (),
                                                    "mainThread", (char*) m_errorMsg->GetBufferPointer () + m_entryPoint );
        errorStr = (char*) m_errorMsg->GetBufferPointer ();
        m_errorMsg->Release ();
        m_errorMsg = nullptr;
      }
      return false;
    }


    hR = D3DCompileFromFile ( files [1], nullptr, nullptr, "main", "ps_4_1", D3DCOMPILE_DEBUG,
                              D3DCOMPILE_EFFECT_CHILD_EFFECT, &m_pixelBuffer, &m_errorMsg );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Compilation of texture pixel shader file failed!" + m_entryPoint );

      if (m_errorMsg)
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
          (char*) m_errorMsg->GetBufferPointer ()
                                                    + m_entryPoint );
        errorStr = (char*) m_errorMsg->GetBufferPointer ();
        m_errorMsg->Release ();
        m_errorMsg = nullptr;
      }
      return false;
    }

    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
    return false;
  }
};


bool Shader::m_initialize ( D3D11_INPUT_ELEMENT_DESC* polygonLayout,
                            unsigned int elmCount,
                            D3D11_SAMPLER_DESC* sampler )
{
  try
  {

    HRESULT hR;


    hR = m_device->CreateVertexShader ( m_vertexBuffer->GetBufferPointer (),
                                        m_vertexBuffer->GetBufferSize (), nullptr, &m_vertexShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of texture vertex shader failed!" + m_entryPoint );
      return false;
    }


    hR = m_device->CreatePixelShader ( m_pixelBuffer->GetBufferPointer (),
                                       m_pixelBuffer->GetBufferSize (), nullptr, &m_pixelShader );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of texture pixel shader failed!" + m_entryPoint );
      return false;
    }


    hR = m_device->CreateInputLayout ( polygonLayout, elmCount,
                                       m_vertexBuffer->GetBufferPointer (),
                                       m_vertexBuffer->GetBufferSize (), &m_inputLayout );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of texture input layout failed!" + m_entryPoint );
      return false;
    }


    m_vertexBuffer->Release ();
    m_vertexBuffer = nullptr;
    m_pixelBuffer->Release ();
    m_pixelBuffer = nullptr;


    if (sampler)
    {
      // testure sampler state creation
      hR = m_device->CreateSamplerState ( sampler, &m_samplerState );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of texture sampler state failed!" + m_entryPoint );
        return false;
      }
    }

    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
    return false;
  }
};


ID3D11VertexShader* const Shader::m_getVertexShader ( void )
{
  return m_vertexShader;
};


ID3D11PixelShader* const Shader::m_getPixelShader ( void )
{
  return m_pixelShader;
};


ID3D11InputLayout* const Shader::m_getInputLayout ( void )
{
  return m_inputLayout;
};


ID3D11SamplerState** const Shader::m_getSamplerState ( void )
{
  return &m_samplerState;
};


void Shader::m_release ( void )
{
  try
  {

    if (m_vertexShader)
    {
      m_vertexShader->Release ();
      m_vertexShader = nullptr;
    }
    if (m_pixelShader)
    {
      m_pixelShader->Release ();
      m_pixelShader = nullptr;
    }
    if (m_inputLayout)
    {
      m_inputLayout->Release ();
      m_inputLayout = nullptr;
    }
    if (m_samplerState)
    {
      m_samplerState->Release ();
      m_samplerState = nullptr;
    }

    m_device = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


ShaderColour::ShaderColour ( ID3D11Device* dev ) :
  Shader ( dev, "ColourShader" ), m_initialized ( false )
{
  try
  {

    // input layout description (passed into the shader)
    // note that the description must match to the vertex types defined in game and shader
    // teaching the GPU how to read a custom vertex structure.
    // note that to improve the rendering speed,
    // the GPU can be told what information with each vertex needs to be stored.
    // note flag D3D11_APPEND_ALIGNED_ELEMENT: the elements are one after each other,
    // therefore automatically figure the spacing out. (no need to define the offset)
    m_polygonLayoutDesc [0].SemanticName = "POSITION"; // what a certain value is used for
    m_polygonLayoutDesc [0].SemanticIndex = 0; // modifies the semantic with an integer index (multiple elements with same semantic)
    m_polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 32 bits for each x, y and z
    m_polygonLayoutDesc [0].InputSlot = 0; // imput - assembler or input slot through which data is fed to GPU ( Direct3D supports sixteen input slots )
    m_polygonLayoutDesc [0].AlignedByteOffset = 0; // the offset between each element in the structure
    m_polygonLayoutDesc [0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // input data class for a single input slot
    m_polygonLayoutDesc [0].InstanceDataStepRate = 0; // for now

    m_polygonLayoutDesc [1].SemanticName = "COLOR"; // colour semantic
    m_polygonLayoutDesc [1].SemanticIndex = 0;
    m_polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 32 bits for each x, y and z
    m_polygonLayoutDesc [1].InputSlot = 0;
    m_polygonLayoutDesc [1].AlignedByteOffset = 12; // offset: 3*4 byte of float type
    m_polygonLayoutDesc [1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [1].InstanceDataStepRate = 0;

    m_elementsCount = 2;

    // compiled by VisualStudio
    m_files [0] = "vertex.cso";
    m_files [1] = "pixel.cso";

    m_initialized = m_initializeCompiled ( m_files, m_polygonLayoutDesc, m_elementsCount );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//ShaderColour::~ShaderColour ( void )
//{
//
//};


const bool& ShaderColour::m_isInitialized ( void )
{
  return m_initialized;
};


ShaderTexture::ShaderTexture ( ID3D11Device* dev ) :
  Shader ( dev, "TextureShader" ), m_initialized ( false )
{
  try
  {

    m_polygonLayoutDesc [0].SemanticName = "POSITION";
    m_polygonLayoutDesc [0].SemanticIndex = 0;
    m_polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    m_polygonLayoutDesc [0].InputSlot = 0;
    m_polygonLayoutDesc [0].AlignedByteOffset = 0;
    m_polygonLayoutDesc [0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [0].InstanceDataStepRate = 0;

    m_polygonLayoutDesc [1].SemanticName = "TEXCOORD"; // texture coordinate semantic
    // note that numbered semantics are introduced here without any numbers
    m_polygonLayoutDesc [1].SemanticIndex = 0;
    m_polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32_FLOAT; // 32 bits for each U (width) and V (height)
    m_polygonLayoutDesc [1].InputSlot = 0;
    m_polygonLayoutDesc [1].AlignedByteOffset = 12; //D3D11_APPEND_ALIGNED_ELEMENT
    m_polygonLayoutDesc [1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [1].InstanceDataStepRate = 0;

    // tecture sampler state description (to interface with texture shader):
    // which pixel or what combination of them to use when drawing (near or far away polygon)
    // liner option: the most expensive in processing and the best visual result,
    // unisng linear interpretation for manification, magnification and mip-level sampling.
    m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // the most important one
    // wrap: ensures the coordinates stay between 0.0f and 1.0f by wrapping anything already outside,
    // around and within these values
    m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.MipLODBias = 0.0f;
    m_samplerDesc.MaxAnisotropy = 1;
    m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    m_samplerDesc.BorderColor [0] = 0.0f;
    m_samplerDesc.BorderColor [1] = 0.0f;
    m_samplerDesc.BorderColor [2] = 0.0f;
    m_samplerDesc.BorderColor [3] = 0.0f;
    m_samplerDesc.MinLOD = 0.0f;
    m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    m_elementsCount = 2;

    m_files [0] = L"./graphics/vertexT.hlsl";
    m_files [1] = L"./graphics/pixelT.hlsl";

    if (m_compile ( m_files ))
      m_initialized = m_initialize ( m_polygonLayoutDesc, m_elementsCount, &m_samplerDesc );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//ShaderTexture::~ShaderTexture ( void )
//{
//
//};


const bool& ShaderTexture::m_isInitialized ( void )
{
  return m_initialized;
};


ShaderDiffuseLight::ShaderDiffuseLight ( ID3D11Device* dev ) :
  Shader ( dev, "DiffuseLightShader" ), m_initialized ( false )
{
  try
  {

    m_polygonLayoutDesc [0].SemanticName = "POSITION";
    m_polygonLayoutDesc [0].SemanticIndex = 0;
    m_polygonLayoutDesc [0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    m_polygonLayoutDesc [0].InputSlot = 0;
    m_polygonLayoutDesc [0].AlignedByteOffset = 0;
    m_polygonLayoutDesc [0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [0].InstanceDataStepRate = 0;

    m_polygonLayoutDesc [1].SemanticName = "TEXCOORD";
    m_polygonLayoutDesc [1].SemanticIndex = 0;
    m_polygonLayoutDesc [1].Format = DXGI_FORMAT_R32G32_FLOAT;
    m_polygonLayoutDesc [1].InputSlot = 0;
    m_polygonLayoutDesc [1].AlignedByteOffset = 12; //D3D11_APPEND_ALIGNED_ELEMENT
    m_polygonLayoutDesc [1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [1].InstanceDataStepRate = 0;

    m_polygonLayoutDesc [2].SemanticName = "NORMAL"; // normal light semantic
    m_polygonLayoutDesc [2].SemanticIndex = 0;
    m_polygonLayoutDesc [2].Format = DXGI_FORMAT_R32G32B32_FLOAT; // each x, y and z of the normal vector 32 bits
    m_polygonLayoutDesc [2].InputSlot = 0;
    m_polygonLayoutDesc [2].AlignedByteOffset = 20; //D3D11_APPEND_ALIGNED_ELEMENT
    m_polygonLayoutDesc [2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    m_polygonLayoutDesc [2].InstanceDataStepRate = 0;

    m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.MipLODBias = 0.0f;
    m_samplerDesc.MaxAnisotropy = 1;
    m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    m_samplerDesc.BorderColor [0] = 0.0f;
    m_samplerDesc.BorderColor [1] = 0.0f;
    m_samplerDesc.BorderColor [2] = 0.0f;
    m_samplerDesc.BorderColor [3] = 0.0f;
    m_samplerDesc.MinLOD = 0.0f;
    m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    m_elementsCount = 3;

    m_files [0] = L"./graphics/vertexL.hlsl";
    m_files [1] = L"./graphics/pixelL.hlsl";

    if (m_compile ( m_files ))
      m_initialized = m_initialize ( m_polygonLayoutDesc, m_elementsCount, &m_samplerDesc );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//ShaderDiffuseLight::~ShaderDiffuseLight ( void )
//{
//
//};


const bool& ShaderDiffuseLight::m_isInitialized ( void )
{
  return m_initialized;
};

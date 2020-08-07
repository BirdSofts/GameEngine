// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Mesh.h"
#include "Shared.h"


template <class tType>
Mesh<tType>::Mesh ( TheCore* coreObj, Shader* shadObj, Texture* texObj, std::string entry, bool rewrite ) :
  m_entryPoint ( " Entry Point: " + entry ), m_dynamic ( rewrite ),
  m_core ( coreObj ), m_shader ( shadObj ), m_texture ( texObj ),
  m_position ( 0.0f, 0.0f, 0.0f ), m_scale ( 0.0f ),
  m_vertexBuffer ( nullptr ), m_indexBuffer ( nullptr )
{
  m_vertexBufferDate = { 0 };
  //m_subResourceDate.pSysMem = nullptr;
  //m_subResourceDate.SysMemPitch = 0;
  //m_subResourceDate.SysMemSlicePitch = 0;
};


//template <class tType>
//Model<tType>::~Model ( void )
//{
//
//};


template <class tType>
bool Mesh<tType>::m_allocate ( tType* data, unsigned long* index, unsigned long& count )
{
  try
  {

    winrt::com_ptr<ID3D11Device3> device { m_core->m_getD3D ()->m_getDevice () };

    // vertex buffer description
    m_vertexBufferDesc.ByteWidth = sizeof ( *data ) * 8; // buffer size
    m_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    if (!m_dynamic)
    {
      m_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // default: only GPC can read and write
      m_vertexBufferDesc.CPUAccessFlags = 0; // CPU access
    } else
    {
      m_vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      m_vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    m_vertexBufferDesc.MiscFlags = 0; // for now
    //vertexBufferDesc.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    m_vertexBufferDate = {
      data, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( device->CreateBuffer ( &m_vertexBufferDesc, &m_vertexBufferDate, &m_vertexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of vertex buffer failed!" + m_entryPoint );
      return false;
    }


    // index buffer description
    m_indexBufferDesc.ByteWidth = sizeof ( *index ) * 36;
    m_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    if (!m_dynamic)
    {
      m_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      m_indexBufferDesc.CPUAccessFlags = 0;
    } else
    {
      m_indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      m_indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    m_indexBufferDesc.MiscFlags = 0;
    //indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceDate = { index, 0, 0 };

    if (FAILED ( device->CreateBuffer ( &m_indexBufferDesc, &subResourceDate, &m_indexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of vertex buffer failed!" + m_entryPoint );
      return false;
    }

    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
    return false;
  }
};


template <class tType>
void Mesh <tType> ::m_render ( void )
{
  try
  {

    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shader->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shader->m_getPixelShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shader->m_getInputLayout () );

    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;

    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, &m_vertexBuffer, &strides, &offset );

    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( 36, 0, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
  }
};


template <class tType>
void Mesh <tType> ::m_updateBuffer ( tType* data )
{
  try
  {

    //DirectX::XMVECTOR outS, outQ, outT {};
    //  DirectX::XMMatrixDecompose ( &outS, &outQ, &outT, test );


    float radians { 1.0f * (DirectX::XM_PI / 180.0f) };

    float sinA = sinf ( radians );
    float cosA = cosf ( radians );

    // Todo: rotate the object
    for (char i = 0; i < 8; i++)
    {

      // to the origin
      float x = data [i].position.x - m_position.x;
      float y = data [i].position.y - m_position.y;
      float z = data [i].position.z - m_position.z;

      //// x-axis
      //x = (x * 1.0f) + (y * 0.0f) + (z * 0.0f) + (1 * 0.0f);
      //y = (x * 0.0f) + (y * cosA) + (z * -sinA) + (1 * 0.0f);
      //z = (x * 0.0f) + (y * sinA) + (z * cosA) + (1 * 0.0f);

      //// y-axis
      //x = (x * cosA) + (y * 0.0f) + (z * sinA) + (1 * 0.0f);
      //y = (x * 0.0f) + (y * 1.0f) + (z * 0.0f) + (1 * 0.0f);
      //z = (x * -sinA) + (y * 0.0f) + (z * cosA) + (1 * 0.0f);

      // z-axis
      x = (x * cosA) + (y * -sinA) + (z * 0.0f) + (1 * 0.0f);
      y = (x * sinA) + (y * cosA) + (z * 0.0f) + (1 * 0.0f);
      z = (x * 0.0f) + (y * 0.0f) + (z * 1.0f) + (1 * 0.0f);

      // repositioning
      data [i].position.x = x + m_position.x;
      data [i].position.y = y + m_position.y;
      data [i].position.z = z + m_position.z;

    }

    m_core->m_getD3D ()->m_getDevCon ()->UpdateSubresource1 ( m_vertexBuffer, 0, NULL, data, 0, 0, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
  }
};


template <class tType>
void Mesh <tType> ::m_release ( void )
{
  try
  {

    if (m_indexBuffer)
    {
      m_indexBuffer->Release ();
      m_indexBuffer = nullptr;
    }
    if (m_vertexBuffer)
    {
      m_vertexBuffer->Release ();
      m_vertexBuffer = nullptr;
    }

    m_core = nullptr;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () + m_entryPoint );
  }
};


void MeshClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  TheCore* core { nullptr };
  Shader* shad { nullptr };
  Texture* tex { nullptr };
  Vertex* data { nullptr };
  VertexT* dataT { nullptr };
  VertexL* dataL { nullptr };
  Mesh<Vertex> tempVertex ( core, shad, tex, "", false );
  tempVertex.m_getIndexBuffer ();
  tempVertex.m_getVertexBuffer ();
  tempVertex.m_render ();
  tempVertex.m_updateBuffer ( data );
  tempVertex.m_release ();
  Mesh<VertexT> tempVertexT ( core, shad, tex, "", false );
  tempVertexT.m_getIndexBuffer ();
  tempVertexT.m_getVertexBuffer ();
  tempVertexT.m_render ();
  tempVertexT.m_updateBuffer ( dataT );
  tempVertexT.m_release ();
  Mesh<VertexL> tempVertexL ( core, shad, tex, "", false );
  tempVertexL.m_getIndexBuffer ();
  tempVertexL.m_getVertexBuffer ();
  tempVertexL.m_render ();
  tempVertexL.m_updateBuffer ( dataL );
  tempVertexL.m_release ();

}


MeshCube::MeshCube ( TheCore* coreObj, Shader* shadObj, Texture* texObj ) :
  Mesh ( coreObj, shadObj, texObj, "\tMeshCube", false ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    DirectX::XMFLOAT3 position { -.4f, .2f, 0.0f };
    float scale = .15f;

    m_getPosition () = position;
    m_getScale () = scale;

    // cube mesh vertices (adapted from Microsoft template and adjusted to right-hand coordinate system)
    m_verticesData = new (std::nothrow) Vertex [8]
    (
      //DirectX::XMFLOAT3 ( 0.1f, 0.1f, 0.1f ), DirectX::XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f ) // no proper constuctor
      );

    m_verticesData [0].position = DirectX::XMFLOAT3 ( scale, scale, scale );
    m_verticesData [0].color = DirectX::XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
    m_verticesData [1].position = DirectX::XMFLOAT3 ( scale, scale, -scale );
    m_verticesData [1].color = DirectX::XMFLOAT4 ( 0.0f, 0.0f, 1.0f, 1.0f );
    m_verticesData [2].position = DirectX::XMFLOAT3 ( scale, -scale, scale );
    m_verticesData [2].color = DirectX::XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 1.0f );
    m_verticesData [3].position = DirectX::XMFLOAT3 ( scale, -scale, -scale );
    m_verticesData [3].color = DirectX::XMFLOAT4 ( 0.0f, 1.0f, 1.0f, 1.0f );

    m_verticesData [4].position = DirectX::XMFLOAT3 ( -scale, scale, scale );
    m_verticesData [4].color = DirectX::XMFLOAT4 ( 1.0f, 0.0f, 0.0f, 1.0f );
    m_verticesData [5].position = DirectX::XMFLOAT3 ( -scale, scale, -scale );
    m_verticesData [5].color = DirectX::XMFLOAT4 ( 1.0f, 0.0f, 1.0f, 1.0f );
    m_verticesData [6].position = DirectX::XMFLOAT3 ( -scale, -scale, scale );
    m_verticesData [6].color = DirectX::XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 1.0f );
    m_verticesData [7].position = DirectX::XMFLOAT3 ( -scale, -scale, -scale );
    m_verticesData [7].color = DirectX::XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );

    for (char i = 0; i < 8; i++)
    {
      m_verticesData [i].position.x += position.x;
      m_verticesData [i].position.y += position.y;
      m_verticesData [i].position.z += position.z;
    }

    // cube mesh indices (each trio build a triangle)
    m_verticesIndex = new (std::nothrow) unsigned long [36]
    {
      0,2,1, // -x
      1,2,3,

      4,5,6, // +x
      5,7,6,

      0,1,5, // -y
      0,5,4,

      2,6,7, // +y
      2,7,3,

      0,4,6, // -z
      0,6,2,

      1,3,7, // +z
      1,7,5,
    };

    //xx Todo
    m_verticesCount = (sizeof ( *m_verticesIndex ) * 36) / 4;

    if (m_allocate ( m_verticesData, m_verticesIndex, m_verticesCount ))
      m_allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//MeshCube::~MeshCube ( void )
//{
//
//};


void MeshCube::m_update ( void )
{
  try
  {

    m_updateBuffer ( m_verticesData );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void MeshCube::m_releaseData ( void )
{
  try
  {

    delete [] m_verticesData;
    m_verticesData = nullptr;
    delete [] m_verticesIndex;
    m_verticesIndex = nullptr;

    m_release ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

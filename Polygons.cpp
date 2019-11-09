// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Polygons.h"
#include "Shared.h"


template <class tType>
Model<tType>::Model ( ID3D11Device* dev, ID3D11DeviceContext* devC, std::string entry, bool rewrite ) :
  m_entryPoint ( " Entry Point: " + entry ), m_dynamic ( rewrite ),
  m_device ( dev ), m_deviceContext ( devC ),
  m_vertexBuffer ( nullptr ), m_indexBuffer ( nullptr )
{
  m_subResourceDate.pSysMem = nullptr;
  m_subResourceDate.SysMemPitch = 0;
  m_subResourceDate.SysMemSlicePitch = 0;
};


//template <class tType>
//Model<tType>::~Model ( void )
//{
//
//};


template <class tType>
bool Model<tType>::m_allocate ( tType* data, unsigned long* index, unsigned long& count )
{
  try
  {

    // vertex buffer description
    m_vertexBufferDesc.ByteWidth = sizeof ( tType ) * count; // buffer size
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
    m_subResourceDate = {
      data, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( m_device->CreateBuffer ( &m_vertexBufferDesc, &m_subResourceDate, &m_vertexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of vertex buffer failed!" + m_entryPoint );
      return false;
    }


    // index buffer description
    m_indexBufferDesc.ByteWidth = sizeof ( long ) * count;
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

    if (FAILED ( m_device->CreateBuffer ( &m_indexBufferDesc, &subResourceDate, &m_indexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of vertex buffer failed!" + m_entryPoint );
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


template <class tType>
ID3D11Buffer** const Model<tType>::m_getVertexBuffer ( void )
{
  return &m_vertexBuffer;
};


template <class tType>
ID3D11Buffer* const Model<tType>::m_getIndexBuffer ( void )
{
  return m_indexBuffer;
};


template <class tType>
void Model<tType>::m_release ( void )
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

    m_device = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void ModelClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  ID3D11Device* dev { nullptr };
  ID3D11DeviceContext* devCon { nullptr };
  Model<Vertex> tempVertex ( dev, devCon, "", false );
  tempVertex.m_getIndexBuffer ();
  tempVertex.m_getVertexBuffer ();
  tempVertex.m_release ();
  Model<VertexT> tempVertexT ( dev, devCon, "", false );
  tempVertexT.m_getIndexBuffer ();
  tempVertexT.m_getVertexBuffer ();
  tempVertexT.m_release ();
  Model<VertexL> tempVertexL ( dev, devCon, "", false );
  tempVertexL.m_getIndexBuffer ();
  tempVertexL.m_getVertexBuffer ();
  tempVertexL.m_release ();

}


Triangles::Triangles ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tThreeTriangles", false ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    // vertices count
    m_verticesCount = 9;

    // three triangles vertices data
    m_verticesData [0].position = DirectX::XMFLOAT3 { -0.95f, 0.0f, 0.0f }; // bottom left
    m_verticesData [0].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };
    m_verticesData [1].position = DirectX::XMFLOAT3 { -0.9f, -0.12f, 0.0f }; // top middle
    m_verticesData [1].color = DirectX::XMFLOAT4 { 0.53f, 0.53f, 0.53f, 1.0f };
    m_verticesData [2].position = DirectX::XMFLOAT3 { -1.0f, -0.12f, 0.0f }; // bottom right
    m_verticesData [2].color = DirectX::XMFLOAT4 { 0.93f, 0.93f, 0.93f, 1.0f };

    m_verticesData [3].position = DirectX::XMFLOAT3 { -0.75f, 0.0f, 0.0f };
    m_verticesData [3].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };
    m_verticesData [4].position = DirectX::XMFLOAT3 { -0.7f, -0.12f, 0.0f };
    m_verticesData [4].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };
    m_verticesData [5].position = DirectX::XMFLOAT3 { -0.8f, -0.12f, 0.0f };
    m_verticesData [5].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };

    m_verticesData [6].position = DirectX::XMFLOAT3 { -0.55f, 0.2f, 0.0f };
    m_verticesData [6].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };
    m_verticesData [7].position = DirectX::XMFLOAT3 { -0.5f, -0.12f, 0.0f };
    m_verticesData [7].color = DirectX::XMFLOAT4 { 0.53f, 0.53f, 0.53f, 1.0f };
    m_verticesData [8].position = DirectX::XMFLOAT3 { -0.6f, -0.12f, 0.0f };
    m_verticesData [8].color = DirectX::XMFLOAT4 { 0.93f, 0.93f, 0.93f, 1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < m_verticesCount; i++)
      m_verticesIndices [i] = i;

    if (m_allocate ( m_verticesData, m_verticesIndices, m_verticesCount ))
      m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Triangles::~Triangles ( void )
//{
//
//};


const unsigned long& Triangles::m_getVerticesCount ( void )
{
  return m_verticesCount;
};


Line::Line ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tClockwiseLine", true ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    // vertices count
    m_verticesCount = 2;

    // a line
    m_verticesData [0].position = DirectX::XMFLOAT3 { -0.2f, 0.0f, 0.0f }; // left position
    m_verticesData [0].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };

    m_verticesData [1].position = DirectX::XMFLOAT3 { 0.2f, 0.0f, 0.0f }; // right position
    m_verticesData [1].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };

    // triangles' vertices indices
    m_verticesIndex [0] = 0;
    m_verticesIndex [1] = 1;

    if (m_allocate ( m_verticesData, m_verticesIndex, m_verticesCount ))
      m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Line::~Line ( void )
//{
//
//};


void Line::m_update ( void )
{
  try
  {

    HRESULT hR;
    char modeX_1 { 0 };
    char modeY_1 { 0 };
    char modeX_2 { 0 };
    char modeY_2 { 0 };


    // map the data back to system memory using a sub-resource
    // second parameter: what CPU does when GPU is busy
    // note that in Direct3D11 a resource may contain sub-resources (additional parameters of device context method)
    // after the resource is mapped, any change to it is reflected to the vertex buffer.
    hR = m_deviceContext->Map ( m_vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_mappedRes );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Mapping the resource data failed!" );
    }


    // update the sub-resource:

    //-- turn the line clockwise
    Vertex* data = reinterpret_cast<Vertex*>(m_mappedRes.pData);

    static float temp { data->position.x };

    if (data->position.x < (data + 1)->position.x)
    {
      modeX_1 = 1;
      modeY_1 = 1;
      modeX_2 = -1;
      modeY_2 = -1;
    } else
      if (data->position.x > ( data + 1 )->position.x)
      {
        modeX_1 = 1;
        modeY_1 = -1;
        modeX_2 = -1;
        modeY_2 = 1;
      }

    if ((data + 1)->position.x < temp)
    {
      temp = data->position.x;
      data->position.x = (data + 1)->position.x;
      (data + 1)->position.x = temp;
      temp = data->position.y;
      data->position.y = (data + 1)->position.y;
      (data + 1)->position.y = temp;
      temp = data->position.x;
    }

    data->position.x += modeX_1 * 0.002f;
    data->position.y += modeY_1 * 0.002f;
    (data + 1)->position.x += modeX_2 * 0.002f;
    (data + 1)->position.y += modeY_2 * 0.002f;

    //-- randomize the colour vertices
    float rnd_1 { 0.0f };
    float rnd_2 { 0.0f };
    float rnd_3 { 0.0f };
    rnd_1 = ((rand () % 100) / static_cast<float>(100));
    rnd_2 = ((rand () % 100) / static_cast<float>(100));
    rnd_3 = ((rand () % 100) / static_cast<float>(100));
    data->color.x = (data + 1)->color.x = rnd_1; // red
    data->color.y = (data + 1)->color.y = rnd_2; // green
    data->color.z = rnd_1 = (data + 1)->color.z = rnd_3; // blue


    // validates the pointer of the vertex buffer's resource and enables the GPU's read access upon.
    m_deviceContext->Unmap ( m_vertexBuffer, 0 );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


const unsigned long& Line::m_getVerticesCount ( void )
{
  return m_verticesCount;
};


TexturedTriangles::TexturedTriangles ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tTexturedTriangles", false ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    // vertices count
    m_verticesCount = 6;

    // a rectangle built using two textured triangles
    m_verticesData [0].position = DirectX::XMFLOAT3 { -0.8f, -0.8f, 0.0f };
    m_verticesData [0].texture = DirectX::XMFLOAT2 { 0.0f, 1.0f };
    m_verticesData [1].position = DirectX::XMFLOAT3 { -0.8f, -0.6f, 0.0f };
    m_verticesData [1].texture = DirectX::XMFLOAT2 { 0.0f, 0.0f };
    m_verticesData [2].position = DirectX::XMFLOAT3 { -0.6f, -0.8f, 0.0f };
    m_verticesData [2].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };

    m_verticesData [3].position = DirectX::XMFLOAT3 { -0.8f, -0.6f, 0.0f };
    m_verticesData [3].texture = DirectX::XMFLOAT2 { 0.0f, 0.0f };
    m_verticesData [4].position = DirectX::XMFLOAT3 { -0.6f, -0.6f, 0.0f };
    m_verticesData [4].texture = DirectX::XMFLOAT2 { 1.0f, 0.0f };
    m_verticesData [5].position = DirectX::XMFLOAT3 { -0.6f, -0.8f, 0.0f };
    m_verticesData [5].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < 6; i++)
      m_verticesIndex [i] = i;

    if (m_allocate ( m_verticesData, m_verticesIndex, m_verticesCount ))
      m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//TexturedTriangles::~TexturedTriangles ( void )
//{
//
//};


const unsigned long& TexturedTriangles::m_getVerticesCount ( void )
{
  return m_verticesCount;
};


LightedTriangle::LightedTriangle ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tLightedTriangles", false ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    // vertices count
    m_verticesCount = 3;

    // the lighted triangle
    m_verticesData [0].position = DirectX::XMFLOAT3 { 0.0f, -0.8f, 0.0f };
    m_verticesData [0].texture = DirectX::XMFLOAT2 { 0.0f, 1.0f };
    m_verticesData [0].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };
    // normal vector: perpendicular to the polygon's face,
    // thus the exact direction the face is pointing is calculable.
    // note: set along the Z axis (-1) so the normal point toward the viewer.

    m_verticesData [1].position = DirectX::XMFLOAT3 { 0.4f, 0.0f, 0.0f };
    m_verticesData [1].texture = DirectX::XMFLOAT2 { 0.5f, 0.0f };
    m_verticesData [1].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };
    m_verticesData [2].position = DirectX::XMFLOAT3 { 0.8f, -0.8f, 0.0f };
    m_verticesData [2].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };
    m_verticesData [2].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < 3; i++)
      m_verticesIndex [i] = i;

    if (m_allocate ( m_verticesData, m_verticesIndex, m_verticesCount ))
      m_allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//LightedTriangle::~LightedTriangle ( void )
//{
//
//};


const unsigned long& LightedTriangle::m_getVerticesCount ( void )
{
  return m_verticesCount;
};


Cube::Cube ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tLightedTriangles", false ),
  m_verticesCount ( 0 ), m_allocated ( false )
{
  try
  {

    // the cube and vertices count
    const char* file { "./models/cube.txt" };
    m_verticesCount = VertexTexDiffuseL::read ( file, &m_verticesData );

    if (m_verticesCount)
    {

      // cube's vertices indices
      m_verticesIndex = new (std::nothrow) unsigned long [m_verticesCount];
      if (m_verticesIndex)
      {
        for (unsigned long i = 0; i < m_verticesCount; i++)
          m_verticesIndex [i] = i;

        if (m_allocate ( m_verticesData, m_verticesIndex, m_verticesCount ))
          m_allocated = true;

        delete m_verticesData;
        delete m_verticesIndex;
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Cube::~Cube ( void )
//{
//
//};


const unsigned long& Cube::m_getVerticesCount ( void )
{
  return m_verticesCount;
};

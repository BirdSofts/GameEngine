// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Polygons.h"
#include "Shared.h"


template <class tType>
Model<tType>::Model ( ID3D11Device* dev, ID3D11DeviceContext* devC, std::string entry, bool rewrite ) :
  entryPoint ( " Entry Point: " + entry ), dynamic ( rewrite ),
  device ( dev ), devCon ( devC ),
  vertexBuffer ( nullptr ), indexBuffer ( nullptr )
{
  subResourceDate.pSysMem = nullptr;
  subResourceDate.SysMemPitch = 0;
  subResourceDate.SysMemSlicePitch = 0;
};


template <class tType>
bool Model<tType>::allocate ( tType* data, unsigned long* index, unsigned long& count )
{
  try
  {

    // vertex buffer description
    vertexBufferDesc.ByteWidth = sizeof ( tType ) * count; // buffer size
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    if (!dynamic)
    {
      vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // default: only GPC can read and write
      vertexBufferDesc.CPUAccessFlags = 0; // CPU access
    } else
    {
      vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    vertexBufferDesc.MiscFlags = 0; // for now
    //vertexBufferDesc.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    subResourceDate = {
      data, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( device->CreateBuffer ( &vertexBufferDesc, &subResourceDate, &vertexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of vertex buffer failed!" + entryPoint );
      return false;
    }

    // index buffer description
    indexBufferDesc.ByteWidth = sizeof ( long ) * count;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    if (!dynamic)
    {
      indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      indexBufferDesc.CPUAccessFlags = 0;
    } else
    {
      indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    indexBufferDesc.MiscFlags = 0;
    //indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceDate = { index, 0, 0 };

    if (FAILED ( device->CreateBuffer ( &indexBufferDesc, &subResourceDate, &indexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of vertex buffer failed!" + entryPoint );
      return false;
    }

    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () + entryPoint );
    return false;
  }
};


template <class tType>
ID3D11Buffer** const Model<tType>::getVertexBuffer ( void )
{
  return &vertexBuffer;
};


template <class tType>
ID3D11Buffer* const Model<tType>::getIndexBuffer ( void )
{
  return indexBuffer;
};


template <class tType>
void Model<tType>::release ( void )
{
  try
  {
    if (indexBuffer)
    {
      indexBuffer->Release ();
      indexBuffer = nullptr;
    }
    if (vertexBuffer)
    {
      vertexBuffer->Release ();
      vertexBuffer = nullptr;
    }

    device = nullptr;
  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void PolygonsClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  ID3D11Device* dev { nullptr };
  ID3D11DeviceContext* devCon { nullptr };
  Model<Vertex> tempVertex ( dev, devCon, "", false );
  tempVertex.getIndexBuffer ();
  tempVertex.getVertexBuffer ();
  tempVertex.release ();
  Model<VertexT> tempVertexT ( dev, devCon, "", false );
  tempVertexT.getIndexBuffer ();
  tempVertexT.getVertexBuffer ();
  tempVertexT.release ();
  Model<VertexL> tempVertexL ( dev, devCon, "", false );
  tempVertexL.getIndexBuffer ();
  tempVertexL.getVertexBuffer ();
  tempVertexL.release ();
}


Triangles::Triangles ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tThreeTriangles", false ),
  verticesCount ( 0 ), allocated ( false )
{
  try
  {

    // vertices count
    verticesCount = 9;

    // three triangles vertices data
    verticesData [0].position = DirectX::XMFLOAT3 { -0.95f, 0.0f, 0.0f }; // bottom left
    verticesData [0].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };
    verticesData [1].position = DirectX::XMFLOAT3 { -0.9f, -0.12f, 0.0f }; // top middle
    verticesData [1].color = DirectX::XMFLOAT4 { 0.53f, 0.53f, 0.53f, 1.0f };
    verticesData [2].position = DirectX::XMFLOAT3 { -1.0f, -0.12f, 0.0f }; // bottom right
    verticesData [2].color = DirectX::XMFLOAT4 { 0.93f, 0.93f, 0.93f, 1.0f };

    verticesData [3].position = DirectX::XMFLOAT3 { -0.75f, 0.0f, 0.0f };
    verticesData [3].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };
    verticesData [4].position = DirectX::XMFLOAT3 { -0.7f, -0.12f, 0.0f };
    verticesData [4].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };
    verticesData [5].position = DirectX::XMFLOAT3 { -0.8f, -0.12f, 0.0f };
    verticesData [5].color = DirectX::XMFLOAT4 { 0.13f, 0.80f, 0.13f, 1.0f };

    verticesData [6].position = DirectX::XMFLOAT3 { -0.55f, 0.2f, 0.0f };
    verticesData [6].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };
    verticesData [7].position = DirectX::XMFLOAT3 { -0.5f, -0.12f, 0.0f };
    verticesData [7].color = DirectX::XMFLOAT4 { 0.53f, 0.53f, 0.53f, 1.0f };
    verticesData [8].position = DirectX::XMFLOAT3 { -0.6f, -0.12f, 0.0f };
    verticesData [8].color = DirectX::XMFLOAT4 { 0.93f, 0.93f, 0.93f, 1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < verticesCount; i++)
      verticesIndices [i] = i;

    if (allocate ( verticesData, verticesIndices, verticesCount ))
      allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


Line::Line ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tClockwiseLine", true ),
  verticesCount ( 0 ), allocated ( false )
{
  try
  {

    // vertices count
    verticesCount = 2;

    // a line
    verticesData [0].position = DirectX::XMFLOAT3 { -0.2f, 0.0f, 0.0f }; // left position
    verticesData [0].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };

    verticesData [1].position = DirectX::XMFLOAT3 { 0.2f, 0.0f, 0.0f }; // right position
    verticesData [1].color = DirectX::XMFLOAT4 { 0.13f, 0.13f, 0.13f, 1.0f };

    // triangles' vertices indices
    verticesIndex [0] = 0;
    verticesIndex [1] = 1;

    if (allocate ( verticesData, verticesIndex, verticesCount ))
      allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Line::update ( void )
{
  try
  {

    // update line vertex buffer
    HRESULT hR;
    char modeX_1 { 0 };
    char modeY_1 { 0 };
    char modeX_2 { 0 };
    char modeY_2 { 0 };

    // map the data back to system memory using a sub-resource
    // second parameter: what CPU does when GPU is busy
    // note that in Direct3D11 a resource may contain sub-resources (additional parameters of device context method)
    // after the resource is mapped, any change to it is reflected to the vertex buffer.
    hR = devCon->Map ( vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedRes );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Mapping the resource data failed!" );
    }

    // update the sub-resource:

    //-- turn the line clockwise
    Vertex* data = reinterpret_cast<Vertex*>(mappedRes.pData);

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

    data->position.x += modeX_1 * 0.0001f;
    data->position.y += modeY_1 * 0.0001f;
    (data + 1)->position.x += modeX_2 * 0.0001f;
    (data + 1)->position.y += modeY_2 * 0.0001f;

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
    devCon->Unmap ( vertexBuffer, 0 );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


TexturedTriangles::TexturedTriangles ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tTexturedTriangles", false ),
  verticesCount ( 0 ), allocated ( false )
{
  try
  {

    // vertices count
    verticesCount = 6;

    // a rectangle built using two textured triangles
    verticesData [0].position = DirectX::XMFLOAT3 { -0.8f, -0.8f, 0.0f };
    verticesData [0].texture = DirectX::XMFLOAT2 { 0.0f, 1.0f };
    verticesData [1].position = DirectX::XMFLOAT3 { -0.8f, -0.6f, 0.0f };
    verticesData [1].texture = DirectX::XMFLOAT2 { 0.0f, 0.0f };
    verticesData [2].position = DirectX::XMFLOAT3 { -0.6f, -0.8f, 0.0f };
    verticesData [2].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };

    verticesData [3].position = DirectX::XMFLOAT3 { -0.8f, -0.6f, 0.0f };
    verticesData [3].texture = DirectX::XMFLOAT2 { 0.0f, 0.0f };
    verticesData [4].position = DirectX::XMFLOAT3 { -0.6f, -0.6f, 0.0f };
    verticesData [4].texture = DirectX::XMFLOAT2 { 1.0f, 0.0f };
    verticesData [5].position = DirectX::XMFLOAT3 { -0.6f, -0.8f, 0.0f };
    verticesData [5].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < 6; i++)
      verticesIndex [i] = i;

    if (allocate ( verticesData, verticesIndex, verticesCount ))
      allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


LightedTriangle::LightedTriangle ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tLightedTriangles", false ),
  verticesCount ( 0 ), allocated ( false )
{
  try
  {

    // vertices count
    verticesCount = 3;

    // the lighted triangle
    verticesData [0].position = DirectX::XMFLOAT3 { 0.0f, -0.8f, 0.0f };
    verticesData [0].texture = DirectX::XMFLOAT2 { 0.0f, 1.0f };
    verticesData [0].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };
    // normal vector: perpendicular to the polygon's face,
    // thus the exact direction the face is pointing is calculable.
    // note: set along the Z axis (-1) so the normal point toward the viewer.

    verticesData [1].position = DirectX::XMFLOAT3 { 0.4f, 0.0f, 0.0f };
    verticesData [1].texture = DirectX::XMFLOAT2 { 0.5f, 0.0f };
    verticesData [1].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };
    verticesData [2].position = DirectX::XMFLOAT3 { 0.8f, -0.8f, 0.0f };
    verticesData [2].texture = DirectX::XMFLOAT2 { 1.0f, 1.0f };
    verticesData [2].normal = DirectX::XMFLOAT3 { 0.0f, 0.0f, -1.0f };

    // triangles' vertices indices
    for (unsigned long i = 0; i < 3; i++)
      verticesIndex [i] = i;

    if (allocate ( verticesData, verticesIndex, verticesCount ))
      allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


Cube::Cube ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  Model ( dev, devC, "\tLightedTriangles", false ),
  verticesCount ( 0 ), allocated ( false )
{
  try
  {

    // the cube and vertices count
    const char* file { "./models/cube.txt" };
    verticesCount = VertexTexDiffuseL::read ( file, &verticesData );

    if (verticesCount)
    {

      // cube's vertices indices
      verticesIndex = new (std::nothrow) unsigned long [verticesCount];
      if (verticesIndex)
      {
        for (unsigned long i = 0; i < verticesCount; i++)
          verticesIndex [i] = i;

        if (allocate ( verticesData, verticesIndex, verticesCount ))
          allocated = true;

        delete verticesData;
        delete verticesIndex;
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};

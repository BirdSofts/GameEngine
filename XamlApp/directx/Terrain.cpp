// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Terrain.h"
#include "Shared.h"


Terrain::Terrain ( TheCore* coreObj, Shader* shaderObj ) :
  m_core ( coreObj ), m_shader ( shaderObj ),
  m_dynamic ( false ), m_density ( 0 ), m_startPosition ( 0.0f, 0.0f, 0.0f ),
  m_vertexBuffer ( nullptr ), m_indexBuffer ( nullptr ), m_entitiesCount ( 0 ),
  m_initialized ( false )
{
  try
  {

    // undone debug undefined behaviour when using higher values (number of entities)
    m_density = 10;

    // undone debug using different start positions 
    m_startPosition = { -5.0f, 0.0f, -5.0f };

    if (m_allocate2 ())
      m_initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Terrain::~Terrain ( void )
//{
//
//};


bool Terrain::m_allocate ( void )
{
  try
  {

    // calculated number of lines to be drawn:
    DirectX::XMFLOAT3 temp { m_startPosition };
    // undone not working for positive start points
    if (m_startPosition.x < 0)
      temp.x = -temp.x;
    if (m_startPosition.z < 0)
      temp.z = -temp.z;
    float points { temp.x + temp.z };
    // number of lines * second point of lines * horizontal ones
    m_entitiesCount = (points * m_density) * 2 * 2;

    Vertex* verticesData = new (std::nothrow) Vertex [m_entitiesCount];
    unsigned long* indicesData = new (std::nothrow) unsigned long [m_entitiesCount];


    unsigned long indexOne = m_entitiesCount / 2;

    float nextPoint { 1.0f / m_density };

    verticesData [0].position = m_startPosition;
    verticesData [1].position.x = m_startPosition.x;
    verticesData [1].position.y = 0.0f;
    verticesData [1].position.z = -m_startPosition.z;

    for (unsigned long i = 2; i < indexOne; i += 2)
    {
      // vertical lines
      verticesData [i].position.x = (verticesData [i - 2].position.x + nextPoint);
      verticesData [i].position.y = 0.0f;
      verticesData [i].position.z = m_startPosition.z;

      verticesData [i + 1].position.x = (verticesData [i - 2].position.x + nextPoint);
      verticesData [i + 1].position.y = 0.0f;
      verticesData [i + 1].position.z = -m_startPosition.z;
    }

    verticesData [indexOne].position = m_startPosition;
    verticesData [indexOne + 1].position.x = -m_startPosition.x;
    verticesData [indexOne + 1].position.y = 0.0f;
    verticesData [indexOne + 1].position.z = m_startPosition.z;

    for (unsigned long i = indexOne + 2; i < m_entitiesCount; i += 2)
    {
      // horizontal lines
      verticesData [i].position.x = m_startPosition.x;
      verticesData [i].position.y = 0.0f;
      verticesData [i].position.z = (verticesData [i - 2].position.z + nextPoint);

      verticesData [i + 1].position.x = -m_startPosition.x;
      verticesData [i + 1].position.y = 0.0f;
      verticesData [i + 1].position.z = (verticesData [i - 2].position.z + nextPoint);
    }


    // indices
    for (unsigned long i = 0; i < m_entitiesCount; i++)
    {
      indicesData [i] = i;

      // colour it
      verticesData [i].color = { 0.13f, 0.13f, 0.13f, 1.0f };
    }


    D3D11_BUFFER_DESC buffersDesc;
    D3D11_SUBRESOURCE_DATA buffersDate;

    winrt::com_ptr<ID3D11Device3> device { m_core->m_getD3D ()->m_getDevice () };

    // vertex buffer description
    buffersDesc.ByteWidth = sizeof ( *verticesData ) * m_entitiesCount; // buffer size
    buffersDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    if (!m_dynamic)
    {
      buffersDesc.Usage = D3D11_USAGE_DEFAULT; // default: only GPC can read and write
      buffersDesc.CPUAccessFlags = 0; // CPU access
    } else
    {
      buffersDesc.Usage = D3D11_USAGE_DYNAMIC;
      buffersDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    buffersDesc.MiscFlags = 0; // for now
    //m_bufferDesc.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    buffersDate = {
      verticesData, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( device->CreateBuffer ( &buffersDesc, &buffersDate, &m_vertexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of terrain vertex buffer failed!" );
      return false;
    }


    // index buffer description
    buffersDesc.ByteWidth = sizeof ( *indicesData ) * m_entitiesCount;
    buffersDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    buffersDate = { indicesData, 0, 0 };

    if (FAILED ( device->CreateBuffer ( &buffersDesc, &buffersDate, &m_indexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of terrain vertex buffer failed!" );
      return false;
    }

    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


bool Terrain::m_allocate2 ( void )
{
  try
  {

    // calculated number of lines to be drawn:
    DirectX::XMFLOAT3 temp { m_startPosition };
    // undone not working for positive start points
    if (m_startPosition.x < 0)
      temp.x = -temp.x;
    if (m_startPosition.z < 0)
      temp.z = -temp.z;
    float points { temp.x + temp.z };

    // number of points in one dimension
    unsigned int mapEntitiesCount = (points * m_density);

    TerrainMap* map = new (std::nothrow) TerrainMap [mapEntitiesCount * mapEntitiesCount];
    float height { 0 };
    unsigned short mapIndex { 0 };

    for (unsigned short i = 0; i < mapEntitiesCount; i++)
    {

      for (unsigned short j = 0; j < mapEntitiesCount; j++)
      {

        if ((i > 60) && (i < 80))
        {
          if ((j > 60) && (j < 80))
          {
            height = 0.7f;
          } else
          {
            height = 0.0f;
          }
        } else
        {
          height = 0.0f;
        }

        if ((i > 63) && (i < 77))
        {
          if ((j > 63) && (j < 77))
          {
            height = 0.8f;
          } else
          {
            height = 0.0f;
          }
        }

        if ((i > 66) && (i < 74))
        {
          if ((j > 66) && (j < 74))
          {
            height = 0.9f;
          } else
          {
            height = 0.0f;
          }
        }

        if ((i > 69) && (i < 71))
        {
          if ((j > 69) && (j < 71))
          {
            height = 1.0f;
          } else
          {
            height = 0.0f;
          }
        }

        mapIndex = (mapEntitiesCount * i) + j;

        map [mapIndex].x = (float ( j ) / m_density) + m_startPosition.x;
        map [mapIndex].y = height;
        map [mapIndex].z = (float ( i ) / m_density) + m_startPosition.z;

        map [mapIndex].nX = 0.0f;
        map [mapIndex].nY = 0.0f;
        map [mapIndex].nZ = 0.0f;

        if (height > 0.0f)
        {
          map [mapIndex].nX = 0.0f;
          map [mapIndex].nY = 1.0f;
          map [mapIndex].nZ = 0.0f;
        }

        //mapIndex++;

      }

    }


    // number of lines * number of horizontal lines * triangle's vertices of each quad
    m_entitiesCount = (mapEntitiesCount - 1) * (mapEntitiesCount - 1) * 6;

    VertexCL* verticesData = new (std::nothrow) VertexCL [m_entitiesCount];
    unsigned long* indicesData = new (std::nothrow) unsigned long [m_entitiesCount];

    unsigned short index { 0 };
    unsigned short bottomLeftIndex { 0 }, upperLeftIndex { 0 }, upperRightIndex { 0 }, bottomRightIndex { 0 };
    DirectX::XMFLOAT4 colour { 0.0f, 0.0f, 0.2f, 1.0f };

    for (unsigned short i = 0; i < mapEntitiesCount - 1; i++)
    {

      for (unsigned short j = 0; j < mapEntitiesCount - 1; j++)
      {

        bottomLeftIndex = (mapEntitiesCount * i) + j;
        upperLeftIndex = (mapEntitiesCount * (i + 1)) + j;
        upperRightIndex = (mapEntitiesCount * (i + 1) + (j + 1));
        bottomRightIndex = (mapEntitiesCount * i) + (j + 1);

        verticesData [index].position = DirectX::XMFLOAT3 ( map [bottomLeftIndex].x, map [bottomLeftIndex].y, map [bottomLeftIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [bottomLeftIndex].nX, map [bottomLeftIndex].nY, map [bottomLeftIndex].nZ );
        indicesData [index] = index;
        index++;

        verticesData [index].position = DirectX::XMFLOAT3 ( map [upperLeftIndex].x, map [upperLeftIndex].y, map [upperLeftIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [upperLeftIndex].nX, map [upperLeftIndex].nY, map [upperLeftIndex].nZ );
        indicesData [index] = index;
        index++;

        verticesData [index].position = DirectX::XMFLOAT3 ( map [upperRightIndex].x, map [upperRightIndex].y, map [upperRightIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [upperRightIndex].nX, map [upperRightIndex].nY, map [upperRightIndex].nZ );
        indicesData [index] = index;
        index++;

        verticesData [index].position = DirectX::XMFLOAT3 ( map [upperRightIndex].x, map [upperRightIndex].y, map [upperRightIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [upperRightIndex].nX, map [upperRightIndex].nY, map [upperRightIndex].nZ );
        indicesData [index] = index;
        index++;

        verticesData [index].position = DirectX::XMFLOAT3 ( map [bottomRightIndex].x, map [bottomRightIndex].y, map [bottomRightIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [bottomRightIndex].nX, map [bottomRightIndex].nY, map [bottomRightIndex].nZ );
        indicesData [index] = index;
        index++;

        verticesData [index].position = DirectX::XMFLOAT3 ( map [bottomLeftIndex].x, map [bottomLeftIndex].y, map [bottomLeftIndex].z );
        verticesData [index].color = colour;
        verticesData [index].normal = DirectX::XMFLOAT3 ( map [bottomLeftIndex].nX, map [bottomLeftIndex].nY, map [bottomLeftIndex].nZ );
        indicesData [index] = index;
        index++;


      }

    }


    D3D11_BUFFER_DESC buffersDesc;
    D3D11_SUBRESOURCE_DATA buffersDate;

    winrt::com_ptr<ID3D11Device3> device { m_core->m_getD3D ()->m_getDevice () };

    // vertex buffer description
    buffersDesc.ByteWidth = sizeof ( *verticesData ) * m_entitiesCount; // buffer size
    buffersDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    if (!m_dynamic)
    {
      buffersDesc.Usage = D3D11_USAGE_DEFAULT; // default: only GPC can read and write
      buffersDesc.CPUAccessFlags = 0; // CPU access
    } else
    {
      buffersDesc.Usage = D3D11_USAGE_DYNAMIC;
      buffersDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    //m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    buffersDesc.MiscFlags = 0; // for now
    //m_bufferDesc.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    buffersDate = {
      verticesData, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( device->CreateBuffer ( &buffersDesc, &buffersDate, &m_vertexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of terrain vertex buffer failed!" );
      return false;
    }


    // index buffer description
    buffersDesc.ByteWidth = sizeof ( *indicesData ) * m_entitiesCount;
    buffersDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    buffersDate = { indicesData, 0, 0 };

    if (FAILED ( device->CreateBuffer ( &buffersDesc, &buffersDate, &m_indexBuffer ) ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of terrain vertex buffer failed!" );
      return false;
    }



    // Todo 
    // feed vertices
    // render triangles

    // build terrainMap from bitmap
    // calculate light



    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


void Terrain::m_render ( void )
{
  try
  {

    m_core->m_getD3D ()->m_getDevCon ()->VSSetShader ( m_shader->m_getVertexShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->PSSetShader ( m_shader->m_getPixelShader (), nullptr, 0 );
    m_core->m_getD3D ()->m_getDevCon ()->IASetInputLayout ( m_shader->m_getInputLayout () );

    unsigned int strides = sizeof ( VertexCL );
    //unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;

    m_core->m_getD3D ()->m_getDevCon ()->IASetVertexBuffers ( 0, 1, &m_vertexBuffer, &strides, &offset );

    m_core->m_getD3D ()->m_getDevCon ()->IASetIndexBuffer ( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    //m_core->m_getD3D ()->m_getDevCon ()->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );

    m_core->m_getD3D ()->m_getDevCon ()->DrawIndexed ( m_entitiesCount, 0, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Terrain::m_release ( void )
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
                                                ex.what () );
  }
};

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Universe.h"
#include "Shared.h"


Universe::Universe ( ID3D11Device* dev, ID3D11DeviceContext* devC ) :
  m_device ( dev ), m_deviceContext ( devC ),
  m_camera ( nullptr ), m_worldRotationMatrix ( 0.0f ), m_matrixBuffer ( nullptr ),
  m_diffuseLight ( nullptr ), m_diffuseLightBuffer ( nullptr ), m_initialized ( false )
{
  try
  {

    HRESULT hR;


    // get description
    D3D11_VIEWPORT viewPort;
    unsigned int numberOf { 1 };
    m_deviceContext->RSGetViewports ( &numberOf, &viewPort );


    // projection matrix setup (usable for shaders)
    // purpose: translates 3D scene into the 2D viewport space
    float viewField { DirectX::XM_PI / 4.0f };
    float screenAspect { (float) viewPort.Width / (float) viewPort.Height };
    // projection matrix creation for 3D rendering
    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH ( viewField, screenAspect, m_screenNear, m_screenDepth );


    // word matrix initialization using identity matrix (usable for shaders)
    // purpose: converts objects' vertices into vertices in the 3D scene,
    // additionally to rotate, translate and scale our objects in 3D space
    m_worldMatrix = DirectX::XMMatrixIdentity ();


    // generally a view matrix representing the camera is initialized in this section (camera class)


    // orthographic projection matrix creation
    // purpose: to render 2D elements like user interface directly and skipping 3D rendering
    m_orthographicMatrix = DirectX::XMMatrixOrthographicLH ( (float) viewPort.Width, (float) viewPort.Height, m_screenNear, m_screenDepth );


    // dynamic matrix constant buffer description
    // purpose: to access internal variables introduced in vertex shader
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof ( MatrixBuffer );
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0; // DirectX 11


    // matrix constant buffer creation (usable to access vertex shader constant buffer)
    hR = m_device->CreateBuffer ( &matrixBufferDesc, nullptr, &m_matrixBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of matrix buffer failed!" );
      return;
    }


    // Camera application instantiation
    m_camera = new (std::nothrow) Camera;
    if (!m_camera->isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Camera initialization failed!" );
      return;
    }


    // dynamic diffuse light constant buffer description
    // purpose: to access internal variables introduced in pixel shader
    D3D11_BUFFER_DESC lightBufferDiffuseDesc;
    lightBufferDiffuseDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDiffuseDesc.ByteWidth = sizeof ( LightBuffer );
    lightBufferDiffuseDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDiffuseDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDiffuseDesc.MiscFlags = 0;
    lightBufferDiffuseDesc.StructureByteStride = 0; // DirectX 11

    // diffuse light constant buffer creation (usable to access pixel shader constant buffer)
    hR = m_device->CreateBuffer ( &matrixBufferDesc, nullptr, &m_diffuseLightBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of diffuse light buffer failed!" );
      return;
    }


    // Diffuse light application instantiation
    m_diffuseLight = new (std::nothrow) DiffuseLight;
    if (!m_diffuseLight->m_isInitialized ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Diffuse light initialization failed!" );
      return;
    }


    m_initialized = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Universe::~Universe ( void )
//{
//
//};


const bool& Universe::m_isInitialized ( void )
{
  return m_initialized;
};


void Universe::m_renderResources ( void )
{
  try
  {

    HRESULT hR;


    D3D11_MAPPED_SUBRESOURCE mappedResource;

    MatrixBuffer* dataPtrMatrix;


    // matrices setup + updating the view matrix each frame:

    // transpose process (preparation for shader)
    DirectX::XMMATRIX world, view, projection;
    world = DirectX::XMMatrixTranspose ( m_worldMatrix );
    view = DirectX::XMMatrixTranspose ( m_camera->getView () );
    projection = DirectX::XMMatrixTranspose ( m_projectionMatrix );


    // prepare for write (lock the constant buffer)
    hR = m_deviceContext->Map ( m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Mapping the matrix buffer failed!" );
      return;
    }


    // initialize the pointer to the data already in the constant buffer
    dataPtrMatrix = (MatrixBuffer*) mappedResource.pData;

    // copy the matrices to the constant buffer
    dataPtrMatrix->world = world;
    dataPtrMatrix->view = view;
    dataPtrMatrix->projection = projection;


    // unlock and make the buffer usable
    m_deviceContext->Unmap ( m_matrixBuffer, 0 );


    // activate the updated constant matrix buffer in the vertex shader
    // first parameter: position of the constant buffer in the vertex shader
    m_deviceContext->VSSetConstantBuffers ( 0, 1, &m_matrixBuffer );


    // diffuse light setup + updating it each frame:

    LightBuffer* dataPtrDiffuseLight;

    // prepare for write (lock the constant buffer)
    hR = m_deviceContext->Map ( m_diffuseLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Mapping the diffuse light buffer failed!" );
      return;
    }


    // initialize the pointer to the data already in the constant buffer
    dataPtrDiffuseLight = (LightBuffer*) mappedResource.pData;

    // copy the diffuse light structure to the constant buffer
    dataPtrDiffuseLight->diffuseColour = m_diffuseLight->m_getColour ();
    dataPtrDiffuseLight->diffuseDirection = m_diffuseLight->m_getDirection ();
    dataPtrDiffuseLight->padding = 0.0f;


    // unlock and make the buffer usable
    m_deviceContext->Unmap ( m_diffuseLightBuffer, 0 );


    // activate the updated constant diffuse light buffer in the pixel shader
    // first parameter: position of the constant buffer in the pixel shader
    m_deviceContext->PSSetConstantBuffers ( 0, 1, &m_diffuseLightBuffer );


  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


Camera* Universe::m_getCamera ( void )
{
  return m_camera;
};


DiffuseLight* Universe::m_getDiffuseLight ( void )
{
  return m_diffuseLight;
};


void Universe::m_update ( void )
{
  try
  {

    m_worldRotationMatrix += 0.01f;
    if (m_worldRotationMatrix > 360.0f)
      m_worldRotationMatrix -= 360.0f;

    // spin the game world using the updated rotation factor
    // the only parameter is angle of rotation.
    // note that looking along the rotation axis toward the origin, angles are measured clockwise.
    m_worldMatrix = DirectX::XMMatrixRotationY ( m_worldRotationMatrix );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Universe::m_release ( void )
{
  try
  {

    if (m_matrixBuffer)
    {
      m_matrixBuffer->Release ();
      m_matrixBuffer = nullptr;
    }
    if (m_diffuseLightBuffer)
    {
      m_diffuseLightBuffer->Release ();
      m_diffuseLightBuffer = nullptr;
    }

    if (m_camera)
    {
      delete m_camera;
      m_camera = nullptr;
    }
    if (m_diffuseLight)
    {
      delete m_diffuseLight;
      m_camera = nullptr;
    }

    m_device = nullptr;
    m_deviceContext = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

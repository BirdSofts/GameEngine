// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Universe.h"
#include "Shared.h"


Universe::Universe ( TheCore* coreObj ) :
  m_core ( coreObj ), m_camera ( nullptr ),
  m_matrixBuffer ( nullptr ), m_worldRotationFactor ( .0f ),
  m_diffuseLight ( nullptr ), m_diffuseLightBuffer ( nullptr ),
  m_initialized ( false )
{
  try
  {

    m_createResources ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Universe::~Universe ( void )
//{
//
//};


void Universe::m_createResources ()
{
  try
  {

    HRESULT hR;


    if (m_matrixBuffer != nullptr)
    {
      m_matrixBuffer->Release ();
      m_matrixBuffer = nullptr;
    }

    if (m_diffuseLightBuffer != nullptr)
    {
      m_diffuseLightBuffer->Release ();
      m_diffuseLightBuffer = nullptr;
    }


    // view parameter initialization
    float width { m_core->m_getMainPageTypes ()->m_getDisplay ()->outputWidthDips };
    float height { m_core->m_getMainPageTypes ()->m_getDisplay ()->outputHeightDips };

    // projection matrix setup (usable for shaders)
    // purpose: translates 3D scene into the 2D viewport space
    float aspectRatio { width / height };
    float fovAngleY { DirectX::XM_PI / 4.0f };
    //float fovAngleY { 70.0f * DirectX::XM_PI / 4.0f };

    // a simple example of possible changes that can be made in portrait or snapped view.
    if (aspectRatio < 1.0f)
      fovAngleY *= 2.0f;

    const float m_screenDepth { 100.0f }; // depth settings
    const float m_screenNear { 0.1f }; // depth settings

    //m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH ( fovAngleY, aspectRatio, m_screenNear, m_screenDepth );

    // right-handed coordinate system using row-major matrices
    DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH ( fovAngleY, aspectRatio, m_screenNear, m_screenDepth );

    // note that based on Microsoft template, orientation transform 3D is post-multiplied here,
    // so that correctly orient the scene to match display orientation, which is required
    // for any draw call to swap chain render target and for draws to other targets,
    // this transform should not be applied.
    DirectX::XMFLOAT4X4 orientation = m_core->m_getMainPageTypes ()->m_getDisplay ()->orientationTransform3D;

    DirectX::XMMATRIX orientationMatrix = DirectX::XMLoadFloat4x4 ( &orientation );

    DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.projection, DirectX::XMMatrixTranspose ( perspectiveMatrix * orientationMatrix ) );

    // word matrix initialization using identity matrix (usable for shaders)
    // purpose: converts objects' vertices into vertices in the 3D scene,
    // additionally to rotate, translate and scale our objects in 3D space
    //m_worldMatrix = DirectX::XMMatrixIdentity ();

    //DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.world, DirectX::XMMatrixIdentity () );
    DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.world, DirectX::XMMatrixRotationX ( 0.2f ) );

    // generally a view matrix representing the camera is initialized in this section (camera class)


    // orthographic projection matrix creation
    // purpose: to render 2D elements like user interface directly and skipping 3D rendering
    //m_orthographicMatrix = DirectX::XMMatrixOrthographicLH ( width, height, m_screenNear, m_screenDepth );


    // dynamic matrix constant buffer description
    // purpose: to access internal variables introduced in vertex shader
    CD3D11_BUFFER_DESC matrixBufferDesc ( sizeof ( worldViewProjectionMatrices ), D3D11_BIND_CONSTANT_BUFFER );
    matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    matrixBufferDesc.ByteWidth = sizeof ( worldViewProjectionMatrices );
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0; // DirectX 11


    // matrix constant buffer creation (usable to access vertex shader constant buffer)
    hR = m_core->m_getD3D ()->m_getDevice ()->CreateBuffer ( &matrixBufferDesc, nullptr, &m_matrixBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of matrix buffer failed!" );
      return;
    }


    // Camera application instantiation
    if (m_camera == nullptr)
    {
      m_camera = new (std::nothrow) Camera ( m_core );
      if (!m_camera->isInitialized ())
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Camera initialization failed!" );
        return;
      }
    }


    // dynamic diffuse light constant buffer description
    // purpose: to access internal variables introduced in pixel shader
    D3D11_BUFFER_DESC diffuesLightBufferDesc;
    diffuesLightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    diffuesLightBufferDesc.ByteWidth = sizeof ( LightBuffer );
    diffuesLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //diffuesLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    diffuesLightBufferDesc.MiscFlags = 0;
    diffuesLightBufferDesc.StructureByteStride = 0; // DirectX 11

    // diffuse light constant buffer creation (usable to access pixel shader constant buffer)
    hR = m_core->m_getD3D ()->m_getDevice ()->CreateBuffer ( &matrixBufferDesc, nullptr, &m_diffuseLightBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of diffuse light buffer failed!" );
      return;
    }


    // Diffuse light application instantiation
    if (m_diffuseLight == nullptr)
    {
      m_diffuseLight = new (std::nothrow) DiffuseLight;
      if (!m_diffuseLight->m_isInitialized ())
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Diffuse light initialization failed!" );
        return;
      }
    }


    m_initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Universe::m_renderResources ( void )
{
  try
  {

    HRESULT hR;


    //D3D11_MAPPED_SUBRESOURCE mappedResource;

    worldViewProjectionMatrices* dataPtrMatrix;


    // matrices setup + updating the view matrix each frame:

    // transpose process (preparation for shader)
    //DirectX::XMFLOAT4X4 world, view, projection;
    //DirectX::XMStoreFloat4x4 ( &world, DirectX::XMMatrixTranspose ( m_worldMatrix ) );
    //DirectX::XMStoreFloat4x4 ( &view, DirectX::XMMatrixTranspose ( m_camera->getView () ) );
    //DirectX::XMStoreFloat4x4 ( &projection, DirectX::XMMatrixTranspose ( m_projectionMatrix ) );

    //DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.world, DirectX::XMMatrixTranspose ( m_worldMatrix ) );
    DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.view, DirectX::XMMatrixTranspose ( m_camera->getView () ) );

    m_core->m_getD3D ()->m_getDevCon ()->UpdateSubresource1 ( m_matrixBuffer, 0, NULL, &m_worldViewProjectionDate, 0, 0, 0 );


    //// prepare for write (lock the constant buffer)
    //hR = m_core->m_getD3D ()->m_getDevCon ()->Map ( m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    //if (FAILED ( hR ))
    //{
    //  PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
    //                                              "Mapping the matrix buffer failed!" );
    //  return;
    //}


    //// initialize the pointer to the data already in the constant buffer
    //dataPtrMatrix = (MatrixBuffer*) mappedResource.pData;

    //// copy the matrices to the constant buffer
    //dataPtrMatrix->world = world;
    //dataPtrMatrix->view = view;
    //dataPtrMatrix->projection = projection;


    //// unlock and make the buffer usable
    //m_core->m_getD3D ()->m_getDevCon ()->Unmap ( m_matrixBuffer, 0 );


    // activate the updated constant matrix buffer in the vertex shader
    // first parameter: position of the constant buffer in the vertex shader
    m_core->m_getD3D ()->m_getDevCon ()->VSSetConstantBuffers ( 0, 1, &m_matrixBuffer );


    // diffuse light setup + updating it each frame:

    LightBuffer dataPtrDiffuseLight;

    // prepare for write (lock the constant buffer)
    //hR = m_core->m_getD3D ()->m_getDevCon ()->Map ( m_diffuseLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    //if (FAILED ( hR ))
    //{
    //  PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
    //                                              "Mapping the diffuse light buffer failed!" );
    //  return;
    //}


    // initialize the pointer to the data already in the constant buffer
    //dataPtrDiffuseLight = (LightBuffer*) mappedResource.pData;

    // copy the diffuse light structure to the constant buffer
    dataPtrDiffuseLight.ambientColour = m_diffuseLight->m_getAmbientColour ();
    dataPtrDiffuseLight.diffuseColour = m_diffuseLight->m_getDiffuseColour ();
    dataPtrDiffuseLight.diffuseDirection = m_diffuseLight->m_getDirection ();
    dataPtrDiffuseLight.padding = 0.0f;


    //// unlock and make the buffer usable
    //m_core->m_getD3D ()->m_getDevCon ()->Unmap ( m_diffuseLightBuffer, 0 );

    m_core->m_getD3D ()->m_getDevCon ()->UpdateSubresource1 ( m_diffuseLightBuffer, 0, NULL, &dataPtrDiffuseLight, 0, 0, 0 );

    // activate the updated constant diffuse light buffer in the pixel shader
    // first parameter: position of the constant buffer in the pixel shader
    m_core->m_getD3D ()->m_getDevCon ()->PSSetConstantBuffers ( 0, 1, &m_diffuseLightBuffer );


  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Universe::m_update ( void )
{
  try
  {


    m_worldRotationFactor -= .005f;
    if (m_worldRotationFactor < -360.0f)
      m_worldRotationFactor = .0f;

    // spin the game world using the updated rotation factor
    // the only parameter is angle of rotation.
    // note that looking along the rotation axis toward the origin, angles are measured clockwise.
    DirectX::XMStoreFloat4x4 ( &m_worldViewProjectionDate.world, DirectX::XMMatrixRotationY ( m_worldRotationFactor ) );

  }
  catch (const std::exception& ex)
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

    if (m_camera)
    {
      m_camera->m_release ();
      delete m_camera;
      m_camera = nullptr;
    }
    if (m_diffuseLight)
    {
      delete m_diffuseLight;
      m_camera = nullptr;
    }


    if (m_diffuseLightBuffer)
    {
      m_diffuseLightBuffer->Release ();
      m_diffuseLightBuffer = nullptr;
    }

    m_core = nullptr;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};

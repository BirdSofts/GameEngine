// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#include "Universe.h"
#include "Shared.h"


Universe::Universe ( ID3D10Device1* dev ) :
  device ( dev ), camera ( nullptr ), matrixWorldRotation ( 0.0f ), matrixBuffer ( nullptr ),
  lightDiffuse ( nullptr ), lightBufferDiffuse ( nullptr ), initialized ( false )
{
  try
  {

    HRESULT hR;

    // get description
    D3D10_VIEWPORT viewPort;
    unsigned int numberOf { 1 };
    device->RSGetViewports ( &numberOf, &viewPort );

    // projection matrix setup (usable for shaders)
    // purpose: translates 3D scene into the 2D viewport space
    float viewField { DirectX::XM_PI / 4.0f };
    float screenAspect { ( float) viewPort.Width / ( float) viewPort.Height };
    // projection matrix creation for 3D rendering
    matrixProjection = DirectX::XMMatrixPerspectiveFovLH ( viewField, screenAspect, screenNear, screenDepth );

    // word matrix initialization using identity matrix (usable for shaders)
    // purpose: converts objects' vertices into vertices in the 3D scene,
    // additionally to rotate, translate and scale our objects in 3D space
    matrixWorld = DirectX::XMMatrixIdentity ();

    // generally a view matrix representing the camera is initialized in this section (camera class)

    // orthographic projection matrix creation
    // purpose: to render 2D elements like user interface directly and skipping 3D rendering
    matrixOrthographic = DirectX::XMMatrixOrthographicLH ( ( float) viewPort.Width, ( float) viewPort.Height, screenNear, screenDepth );

    // dynamic matrix constant buffer description
    // purpose: to access internal variables introduced in vertex shader
    D3D10_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof ( MatrixBuffer );
    matrixBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    //matrixBufferDesc.StructureByteStride = 0; // DirectX 11

    // matrix constant buffer creation (usable to access vertex shader constant buffer)
    hR = device->CreateBuffer ( &matrixBufferDesc, nullptr, &matrixBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of matrix buffer failed!" );
      return;
    }

    // Camera application instantiation
    camera = new (std::nothrow) Camera;
    if (!camera->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Camera initialization failed!" );
      return;
    }

    // dynamic diffuse light constant buffer description
    // purpose: to access internal variables introduced in pixel shader
    D3D10_BUFFER_DESC lightBufferDiffuseDesc;
    lightBufferDiffuseDesc.Usage = D3D10_USAGE_DYNAMIC;
    lightBufferDiffuseDesc.ByteWidth = sizeof ( LightBuffer );
    lightBufferDiffuseDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    lightBufferDiffuseDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    lightBufferDiffuseDesc.MiscFlags = 0;
    //lightBufferDiffuseDesc.StructureByteStride = 0; // DirectX 11

    // diffuse light constant buffer creation (usable to access pixel shader constant buffer)
    hR = device->CreateBuffer ( &matrixBufferDesc, nullptr, &lightBufferDiffuse );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of diffuse light buffer failed!" );
      return;
    }

    // Diffuse light application instantiation
    lightDiffuse = new (std::nothrow) DiffuseLight;
    if (!lightDiffuse->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Diffuse light initialization failed!" );
      return;
    }

    initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Universe::isInitialized ( void )
{
  return initialized;
};


void Universe::renderResources ( void )
{
  try
  {

    HRESULT hR;

    void* mappedResource;

    MatrixBuffer* dataPtrMatrix;

    // matrices setup + updating the view matrix each frame

    // transpose process (preparation for shader)
    DirectX::XMMATRIX world, view, projection;
    world = DirectX::XMMatrixTranspose ( matrixWorld );
    view = DirectX::XMMatrixTranspose ( camera->getView () );
    projection = DirectX::XMMatrixTranspose ( matrixProjection );

    // prepare for write (lock the constant buffer)
    hR = matrixBuffer->Map ( D3D10_MAP_WRITE_DISCARD, 0, &mappedResource );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Mapping the matrix buffer failed!" );
      return;
    }

    // initialize the pointer to the data already in the constant buffer
    dataPtrMatrix = ( MatrixBuffer*) mappedResource;

    // copy the matrices to the constant buffer
    dataPtrMatrix->world = world;
    dataPtrMatrix->view = view;
    dataPtrMatrix->projection = projection;

    // unlock and make the buffer usable
    matrixBuffer->Unmap ();

    // activate the updated constant matrix buffer in the vertex shader
    // first parameter: position of the constant buffer in the vertex shader
    device->VSSetConstantBuffers ( 0, 1, &matrixBuffer );

    // diffuse light setup + updating it each frame

    LightBuffer* dataPtrDiffuseLight;

    // prepare for write (lock the constant buffer)
    hR = lightBufferDiffuse->Map ( D3D10_MAP_WRITE_DISCARD, 0, &mappedResource );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Mapping the diffuse light buffer failed!" );
      return;
    }

    // initialize the pointer to the data already in the constant buffer
    dataPtrDiffuseLight = ( LightBuffer*) mappedResource;

    // copy the diffuse light structure to the constant buffer
    dataPtrDiffuseLight->diffuseColour = lightDiffuse->getColour ();
    dataPtrDiffuseLight->diffuseDirection = lightDiffuse->getDirection ();
    dataPtrDiffuseLight->padding = 0.0f;

    // unlock and make the buffer usable
    lightBufferDiffuse->Unmap ();

    // activate the updated constant diffuse light buffer in the pixel shader
    // first parameter: position of the constant buffer in the pixel shader
    device->PSSetConstantBuffers ( 0, 1, &lightBufferDiffuse );


  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


Camera* Universe::getCamera ( void )
{
  return camera;
};


DiffuseLight* Universe::getDiffuseLight ( void )
{
  return lightDiffuse;
};


void Universe::update ( void )
{
  try
  {

    matrixWorldRotation += 0.0005f;
    if (matrixWorldRotation > 360.0f)
      matrixWorldRotation -= 360.0f;

    // spin the game world using the updated rotation factor
    // the only parameter is angle of rotation.
    // note that looking along the rotation axis toward the origin, angles are measured clockwise.
    matrixWorld = DirectX::XMMatrixRotationY ( matrixWorldRotation );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Universe::release ( void )
{
  try
  {

    if (matrixBuffer)
    {
      matrixBuffer->Release ();
      matrixBuffer = nullptr;
    }
    if (lightBufferDiffuse)
    {
      lightBufferDiffuse->Release ();
      lightBufferDiffuse = nullptr;
    }

    if (camera)
    {
      delete camera;
      camera = nullptr;
    }
    if (lightDiffuse)
    {
      delete lightDiffuse;
      camera = nullptr;
    }

    device = nullptr;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Direct3D.h"
#include "Shared.h"


Direct3D::Direct3D ( TheCore* coreObj ) :
  m_core ( coreObj ),
  m_vSync ( false ), m_allocated ( false ), m_initialized ( false )
{

  // reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
  m_backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
  m_backBufferCount = 2;

  m_depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

  m_featureLevel = {};

  m_displayModes = nullptr;
  m_displayModesCount = 0;
  m_displayMode = {};
  m_displayModeIndex = 0;
  m_videoCardMemory = 0;
  m_videoCardDescription = L"";

  m_initialized = true;
  PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "Direct3D is successfully initialized." );

  if (!m_createResources ()) // called on first initialization or in case of device lost
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Allocation of Direct3D resources failed." );

};


//Direct3D::~Direct3D ( void )
//{
//
//};


bool Direct3D::m_createResources ( void )
{
  try
  {

    HRESULT hR; // functions return value


    //vSync = PointerProvider::getConfiguration ()->getSettings ().vsync;


    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    // additional conditioned flag: device creation with debug options
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING


    // feature levels
    D3D_FEATURE_LEVEL featureLevels []
    {
      D3D_FEATURE_LEVEL_12_1,
      D3D_FEATURE_LEVEL_12_0,
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };



    // device and device context creation
    // first parameter: pointer to the present adapter on system
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> devCon;
    hR = D3D11CreateDevice ( nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                             creationFlags, featureLevels, 1, D3D11_SDK_VERSION,
                             device.put (), &m_featureLevel, devCon.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of Direct3D device failed! Error: " + std::to_string ( hR ) );
      return false;
    }
    hR = device.try_as ( m_device );
    if (SUCCEEDED ( hR ))
    {
      hR = devCon.try_as ( m_deviceContext );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Upgrading Direct3D device and its context failed! Error: " + std::to_string ( hR ) );
        return false;
      }
    }



#ifndef _NOT_DEBUGGING
    // acquiring the device's debug layer
    winrt::com_ptr<ID3D11Debug> debug;
    hR = m_device.try_as ( debug );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Retrieving Direct3D device debug layer failed! Error: " + std::to_string ( hR ) );
      return false;
    }
    winrt::com_ptr<ID3D11InfoQueue> infoQueue;
    hR = m_device.try_as ( infoQueue );
    if (SUCCEEDED ( hR ))
    {

#ifdef _DEBUG_SEVERITY
      infoQueue->SetBreakOnSeverity ( D3D11_MESSAGE_SEVERITY_CORRUPTION, true );
      infoQueue->SetBreakOnSeverity ( D3D11_MESSAGE_SEVERITY_ERROR, true );
#endif // !_NOT_DEBUGGING

      D3D11_MESSAGE_ID hide [] =
      {
        D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
        // needed additional message IDs can be added
      };
      D3D11_INFO_QUEUE_FILTER filter = {};
      filter.DenyList.NumIDs = _countof ( hide );
      filter.DenyList.pIDList = hide;
      infoQueue->AddStorageFilterEntries ( &filter );

    } else
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Adjusting device debug layer failed! Error: " + std::to_string ( hR ) );
      return false;
    }
#endif // !NDEBUG

    if (m_createDeviceResources ())
      return true;
    else
      return false;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


bool Direct3D::m_createDeviceResources ( void )
{
  try
  {

    HRESULT hR;

    if (m_swapChain)
    {

      // resizing the swap chain buffers (on resize of the client window)
      // BufferCount and SwapChainFlags: 0 do not change the current
      // 0 for the next two parameters to adjust to the current client window size automatically
      // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
      hR = m_swapChain->ResizeBuffers ( m_backBufferCount,
                                        m_core->m_mainPageTypes->m_getDisplay ()->outputWidthDips,
                                        m_core->m_mainPageTypes->m_getDisplay ()->outputHeightDips,
                                        m_backBufferFormat, 0 );
      if (hR == DXGI_ERROR_DEVICE_REMOVED || hR == DXGI_ERROR_DEVICE_RESET)
      {
        // on device lost/reset on any reason, a new device and swap chain is needed
        m_core->m_onDeviceLost ();
        return false; // so not continuing this and other procedures
      } else
      {
        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Resizing the swap chain failed! Error: " + std::to_string ( hR ) );
          return false;
        }
      }

    } else
    {

      // retrieve underlying DXGI device ( display modes + swap chain need)
      winrt::com_ptr<IDXGIDevice1> dxgiDevice; // DXGI device
      hR = m_device.try_as ( dxgiDevice );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Retrieving DXGI factory failed! Error: " + std::to_string ( hR ) );
        return false;
      }

      // present physical adapter (GPU or card) identification
      winrt::com_ptr<IDXGIAdapter> dxgiAdapter; // DXGI adapter
      hR = dxgiDevice->GetAdapter ( dxgiAdapter.put () );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Identification of DXGI adapter failed! Error: " + std::to_string ( hR ) );
        return false;
      }


      // primary output adapter (monitor) enumeration
      winrt::com_ptr<IDXGIOutput> dxgiOutput; // DXGI output
      hR = dxgiAdapter->EnumOutputs ( 0, dxgiOutput.put () );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the output device failed! Error: " + std::to_string ( hR ) );
        return false;
      }



      // two calls to enumerate the number and supported monitor display modes for the colour format:
      // -- the number of supported display modes (refer to MSDN for flags parameter)
      hR = dxgiOutput->GetDisplayModeList ( m_backBufferFormat, DXGI_ENUM_MODES_INTERLACED, &m_displayModesCount, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the number of supported display modes failed! Error: " + std::to_string ( hR ) );
        return false;
      }

      // -- acquiring all the supported display modes for current monitor / video card combination (refer to MSDN for flags parameter)
      m_displayModes = new (std::nothrow) DXGI_MODE_DESC [m_displayModesCount];
      if (!m_displayModes)
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Dynamic memory allocation for supported display modes failed! Error: " + std::to_string ( hR ) );
        return false;
      }
      hR = dxgiOutput->GetDisplayModeList ( m_backBufferFormat, DXGI_ENUM_MODES_INTERLACED, &m_displayModesCount, m_displayModes );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring all the supported display modes failed! Error: " + std::to_string ( hR ) );
        return false;
      }

      m_setDisplayMode ();



      // acquiring the video card description
      DXGI_ADAPTER_DESC dxgiAdapterDesc;
      hR = dxgiAdapter->GetDesc ( &dxgiAdapterDesc );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the description of video card failed! Error: " + std::to_string ( hR ) );
        return false;
      }
      m_videoCardMemory = (unsigned int) (dxgiAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
      m_videoCardDescription = dxgiAdapterDesc.Description;



      // obtain DXGI (DirectX graphics interface) factory (additional swap chain creation need)
      winrt::com_ptr<IDXGIFactory2> dxgiFactory; // DXGI factory
      hR = dxgiAdapter->GetParent ( IID_PPV_ARGS ( &dxgiFactory ) );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Obtaining DXGI factory failed! Error: " + std::to_string ( hR ) );
        return false;
      }



      DXGI_SCALING scaling = m_core->m_mainPageTypes->m_getDisplay ()->supportHighResolution ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
      // filling a swap chain description structure (the type of swap chain)
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
      swapChainDesc.Width = m_core->m_mainPageTypes->m_getDisplay ()->outputWidthDips; // back buffer size, 0: automatic adjustment from already calculated client window area
      swapChainDesc.Height = m_core->m_mainPageTypes->m_getDisplay ()->outputHeightDips; // the same
      //if (vSync) // lock to system settings 60Hz
      //{
      //  // back buffer to front buffer (screen) draw rate
      //  swapChainDesc.BufferDesc.RefreshRate.Numerator = displayMode.RefreshRate.Numerator;
      //  swapChainDesc.BufferDesc.RefreshRate.Denominator = displayMode.RefreshRate.Denominator;
      //} else // draw as many times as possible (may cause some visual artefacts)
      //{
      //  // note that not supported values may cause the DirectX to perform a blit instead of a buffer flip,
      //  // ending in degraded performance and unknown errors when debugging.
      //  swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
      //  swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
      //}
      swapChainDesc.Format = m_backBufferFormat; // display format
      swapChainDesc.Stereo = false;
      // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
      swapChainDesc.BufferCount = m_backBufferCount; // 3: including the front buffer (one front buffer and two back buffers)
      //swapChainDesc.OutputWindow = core->getHandle (); // handle to main window
      //swapChainDesc.Windowed = true; // recommendation: windowed creation and switch to full screen
      // flip (in windowed mode: blit) and discard the content of back buffer after presentation
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
      swapChainDesc.Flags = 0;
      //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode (advanced)
      //swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
      swapChainDesc.Scaling = scaling; // image size adjustment to back buffer resolution
      swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

      winrt::com_ptr<IDXGISwapChain1> swapChain;

      // swap chain creation
      hR = dxgiFactory->CreateSwapChainForComposition ( m_device.get (), &swapChainDesc, nullptr, swapChain.put () );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of swap chain failed! Error: " + std::to_string ( hR ) );
        return false;
      }
      hR = swapChain.try_as ( m_swapChain );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Upgrading swap chain failed! Error: " + std::to_string ( hR ) );
        return false;
      }



      hR = dxgiDevice->SetMaximumFrameLatency ( 1 );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "setting maximum frame latency failed! Error: " + std::to_string ( hR ) );
        return false;
      }

    }

    if (m_createDeviceContextResources ())
    {
      m_allocated = true;
      return true;
    } else
      return false;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


bool Direct3D::m_createDeviceContextResources ( void )
{
  try
  {

    HRESULT hR;


    // rotate swap chain
    hR = m_swapChain->SetRotation ( m_core->m_mainPageTypes->m_getDisplay ()->displayRotation );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Rotating the swap chain failed! Error: " + std::to_string ( hR ) );
      return false;
    }



    // Todo: research
    // setup inverse scale on the swap chain
    DXGI_MATRIX_3X2_F inverseScale = { 0 };
    inverseScale._11 = 1.0f / m_core->m_mainPageTypes->m_getDisplay ()->effectiveCompositionScaleX;
    inverseScale._22 = 1.0f / m_core->m_mainPageTypes->m_getDisplay ()->effectiveCompositionScaleY;
    winrt::com_ptr<IDXGISwapChain2> spSwapChain2;
    hR = m_swapChain.try_as ( spSwapChain2 );
    if (SUCCEEDED ( hR ))
      hR = spSwapChain2->SetMatrixTransform ( &inverseScale );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Setupping inverse scale on swap chain failed! Error: " + std::to_string ( hR ) );
      return false;
    }



    // render target buffer allocation
    // obtain a pointer to the current back buffer of swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    winrt::com_ptr<ID3D11Texture2D> rtBuffer; // render target view buffer
    hR = m_swapChain->GetBuffer ( 0, IID_PPV_ARGS ( &rtBuffer ) );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Acquiring the back buffer failed! Error: " + std::to_string ( hR ) );
      return false;
    }

    // render target view creation (attach the obtained back buffer to swap chain)
    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hR = m_device->CreateRenderTargetView1 ( rtBuffer.get (), nullptr,
                                             m_renderTargetView.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of render target view failed! Error: " + std::to_string ( hR ) );
      return false;
    }



    // depth-stencil buffer creation
    // depth buffer purpose: to render polygons properly in 3D space
    // stencil buffer purpose: to achieve effects such as motion blur, volumetric shadows etc.
    CD3D11_TEXTURE2D_DESC depthBufferDesc;
    //rtBuffer->GetDesc ( &depthBufferDesc ); // retrieves the back buffer description and fill
    depthBufferDesc.Format = m_depthBufferFormat; // 24 bits for depth and 8 bits for stencil
    depthBufferDesc.Width = m_core->m_mainPageTypes->m_getDisplay ()->outputWidthDips;
    depthBufferDesc.Height = m_core->m_mainPageTypes->m_getDisplay ()->outputHeightDips;
    depthBufferDesc.MipLevels = 1; // a single mipmap level
    depthBufferDesc.ArraySize = 1; // only one texture
    depthBufferDesc.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    // note texture 2d function: sorted and rasterized polygons are just coloured pixels in 2d representation
    hR = m_device->CreateTexture2D ( &depthBufferDesc, nullptr, m_depthStencilBuffer.put () );
    //hR = dsBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dsSurface );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of depth-stencil buffer failed! Error: " + std::to_string ( hR ) );
      return false;
    }



    // depth-stencil state description
    // to control the depth test and its type, that Direct3D performs for each pixel
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    // stencil operations (if pixel is front facing)
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // stencil operations (if pixel is front facing)
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // depth stencil state creation
    hR = m_device->CreateDepthStencilState ( &depthStencilDesc, m_depthStencilState.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of depth-stencil state failed! Error: " + std::to_string ( hR ) );
      return false;
    }
    // set the active depth stencil state
    m_deviceContext->OMSetDepthStencilState ( m_depthStencilState.get (), 1 );



    // depth-stencil view description
    // purpose: so the Direct3D use the depth buffer as a depth stencil texture.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = m_depthBufferFormat;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hR = m_device->CreateDepthStencilView ( m_depthStencilBuffer.get (), &depthStencilViewDesc, m_depthStencilView.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation the depth-stencil view failed! Error: " + std::to_string ( hR ) );
      return false;
    }



    // binding render target view and depth-stencil view to output render pipeline (for now one render target view)
    // purpose: rendered graphics by the pipeline will be drawn to the back buffer.
    // second parameter: pointer to first element of a list of render target view pointers
    //m_deviceContext->OMSetRenderTargets ( 1, m_renderTview.put (), m_depthSview.get () );
    ID3D11RenderTargetView* const targets [1] = { m_renderTargetView.get () };
    m_deviceContext->OMSetRenderTargets ( 1, targets, m_depthStencilView.get () );



    // rasterizer description (determines how and which polygons is to be rendered)
    // for example: render scenes in wireframe mode, draw both front and back faces of polygons
    // note that by default DirectX creates one rasterizer the same as below, on which developers have no control.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    // rasterizer creation
    hR = m_device->CreateRasterizerState ( &rasterizerDesc, m_rasterizerState.put () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation the rasterizer state failed! Error: " + std::to_string ( hR ) );
      return false;
    }
    // set the active rasterizer state
    m_deviceContext->RSSetState ( m_rasterizerState.get () );



    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    // with other words: so Direct3D can map clip space coordinates to the render target space
    m_screenViewPort.Width = float ( m_core->m_mainPageTypes->m_getDisplay ()->outputWidthDips );
    m_screenViewPort.Height = float ( m_core->m_mainPageTypes->m_getDisplay ()->outputHeightDips );
    m_screenViewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    m_screenViewPort.MaxDepth = 1.0f;
    m_screenViewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    m_screenViewPort.TopLeftY = 0;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    m_deviceContext->RSSetViewports ( 1, &m_screenViewPort );



    m_clearBuffers ();

    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


bool Direct3D::m_initialize ( void )
{
  try
  {
    //
    return true;
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


void Direct3D::m_setDisplayMode ( void )
{
  try
  {

    //HRESULT hR;


    if (m_displayMode.Width == 0)
    {
      for (unsigned int i = 0; i < m_displayModesCount; i++)
      {
        // support check for current resolution of the client window (at engine initialization streamed from settings file)
        if (m_displayModes [i].Width == m_core->m_mainPageTypes->m_getDisplay ()->panelWidthPixels &&
             m_displayModes [i].Height == m_core->m_mainPageTypes->m_getDisplay ()->panelHeightPixels)
        {
          m_displayMode = m_displayModes [i];
          m_displayModeIndex = i;
          break;
        }
      }

      if (m_displayMode.Width == 0)
      {
        // not supported: set to the lowest supported resolution
        m_displayMode = m_displayModes [0];
        m_displayModeIndex = 0;

        PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                    "The chosen resolution is not supported!" );

        // rewrite a not valid configurations with defaults: the file is probably modified from outside of the application
        if (!PointerProvider::getConfiguration ()->m_apply ( PointerProvider::getConfiguration ()->m_getDefaults () ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Rewriting the Configuration file with default settings failed." );
        }
      }
    }
  }


  //if (m_swapChain)
  //{
  //  // setting the resolution
  //  hR = m_swapChain->ResizeTarget ( &m_displayMode );
  //  if (FAILED ( hR ))
  //  {
  //    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
  //                                                "Setting a supported resolution failed." );
  //  }
  //}

  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct3D::m_release ( void )
{

  // power suspension procedure
  //m_deviceContext->ClearState ();
  //winrt::com_ptr<IDXGIDevice3> dxgiDevice;
  //if (SUCCEEDED ( m_device.try_as ( dxgiDevice ) ))
  //{
  //  dxgiDevice->Trim ();
  //  PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
  //                                              "Direct3D is successfully suspended." );
  //  dxgiDevice->Release ();
  //}


  unsigned long rC { 0 };
  HRESULT hR;

  if (m_allocated)
  {

    m_allocated = false;

    m_deviceContext->ClearState ();
    rC = m_rasterizerState->Release ();
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_deviceContext->OMSetRenderTargets ( _countof ( nullViews ), nullViews, nullptr );
    rC = m_depthStencilView->Release ();
    rC = m_depthStencilState->Release ();
    rC = m_depthStencilBuffer->Release ();
    rC = m_renderTargetView->Release ();

    m_deviceContext->Flush ();

  }

  rC = m_swapChain->Release ();
  rC = m_deviceContext->Release ();
  rC = m_device->Release ();

  m_swapChain.detach ();
  m_deviceContext.detach ();
  m_device.detach ();

  if (rC)
    PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                "Releasing one or more Direct3D resources failed!" );

  if (!m_core->m_deviceRestored)
  {

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct3D resources are successfully released." );

  } else
  {

    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct3D resources are successfully destructed." );

    m_initialized = false;

    m_core = nullptr;

  }

};


void Direct3D::m_validate ( void )
{

  // if default adapter since creation of D3D device is changed or removed, the device is not valid any more

  HRESULT hR;


  // first test preparation
  DXGI_ADAPTER_DESC t_previousDesc {};

  winrt::com_ptr<IDXGIDevice3> t_dxgiDevice;
  hR = m_device.try_as ( t_dxgiDevice );
  if (SUCCEEDED ( hR ))
  {

    winrt::com_ptr<IDXGIAdapter> t_deviceAdapter {};
    hR = t_dxgiDevice->GetAdapter ( t_deviceAdapter.put () );
    if (SUCCEEDED ( hR ))
    {

      winrt::com_ptr<IDXGIFactory2> t_dxgiFactory;
      t_deviceAdapter->GetParent ( IID_PPV_ARGS ( &t_dxgiFactory ) );
      if (SUCCEEDED ( hR ))
      {

        winrt::com_ptr<IDXGIAdapter1> t_previousDefaultAdapter;
        hR = t_dxgiFactory->EnumAdapters1 ( 0, t_previousDefaultAdapter.put () );

        if (SUCCEEDED ( hR ))
          t_previousDefaultAdapter->GetDesc ( &t_previousDesc );


        if (FAILED ( hR ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Device Validation on stage one failed." );
        }

      }
    }
  }


  // second test preparation
  DXGI_ADAPTER_DESC t_currentDesc {};

  winrt::com_ptr<IDXGIFactory2> t_currentFactory;
  CreateDXGIFactory1 ( IID_PPV_ARGS ( &t_currentFactory ) );
  if (SUCCEEDED ( hR ))
  {

    winrt::com_ptr<IDXGIAdapter1> t_currentDefaultAdapter;
    hR = t_currentFactory->EnumAdapters1 ( 0, t_currentDefaultAdapter.put () );

    if (SUCCEEDED ( hR ))
      t_currentDefaultAdapter->GetDesc ( &t_currentDesc );

    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Device Validation on stage two failed." );
    }

  }


  // test: compare adapter LUIDs additionally to enumeration of devices's removal reason
  if (t_previousDesc.AdapterLuid.LowPart != t_currentDesc.AdapterLuid.LowPart
       || t_previousDesc.AdapterLuid.HighPart != t_currentDesc.AdapterLuid.HighPart
       || FAILED ( m_device->GetDeviceRemovedReason () ))
  {
    m_core->m_onDeviceLost ();
  }

};


void Direct3D::m_clearBuffers ( void )
{
  try
  {

    const float blue [] { 0.11f, 0.33f, 0.55f, 1.0f };
    // filling the entire back buffer with a single colour
    m_deviceContext->ClearRenderTargetView ( m_renderTargetView.get (), blue );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    if (m_depthStencilView)
      m_deviceContext->ClearDepthStencilView ( m_depthStencilView.get (), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct3D::m_present ( void )
{
  try
  {

    HRESULT hR;

    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    if (m_vSync)
      hR = { m_swapChain->Present ( 1, DXGI_PRESENT_DO_NOT_WAIT ) }; // consider screen refresh rate
    else
      hR = { m_swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) }; // as fast as possible

    // check device removal happened either on a disconnection or a driver upgrade
    if (hR == DXGI_ERROR_DEVICE_REMOVED || hR == DXGI_ERROR_DEVICE_RESET)
    {
      // on device lost/reset, a new device and swap chain is needed
      m_core->m_onDeviceLost ();
    } else

      if ((FAILED ( hR )) &&
           (hR != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
      {
        PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "gameThread",
                                                    "Presentation of the scene failed!" );
      }



    // valid only when existing content are to be overwritten,
    // if dirty or scroll rects are used, these calls must be modified
    //m_deviceContext->DiscardView1 ( m_renderTview.get (), nullptr, 0 );
    //m_deviceContext->DiscardView1 ( m_depthSview.get (), nullptr, 0 );



    // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
    //m_deviceContext->OMSetRenderTargets ( 1, m_renderTview.put (), m_depthSview.get () );
    ID3D11RenderTargetView* const targets [1] = { m_renderTargetView.get () };
    m_deviceContext->OMSetRenderTargets ( 1, targets, m_depthStencilView.get () );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "gameThread",
                                                ex.what () );
  }
};

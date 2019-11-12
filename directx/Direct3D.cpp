// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,12.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Direct3D.h"
#include "Shared.h"


using namespace Microsoft::WRL;


Direct3D::Direct3D ( TheCore* coreObj ) :
  m_core ( coreObj ),
  m_vSync ( false ), m_allocated ( false ), m_initialized ( false )
{

  // reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
  m_backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
  m_backBufferCount = 3;

  m_depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

  m_featureLevel = {};

  m_displayModes = nullptr;
  m_displayModesCount = 0;
  m_displayMode = {};
  m_displayModeIndex = 0;
  m_videoCardMemory = 0;
  m_videoCardDescription = L"";

  m_creation ();

};


//Direct3D::~Direct3D ( void )
//{
//
//};


const bool& Direct3D::m_isInitialized ( void )
{
  return m_initialized;
};


void Direct3D::m_creation ( void )
{
  try
  {

    HRESULT hR; // functions return value


    // clear previous creations
    m_initialized = false;
    m_device.Reset ();
    m_deviceContext.Reset ();



    //vSync = PointerProvider::getConfiguration ()->getSettings ().vsync;


    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    // additional conditioned flag: device creation with debug options
#ifndef _NOT_DEBUGGING
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



    // creation of the device
    // first parameter: pointer to the present adapter on system
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> devCon;
    hR = D3D11CreateDevice ( nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                             creationFlags, featureLevels, 1, D3D11_SDK_VERSION,
                             &device, &m_featureLevel, &devCon );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of Direct3D device failed!" );
      return;
    }
    hR = device.As ( &m_device );
    if (SUCCEEDED ( hR ))
    {
      hR = devCon.As ( &m_deviceContext );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Upgrading Direct3D device and its context failed!" );
        return;
      }
    }



#ifndef _NOT_DEBUGGING
    // acquiring the device's debug layer
    ComPtr<ID3D11Debug> debug;
    hR = m_device.As ( &debug );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Retrieving Direct3D device debug layer failed!" );
      return;
    }
    ComPtr<ID3D11InfoQueue> infoQueue;
    hR = m_device.As ( &infoQueue );
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

    }
#endif // !NDEBUG



    m_initialized = true;
    PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Direct3D is successfully initialized." );



    m_allocation ();
    if (!m_allocated)
      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "Allocation of Direct3D resources failed." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct3D::m_allocation ( void )
{
  try
  {

    HRESULT hR;


    // clear previous allocations
    m_allocated = false;
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_deviceContext->OMSetRenderTargets ( _countof ( nullViews ), nullViews, nullptr );
    m_renderTview.Reset ();
    m_depthSview.Reset ();
    m_deviceContext->Flush ();



    if (m_swapChain)
    {

      // resizing the swap chain buffers (on resize of the client window)
      // BufferCount and SwapChainFlags: 0 do not change the current
      // 0 for the next two parameters to adjust to the current client window size automatically
      // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
      hR = m_swapChain->ResizeBuffers ( m_backBufferCount, m_displayMode.Width,
                                        m_displayMode.Height, m_backBufferFormat, 0 );
      if (hR == DXGI_ERROR_DEVICE_REMOVED || hR == DXGI_ERROR_DEVICE_RESET)
      {
        // on device lost/reset, a new device and swap chain is needed
        m_onDeviceLost ();
        //return; ///
      } else
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Resizing the swap chain failed!" );
      }

    } else
    {



      // retrieve underlying DXGI device ( display modes + swap chain need)
      ComPtr<IDXGIDevice1> dxgiDevice; // DXGI device
      hR = m_device.As ( &dxgiDevice );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Retrieving DXGI factory failed!" );
        return;
      }

      // present physical adapter (GPU or card) identification
      ComPtr<IDXGIAdapter> dxgiAdapter; // DXGI adapter
      hR = dxgiDevice->GetAdapter ( &dxgiAdapter );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Identification of DXGI adapter failed!" );
        return;
      }


      // primary output adapter (monitor) enumeration
      ComPtr<IDXGIOutput> dxgiOutput; // DXGI output
      hR = dxgiAdapter->EnumOutputs ( 0, &dxgiOutput );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the output device failed!" );
        return;
      }



      // two calls to enumerate the number and supported monitor display modes for the colour format:
      // -- the number of supported display modes (refer to MSDN for flags parameter)
      hR = dxgiOutput->GetDisplayModeList ( m_backBufferFormat, DXGI_ENUM_MODES_INTERLACED, &m_displayModesCount, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the number of supported display modes failed!" );
        return;
      }

      // -- acquiring all the supported display modes for current monitor / video card combination (refer to MSDN for flags parameter)
      m_displayModes = new (std::nothrow) DXGI_MODE_DESC [m_displayModesCount];
      if (!m_displayModes)
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Dynamic memory allocation for supported display modes failed!" );
        return;
      }
      hR = dxgiOutput->GetDisplayModeList ( m_backBufferFormat, DXGI_ENUM_MODES_INTERLACED, &m_displayModesCount, m_displayModes );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring all the supported display modes failed!" );
        return;
      }

      m_setDisplayMode ();



      // acquiring the video card description
      DXGI_ADAPTER_DESC dxgiAdapterDesc;
      hR = dxgiAdapter->GetDesc ( &dxgiAdapterDesc );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Acquiring the description of video card failed!" );
        return;
      }
      m_videoCardMemory = (unsigned int) (dxgiAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
      m_videoCardDescription = dxgiAdapterDesc.Description;



      // obtain DXGI (DirectX graphics interface) factory (additional swap chain creation need)
      ComPtr<IDXGIFactory2> dxgiFactory; // DXGI factory
      hR = dxgiAdapter->GetParent ( IID_PPV_ARGS ( &dxgiFactory ) );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Obtaining DXGI factory failed!" );
        return;
      }



      // filling a swap chain description structure (the type of swap chain)
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
      swapChainDesc.Width = m_displayMode.Width; // back buffer size, 0: automatic adjustment from already calculated client window area
      swapChainDesc.Height = m_displayMode.Height; // the same
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
      // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
      swapChainDesc.BufferCount = m_backBufferCount; // 3: including the front buffer (one front buffer and two back buffers)
      //swapChainDesc.OutputWindow = core->getHandle (); // handle to main window
      //swapChainDesc.Windowed = true; // recommendation: windowed creation and switch to full screen
      // flip (in windowed mode: blit) and discard the content of back buffer after presentation
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode (advanced)
      //swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
      swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH; // image size adjustment to back buffer resolution
      swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

      Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

      // swap chain creation
      hR = dxgiFactory->CreateSwapChainForCoreWindow ( m_device.Get (), m_core->m_appWindow, &swapChainDesc, nullptr, &swapChain );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Creation of swap chain failed!" );
        return;
      }
      hR = swapChain.As ( &m_swapChain );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Upgrading swap chain failed!" );
        return;
      }

    }



    // render target buffer allocation
    // obtain a pointer to the current back buffer of swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    ComPtr<ID3D11Texture2D> rtBuffer; // render target view buffer
    hR = m_swapChain->GetBuffer ( 0, IID_PPV_ARGS ( &rtBuffer ) );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Acquiring the back buffer failed!" );
      return;
    }

    // render target view creation (attach the obtained back buffer to swap chain)
    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hR = m_device->CreateRenderTargetView ( rtBuffer.Get (), nullptr,
                                            m_renderTview.ReleaseAndGetAddressOf () );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of render target view failed!" );
      return;
    }



    // depth-stencil buffer creation
    // depth buffer purpose: to render polygons properly in 3D space
    // stencil buffer purpose: to achieve effects such as motion blur, volumetric shadows etc.
    CD3D11_TEXTURE2D_DESC depthBufferDesc;
    //rtBuffer->GetDesc ( &depthBufferDesc ); // retrieves the back buffer description and fill
    depthBufferDesc.Format = m_depthBufferFormat; // 24 bits for depth and 8 bits for stencil
    depthBufferDesc.Width = m_displayMode.Width;
    depthBufferDesc.Height = m_displayMode.Height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    // note texture 2d function: sorted and rasterized polygons are just coloured pixels in 2d representation
    hR = m_device->CreateTexture2D ( &depthBufferDesc, nullptr, &m_depthSbuffer );
    //hR = dsBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dsSurface );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of depth-stencil buffer failed!" );
      return;
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
    hR = m_device->CreateDepthStencilState ( &depthStencilDesc, &m_depthSstate );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of depth-stencil state failed!" );
      return;
    }
    // set the active depth stencil state
    m_deviceContext->OMSetDepthStencilState ( m_depthSstate.Get (), 1 );



    // depth-stencil view description
    // purpose: so the Direct3D use the depth buffer as a depth stencil texture.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = m_depthBufferFormat;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hR = m_device->CreateDepthStencilView ( m_depthSbuffer.Get (), &depthStencilViewDesc, &m_depthSview );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation the depth-stencil view failed!" );
      return;
    }



    // binding render target view and depth-stencil view to output render pipeline (for now one render target view)
    // purpose: rendered graphics by the pipeline will be drawn to the back buffer.
    // second parameter: pointer to first element of a list of render target view pointers
    m_deviceContext->OMSetRenderTargets ( 1, m_renderTview.GetAddressOf (), m_depthSview.Get () );



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
    hR = m_device->CreateRasterizerState ( &rasterizerDesc, &m_rasterizerState );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation the rasterizer state failed!" );
      return;
    }
    // set the active rasterizer state
    m_deviceContext->RSSetState ( m_rasterizerState.Get () );



    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    // with other words: so Direct3D can map clip space coordinates to the render target space
    D3D11_VIEWPORT viewPort;
    viewPort.Width = float ( m_displayMode.Width );
    viewPort.Height = float ( m_displayMode.Height );
    viewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    viewPort.MaxDepth = 1.0f;
    viewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    viewPort.TopLeftY = 0;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    m_deviceContext->RSSetViewports ( 1, &viewPort );



    m_allocated = true;



    m_clearBuffers ();

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct3D::m_setDisplayMode ( void )
{
  try
  {

    HRESULT hR;


    if (m_displayMode.Width == 0)
    {
      for (unsigned int i = 0; i < m_displayModesCount; i++)
      {
        // support check for current resolution of the client window (at engine initialization streamed from settings file)
        if (m_displayModes [i].Width == static_cast<UINT>(m_core->m_outputWidth))
          if (m_displayModes [i].Height == static_cast<UINT>(m_core->m_outputHeight))
          {
            m_displayMode = m_displayModes [i];
            m_displayModeIndex = i;
            break;
          } else
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

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Direct3D::m_validate ( void )
{

  // if default adapter since creation of D3D device is changed or removed, the device is not valid any more

  HRESULT hR;


  // first test preparation
  DXGI_ADAPTER_DESC t_previousDesc;
  {

    ComPtr<IDXGIDevice3> t_dxgiDevice;
    hR = m_device.As ( &t_dxgiDevice );
    if (SUCCEEDED ( hR ))
    {

      ComPtr<IDXGIAdapter> t_deviceAdapter;
      hR = t_dxgiDevice->GetAdapter ( &t_deviceAdapter );
      if (SUCCEEDED ( hR ))
      {

        ComPtr<IDXGIFactory2> t_dxgiFactory;
        t_deviceAdapter->GetParent ( IID_PPV_ARGS ( &t_dxgiFactory ) );
        if (SUCCEEDED ( hR ))
        {

          ComPtr<IDXGIAdapter1> t_previousDefaultAdapter;
          hR = t_dxgiFactory->EnumAdapters1 ( 0, &t_previousDefaultAdapter );

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
  }


  // second test preparation
  DXGI_ADAPTER_DESC t_currentDesc;
  {

    ComPtr<IDXGIFactory2> t_currentFactory;
    CreateDXGIFactory1 ( IID_PPV_ARGS ( &t_currentFactory ) );
    if (SUCCEEDED ( hR ))
    {

      ComPtr<IDXGIAdapter1> t_currentDefaultAdapter;
      hR = t_currentFactory->EnumAdapters1 ( 0, &t_currentDefaultAdapter );

      if (SUCCEEDED ( hR ))
        t_currentDefaultAdapter->GetDesc ( &t_currentDesc );

      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Device Validation on stage two failed." );
      }

    }
  }


  // test: compare adapter LUIDs additionally to enumeration of devices's removal reason
  if (t_previousDesc.AdapterLuid.LowPart != t_currentDesc.AdapterLuid.LowPart
       || t_previousDesc.AdapterLuid.HighPart != t_currentDesc.AdapterLuid.HighPart
       || FAILED ( m_device->GetDeviceRemovedReason () ))
  {
    // one is enough for a new creation! :)
    m_onDeviceLost ();
  }

};


void Direct3D::m_onDeviceLost ( void )
{

  //m_core->m_D2D->m_initialized = false;
  //m_core->m_D2D->m_dcBitmap.Reset ();
  //m_core->m_D2D->m_deviceContext.Reset ();
  //m_core->m_D2D->m_dcBuffer.Reset ();
  //m_core->m_D2D->m_device.Reset ();
  //m_core->m_D2D->m_factory.Reset ();
  //m_core->m_D2D->m_writeFactory.Reset ();

  m_initialized = false;
  m_deviceContext->ClearState ();
  m_rasterizerState.Reset ();
  m_deviceContext->OMSetRenderTargets ( 0, nullptr, nullptr );
  m_depthSview.Reset ();
  m_depthSstate.Reset ();
  m_depthSbuffer.Reset ();
  m_renderTview.Reset ();
  m_swapChain.Reset ();
  m_device.Reset ();

  m_creation ();

};


void Direct3D::m_clearBuffers ( void )
{
  try
  {

    const float blue [] { 0.11f, 0.33f, 0.55f, 1.0f };
    // filling the entire back buffer with a single colour
    m_deviceContext->ClearRenderTargetView ( m_renderTview.Get (), blue );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    if (m_depthSview)
      m_deviceContext->ClearDepthStencilView ( m_depthSview.Get (), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

  }
  catch (const std::exception & ex)
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

    if ((FAILED ( hR )) &&
      (hR != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
    {
      PointerProvider::getFileLogger ()->m_push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                  "The presentation of the scene failed!" );
    }

    // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
    if (m_depthSview)
      m_deviceContext->OMSetRenderTargets ( 1, m_renderTview.GetAddressOf (), m_depthSview.Get () );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


const ComPtr<ID3D11Device> Direct3D::m_getDevice ( void )
{
  return m_device;
};


const ComPtr<ID3D11DeviceContext3> Direct3D::m_getDevCon ( void )
{
  return m_deviceContext;
};


const Microsoft::WRL::ComPtr<IDXGISwapChain3> Direct3D::m_getSwapChain ( void )
{
  return m_swapChain;
};


const DXGI_FORMAT& Direct3D::m_getBackBufferFormat ()
{
  return m_backBufferFormat;
};

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Direct3D.h"


Direct3D::Direct3D ( TheCore* coreObj ) :
  core ( coreObj ), colourFormat ( DXGI_FORMAT_B8G8R8A8_UNORM ),
  // reserve 8 bits for red, green, blue and transparency each in unsigned normalized integer
  displayModesCount ( 0 ), displayModeIndex ( 0 ), videoCardMemory ( 0 ), videoCardDescription ( L"" ),
  fullscreen ( false ), vSync ( false ), initialized ( false ), allocated ( false )
{
  try
  {

    HRESULT hR; // functions return value
    unsigned long rC { 0 }; // reference counts

    fullscreen = PointerProvider::getConfiguration ()->getSettings ().fullscreen;

    //vSync = PointerProvider::getConfiguration ()->getSettings ().vsync;

    // DXGI (DirectX graphics interface) factory creation
    IDXGIFactory2* dxgiFactory; // DXGI factory
    hR = CreateDXGIFactory1 ( __uuidof(IDXGIFactory1), (void**) &dxgiFactory );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of DXGI factory failed!" );
      return;
    }

    // adapter creation for physical graphics interface using DXGI factory (0: primary device)
    IDXGIAdapter1* dxgiAdapter; // DXGI adapter
    hR = dxgiFactory->EnumAdapters1 ( 0, &dxgiAdapter );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of DXGI adapter failed!" );
      return;
    }

    // primary output adapter (monitor) enumeration
    IDXGIOutput* dxgiOutput; // DXGI output
    hR = dxgiAdapter->EnumOutputs ( 0, &dxgiOutput );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Acquiring the output device failed!" );
      return;
    }

    // two calls to enumerate the number and supported monitor display modes for the colour format:
    // -- the number of supported display modes (refer to MSDN for flags parameter)
    hR = dxgiOutput->GetDisplayModeList ( colourFormat, DXGI_ENUM_MODES_INTERLACED, &displayModesCount, nullptr );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Acquiring the number of supported display modes failed!" );
      return;
    }

    // -- acquiring all the supported display modes for current monitor / video card combination (refer to MSDN for flags parameter)
    displayModes = new (std::nothrow) DXGI_MODE_DESC [displayModesCount];
    if (!displayModes)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Dynamic memory allocation for supported display modes failed!" );
      return;
    }
    hR = dxgiOutput->GetDisplayModeList ( colourFormat, DXGI_ENUM_MODES_INTERLACED, &displayModesCount, displayModes );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Acquiring all the supported display modes failed!" );
      return;
    }

    for (unsigned int i = 0; i < displayModesCount; i++)
    {
      // support check for current resolution of the client window (input streamed from setting file)
      if (displayModes [i].Width == PointerProvider::getConfiguration ()->getSettings ().Width) ///
        if (displayModes [i].Height == PointerProvider::getConfiguration ()->getSettings ().Height)
        {
          displayMode = displayModes [i];
          displayModeIndex = i;
          break;
        } else
        {
          // not supported: set to the lowest supported resolution
          displayMode = displayModes [0];
          displayModeIndex = 0;

          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                    "The chosen resolution is not supported!" );

          // rewrite a not valid configurations with defaults: the file is probably modified from outside of the application
          if (!PointerProvider::getConfiguration ()->apply ( PointerProvider::getConfiguration ()->getDefaults () ))
          {
            PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Rewriting the Configuration file with default settings failed." );
          }
        }
    }
    rC = dxgiOutput->Release ();
    dxgiOutput = nullptr;

    // acquiring the video card description
    DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
    hR = dxgiAdapter->GetDesc1 ( &dxgiAdapterDesc );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Acquiring the description of video card failed!" );
      return;
    }
    videoCardMemory = (unsigned int) (dxgiAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
    videoCardDescription = dxgiAdapterDesc.Description;
    rC = dxgiAdapter->Release ();
    dxgiAdapter = nullptr;

    // flag: needed to get Direct2D interoperability with Direct3D resources
    unsigned int deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    // device creation debug option flag
#ifndef _NOT_DEBUGGING
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // creation with debug layer
#endif // !_NOT_DEBUGGING

    // creation of the device
    // first parameter: pointer to the present adapter on system
    D3D_FEATURE_LEVEL featureLevels { D3D_FEATURE_LEVEL_11_1 };
    D3D_FEATURE_LEVEL featureLevel {};
    hR = D3D11CreateDevice ( nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                             deviceFlags, &featureLevels, 1, D3D11_SDK_VERSION, &device, &featureLevel, &devCon );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "The creation of Direct3D device failed!" );
      return;
    }

#ifndef _NOT_DEBUGGING
    // acquiring the device's debug layer
    // note that live report is available from Direct3D 11
    hR = device->QueryInterface ( __uuidof(ID3D11Debug), &debug );

    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "The creation of Direct3D device debug layer failed!" );
      return;
    }
#endif // !_NOT_DEBUGGING

    // filling a swap chain description structure (the type of swap chain)
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = displayMode.Width; // back buffer size, 0: automatic adjustment from already calculated client window area
    swapChainDesc.Height = displayMode.Height; // the same
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
    swapChainDesc.Format = colourFormat; // display format
    // number of multi samplings per pixel and image quality (1 and 0: disable multi sampling (no anti-aliasing))
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // back buffer as render output target
    swapChainDesc.BufferCount = 3; // including the front buffer (one front buffer and two back buffers)
    //swapChainDesc.OutputWindow = core->getHandle (); // handle to main window
    //swapChainDesc.Windowed = true; // recommendation: windowed creation and switch to full screen
    // flip (in windowed mode: blit) and discard the content of back buffer after presentation
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switching the display mode (advanced)
    //swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // scan-line drawing
    swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH; // image size adjustment to back buffer resolution
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    // swap chain creation
    hR = dxgiFactory->CreateSwapChainForCoreWindow ( device.Get (), core->appWindow, &swapChainDesc, nullptr, &swapChain );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of swap chain failed!" );
      return;
    }
    rC = dxgiFactory->Release ();
    dxgiFactory = nullptr;

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                              "Direct3D is successfully initialized." );

    displayModeSetter ();
    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                "Allocation of Direct3D resources failed." );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


const bool& Direct3D::isInitialized ( void )
{
  return initialized;
};


const ID3D11Device& Direct3D::getDevice ( void )
{
  return *device.Get ();
};


const bool& Direct3D::isFullscreen ( void )
{
  return fullscreen;
};


void Direct3D::displayModeSetter ( void )
{
  try
  {

    HRESULT hR;

    if (fullscreen)
    {
      // switch to fullscreen mode
      hR = swapChain->SetFullscreenState ( true, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Switching to fullscreen mode failed." );
        fullscreen = false;
        return;
      }
    } else
    {
      // switch to windowed mode
      hR = swapChain->SetFullscreenState ( false, nullptr );
      if (FAILED ( hR ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Switching to windowed mode failed." );
        fullscreen = true;
        return;
      }
    }

    // setting the resolution
    hR = swapChain->ResizeTarget ( &displayMode );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Setting a supported resolution failed." );
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Direct3D::allocateResources ( void )
{
  try
  {

    allocated = false;
    HRESULT hR;
    unsigned long rC { 0 };

    // resizing the swap chain buffers (on resize of the client window)
    // BufferCount and SwapChainFlags: 0 do not change the current
    // 0 for the next two parameters to adjust to the current client window size automatically
    // next parameter: set to DXGI_FORMAT_UNKNOWN to preserve the current
    hR = swapChain->ResizeBuffers ( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Resizing the swap chain failed!" );
      return;
    }

    // obtain a pointer to the current back buffer of swap chain
    // the zero-th buffer is accessible, since already created using flip discarding effect.
    // second parameter: interface type (most cases 2D- texture)
    // the last parameter returns a pointer to the actual back buffer
    ID3D11Texture2D* rtBuffer; // render target view buffer
    hR = swapChain->GetBuffer ( 0, __uuidof(ID3D11Texture2D), (LPVOID*) &rtBuffer );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Acquiring the back buffer failed!" );
      return;
    }

    // render target view creation (attach the obtained back buffer to swap chain)
    // first parameter: the resource for which the render target is created for
    // second parameter describes data type of the specified resource (mipmap but 0 for now)
    // the last parameter returns a pointer to the created render target view
    hR = device->CreateRenderTargetView ( rtBuffer, nullptr, &rTview );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of render target view failed!" );
      return;
    }

    // depth-stencil buffer creation
    // depth buffer purpose: to render polygons properly in 3D space
    // stencil buffer purpose: to achieve effects such as motion blur, volumetric shadows etc.
    CD3D11_TEXTURE2D_DESC depthBufferDesc;
    rtBuffer->GetDesc ( &depthBufferDesc ); // retrieves the back buffer description and fill
    //descDepth.Width = ;
    //descDepth.Height = ;
    //descDepth.MipLevels = 1;
    //descDepth.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
    //descDepth.SampleDesc.Count = 1; // multi-sampling (anti-aliasing) match to settings of render target
    //descDepth.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; // value: only GPU will be reading and writing to the resource
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // how to bind to the different pipeline stages
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    rC = rtBuffer->Release ();
    rtBuffer = nullptr;

    // texture creation:
    // the second parameter: pointer to initial data (zero for any data, since depth-stencil buffer)
    // note texture 2d function: sorted and rasterized polygons are just coloured pixels in 2d representation
    hR = device->CreateTexture2D ( &depthBufferDesc, nullptr, &dSbuffer );
    //hR = dsBuffer->QueryInterface ( __uuidof(IDXGISurface1), &dsSurface );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
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
    hR = device->CreateDepthStencilState ( &depthStencilDesc, &dSstate );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of depth-stencil state failed!" );
      return;
    }
    // set the active depth stencil state
    devCon->OMSetDepthStencilState ( dSstate.Get (), 1 );

    // depth-stencil view description
    // purpose: so the Direct3D use the depth buffer as a depth stencil texture.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    // depth-stencil view creation
    // the second parameter: zero to access the mipmap level 0
    hR = device->CreateDepthStencilView ( dSbuffer.Get (), &depthStencilViewDesc, &dSview );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation the depth-stencil view failed!" );
      return;
    }

    // binding render target view and depth-stencil view to output render pipeline (for now one render target view)
    // purpose: rendered graphics by the pipeline will be drawn to the back buffer.
    // second parameter: pointer to first element of a list of render target view pointers
    devCon->OMSetRenderTargets ( 1, rTview.GetAddressOf (), dSview.Get () );

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
    hR = device->CreateRasterizerState ( &rasterizerDesc, &rasterizerState );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation the rasterizer state failed!" );
      return;
    }
    // set the active rasterizer state
    devCon->RSSetState ( rasterizerState.Get () );

    // viewport structure: set the viewport to entire back buffer (what area should be rendered to)
    // with other words: so Direct3D can map clip space coordinates to the render target space
    D3D11_VIEWPORT viewPort;
    viewPort.Width = depthBufferDesc.Width;
    viewPort.Height = depthBufferDesc.Height;
    viewPort.MinDepth = 0.0f; // minimum and maximum depth buffer values
    viewPort.MaxDepth = 1.0f;
    viewPort.TopLeftX = 0; // first four integers: viewport rectangle (relative to client window rectangle)
    viewPort.TopLeftY = 0;
    // setting the viewport
    // the second parameter is a pointer to an array of viewports
    devCon->RSSetViewports ( 1, &viewPort );

    clearBuffers ();

    allocated = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Direct3D::clearBuffers ( void )
{
  try
  {

    //const float blue [] { 0.11f, 0.33f, 0.55f, 1.0f };
    const float blue [] { 0.0f, 0.0f, 0.0f, 1.0f };
    // filling the entire back buffer with a single colour
    devCon->ClearRenderTargetView ( rTview.Get (), blue );
    // second parameter: the type of data to clear (obviously set to clear both depth-stencil)
    // the values are used to override the entire depth-stencil buffer with
    if (dSview)
      devCon->ClearDepthStencilView ( dSview.Get (), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void Direct3D::present ( void )
{
  try
  {

    HRESULT hR;

    // SyncInterval parameter: the way a frame is synchronized with VBLANK:
    // in flip mode: the n = 0 sets the DirectX to cancel the remaining time of previously rendered scene
    // and discard this frame if a newer frame is on the queue. (screen tearing might occur)
    // the n = 1 to 4 values: synchronize the presentation after n-th vertical blank.
    // the second parameter: not waiting for v-sync.
    if (vSync)
      hR = { swapChain->Present ( 1, DXGI_PRESENT_DO_NOT_WAIT ) }; // consider screen refresh rate
    else
      hR = { swapChain->Present ( 0, DXGI_PRESENT_DO_NOT_WAIT ) }; // as fast as possible

    if ((FAILED ( hR )) &&
      (hR != DXGI_ERROR_WAS_STILL_DRAWING)) // occurs, if the calling thread is blocked
    {
      PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), "mainThread",
                                                "The presentation of the scene failed!" );
    }

    // rebind: the process is needed after each call to present, since in flip and discard mode the view targets are released.
    if (dSview)
      devCon->OMSetRenderTargets ( 1, rTview.GetAddressOf (), dSview.Get () );

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};

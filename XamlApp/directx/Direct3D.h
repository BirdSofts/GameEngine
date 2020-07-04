// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef DIRECT3D_H
#define DIRECT3D_H


#include "../Core.h"


// Direct3D wrapper
class Direct3D
{
  friend class TheCore;
private:
  TheCore* m_core; // pointer to the application core

  // note that this application consumes Direct3d 11 APIs.
  // to easily handle the life cycle of COM objects, they can be defined using smart pointers known as COM pointers,
  // note that each COM interface must be created in its own way.
  // the core of Direct3D is two COM objects the device and the device context:

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  winrt::com_ptr<ID3D11Device3> m_device; // Direct3D device

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output,
  // therefore it can be seen as the control panel of the GPU,
  // through which the transformation of three-dimensional to the final two-dimensional,
  // and the process of rendering that image to screen is controlled.
  winrt::com_ptr<ID3D11DeviceContext3> m_deviceContext; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffers (double or triple) and drawing to the display surface
  winrt::com_ptr<IDXGISwapChain3> m_swapChain; // the swap chain

  D3D_FEATURE_LEVEL m_featureLevel; // current supported feature level
  unsigned int m_displayModesCount; // the number of supported display modes
  DXGI_MODE_DESC* m_displayModes; // list of supported display modes
  unsigned int m_displayModeIndex; // the index of current display mode
  DXGI_MODE_DESC m_displayMode; // current display mode
  unsigned int m_videoCardMemory; // dedicated video card memory (megabytes)
  std::wstring m_videoCardDescription; // string representing the physical adapter name

  winrt::com_ptr<ID3D11RenderTargetView1> m_renderTview; // render target view

  winrt::com_ptr<ID3D11Texture2D> m_depthSbuffer; // depth-stencil view buffer
  winrt::com_ptr<ID3D11DepthStencilState> m_depthSstate; // depth-stencil state
  winrt::com_ptr<ID3D11DepthStencilView> m_depthSview; // depth-stencil view

  DXGI_FORMAT m_backBufferFormat; // back buffer colour format
  UINT m_backBufferCount; // back buffers count
  DXGI_FORMAT m_depthBufferFormat; // depth buffer format

  winrt::com_ptr<ID3D11RasterizerState> m_rasterizerState; // rasterizer state

  bool m_vSync; // application configuration (if true render according installed monitor refresh rate)
  bool m_allocated; // true if resources allocation was successful
  bool m_initialized; // true if initialization was successful

  void m_createIndependentResources ( void ); // Direct3D independent resource
  void m_createDeviceDependentResources ( void ); // Direct3D device dependent resources
  void m_createWindowDependentResources ( void ); // Direct3D window dependent resources
  void m_allocation (); // Direct3D resources resize/creation
  void m_setDisplayMode ( void ); // Direct3D display mode change/adjust
  void m_onSuspending ( void ); // suspension preparation
  void m_validate ( void ); // validate the correct state of Direct3D resources
  void m_onDeviceLost ( void ); // clean and reallocate
public:
  Direct3D ( TheCore* );
  ~Direct3D ( void ) { /**/ };

  void m_clearBuffers ( void ); // clear depth-stencil buffers
  void m_present ( void ); // swapping: present the buffer chain by flipping the buffers

  const bool& Direct3D::m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  const winrt::com_ptr<ID3D11Device> m_getDevice ( void ) { return m_device; }; // get the pointer to D3D device
  const winrt::com_ptr<ID3D11DeviceContext3> m_getDevCon ( void ) { return m_deviceContext; }; // get the pointer to D3D device context
  const winrt::com_ptr<IDXGISwapChain3> m_getSwapChain ( void ) { return m_swapChain; }; // get the pointer to D3D swap chain
  const DXGI_FORMAT& m_getBackBufferFormat () { return m_backBufferFormat; }; // get current format of back/depth buffers
};


#endif // !DIRECT3D_H

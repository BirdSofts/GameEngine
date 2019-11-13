// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,13.11.2019</changed>
// ********************************************************************************

#ifndef DIRECT3D_H
#define DIRECT3D_H


#include "../Core.h"


// Direct3D wrapper
class Direct3D
{
  friend class TheCore;
private:
  TheCore* m_core; // pointer to the framework core

  // note that this application put the power of Direct3d 10 into practice.
  // to easily handle the life cycle of COM objects, they can be defined using smart pointers known as COM pointers,
  // note that each COM interface must be created in its own way.
  // the core of Direct3D is two COM objects the device and the device context:

  // -- the device object: virtual representation of the video adapter
  // purpose: access to GPU memory and creation of Direct3D COM objects.
  Microsoft::WRL::ComPtr<ID3D11Device3> m_device; // Direct3D device

  // -- the device context: the structure defining a set of graphic objects
  // and their associated attributes, additionally graphic modes that affect output,
  // therefore it can be seen as the control panel of the GPU,
  // through which the transformation of three-dimensional to the final two-dimensional,
  // and the process of rendering that image to screen is controlled.
  Microsoft::WRL::ComPtr<ID3D11DeviceContext3> m_deviceContext; // Direct3D device context

  // The COM interface representing the swap chain:
  // purpose: swapping the back buffers (double or triple) and drawing to the display surface
  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain; // the swap chain

  D3D_FEATURE_LEVEL m_featureLevel; // current supported feature level
  unsigned int m_displayModesCount; // the number of supported display modes
  DXGI_MODE_DESC* m_displayModes; // list of supported display modes
  unsigned int m_displayModeIndex; // the index of current display mode
  DXGI_MODE_DESC m_displayMode; // current display mode
  unsigned int m_videoCardMemory; // dedicated video card memory (megabytes)
  std::wstring m_videoCardDescription; // string representing the physical adapter name

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTview; // render target view

  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthSbuffer; // depth-stencil view buffer
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthSstate; // depth-stencil state
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthSview; // depth-stencil view

  DXGI_FORMAT m_backBufferFormat; // back buffer colour format
  UINT m_backBufferCount; // back buffers count
  DXGI_FORMAT m_depthBufferFormat; // depth buffer format

  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState; // rasterizer state

  bool m_vSync; // application configuration (if true render according installed monitor refresh rate)
  bool m_allocated; // true if resources allocation was successful
  bool m_initialized; // true if initialization was successful

  void m_creation ( void ); // Direct3D device creation
  void m_allocation (); // Direct3D resources resize/creation
  void m_setDisplayMode ( void ); // Direct3D display mode change/adjus
  void m_onSuspending ( void ); // suspension preparation
  void m_validate ( void ); // validate the correct state of Direct3D resources
  void m_onDeviceLost ( void ); // clean and reallocate
public:
  Direct3D ( TheCore* );
  //~Direct3D ( void );
  const bool& m_isInitialized ( void ); // get the initialized state

  void m_clearBuffers ( void ); // clear depth-stencil buffers
  void m_present ( void ); // swapping: present the buffer chain by flipping the buffers
  const Microsoft::WRL::ComPtr<ID3D11Device> m_getDevice ( void ); // get the pointer to D3D device
  const Microsoft::WRL::ComPtr<ID3D11DeviceContext3> m_getDevCon ( void ); // get the pointer to D3D device context
  const Microsoft::WRL::ComPtr<IDXGISwapChain3> m_getSwapChain ( void ); // get the pointer to D3D swap chain
  const DXGI_FORMAT& m_getBackBufferFormat (); // get current format of back/depth buffers
};


#endif // !DIRECT3D_H

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#ifndef UNIVERSE_H
#define UNIVERSE_H


#include "Camera.h"
#include "Light.h"


// matrix buffer (matching the global cbuffer type introduced in vertex shader)
struct MatrixBuffer
{
  DirectX::XMMATRIX world;
  DirectX::XMMATRIX view;
  DirectX::XMMATRIX projection;
};


// light buffer (matching the global cbuffer type introduced in pixel shader)
struct LightBuffer
{
  DirectX::XMFLOAT4 diffuseColour;
  DirectX::XMFLOAT3 diffuseDirection;
  float padding; // extra, so the structure size match a multiple of 16 (function 'CreateBuffer' requirements)
};


// objects of game space (world, camera, views, lights) and their effects on polygons
class Universe
{
private:
  ID3D11Device* m_device; // pointer to DirecX device
  ID3D11DeviceContext* m_deviceContext; // pointer to DirectX device context

  Camera* m_camera; // pointer to the camera application
  DirectX::XMMATRIX m_projectionMatrix; // projection matrix (translation of 3D scene into the 2D viewport space)
  DirectX::XMMATRIX m_worldMatrix; // world matrix (to convert into 3D scenes' vertices)
  float m_worldRotationMatrix; // world matrix rotation factor
  DirectX::XMMATRIX m_orthographicMatrix; // orthographic matrix (2D rendering)
  const float m_screenDepth { 1000.0f }; // depth settings
  const float m_screenNear { 0.1f }; // depth settings
  ID3D11Buffer* m_matrixBuffer; // constant matrix buffer (to interface with shader)

  DiffuseLight* m_diffuseLight; // pointer to the diffuse light application
  ID3D11Buffer* m_diffuseLightBuffer; // constant light buffer (to interface with shader)

  bool m_initialized; // true if initialization was successful
public:
  Universe ( ID3D11Device*, ID3D11DeviceContext* );
  //~Universe ( void );
  const bool& m_isInitialized ( void ); // get the initialized state

  void m_renderResources ( void ); // map matrix buffer and update
  Camera* m_getCamera ( void ); // get the pointer to camera application
  DiffuseLight* m_getDiffuseLight ( void ); // get the pointer to diffuse light application
  void m_update ( void ); // update the game universe
  void m_release ( void ); // release the resource
};


#endif // !UNIVERSE_H

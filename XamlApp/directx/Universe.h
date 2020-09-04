// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef UNIVERSE_H
#define UNIVERSE_H


#include "Core.h"
#include "Camera.h"
#include "../resources/Light.h"


// matrix buffer (matching the global cbuffer type introduced in vertex shader)
struct worldViewProjectionMatrices
{
  DirectX::XMFLOAT4X4 world;
  DirectX::XMFLOAT4X4 view;
  DirectX::XMFLOAT4X4 projection;
};


// light buffer (matching the global cbuffer type introduced in pixel shader)
struct LightBuffer
{
  DirectX::XMFLOAT4 ambientColour;
  DirectX::XMFLOAT4 diffuseColour;
  DirectX::XMFLOAT3 diffuseDirection;
  float padding; // extra, so the structure size match a multiple of 4 (function 'CreateBuffer' requirements)
};


// objects of game space (world, camera, views, lights) and their effects on polygons
class Universe
{
private:
  TheCore* m_core; // pointer to the application core

  Camera* m_camera; // pointer to the camera application

  //DirectX::XMMATRIX m_projectionMatrix; // projection matrix (translation of 3D scene into the 2D viewport space)
  //DirectX::XMMATRIX m_worldMatrix; // world matrix (to convert into 3D scenes' vertices)
  //DirectX::XMMATRIX m_orthographicMatrix; // orthographic matrix (2D rendering)
  float m_worldRotationFactor;

  worldViewProjectionMatrices m_worldViewProjectionDate;
  ID3D11Buffer* m_matrixBuffer; // constant world view projection matrices buffer (to interface with shader)

  DiffuseLight* m_diffuseLight; // pointer to the diffuse light application
  ID3D11Buffer* m_diffuseLightBuffer; // constant diffuse light buffer (to interface with shader)

  bool m_initialized; // true in case of successful initialization
public:
  Universe ( TheCore* coreObj );
  //~Universe ( void );

  void m_createResources ( void ); // create dependent resources
  void m_renderResources ( void ); // map matrix buffer and update
  void m_update ( void ); // update the game universe
  void m_release ( void ); // release the resource

  const bool& m_isInitialized ( void ) { return m_initialized; }; // get the initialized state
  Camera* m_getCamera ( void ) { return m_camera; }; // get pointer to camera
  DiffuseLight* m_getDiffuseLight ( void ) { return m_diffuseLight; }; // get pointer to diffuse light
};


#endif // !UNIVERSE_H

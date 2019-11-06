// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,05.11.2019</changed>
// ********************************************************************************

#ifndef SPACE_H
#define SPACE_H


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
  ID3D11Device* device; // pointer to DirecX device
  ID3D11DeviceContext* devCon; // pointer to DirectX device context

  Camera* camera; // pointer to the camera application
  DirectX::XMMATRIX matrixProjection; // projection matrix (translation of 3D scene into the 2D viewport space)
  DirectX::XMMATRIX matrixWorld; // world matrix (to convert into 3D scenes' vertices)
  float matrixWorldRotation; // world matrix rotation factor
  DirectX::XMMATRIX matrixOrthographic; // orthographic matrix (2D rendering)
  const float screenDepth { 1000.0f }; // depth settings
  const float screenNear { 0.1f }; // depth settings
  ID3D11Buffer* matrixBuffer; // constant matrix buffer (to interface with shader)

  DiffuseLight* lightDiffuse; // pointer to the diffuse light application
  ID3D11Buffer* lightBufferDiffuse; // constant light buffer (to interface with shader)

  bool initialized; // true if initialization was successful
public:
  Universe ( ID3D11Device*, ID3D11DeviceContext* );
  const bool& isInitialized ( void ); // get the initialized state
  void renderResources ( void ); // map matrix buffer and update
  Camera* getCamera ( void ); // get the pointer to camera application
  DiffuseLight* getDiffuseLight ( void ); // get the pointer to diffuse light application
  void update ( void ); // update the game universe
  void release ( void ); // release the resource
};


#endif // !SPACE_H

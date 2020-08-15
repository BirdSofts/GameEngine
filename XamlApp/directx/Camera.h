// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef CAMERA_H
#define CAMERA_H


#include "Core.h"


// camera wrapper
class Camera
{
private:
  TheCore* m_core;

  DirectX::XMFLOAT3 m_position; // camera position data
  DirectX::XMFLOAT3 m_rotation; // camera rotation data

  //float m_movementsSpeed;

  // used by DirectX knowing from where and how the scene is looked at,
  // additionally the generated view matrix is passed into HLSL shader as rendering parameter
  DirectX::XMMATRIX m_matrixView; // camera view matrix (location of the camera in the world)

  bool m_initialized; // true if initialization was successful
public:
  Camera ( TheCore* coreObj );
  //~Camera ( void );

  void m_renderCamera ( void ); // view matrix generation/update based on the camera position/ rotation

  DirectX::XMFLOAT3& m_getPosition ( void ); // get/set camera position
  void m_setPosition ( const float& x, const float& y, const float& z ); // set camera position

  DirectX::XMFLOAT3& m_getRotation ( void ); // get/set camera rotation
  void m_setRotation ( const float& x, const float& y, const float& z ); // set camera rotation

  void m_goLeftRight ( const float& x ); // set position towards left/right
  void m_goUpDown ( const float& y ); // set position towards up/down
  void m_goForwardBackward ( const float& z ); // set position towards forward/backward

  void m_lookUpDown ( const float& x ); // set rotation to look up/down
  void m_lookLeftRight ( const float& y ); // set rotation to look left/right
  void m_lookSidedLeftRight ( const float& z ); // set rotation to look sided left/right

  void m_release ( void ); // release the resource

  const bool& isInitialized ( void ) { return m_initialized; }; // get the initialized state
  const DirectX::XMMATRIX& getView ( void ) { return m_matrixView; }; // get view matrix
};


#endif // !CAMERA_H

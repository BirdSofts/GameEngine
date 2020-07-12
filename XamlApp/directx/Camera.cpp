// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Camera.h"
#include "Shared.h"


Camera::Camera ( void )
{
  try
  {

    // initialize the camera to the origin of the scene.
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;

    initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Camera::~Camera ( void )
//{
//
//};


void Camera::renderCamera ( void )
{
  try
  {

    DirectX::XMFLOAT3 up, pos, lookAt;
    DirectX::XMVECTOR upVector, positionVector, lookAtVector;

    // vector setup: the one that points upwards
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    upVector = DirectX::XMLoadFloat3 ( &up ); // load into structure

    // camera position setup (in the world)
    pos.x = position.x;
    pos.y = position.y;
    pos.z = position.z;
    positionVector = DirectX::XMLoadFloat3 ( &pos );

    // camera looking setup (default)
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;
    lookAtVector = DirectX::XMLoadFloat3 ( &lookAt );

    float
      pitch, // X axis
      yaw, // Y axis
      roll; // Z axis

    // setup axes in radians
    pitch = rotation.x * 0.01745329225f;
    yaw = rotation.y * 0.01745329225f;
    roll = rotation.z * 0.01745329225f;

    // rotation matrix creation
    DirectX::XMMATRIX rotationMatrix;
    rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw ( pitch, yaw, roll );

    // view correction at the origin: transformation of camera look and up vector by the rotation matrix
    lookAtVector = DirectX::XMVector3TransformCoord ( lookAtVector, rotationMatrix );
    upVector = DirectX::XMVector3TransformCoord ( upVector, rotationMatrix );

    // rotated camera position translation: to the location of viewer
    lookAtVector = DirectX::XMVectorAdd ( positionVector, lookAtVector );

    // finally: view matrix creation (from above updated vectors)
    matrixView = DirectX::XMMatrixLookAtLH ( positionVector, lookAtVector, upVector );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void Camera::setPosition ( DirectX::XMFLOAT3& pos )
{
  position.x = pos.x;
  position.y = pos.y;
  position.z = pos.z;
};


void Camera::setPosition ( float x, float y, float z )
{
  position.x = x;
  position.y = y;
  position.z = z;
};


void Camera::forwardBackward ( float z )
{
  position.z += z;
};

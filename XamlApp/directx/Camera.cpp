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


Camera::Camera ( TheCore* coreObj ) :
  m_core ( coreObj ), m_initialized ( false )
{
  try
  {

    // initialize the camera to the origin of the scene.

    m_position.x = 0.0f; // go left/right
    m_position.y = 0.0f; // go up/down
    m_position.z = 0.0f; // go forward/backward

    m_rotation.x = 0.0f; // look up/down
    m_rotation.y = 0.0f; // look left/right
    m_rotation.z = 0.0f; // look sided left/right

    m_matrixView = DirectX::XMMatrixIdentity ();

    m_initialized = true;

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


void Camera::m_renderCamera ( void )
{
  try
  {

    DirectX::XMFLOAT3 up, pos, lookAt;
    DirectX::XMVECTOR upVector, positionVector, lookAtVector;

    // up vector setup (along the y-axis): the one that points upwards
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    upVector = DirectX::XMLoadFloat3 ( &up ); // load into structure

    // camera position (eye) setup (in the world)
    pos.x = m_position.x;
    pos.y = m_position.y;
    pos.z = m_position.z;
    positionVector = DirectX::XMLoadFloat3 ( &pos );

    // camera looking at setup (default)
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;
    lookAtVector = DirectX::XMLoadFloat3 ( &lookAt );

    float
      pitch, // X axis
      yaw, // Y axis
      roll; // Z axis

    // setup axes in radians
    pitch = m_rotation.x * (DirectX::XM_PI / 180.0f);
    yaw = m_rotation.y * (DirectX::XM_PI / 180.0f);
    roll = m_rotation.z * (DirectX::XM_PI / 180.0f);

    // rotation matrix creation
    DirectX::XMMATRIX rotationMatrix;
    rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw ( pitch, yaw, roll );

    // view correction at the origin: transformation of camera look and up vector by the rotation matrix
    lookAtVector = DirectX::XMVector3TransformCoord ( lookAtVector, rotationMatrix );
    upVector = DirectX::XMVector3TransformCoord ( upVector, rotationMatrix );
    positionVector = DirectX::XMVector3TransformCoord ( positionVector, rotationMatrix );

    // rotated camera position translation: to the location of viewer
    lookAtVector = DirectX::XMVectorAdd ( positionVector, lookAtVector );

    // finally: view matrix creation (from above updated vectors)
    m_matrixView = DirectX::XMMatrixLookAtLH ( positionVector, lookAtVector, upVector );


    // provide needed debug infos
    if (m_core->m_isDebugging () == true)
    {
      std::string temp { "" };
      temp += "CameraPos X:" + std::to_string ( m_position.x );
      temp += " ,Y:" + std::to_string ( m_position.y );
      temp += " ,Z:" + std::to_string ( m_position.z );
      m_core->m_getMainPageTypes ()->m_getCameraPosition () = (Converter::strConverter ( temp ));

      temp = "";
      temp += "CameraRot X:" + std::to_string ( m_rotation.x );
      temp += " ,Y:" + std::to_string ( m_rotation.y );
      temp += " ,Z:" + std::to_string ( m_rotation.z );
      m_core->m_getMainPageTypes ()->m_getCameraRotation () = (Converter::strConverter ( temp ));
    }

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


DirectX::XMFLOAT3& Camera::m_getPosition ( void )
{
  return m_position;
};


void Camera::m_setPosition ( const float& x, const float& y, const float& z )
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;
};


DirectX::XMFLOAT3& Camera::m_getRotation ( void )
{
  return m_rotation;
};


void Camera::m_setRotation ( const float& x, const float& y, const float& z )
{
  m_rotation.x = x;
  m_rotation.y = y;
  m_rotation.z = z;
};


void Camera::m_goLeftRight ( const float& x )
{
  m_position.x += x;
};
void Camera::m_goUpDown ( const float& y )
{
  m_position.y += y;
};
void Camera::m_goForwardBackward ( const float& z )
{
  m_position.z += z;
};


void Camera::m_lookUpDown ( const float& x )
{
  m_rotation.x += x;
  if ((m_rotation.x == 360.0f) || (m_rotation.x == -360.0f))
    m_rotation.x = 0.0f;
};
void Camera::m_lookLeftRight ( const float& y )
{
  m_rotation.y += y;
  if ((m_rotation.y == 360.0f) || (m_rotation.y == -360.0f))
    m_rotation.y = 0.0f;
};
void Camera::m_lookSidedLeftRight ( const float& z )
{
  m_rotation.z += z;
  if ((m_rotation.z == 360.0f) || (m_rotation.z == -360.0f))
    m_rotation.z = 0.0f;
};


void Camera::m_release ( void )
{
  m_initialized = false;
  m_core = nullptr;
};

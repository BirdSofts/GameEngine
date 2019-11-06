// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,03.11.2019</changed>
// ********************************************************************************

#ifndef CAMERA_H
#define CAMERA_H


// camera wrapper
class Camera
{
private:
  DirectX::XMFLOAT3 position; // camera position data
  DirectX::XMFLOAT3 rotation; // camera rotation data

  // used by DirectX knowing from where and how the scene is looked at,
  // additionally the generated view matrix is passed into HLSL shader as rendering parameter
  DirectX::XMMATRIX matrixView; // camera view matrix (location of the camera in the world)

  bool initialized; // true if initialization was successful
public:
  Camera ( void );
  const bool& isInitialized ( void ); // get the initialized state
  void setPosition ( DirectX::XMFLOAT3& ); // set camera position
  void setPosition ( float, float, float ); // set camera position
  void forwardBackward ( float ); // set position forward/backward
  void renderCamera ( void ); // view matrix generation/update based on the camera position
  const DirectX::XMMATRIX& getView ( void ); // get view matrix
};


#endif // !CAMERA_H

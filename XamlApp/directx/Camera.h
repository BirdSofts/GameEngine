// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
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
  //~Camera ( void );

  void renderCamera ( void ); // view matrix generation/update based on the camera position
  void setPosition ( DirectX::XMFLOAT3& pos ); // set camera position
  void setPosition ( float x, float y, float z ); // set camera position
  void forwardBackward ( float z ); // set position forward/backward

  const bool& isInitialized ( void ) { return initialized; }; // get the initialized state
  const DirectX::XMMATRIX& getView ( void ) { return matrixView; }; // get view matrix
};


#endif // !CAMERA_H

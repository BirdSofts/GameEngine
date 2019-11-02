// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef LIGHT_H
#define LIGHT_H


#include <DirectXMath.h>


// lightening wrapper
//-- diffuse lighting of type directional lighting:
//- similar to the sun light illuminated from a great distance away
//- using its direction the amount of light is determinable
//- unlike ambient lighting, no effect of surfaces it doesn't directly touch
//- requirements: direction and a normal polygon vector implemented in both pixel and vertex shader
class DiffuseLight
{
private:
  DirectX::XMFLOAT4 diffuseColour;
  DirectX::XMFLOAT3 diffuseDirection;
  bool initialized; // true if initialization was successful
public:
  DiffuseLight ( void );
  const bool& isInitialized ( void ); // get the initialized state
  void setColour ( float, float, float, float ); // set diffuse light colour
  void setDirection ( float, float, float ); // set diffuse light direction
  DirectX::XMFLOAT4 getColour ( void ); // get diffuse light colour
  DirectX::XMFLOAT3 getDirection ( void ); // get diffuse light direction
};


#endif // !LIGHT_H

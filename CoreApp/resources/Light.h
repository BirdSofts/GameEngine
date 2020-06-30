// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#ifndef LIGHT_H
#define LIGHT_H


// lightening wrapper
//-- diffuse lighting of type directional lighting:
//- similar to the sun light illuminated from a great distance away
//- using its direction the amount of light is determinable
//- unlike ambient lighting, no effect of surfaces it doesn't directly touch
//- requirements: direction and a normal polygon vector implemented in both pixel and vertex shader
class DiffuseLight
{
private:
  DirectX::XMFLOAT4 m_diffuseColour;
  DirectX::XMFLOAT3 m_diffuseDirection;
  bool m_initialized; // true if initialization was successful
public:
  DiffuseLight ( void );
  //~DiffuseLight ( void );
  const bool& m_isInitialized ( void ); // get the initialized state

  void m_setColour ( const float* ); // set diffuse light colour
  void m_setDirection ( const float* ); // set diffuse light direction
  DirectX::XMFLOAT4 m_getColour ( void ); // get diffuse light colour
  DirectX::XMFLOAT3 m_getDirection ( void ); // get diffuse light direction
};


#endif // !LIGHT_H

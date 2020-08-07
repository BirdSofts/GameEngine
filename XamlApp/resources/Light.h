// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
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

  //void m_update ( void ); // update diffuse light resource
  void m_setColour ( const float* colour ); // set diffuse light colour
  void m_setDirection ( const float* direction ); // set diffuse light direction

  const bool& m_isInitialized ( void ) { return m_initialized; };; // get the initialized state
  DirectX::XMFLOAT4 m_getColour ( void ) { return m_diffuseColour; }; // get diffuse light colour
  DirectX::XMFLOAT3 m_getDirection ( void ) { return m_diffuseDirection; }; // get diffuse light direction
};


#endif // !LIGHT_H

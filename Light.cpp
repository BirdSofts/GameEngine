// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,03.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Light.h"


DiffuseLight::DiffuseLight () : initialized ( false )
{
  diffuseColour = DirectX::XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
  diffuseDirection = DirectX::XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
  initialized = true;
};


const bool& DiffuseLight::isInitialized ( void )
{
  return initialized;
};


void DiffuseLight::setColour ( float red, float green, float blue, float alpha )
{
  diffuseColour = DirectX::XMFLOAT4 { red, green, blue, alpha };
};


void DiffuseLight::setDirection ( float x, float y, float z )
{
  diffuseDirection = DirectX::XMFLOAT3 { x, y, z };
};


DirectX::XMFLOAT4 DiffuseLight::getColour ( void )
{
  return diffuseColour;
};


DirectX::XMFLOAT3 DiffuseLight::getDirection ( void )
{
  return diffuseDirection;
};


//const bool& DiffuseLight::isInitialized ( void )
//{
//  return initialized;
//};

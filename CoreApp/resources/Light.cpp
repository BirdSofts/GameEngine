// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,12.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Light.h"


DiffuseLight::DiffuseLight () : m_initialized ( false )
{
  m_diffuseColour = DirectX::XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
  m_diffuseDirection = DirectX::XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
  m_initialized = true;
};


//DiffuseLight::~DiffuseLight ( void )
//{
//
//};


const bool& DiffuseLight::m_isInitialized ( void )
{
  return m_initialized;
};


void DiffuseLight::m_setColour ( const float* colour )
{
  m_diffuseColour = DirectX::XMFLOAT4 { colour [0], colour [1], colour [2], colour [3] };
};


void DiffuseLight::m_setDirection ( const float* direction )
{
  m_diffuseDirection = DirectX::XMFLOAT3 { direction [0], direction [1], direction [2] };
};


DirectX::XMFLOAT4 DiffuseLight::m_getColour ( void )
{
  return m_diffuseColour;
};


DirectX::XMFLOAT3 DiffuseLight::m_getDirection ( void )
{
  return m_diffuseDirection;
};

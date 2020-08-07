// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "Light.h"


DiffuseLight::DiffuseLight () :
  /*m_diffuseLightBuffer ( nullptr ),*/ m_initialized ( false )
{

  m_diffuseColour = DirectX::XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
  m_diffuseDirection = DirectX::XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
  m_initialized = true;

  //if (m_diffuseLightBuffer)
  //{
  //  m_diffuseLightBuffer->Release ();
  //  m_diffuseLightBuffer = nullptr;
  //}

};


//DiffuseLight::~DiffuseLight ( void )
//{
//
//};


void DiffuseLight::m_setColour ( const float* colour )
{
  m_diffuseColour = DirectX::XMFLOAT4 { colour [0], colour [1], colour [2], colour [3] };
};


void DiffuseLight::m_setDirection ( const float* direction )
{
  m_diffuseDirection = DirectX::XMFLOAT3 { direction [0], direction [1], direction [2] };
};

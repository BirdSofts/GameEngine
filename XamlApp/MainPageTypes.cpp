// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,10.07.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "MainPageTypes.h"


MainPageTypes::MainPageTypes ()
{

  m_display.Dpi = 96.f;
  m_display.windowWidthDips = 0.0f;
  m_display.windowHeightDips = 0.0f;
  m_display.windowWidthPixels = 0;
  m_display.windowHeightPixels = 0;
  m_display.panelWidthDips = 0.0f;
  m_display.panelHeightDips = 0.0f;
  m_display.panelWidthPixels = 0;
  m_display.panelHeightPixels = 0;
  m_display.fullscreen = false;

  m_pointer = L"";

};


//MainPageTypes::~MainPageTypes ()
//{
//
//};


void MainPageTypes::Display::updatePixels ()
{
  // to pixels ( dips * c_DPI / 96.f + 0.5f )
  windowWidthPixels = static_cast<int>(windowWidthDips * Dpi / 96.f + 0.5f);
  windowHeightPixels = static_cast<int>(windowHeightDips * Dpi / 96.f + 0.5f);
  panelWidthPixels = static_cast<int>(panelWidthDips * Dpi / 96.f + 0.5f);
  panelHeightPixels = static_cast<int>(panelHeightDips * Dpi / 96.f + 0.5f);
};


void MainPageTypes::Display::updateDips ()
{
  // to dips ( float (pixels) * 96.f / c_DPI )
  windowWidthDips = (static_cast<float> (windowWidthPixels) * 96.f / Dpi);
  windowHeightDips = (static_cast<float> (windowHeightPixels) * 96.f / Dpi);
  panelWidthDips = (static_cast<float> (panelWidthPixels) * 96.f / Dpi);
  panelHeightDips = (static_cast<float> (panelHeightPixels) * 96.f / Dpi);
};

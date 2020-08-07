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

  m_pointer = L"";

};


//MainPageTypes::~MainPageTypes ()
//{
//
//};


MainPageTypes::Display::Display ()
{

  windowWidthDips = .0f;
  windowHeightDips = .0f;
  windowWidthPixels = 0;
  windowHeightPixels = 0;

  panelWidthDips = .0f;
  panelHeightDips = .0f;
  panelWidthPixels = 0;
  panelHeightPixels = 0;

  outputWidthDips = .0f;
  outputHeightDips = .0f;

  fullscreen = false;

  Dpi = 96.f;
  compositionScaleX = .0f;
  compositionScaleY = .0f;

  effectiveDpi = -1.0f;
  effectiveCompositionScaleX = .0f;
  effectiveCompositionScaleY = .0f;

  orientationNative = winrt::Windows::Graphics::Display::DisplayOrientations::None; // native orientation
  orientationCurrent = winrt::Windows::Graphics::Display::DisplayOrientations::None; // current orientation

  orientationTransform2D = D2D1::Matrix3x2F::Identity (); // orientation 2D transform
  orientationTransform3D = DirectX::XMFLOAT4X4 (); // orientation 3D transform

  displayRotation = DXGI_MODE_ROTATION_UNSPECIFIED;
  swapedDimensions = false;

};


void MainPageTypes::Display::updatePixels ()
{
  // to pixels ( dips * c_DPI / 96.f + 0.5f )
  windowWidthPixels = static_cast<int>(windowWidthDips * Dpi / 96.f + 0.5f);
  windowHeightPixels = static_cast<int>(windowHeightDips * Dpi / 96.f + 0.5f);
  panelWidthPixels = static_cast<int>(panelWidthDips * Dpi / 96.f + 0.5f);
  panelHeightPixels = static_cast<int>(panelHeightDips * Dpi / 96.f + 0.5f);

  computeEffectiveDimensions ();
};


void MainPageTypes::Display::updateDips ()
{
  // to dips ( float (pixels) * 96.f / c_DPI )
  windowWidthDips = (static_cast<float> (windowWidthPixels) * 96.f / Dpi);
  windowHeightDips = (static_cast<float> (windowHeightPixels) * 96.f / Dpi);
  panelWidthDips = (static_cast<float> (panelWidthPixels) * 96.f / Dpi);
  panelHeightDips = (static_cast<float> (panelHeightPixels) * 96.f / Dpi);

  computeEffectiveDimensions ();
};


void MainPageTypes::Display::computeEffectiveDimensions ( void )
{

  if (!supportHighResolution && Dpi > DpiThreshold)
  {
    if (max ( panelWidthDips, panelHeightDips ) > widthThreshold &&
         min ( panelWidthDips, panelHeightDips ) > heightThreshold)
    {
      // to scale the app, effective Dpi needs to be changed
      effectiveDpi = Dpi / 2.0f;
      effectiveCompositionScaleX = compositionScaleX / 2.0f;
      effectiveCompositionScaleY = compositionScaleY / 2.0f;

      outputWidthDips = (static_cast<float> (panelWidthPixels) * 96.f / effectiveDpi);
      outputHeightDips = (static_cast<float> (panelHeightPixels) * 96.f / effectiveDpi);
    }
  } else
  {
    effectiveDpi = Dpi;
    effectiveCompositionScaleX = compositionScaleX;
    effectiveCompositionScaleY = compositionScaleY;
    outputWidthDips = panelWidthDips;
    outputHeightDips = panelHeightDips;
  }

};


void MainPageTypes::Display::computeRotation ( void )
{

  // Note: native orientation can be either Landscape or Portrait
  // while DisplayOrientation additionally provides other values
  switch (orientationNative)
  {
    case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
      switch (orientationCurrent)
      {
        case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
          displayRotation = DXGI_MODE_ROTATION_IDENTITY;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
          displayRotation = DXGI_MODE_ROTATION_ROTATE270;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
          displayRotation = DXGI_MODE_ROTATION_ROTATE180;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
          displayRotation = DXGI_MODE_ROTATION_ROTATE90;
          break;
      }
      break;
    case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
      switch (orientationCurrent)
      {
        case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
          displayRotation = DXGI_MODE_ROTATION_ROTATE90;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
          displayRotation = DXGI_MODE_ROTATION_IDENTITY;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
          displayRotation = DXGI_MODE_ROTATION_ROTATE270;
          break;
        case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
          displayRotation = DXGI_MODE_ROTATION_ROTATE180;
          break;
      }
      break;
  }



  // on not display's native orientations, dimensions (width, height) need to be swapped
  switch (displayRotation)
  {
    case DXGI_MODE_ROTATION_ROTATE90:
    case DXGI_MODE_ROTATION_ROTATE270:
      swapedDimensions = true;
      break;
  }

  if (swapedDimensions)
  {

    float tempDips { .0f };
    uint32_t tempPixels { 0 };
    tempDips = windowWidthDips; windowWidthDips = windowHeightDips; windowHeightDips = tempDips;
    tempPixels = windowWidthPixels; windowWidthPixels = windowHeightPixels; windowHeightPixels = tempPixels;

    tempDips = panelWidthDips; panelWidthDips = panelHeightDips; panelHeightDips = tempDips;
    tempPixels = panelWidthPixels; panelWidthPixels = panelHeightPixels; panelHeightPixels = tempPixels;

  }



  // since rendering to a rotated swap chain, generate the needed 2D, 3D transformation matrices
  // note that due to differences in coordinate space,
  // rotation angel of 2D transform differs to that of 3D
  // additionally, based on Microsoft template, 3D matrix is explicitly specified to avoid rounding errors.
  switch (displayRotation)
  {
    case DXGI_MODE_ROTATION_IDENTITY:
      orientationTransform2D = D2D1::Matrix3x2F::Identity ();
      orientationTransform3D = ScreenRotation::Rotation0;
      break;
    case DXGI_MODE_ROTATION_ROTATE90:
      orientationTransform2D = D2D1::Matrix3x2F::Rotation ( 90.0f ) *
        D2D1::Matrix3x2F::Translation ( panelHeightPixels, 0.0f );
      orientationTransform3D = ScreenRotation::Rotation270;
      break;
    case DXGI_MODE_ROTATION_ROTATE180:
      orientationTransform2D = D2D1::Matrix3x2F::Rotation ( 180.0f ) *
        D2D1::Matrix3x2F::Translation ( panelWidthPixels, panelHeightPixels );
      orientationTransform3D = ScreenRotation::Rotation180;
      break;
    case DXGI_MODE_ROTATION_ROTATE270:
      orientationTransform2D = D2D1::Matrix3x2F::Rotation ( 270.0f ) *
        D2D1::Matrix3x2F::Translation ( 0.0f, panelWidthPixels );
      orientationTransform3D = ScreenRotation::Rotation90;
      break;
  }

};

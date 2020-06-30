// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,30.06.2020</changed>
// ********************************************************************************

#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"


using namespace winrt;
using namespace Windows::UI::Xaml;


namespace winrt::GameEngine::implementation
{

  MainPage::MainPage () :
    /*m_appWindow ( nullptr ),*/ m_visible ( false ),
    m_inResizeMove ( false ), m_fullscreen ( false ),
    m_game ( nullptr ), m_initialized ( false )
  {
    InitializeComponent ();
  }

  //MainPage::~MainPage ()
  //{
  //  // tear down
  //}

  MainPage::Display::Display ()
  {
    m_DPI = 96.f;
    m_clientWidth = 0.0f;
    m_clientHeight = 0.0f;
    m_currentWidth = 0;
    m_currentHeight = 0;
  };
  void MainPage::Display::update ( bool prm = false )
  {
    if (!prm)
    {
      // to pixels ( dips * c_DPI / 96.f + 0.5f )
      m_currentWidth = static_cast<int>(m_clientWidth * m_DPI / 96.f + 0.5f);
      m_currentHeight = static_cast<int>(m_clientHeight * m_DPI / 96.f + 0.5f);
    } else
    {
      // to dips ( float (pixels) * 96.f / c_DPI )
      m_clientHeight = (static_cast<float> (m_currentHeight) * 96.f / m_DPI);
      m_clientWidth = (static_cast<float> (m_currentWidth) * 96.f / m_DPI);
    }
  };

  int32_t MainPage::MyProperty ()
  {
    throw hresult_not_implemented ();
  }

  void MainPage::MyProperty ( int32_t /* value */ )
  {
    throw hresult_not_implemented ();
  }

  //void MainPage::ClickHandler ( IInspectable const&, RoutedEventArgs const& )
  //{

  //}

  void MainPage::Button_Click ( winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e )
  {
    textBlock ().Text ( L"test" );
    winrt::Windows::UI::Text::FontWeight a;
    a.Weight = 5000;
    textBlock ().FontWeight ( a );
  }
}


namespace winrt::GameEngine::factory_implementation
{

}

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef MAINPAGE_H
#define MAINPAGE_H


#include "MainPage.g.h"
#include "Core.h"
#include "Game.h"
#include "Shared.h"


namespace winrt::GameEngine::implementation
{
  struct MainPage : MainPageT<MainPage> // main window wrapper
  {
  private:
    winrt::agile_ref<winrt::Windows::UI::Core::CoreWindow> m_appWindow;
    bool m_visible; // application window visibility state
    bool m_inResizeMove; // true if client window is set to resize and/or move

    struct Display // resolution wrapper
    {
      float Dpi; // current DPI
      float windowWidthDips; // current window width (Dips)
      float windowHeightDips; // current window height (Dips)
      int windowWidthPixels; // current window width (Pixels) (feed from configuration)
      int windowHeightPixels; // current window height (Pixels) (feed from configuration)
      float panelWidthDips; // current swap chain panel width (Dips)
      float panelHeightDips; // current swap chain panel height (Dips)
      int panelWidthPixels; // current swap chain panel width (Pixels)
      int panelHeightPixels; // current swap chain panel height (Pixels)
      bool fullscreen; // current full screen state (feed from configuration)
      Display ();
      void updatePixels ();
      void updateDips ();
    } m_display;
    // Orientation to be add

    TheCore* m_core; // pointer to the application core
    //std::unique_ptr <TheCore>

    std::unique_ptr <Game> m_game; // unique pointer to game wrapper

    // pointers to input (background worker thread)
    Windows::Foundation::IAsyncAction* m_inputLoop; // worker thread
    Windows::UI::Core::CoreIndependentInputSource* m_inputCore; // independent input

    bool m_initialized; // true in case of successful initialization

    // XAML low-level rendering event
    void m_onRendering ( Windows::Foundation::IInspectable const& sender,
                         Windows::Foundation::IInspectable const& args ); // on application window activation

    // window events
    //void m_onActivated ( Windows::UI::Core::CoreWindow const& sender,
    //                     Windows::UI::Core::WindowActivatedEventArgs const& args ); // on application window activation

    //void m_onFocused ( Windows::UI::Core::CoreWindow const& sender,
    //                   Windows::UI::Core::WindowActivatedEventArgs const& args ); // on application window focused/unfocused

    void m_onWindowResized ( Windows::Foundation::IInspectable const& sender,
                             Windows::UI::Core::WindowSizeChangedEventArgs const& args ); // on application window resize

    void m_onVisibilityChanged ( Windows::Foundation::IInspectable const& sender,
                                 Windows::UI::Core::VisibilityChangedEventArgs const& args ); // on application window visibility

    void m_onDpiChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                          Windows::Foundation::IInspectable const& args ); // on client display DPI changed

    void m_onOrientationChanged ( Windows::Graphics::Display::DisplayInformation const& sender,
                                  Windows::Foundation::IInspectable const& args ); // on client display orientation changed

    void m_onDisplayContentInvalidated ( Windows::Graphics::Display::DisplayInformation const& sender,
                                         Windows::Foundation::IInspectable const& args ); // on application window content invalidation

    // other events
    void m_onCompositionScaleChanged ( Windows::UI::Xaml::Controls::SwapChainPanel const& sender,
                                       Windows::Foundation::IInspectable const& args ); // on composition scale resize

    void m_onSwapChainPanelSizeChanged ( Windows::Foundation::IInspectable const& sender,
                                         Windows::UI::Xaml::SizeChangedEventArgs const& e ); // on swap chain panel resize

    // input events
    void m_onAcceleratorKeyActivated ( winrt::Windows::UI::Core::CoreDispatcher const& sender,
                                       winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& e ); // on accelerator key pressed

    void m_onPointerPressed ( Windows::Foundation::IInspectable const& sender,
                              winrt::Windows::UI::Core::PointerEventArgs const& e ); // on pointer pressed

    void m_onPointerMoved ( Windows::Foundation::IInspectable const& sender,
                            winrt::Windows::UI::Core::PointerEventArgs const& e ); // on pointer moved

    void m_onPointerReleased ( Windows::Foundation::IInspectable const& sender,
                               winrt::Windows::UI::Core::PointerEventArgs const& e ); // on pointer released

  public:
    MainPage ();
    //~MainPage ();

    int32_t MyProperty ();
    void MyProperty ( int32_t value );

    // save/load internal state on app resume (exposed as public)
    //void internal state

    // control events
    //void ClickHandler ( Windows::Foundation::IInspectable const& sender,
    //                    Windows::UI::Xaml::RoutedEventArgs const& e );

    void AppBarButton_Click ( Windows::Foundation::IInspectable const& sender,
                              Windows::UI::Xaml::RoutedEventArgs const& e );

    void Button_Click ( winrt::Windows::Foundation::IInspectable const& sender,
                        winrt::Windows::UI::Xaml::RoutedEventArgs const& e );
  };
}

namespace winrt::GameEngine::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  private:
  public:
  };
}


#endif // !MAINPAGE_H

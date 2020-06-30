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
#include "common/Utilities.h"
#include "Shared.h"
#include "Game.h"


namespace winrt::GameEngine::implementation
{
  struct MainPage : MainPageT<MainPage> // main window wrapper
  {
  private:
    //winrt::agile_ref<CoreWindow> m_appWindow; //xx (swapChain) reference to application window
    bool m_visible; // application window visibility state
    bool m_inResizeMove; // true if client window is set to resize and/or move
    bool m_fullscreen; // current full screen state (additionally fed from configuration)

    struct Display // resolution wrapper
    {
      float m_DPI; // current client window DPI
      float m_clientWidth; // current client window width
      float m_clientHeight; // current client window height
      int m_currentWidth; // current client window width
      int m_currentHeight; // current client window height
      Display ();
      void update ( bool );
    } m_resolution;
    // Orientation to be add

    std::unique_ptr <Game> m_game; // unique pointer to game wrapper

    // pointers to input (background worker thread)
    Windows::Foundation::IAsyncAction* m_inputLoop; // worker thread
    Windows::UI::Core::CoreIndependentInputSource* m_inputCore; // independent input

    bool m_initialized; // true in case of successful initialization

    // XAML low-level rendering event
    void m_onRendering ( Windows::Foundation::IInspectable const& /*sender*/,
                         Windows::Foundation::IInspectable const& /*args*/ ); // on application window activation

    // window events
    void m_onActivated ( Windows::UI::Core::CoreWindow const& /*applicationView*/,
                         Windows::UI::Core::WindowActivatedEventArgs const& /*args*/ ); // on application window activation

    void m_onFocused ( Windows::UI::Core::CoreWindow const& /*sender*/,
                       Windows::UI::Core::WindowActivatedEventArgs const& /*args*/ ); // on application window focused/unfocused

    void m_onWindowResized ( Windows::UI::Core::CoreWindow const& /*sender*/,
                             Windows::UI::Core::WindowSizeChangedEventArgs const& /*args*/ ); // on application window resize

    void m_onVisibilityChanged ( Windows::UI::Core::CoreWindow const& /*sender*/,
                                 Windows::UI::Core::VisibilityChangedEventArgs const& /*args*/ ); // on application window visibility

    void m_onDpiChanged ( Windows::Graphics::Display::DisplayInformation const& /*sender*/,
                          Windows::Foundation::IInspectable const& /*args*/ ); // on client display DPI changed

    void m_onOrientationChanged ( Windows::Graphics::Display::DisplayInformation const& /*sender*/,
                                  Windows::Foundation::IInspectable const& /*args*/ ); // on client display orientation changed

    void m_onDisplayContentInvalidated ( Windows::Graphics::Display::DisplayInformation const& /*sender*/,
                                         Windows::Foundation::IInspectable const& /*args*/ ); // on application window content invalidation

    // other events
    void m_onCompositionScaleChanged ( Windows::UI::Xaml::Controls::SwapChainPanel const& /*sender*/,
                                       Windows::Foundation::IInspectable const& /*args*/ ); // on composition scale resize

    void m_onSwapChainPanelSizeChanged ( Windows::Foundation::IInspectable const& /*sender*/,
                                         Windows::UI::Xaml::SizeChangedEventArgs const& /*args*/ ); // on swap chain panel resize

    // input events
    void m_onAcceleratorKeyActivated ( winrt::Windows::UI::Core::CoreDispatcher const& /*sender*/,
                                       winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& /*args*/ ); // on accelerator key pressed

    void m_onPointerPressed ( Windows::Foundation::IInspectable const& /*sender*/,
                                       winrt::Windows::UI::Core::PointerEventArgs const& /*args*/ ); // on pointer pressed

    void m_onPointerMoved ( Windows::Foundation::IInspectable const& /*sender*/,
                              winrt::Windows::UI::Core::PointerEventArgs const& /*args*/ ); // on pointer moved

    void m_onPointerReleased ( Windows::Foundation::IInspectable const& /*sender*/,
                            winrt::Windows::UI::Core::PointerEventArgs const& /*args*/ ); // on pointer released

    // control events
    //void ClickHandler ( Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args );
    void AppBarButton_Click ( Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args );

  public:
    MainPage ();
    //~MainPage ();

    int32_t MyProperty ();
    void MyProperty ( int32_t value );

    void Button_Click ( winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e );

    // save/load internal state on app resume (exposed as public)
    //void internal state
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

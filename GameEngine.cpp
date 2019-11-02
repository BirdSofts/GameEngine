// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

//#include "GameEngine.h"
#include "pch.h"


using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;


class View : public winrt::implements<View, IFrameworkViewSource, IFrameworkView>
{
private:
  winrt::agile_ref<CoreWindow> m_appWindow; // reference to application window
  bool m_running;
  bool m_visible;
  // resize
  // DPI
  // Width / Height
  // Orientation
  //std::unique_ptr <CoreDX>;
protected:
  void m_onActivated ( CoreApplicationView const& /*applicationView*/,
                       IActivatedEventArgs const& /*args*/ ); // on application window activation
  void m_onVisibilityChanged ( CoreWindow const& /*sender*/,
                               VisibilityChangedEventArgs const& /*args*/ ); // on application window resize
  void m_onWindowResized ( CoreWindow const& /*sender*/,
                           WindowSizeChangedEventArgs const& /*args*/ ); // on application window resize
  void m_onSuspending ( IInspectable const& /*sender*/,
                        SuspendingEventArgs const& /*args*/ ); // on application window entering the suspended state
  void m_onResuming ( IInspectable const& /*sender*/,
                      IInspectable const& /*args*/ ); // on application window resuming from suspended state

  //void m_on Key Pressed
  //void m_on Dpi Changed
  //void m_on Orientation Changed
  //void m_on Display Content Invalidated
  //
public:
  View ( void );
  //~View ( void );

  // implementation documentation:
  // for being able to implement the object which implements IFrameworkView interface,
  // it should made ready by the IFrameworkViewSource factory interface
  // the order in which these methods are called (after defined by a CoreApplication singleton instance):
  // CreateView, Initialize, Load, Run

  // IFrameworkViewSource methods:
  IFrameworkView CreateView ( void ); // creation method overload

  // IFrameworkView methods:
  void Initialize ( CoreApplicationView const& ); // initialization method overload (called on app launch)
  void Load ( winrt::hstring const& /*entryPoint*/ ); // load method overload
  void Run ( void ); // execution method overload (called after load method)
  void SetWindow ( CoreWindow const& ); // window set method overload (after initialization)
  void Uninitialize ( void ); // un-initialization method overload
};


int WINAPI wWinMain ( _In_ HINSTANCE /*hInstance*/,
                      _In_ HINSTANCE /*hPrevInstance*/,
                      _In_ LPWSTR /*LpCmdLine*/,
                      _In_ int /*nCmdShow*/ )
{

  if (!DirectX::XMVerifyCPUSupport ())
  {
    // error handling
  }

  auto view = winrt::make<View> ();
  CoreApplication::Run ( view );
  //CoreApplication::Run ( View () );
  return EXIT_SUCCESS;

}


View::View ( void ) :
  m_running ( false ), m_visible ( false )
{

};


//View::~View ( void )
//{
//
//};


IFrameworkView View::CreateView ( void )
{
  return *this;
};


void View::m_onActivated ( CoreApplicationView const& /*applicationView*/, IActivatedEventArgs const& /*args*/ )
{

  // visible the activated application window and enables it to receive events
  CoreWindow::GetForCurrentThread ().Activate ();

};


void View::m_onVisibilityChanged ( CoreWindow const& /*sender*/, VisibilityChangedEventArgs const& args )
{
  m_visible = args.Visible ();
  //if(m_visible)
    // Todo: CoreDX, present to screen
};


void View::m_onWindowResized ( CoreWindow const& /*sender*/, WindowSizeChangedEventArgs const& /*args*/ )
{
  // Todo: window behaviour and needed procedure
  // Todo: resize the DirectX resources
};


void View::m_onSuspending ( IInspectable const& /*sender*/, SuspendingEventArgs const& /*args*/ )
{

};


void View::m_onResuming ( IInspectable const& /*sender*/, IInspectable const& /*args*/ )
{

};


void View::Initialize ( CoreApplicationView const& applicationView )
{

  m_running = true;

  // response to application window activation
  applicationView.Activated ( { this, &View::m_onActivated } );

  // response to application window suspension
  CoreApplication::Suspending ( { this, &View::m_onSuspending } );

  // response to application window resuming from suspension
  CoreApplication::Resuming ( { this, &View::m_onResuming } );

  // Todo: 
  //CoreDX = std::make_unique<CoreDX> ();

};


void View::Load ( winrt::hstring const& /*entryPoint*/ )
{

};


void View::Run ( void )
{

  while (m_running)
  {
    if (m_visible)
    {

      // Todo: tick the timer and initiate, update and present the scene

      CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
        CoreProcessEventsOption::ProcessAllIfPresent );

    } else
    {

      // Todo: additional suitable processes for hibernation state

      CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
        CoreProcessEventsOption::ProcessOneIfPresent );
      // if no task go to hibernation
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );

      // Microsoft's resource (suspension)
      //CoreWindow::GetForCurrentThread ().Dispatcher ().ProcessEvents (
      //  CoreProcessEventsOption::ProcessOneAndAllPending );

    }
  }

};


void View::SetWindow ( CoreWindow const& window )
{

  m_appWindow = window;

  // cursor type: standard arrow cursor resource
  m_appWindow.get ().PointerCursor ( CoreCursor { CoreCursorType::Arrow, 0 } );

  // response to window size events
  m_appWindow.get ().SizeChanged ( { this, &View::m_onWindowResized } );

  // response to window close events
  m_appWindow.get ().Closed ( [this]( auto&&, auto&& ) { m_running = false; } );

  // response to window close events
  m_appWindow.get ().VisibilityChanged ( { this, &View::m_onVisibilityChanged } );

};


void View::Uninitialize ( void )
{
  // Todo: free the taken resources
  // shutdown precess
};

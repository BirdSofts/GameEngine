// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,30.06.2020</changed>
// ********************************************************************************

#ifndef APP_H
#define APP_H


#include "App.xaml.g.h"
#include "MainPage.h"


namespace winrt::GameEngine::implementation
{
  struct App : AppT<App>
  {
  private:
    MainPage* m_mainPage;

    static bool failure; // true if exiting with failure

    void OnSuspending ( IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const& );
    void OnNavigationFailed ( IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const& );

  public:
    App ();

    void OnLaunched ( Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const& );
  };
}


#endif // !APP_H

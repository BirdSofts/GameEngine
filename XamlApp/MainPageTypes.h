// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,10.07.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef MAINPAGETYPES_H
#define MAINPAGETYPES_H


class MainPageTypes
{
private:
  struct Display // resolution wrapper
  {
    float Dpi; // current DPI
    float windowWidthDips; // current window width (Dips)
    float windowHeightDips; // current window height (Dips)
    uint32_t windowWidthPixels; // current window width (Pixels) (feed from configuration)
    uint32_t windowHeightPixels; // current window height (Pixels) (feed from configuration)
    float panelWidthDips; // current swap chain panel width (Dips)
    float panelHeightDips; // current swap chain panel height (Dips)
    uint32_t panelWidthPixels; // current swap chain panel width (Pixels)
    uint32_t panelHeightPixels; // current swap chain panel height (Pixels)
    bool fullscreen; // current full screen state (feed from configuration)

    void updatePixels ();
    void updateDips ();
  } m_display;
  // Orientation to be add

  winrt::hstring m_pointer;
public:
  MainPageTypes ();
  //~MainPageTypes ();

  Display* m_getDisplay ( void ) { return &m_display; };
  winrt::hstring& m_getPointer ( void ) { return m_pointer; };
};


#endif // !MAINPAGETYPES_H

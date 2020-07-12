// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef SETTINGS_H
#define SETTINGS_H


// configurations container
struct Configs
{
  unsigned int Width;
  unsigned int Height;
  bool fullscreen;
};


// application configurations wrapper
class Configurations
{
private:
  std::wstring m_path;
  Configs m_defaults; // application defaults
  Configs m_currents; // user settings

  bool m_valid; // true if settings file is not corrupt
public:
  Configurations ( void );
  //~Configurations ( void );

  const bool m_apply ( const Configs& object ); // apply method

  const bool& isValid ( void ) { return m_valid; }; // get the state of settings file
  const Configs& m_getDefaults ( void ) { return m_defaults; }; // get application defaults
  const Configs& m_getSettings ( void ) { return m_currents; }; // get current/user settings
};


#endif // !SETTINGS_H

// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
//xx <changed>ʆϒʅ,28.06.2020</changed>
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
  Configs m_defaults;
  Configs m_currents;

  bool m_valid;
public:
  Configurations ( void );
  //~Configurations ( void );
  const bool& isValid ( void );

  const Configs& m_getDefaults ( void );
  const Configs& m_getSettings ( void );
  const bool m_apply ( const Configs& ); // apply method
};


#endif // !SETTINGS_H

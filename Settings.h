// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,06.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#ifndef SETTINGS_H
#define SETTINGS_H


// configurations container
struct ConfigsContainer
{
  unsigned int Width;
  unsigned int Height;
  bool fullscreen;
};


// application configurations class
class Configurations
{
private:
  std::wstring path;
  bool valid;
  ConfigsContainer defaults;
  ConfigsContainer currents;
public:
  Configurations ( void );
  const bool& isValid ( void );
  const ConfigsContainer& getDefaults ( void );
  const ConfigsContainer& getSettings ( void );
  const bool apply ( const ConfigsContainer& ); // apply method
};// application configurations container


#endif // !SETTINGS_H

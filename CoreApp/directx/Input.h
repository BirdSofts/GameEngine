// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,25.06.2020</created>
/// <changed>ʆϒʅ,27.06.2020</changed>
// ********************************************************************************

#ifndef INPUT_H
#define INPUT_H


// DirectX Input wrapper
class Input
{
private:
  /*Microsoft::WRL::ComPtr<IDirectInput8> m_dInput;
  Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;
  Microsoft::WRL::ComPtr<IDirectInputDevice8> m_mouse;*/

  //DIMOUSESTATE;

  bool initialized; // true if initialization was successful
public:
  Input ( void );
  //~Input ( void );
  const bool& isInitialized ( void ); // get the initialized state

};


#endif // !INPUT_H

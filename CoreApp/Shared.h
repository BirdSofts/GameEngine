// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,12.11.2019</changed>
// ********************************************************************************

#ifndef SHARED_H
#define SHARED_H


#include "common/Logger.h"
#include "common/Settings.h"
#include "common/Utilities.h"


// application common variables container
struct Variables
{
  bool running;
  std::string currentState;
  Variables ( void );
};


//template<class tType>
  //static std::shared_ptr<tType> pointerEntity;
  //static tType* get (); // generic locator
  //static void provider ( std::shared_ptr<tType> ); // generic provider


class PointerProvider
{
private:
  static std::shared_ptr<Variables> variables; // shared pointer to application common variables container
  static std::shared_ptr<TheException> exception; // shared pointer to exception class
  static std::shared_ptr<Configurations> configuration; // shared pointer to configuration class
  static std::shared_ptr<Logger<ToFile>> fileLogger; // shared pointer to file logger class
public:

  // smart shared pointers point to exceptions, provide extended lifetime and are passable across threads:
  //static std::exception_ptr exceptionPointer; // smart shared pointer-like type

  static Variables* getVariables ( void ); // application common variables locator
  static TheException* getException ( void ); // exception locator
  static Configurations* getConfiguration ( void ); // configuration locator
  static Logger<ToFile>* getFileLogger ( void ); // file logger locator
  static void providerVariables ( std::shared_ptr<Variables> ); // application common variables provider
  static void providerException ( std::shared_ptr<TheException> ); // exception provider
  static void providerConfiguration ( std::shared_ptr<Configurations> ); // configuration provider
  static void providerFileLogger ( std::shared_ptr<Logger<ToFile>> ); // file logger provider
};


#endif // !SHARED_H

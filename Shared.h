// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,02.11.2019</changed>
// ********************************************************************************

#ifndef SHARED_H
#define SHARED_H


//#define _NOT_DEBUGGING // shipping time definition


#include "Utilities.h"


// global:
extern bool running;

extern std::wstring gameState;


//template<class tType>
  //static std::shared_ptr<tType> pointerEntity;
  //static tType* get (); // generic locator
  //static void provider ( std::shared_ptr<tType> ); // generic provider


class PointerProvider
{
private:
  static std::shared_ptr<theException> exception; // shared pointer to exception class
  static std::shared_ptr<Configurations> configuration; // shared pointer to configuration class
  static std::shared_ptr<Logger<toFile>> fileLogger; // shared pointer to file logger class
public:

  // smart shared pointers point to exceptions, provide extended lifetime and are passable across threads:
  static std::exception_ptr exceptionPointer; // smart shared pointer-like type

  static theException* getException ( void ); // exception locator
  static Configurations* getConfiguration ( void ); // configuration locator
  static Logger<toFile>* getFileLogger ( void ); // file logger locator
  static void exceptionProvider ( std::shared_ptr<theException> ); // exception provider
  static void configurationProvider ( std::shared_ptr<Configurations> ); // configuration provider
  static void fileLoggerProvider ( std::shared_ptr<Logger<toFile>> ); // file logger provider
};


#endif // !SHARED_H

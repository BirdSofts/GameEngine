// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Shared.h"


Variables::Variables ( void )
{
  running = false;
  currentState = "";
};


//template<class tType>
//std::shared_ptr<tType> PointerProvider<tType>::pointerEntity = nullptr;
//template<class tType>
//tType* PointerProvider<tType>::get ()
//{
//  return pointerEntity.get ();
//};
//template<class tType>
//void PointerProvider<tType>::provider ( std::shared_ptr<tType> object )
//{
//  pointerEntity = object;
//};


std::shared_ptr<Variables> PointerProvider::variables = nullptr;

std::shared_ptr<TheException> PointerProvider::exception = nullptr;

std::shared_ptr<Configurations> PointerProvider::configuration = nullptr;

std::shared_ptr<Logger<ToFile>> PointerProvider::fileLogger = nullptr;


//std::exception_ptr PointerProvider::exceptionPointer = nullptr;


Variables* PointerProvider::getVariables ( void )
{
  return variables.get ();
};


TheException* PointerProvider::getException ( void )
{
  return exception.get ();
};


Configurations* PointerProvider::getConfiguration ( void )
{
  return configuration.get ();
};


Logger<ToFile>* PointerProvider::getFileLogger ( void )
{
  return fileLogger.get ();
};


void PointerProvider::providerVariables ( std::shared_ptr<Variables> object )
{
  variables = object;
};


void PointerProvider::providerException ( std::shared_ptr<TheException> object )
{
  exception = object;
};


void PointerProvider::providerConfiguration ( std::shared_ptr<Configurations> object )
{
  configuration = object;
};


void PointerProvider::providerFileLogger ( std::shared_ptr<Logger<ToFile>> object )
{
  fileLogger = object;
};

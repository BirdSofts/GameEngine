// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,25.06.2020</created>
/// <changed>ʆϒʅ,27.06.2020</changed>
// ********************************************************************************

#include "pch.h"
#include "Input.h"
#include "Shared.h"


Input::Input ( void )
{
  try
  {



  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Input::~Input ( void )
//{
//
//};


const bool& Input::isInitialized ( void )
{
  return initialized;
};

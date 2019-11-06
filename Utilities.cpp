// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Utilities.h"
#include "Shared.h"


using namespace winrt::Windows::Storage;


TheException::TheException ( void ) : expected ( "null" ) {};
void TheException::set ( const char* prm )
{
  expected = prm;
};
const char* TheException::what ( void ) const throw()
{
  return expected;
};


std::string Converter::strConverter ( const winrt::hstring& hStr )
{
  try
  {

    std::wstring strW { hStr.begin () };
    std::string str { strW.begin (), strW.end () };
    const char* charStr = str.c_str ();
    char charAr [512];
    //sprintf_s ( file, "%s\\dump.log", charStr );
    sprintf_s ( charAr, charStr );
    std::string result { charAr };
    return result;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return "";
  }
};


std::string Converter::strConverter ( const std::wstring& strW )
{
  try
  {

    std::string str { strW.begin (), strW.end () };
    const char* charStr = str.c_str ();
    char charAr [512];
    sprintf_s ( charAr, charStr );
    std::string result { charAr };
    return result;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return "";
  }
};


std::wstring Converter::strConverter ( const std::string& str )
{
  try
  {

    const char* charStr = str.c_str ();
    char charAr [512];
    sprintf_s ( charAr, charStr );
    std::wstringstream test;
    test << charStr;
    std::wstring result { test.str () };
    return result;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return L"";
  }
};

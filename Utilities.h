// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H


// special expansion to exception container
class TheException : public std::exception
{
private:
  const char* expected;
public:
  TheException ( void );
  void set ( const char* );
  const char* what ( void ) const throw();
};


// string converter class
class Converter
{
private:
public:
  // standard string types converters (Lua needs)
  // Windows 10 builds upon Unicode natively, thus using wide strings is wise.

  // warning: just ASCI characters:
  static std::string strConverter ( const winrt::hstring& );
  static std::string strConverter ( const std::wstring& );
  static std::wstring strConverter ( const std::string& );

  // Todo: Tests:
  //const char* test { "AAAAAABB//.;:?*" };
  //std::string result { test };
  //std::wstring resultTwo { Converter::strConverter ( test ) };
  //winrt::hstring resultThree { Converter::strConverter ( test ) };
};


#endif // !UTILITIES_H

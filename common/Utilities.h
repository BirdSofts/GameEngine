// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H


// special expansion to exception container
class TheException : public std::exception
{
private:
  const char* m_expected;
public:
  TheException ( void );

  void m_set ( const char* );
  const char* what ( void ) const throw(); // standard exception function overload
};


// string converter class
class Converter
{
private:
public:
  // standard string types converters (application needs)
  // Windows 10 builds upon Unicode natively, thus using wide strings is wise.

  // warning: just ASCI characters are supported:
  static std::string strConverter ( const winrt::hstring& ); // h_string to std_string
  static std::string strConverter ( const std::wstring& ); // std_wstring to std_string
  static std::wstring strConverter ( const std::string& ); // std_string to std_wstring

  // Todo: usable for Tests:
  //const char* test { "AAAAAABB//.;:?*" };
  //std::string result { test };
  //std::wstring resultTwo { Converter::strConverter ( test ) };
  //winrt::hstring resultThree { Converter::strConverter ( test ) };
};


#endif // !UTILITIES_H

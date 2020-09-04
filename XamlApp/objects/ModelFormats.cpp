// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "ModelFormats.h"
#include "Shared.h"


unsigned int VertexTexDiffuseL::read ( const char* path, VertexTL** vertices )
{
  try
  {

    std::ifstream file ( path );
    unsigned int count { 0 };
    std::string type { "" };

    if (file.is_open ())
    {
      std::string input;
      std::stringstream stream;

      std::getline ( file, input );
      stream << input;
      stream >> count;
      stream >> type;
      stream.clear ();

      *vertices = new (std::nothrow) VertexTL [count];

      unsigned int index { 0 };
      for (unsigned int i = 3; i <= count; i += 3)
      {

        std::getline ( file, input );
        stream << input;
        index = i - 3;

        // read one triangle
        while (index != i)
        {
          stream >> (*vertices) [index].position.x >> (*vertices) [index].position.y >> (*vertices) [index].position.z;
          stream >> (*vertices) [index].texture.x >> (*vertices) [index].texture.y;
          stream >> (*vertices) [index].normal.x >> (*vertices) [index].normal.y >> (*vertices) [index].normal.z;
          index++;
        }

        stream.clear ();

      }
    }
    file.close ();

    return count;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return 0;
  }
};


//void VertexTexDiffuseL::write ( VertexTL* /*data*/, unsigned int& /*count*/ )
//{
//  try
//  {
//
//  }
//  catch (const std::exception&)
//  {
//
//  }
//};

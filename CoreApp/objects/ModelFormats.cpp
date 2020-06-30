// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,24.06.2020</changed>
// ********************************************************************************

#include "pch.h"
#include "ModelFormats.h"
#include "Shared.h"


unsigned int VertexTexDiffuseL::read ( const char* path, VertexL** vertices )
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

      *vertices = new (std::nothrow) VertexL [count];

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
          (*vertices) [index].position.x += .2f;
          /*(*vertices) [index].position.y += 1;
          (*vertices) [index].position.z += 1;*/
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


void VertexTexDiffuseL::write ( VertexL* /*data*/, unsigned int& /*count*/ )
{
  try
  {

  }
  catch (const std::exception&)
  {

  }
};

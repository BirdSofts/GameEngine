// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#include "pch.h"
#include "texture.h"
#include "Shared.h"


Texture::Texture ( TheCore* coreObj, const char* path ) :
  m_core ( coreObj ),
  m_fileTga ( nullptr ), m_filePng ( nullptr ), m_data ( nullptr ),
  m_texture ( nullptr ), m_textureView ( nullptr ), m_initialized ( false )
{
  try
  {

    HRESULT hR;
    winrt::com_ptr<ID3D11Device3> device { m_core->m_getD3D ()->m_getDevice () };
    winrt::com_ptr<ID3D11DeviceContext3> devCon { m_core->m_getD3D ()->m_getDevCon () };


    m_fileTga = new (std::nothrow) TgaHeader ();


    if (!m_loadTga ( path ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Loading the texture failed! file: "
                                                  + std::string ( path ) );
      if (m_data)
        delete [] m_data;
      m_data = nullptr;
      return;
    }


    // texture description
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Height = m_fileTga->height;
    textureDesc.Width = m_fileTga->width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bit RGBA
    textureDesc.SampleDesc.Count = 1; // default
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    // default (UpdateSubresource): higher speed memory, gets cache retention preference (for not removed or reloaded data)
    // dynamic (Map,Unmap): will place in the not cached memory locations (data will be rewritten shortly)

    // settings required for mipmaped textures
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    // empty texture creation
    hR = device->CreateTexture2D ( &textureDesc, nullptr, &m_texture );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of texture 2D failed!"
                                                  + std::string ( path ) );
      return;
    }


    // row pitch of targa image data
    unsigned int rowPitch = (m_fileTga->width * 4) * sizeof ( char );

    // targa image data array into texture
    // note that the use of Map and Unmap is possible and generally a lot quicker
    // recommendations for correct choice:
    //-- Map and Unmap for data that is reloaded each frame or on a very regular basis.
    //-- UpdateSubresource: for data that is loaded once or rarely during loading sequences
    devCon->UpdateSubresource ( m_texture, 0, nullptr, m_data, rowPitch, 0 );


    // shader view description
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResDataDesc;
    shaderResDataDesc.Format = textureDesc.Format;
    shaderResDataDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    // important mipmap variables: below: full range of Mipmap levels for high quality texture rendering at any distance
    shaderResDataDesc.Texture2D.MostDetailedMip = 0;
    shaderResDataDesc.Texture2D.MipLevels = 1;

    // texture shader resource view creation
    // purpose: to set the texture in shaders
    hR = device->CreateShaderResourceView ( m_texture, &shaderResDataDesc, &m_textureView );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of shader resource view failed!"
                                                  + std::string ( path ) );
      return;
    }
    m_textureView->GetDesc ( &shaderResDataDesc );


    // texture mipmaps generation
    devCon->GenerateMips ( m_textureView );


    delete [] m_data;
    m_data = nullptr;

    m_initialized = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Texture::~Texture ()
//{
//
//};


// .tga loader
bool Texture::m_loadTga ( const char* path )
{
  try
  {

    FILE* filePtr { nullptr };


    if (fopen_s ( &filePtr, path, "rb" ) != 0) // open in binary for read
      return false;


    // read one full item (targa file Header) introduced by count parameter
    // note that the function returns the number of full items successfully read
    if (fread ( m_fileTga, sizeof ( TgaHeader ), 1, filePtr ) != 1) // file header
      return false;


    // format description
    int imageSize { m_fileTga->width * m_fileTga->height * 4 };
    int imageSizeCalculated { 0 }; // 24 bit targa image file support

    if (m_fileTga->bpp == 24) // if 24 bit (32 bit targa files with support for alpha channels)
      imageSizeCalculated = m_fileTga->width * m_fileTga->height * 3; // 32 bit image data size calculation
    else
      if (m_fileTga->bpp == 32)
        imageSizeCalculated = imageSize;
      else
        return false;


    // targa format is stored upside down, therefore allocations for correction procedure:
    unsigned char* tempData;
    tempData = new (std::nothrow) unsigned char [imageSizeCalculated]; // targa image data holder
    if (!tempData)
      return false;
    m_data = new (std::nothrow) unsigned char [imageSize]; // targa image data holder in correct order
    if (!m_data)
      return false;


    // read the data and free the file resource
    if (fread ( tempData, 1, imageSizeCalculated, filePtr ) != imageSizeCalculated)
    {
      delete [] tempData;
      return false;
    }
    if (fclose ( filePtr ) != 0)
    {
      delete [] tempData;
      return false;
    }


    // correction procedure
    if (m_fileTga->bpp == 32) // if 24 bit (32 bit targa files with support for alpha channels)
    {

      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSize - (m_fileTga->width * 4) }; // index (the actual on disk targa image data)

      // the actual restoring in correct order
      for (int i = 0; i < m_fileTga->height; i++)
      {
        for (int j = 0; j < m_fileTga->width; j++)
        {

          m_data [indexD + 0] = tempData [indexS + 0]; // Read
          m_data [indexD + 1] = tempData [indexS + 3]; // Green
          m_data [indexD + 2] = tempData [indexS + 2]; // Blue
          m_data [indexD + 3] = tempData [indexS + 1]; // Alpha

          indexD += 4; // increment indices
          indexS += 4;

        }
        indexS -= (m_fileTga->width * 8); // set to begin of the column of next preceding row
      }

    } else
    {

      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSizeCalculated - (m_fileTga->width * 3) }; // index (the actual on disk targa image data)

      unsigned char alpha { 0 };

      for (int i = 0; i < m_fileTga->height; i++)
      {
        for (int j = 0; j < m_fileTga->width; j++)
        {

          m_data [indexD + 0] = tempData [indexS + 0]; // Read
          m_data [indexD + 1] = tempData [indexS + 2]; // Green
          m_data [indexD + 2] = tempData [indexS + 1]; // Blue
          m_data [indexD + 3] = alpha; // Alpha

          indexD += 4;
          indexS += 3;

        }
        indexS -= (m_fileTga->width * 6);
      }

    }


    delete [] tempData;
    return true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


// .png loader specialization
//template<typename PngHeader>
//bool Texture::m_load ( const char* /*path*/ )
//{
//  //Todo research more interesting formats to add
//  return false;
//};


void Texture::m_release ()
{
  try
  {

    m_initialized = false;

    if (m_fileTga != nullptr)
    {
      delete m_fileTga;
      m_fileTga = nullptr;
    }

    if (m_filePng != nullptr)
    {
      delete m_filePng;
      m_filePng = nullptr;
    }

    if (m_data != nullptr)
    {
      delete [] m_data;
      m_data = nullptr;
    }

    if (m_texture != nullptr)
    {
      m_texture->Release ();
      m_texture = nullptr;
    }
    if (m_textureView != nullptr)
    {
      m_textureView->Release ();
      m_textureView = nullptr;
    }

    m_core = nullptr;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//void TextureClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
//{
//
//  TheCore* core { nullptr };
//  Texture<TargaHeader> tempTex ( core, "" );
//  tempTex.m_getTexture ();
//  tempTex.m_release ();
//
//}

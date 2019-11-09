// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "texture.h"
#include "Shared.h"


template <typename fileType>
Texture<fileType>::Texture ( ID3D11Device* dev, ID3D11DeviceContext* devC, const char* path ) :
  m_device ( dev ), m_deviceContext ( devC ),
  m_data ( nullptr ), m_texture ( nullptr ), m_textureView ( nullptr ), m_initialized ( false )
{
  try
  {

    HRESULT hR;


    if (!m_load ( path ))
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
    textureDesc.Height = m_file.height;
    textureDesc.Width = m_file.width;
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
    hR = m_device->CreateTexture2D ( &textureDesc, nullptr, &m_texture );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of texture 2D failed!"
                                                  + std::string ( path ) );
      return;
    }


    // row pitch of targa image data
    unsigned int rowPitch = (m_file.width * 4) * sizeof ( char );

    // targa image data array into texture
    // note that the use of Map and Unmap is possible and generally a lot quicker
    // recommendations for correct choice:
    //-- Map and Unmap for data that is reloaded each frame or on a very regular basis.
    //-- UpdateSubresource: for data that is loaded once or rarely during loading sequences
    m_deviceContext->UpdateSubresource ( m_texture, 0, nullptr, m_data, rowPitch, 0 );


    // shader view description
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResDataDesc;
    shaderResDataDesc.Format = textureDesc.Format;
    shaderResDataDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    // important mipmap variables: below: full range of Mipmap levels for high quality texture rendering at any distance
    shaderResDataDesc.Texture2D.MostDetailedMip = 0;
    shaderResDataDesc.Texture2D.MipLevels = 1;

    // texture shader resource view creation
    // purpose: to set the texture in shaders
    hR = m_device->CreateShaderResourceView ( m_texture, &shaderResDataDesc, &m_textureView );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Creation of shader resource view failed!"
                                                  + std::string ( path ) );
      return;
    }
    m_textureView->GetDesc ( &shaderResDataDesc );


    // texture mipmaps generation
    m_deviceContext->GenerateMips ( m_textureView );


    delete [] m_data;
    m_data = nullptr;

    m_initialized = true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//template <typename fileType>
//Texture<fileType>::~Texture ()
//{
//
//};


template <typename fileType>
const bool& Texture<fileType>::m_isInitialized ( void )
{
  return m_initialized;
};


// .tga loader specialization
//template<typename TargaHeader>
bool Texture<TargaHeader>::m_load ( const char* path )
{
  try
  {

    FILE* filePtr { nullptr };


    if (fopen_s ( &filePtr, path, "rb" ) != 0) // open in binary for read
      return false;


    // read one full item (targa file Header) introduced by count parameter
    // note that the function returns the number of full items successfully read
    if (fread ( &m_file, sizeof ( TargaHeader ), 1, filePtr ) != 1) // file header
      return false;


    // format description
    int imageSize { m_file.width * m_file.height * 4 };
    int imageSizeCalculated { 0 }; // 24 bit targa image file support

    if (m_file.bpp == 24) // if 24 bit (32 bit targa files with support for alpha channels)
      imageSizeCalculated = m_file.width * m_file.height * 3; // 32 bit image data size calculation
    else
      if (m_file.bpp == 32)
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
    if (m_file.bpp == 32) // if 24 bit (32 bit targa files with support for alpha channels)
    {

      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSize - (m_file.width * 4) }; // index (the actual on disk targa image data)

      // the actual restoring in correct order
      for (int i = 0; i < m_file.height; i++)
      {
        for (int j = 0; j < m_file.width; j++)
        {

          m_data [indexD + 0] = tempData [indexS + 0]; // Read
          m_data [indexD + 1] = tempData [indexS + 3]; // Green
          m_data [indexD + 2] = tempData [indexS + 2]; // Blue
          m_data [indexD + 3] = tempData [indexS + 1]; // Alpha

          indexD += 4; // increment indices
          indexS += 4;

        }
        indexS -= (m_file.width * 8); // set to begin of the column of next preceding row
      }

    } else
    {

      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSizeCalculated - (m_file.width * 3) }; // index (the actual on disk targa image data)

      unsigned char alpha { 0 };

      for (int i = 0; i < m_file.height; i++)
      {
        for (int j = 0; j < m_file.width; j++)
        {

          m_data [indexD + 0] = tempData [indexS + 0]; // Read
          m_data [indexD + 1] = tempData [indexS + 2]; // Green
          m_data [indexD + 2] = tempData [indexS + 1]; // Blue
          m_data [indexD + 3] = alpha; // Alpha

          indexD += 4;
          indexS += 3;

        }
        indexS -= (m_file.width * 6);
      }

    }


    delete [] tempData;
    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};


// .tga loader specialization
//template<typename PngHeader>
bool Texture<PngHeader>::m_load ( const char* /*path*/ )
{
  //Todo research more interesting formats to add
  return false;
};


template<typename fileType>
ID3D11ShaderResourceView** const Texture<fileType>::m_getTexture ()
{
  return &m_textureView;
};


template<typename fileType>
void Texture<fileType>::m_release ()
{
  try
  {

    m_initialized = false;

    if (m_data)
      delete [] m_data;
    if (m_texture)
    {
      m_texture->Release ();
      m_texture = nullptr;
    }
    if (m_textureView)
    {
      m_textureView->Release ();
      m_textureView = nullptr;
    }

    m_device = nullptr;
    m_deviceContext = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


void TextureClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  ID3D11Device* dev { nullptr };
  ID3D11DeviceContext* devCon { nullptr };
  Texture<TargaHeader> tempTex ( dev, devCon, "" );
  tempTex.m_getTexture ();
  tempTex.m_release ();

}

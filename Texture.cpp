// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,01.11.2019</created>
/// <changed>ʆϒʅ,06.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "texture.h"
#include "Shared.h"


template <typename fileType>
Texture<fileType>::Texture ( ID3D11Device* dev, ID3D11DeviceContext* devC, const char* path ) :
  device ( dev ), devCon ( devC ),
  data ( nullptr ), texture ( nullptr ), textureView ( nullptr )
{
  try
  {

    HRESULT hR;

    if (!Load ( path ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Loading the texture failed! file: "
                                                + std::string ( path ) );
      if (data)
        delete [] data;
      data = nullptr;
      return;
    }

    // texture description
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Height = file.height;
    textureDesc.Width = file.width;
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
    hR = device->CreateTexture2D ( &textureDesc, nullptr, &texture );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of texture 2D failed!"
                                                + std::string ( path ) );
      return;
    }

    // row pitch of targa image data
    unsigned int rowPitch = (file.width * 4) * sizeof ( char );

    // targa image data array into texture
    // note that the use of Map and Unmap is possible and generally a lot quicker
    // recommendations for correct choice:
    //-- Map and Unmap for data that is reloaded each frame or on a very regular basis.
    //-- UpdateSubresource: for data that is loaded once or rarely during loading sequences
    devCon->UpdateSubresource ( texture, 0, nullptr, data, rowPitch, 0 );

    // shader view description
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResdataDesc;
    shaderResdataDesc.Format = textureDesc.Format;
    shaderResdataDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    // important mipmap variables: below: full range of Mipmap levels for high quality texture rendering at any distance
    shaderResdataDesc.Texture2D.MostDetailedMip = 0;
    shaderResdataDesc.Texture2D.MipLevels = -1;

    // texture shader resource view creation
    // purpose: to set the texture in shaders
    hR = device->CreateShaderResourceView ( texture, &shaderResdataDesc, &textureView );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Creation of shader resource view failed!"
                                                + std::string ( path ) );
      return;
    }
    textureView->GetDesc ( &shaderResdataDesc );

    // texture mipmaps generation
    devCon->GenerateMips ( textureView );

    delete [] data;
    data = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


// .tga loader specialization
//template<typename TargaHeader>
bool Texture<TargaHeader>::Load ( const char* path )
{
  try
  {

    FILE* filePtr { nullptr };

    if (fopen_s ( &filePtr, path, "rb" ) != 0) // open in binary for read
      return false;

    // read one full item (targa file Header) introduced by count parameter
    // note that the function returns the number of full items successfully read
    if (fread ( &file, sizeof ( TargaHeader ), 1, filePtr ) != 1) // file header
      return false;

    int imageSize { file.width * file.height * 4 };
    int imageSizeTmp { 0 }; // 24 bit targa image file support

    if (file.bpp == 24) // if 24 bit (32 bit targa files with support for alpha channels)
      imageSizeTmp = file.width * file.height * 3; // 32 bit image data size calculation
    else
      if (file.bpp == 32)
        imageSizeTmp = imageSize;
      else
        return false;

    unsigned char* temp; // targa format is stored upside down

    temp = new (std::nothrow) unsigned char [imageSizeTmp]; // targa image data holder
    if (!temp)
      return false;

    data = new (std::nothrow) unsigned char [imageSize]; // targa image data holder in correct order
    if (!data)
      return false;

    if (fread ( temp, 1, imageSizeTmp, filePtr ) != imageSizeTmp)
    {
      delete [] temp;
      return false;
    }

    if (fclose ( filePtr ) != 0)
    {
      delete [] temp;
      return false;
    }

    if (file.bpp == 32) // if 24 bit (32 bit targa files with support for alpha channels)
    {
      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSize - (file.width * 4) }; // index (the actual on disk targa image data)

      // the actual restoring in correct order
      for (int i = 0; i < file.height; i++)
      {
        for (int j = 0; j < file.width; j++)
        {
          data [indexD + 0] = temp [indexS + 0]; // Read
          data [indexD + 1] = temp [indexS + 3]; // Green
          data [indexD + 2] = temp [indexS + 2]; // Blue
          data [indexD + 3] = temp [indexS + 1]; // Alpha

          indexD += 4; // increment indices
          indexS += 4;
        }
        indexS -= (file.width * 8); // set to begin of the column of next preceding row
      }
    } else
    {
      int indexD { 0 }; // index (targa destination data array)
      int indexS { imageSizeTmp - (file.width * 3) }; // index (the actual on disk targa image data)

      unsigned char alpha { 0 };

      for (int i = 0; i < file.height; i++)
      {
        for (int j = 0; j < file.width; j++)
        {
          data [indexD + 0] = temp [indexS + 0]; // Read
          data [indexD + 1] = temp [indexS + 2]; // Green
          data [indexD + 2] = temp [indexS + 1]; // Blue
          data [indexD + 3] = alpha; // Alpha

          indexD += 4;
          indexS += 3;
        }
        indexS -= (file.width * 6);
      }
    }

    delete [] temp;
    return true;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return false;
  }
};


// .tga loader specialization
//template<typename PngHeader>
bool Texture<PngHeader>::Load ( const char* path )
{
  //Todo research more interesting formats to add
  return false;
};


template<typename fileType>
ID3D11ShaderResourceView** const Texture<fileType>::getTexture ()
{
  return &textureView;
};


template<typename fileType>
void Texture<fileType>::release ()
{
  try
  {

    if (data)
      delete [] data;
    if (texture)
    {
      texture->Release ();
      texture = nullptr;
    }
    if (textureView)
    {
      textureView->Release ();
      textureView = nullptr;
    }

    device = nullptr;
    devCon = nullptr;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


void TextureClassLinker ( void ) // don't call this function: solution for linker error, when using templates.
{

  ID3D11Device* dev { nullptr };
  ID3D11DeviceContext* devCon { nullptr };
  Texture<TargaHeader> tempTex ( dev, devCon, "" );
  tempTex.getTexture ();
  tempTex.release ();

}

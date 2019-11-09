// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,06.11.2019</created>
/// <changed>ʆϒʅ,09.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Settings.h"
#include "Shared.h"


using namespace winrt::Windows::Storage;


Configurations::Configurations ( void ) :
  m_valid ( false )
{
  try
  {

    std::this_thread::sleep_for ( std::chrono::milliseconds { 30 } );


    // defaults initialization:
    m_defaults.Width = 640;
    m_defaults.Height = 480;
    m_defaults.fullscreen = false;

    // currents initialization:
    m_currents.Width = 0;
    m_currents.Height = 0;
    m_currents.fullscreen = false;


    StorageFolder folder ( ApplicationData::Current ().RoamingFolder () );
    m_path = folder.Path () + L"\\settings.lua";


    std::ifstream validation ( m_path );

    // settings file existence validation
    if (!validation.good ())
    {
      PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Retrieving the configuration file failed (Non-existent or invalid)!" );

      // rewrite the configuration file with defaults
      if (!m_apply ( m_defaults ))
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Rewriting the Configuration file using default settings failed." );
      }
    }
    validation.close ();


    while (!m_valid)
    {
      // read the configuration
      sol::state configs;
      try
      {
        configs.safe_script_file ( Converter::strConverter ( m_path ) ); // opening the configuration file
        // read the configuration or use the application defaults:
        m_currents.Width = configs ["configurations"]["resolution"]["width"].get_or ( m_currents.Width );
        // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
        m_currents.Height = configs ["configurations"]["resolution"]["height"].get_or ( m_currents.Height );
        unsigned int temp = configs ["configurations"]["display"]["fullscreen"].get_or ( temp );
        m_currents.fullscreen = temp;
      }
      catch (const std::exception & ex)
      {
        PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    ex.what () );
      }

      // validation
      if ((m_currents.Width != 0) && (!m_valid))
      {
        m_valid = true;
        PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                    "The configuration file is successfully read:\n\tResolution: (" +
                                                    std::to_string ( m_currents.Width ) + " x "
                                                    + std::to_string ( m_currents.Height ) + " )\t\t" +
                                                    "fullscreen: " + std::to_string ( m_currents.fullscreen ) );
      } else
      {
        // rewrite the configuration file with defaults
        if (!m_apply ( m_defaults ))
        {
          PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                      "Rewriting the Configuration file using default settings failed." );
        }
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
  }
};


//Configurations::~Configurations ( void )
//{
//
//};


const bool& Configurations::isValid ( void )
{
  return m_valid;
};


const Configs& Configurations::m_getDefaults ( void )
{
  return m_defaults;
};


const Configs& Configurations::m_getSettings ( void )
{
  return m_currents;
};


const bool Configurations::m_apply ( const Configs& object )
{
  try
  {

    std::ofstream writeStream ( m_path );
    if (writeStream.good ())
    {

      std::stringstream settingsLine;
      settingsLine << "configurations =\n\t{\n" <<
        "\t\tresolution = { width = " << std::to_string ( object.Width ) <<
        " , height = " << std::to_string ( object.Height ) << " },\n" <<
        "\t\tdisplay = { fullscreen = " << std::to_string ( object.fullscreen ) << " }\n\t}";
      writeStream << settingsLine.str ();
      writeStream.close ();

      PointerProvider::getFileLogger ()->m_push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The configuration file is successfully written:\n\tResolution: (" +
                                                  std::to_string ( object.Width ) + " x "
                                                  + std::to_string ( object.Height ) + " )\t\t" +
                                                  "fullscreen: " + std::to_string ( object.fullscreen ) );
      return true;

    } else
      return false;

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->m_push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                ex.what () );
    return false;
  }
};

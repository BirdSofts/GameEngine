// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,06.11.2019</created>
/// <changed>ʆϒʅ,07.11.2019</changed>
// ********************************************************************************

#include "pch.h"
#include "Settings.h"
#include "Shared.h"


using namespace winrt::Windows::Storage;


Configurations::Configurations ( void )
{
  try
  {

    valid = false;
    std::this_thread::sleep_for ( std::chrono::milliseconds { 30 } );

    // defaults initialization:
    defaults.Width = 640;
    defaults.Height = 480;
    defaults.fullscreen = false;

    // currents initialization:
    currents.Width = 0;
    currents.Height = 0;
    currents.fullscreen = false;

    StorageFolder folder ( ApplicationData::Current ().RoamingFolder () );
    path = folder.Path () + L"\\settings.lua";

    //!? temporary statement: development time path
    //path = { L"C:\\Users\\Mehrdad\\source\\repos\\GameEngine\\settings.lua" };
    //pathStr = { "./settings.lua" }; ///

    std::ifstream validation ( path );

    // settings file existence validation
    if (!validation.good ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                "Retrieving the configuration file failed (Non-existent or invalid)!" );

      // rewrite the configuration file with defaults
      if (!apply ( defaults ))
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  "Rewriting the Configuration file using default settings failed." );
      }
    }
    validation.close ();

    while (!valid)
    {
      // read the configuration
      sol::state configs;
      try
      {
        configs.safe_script_file ( Converter::strConverter ( path ) ); // opening the configuration file
        // read the configuration or use the application defaults:
        currents.Width = configs ["configurations"]["resolution"]["width"].get_or ( currents.Width );
        // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
        currents.Height = configs ["configurations"]["resolution"]["height"].get_or ( currents.Height );
        unsigned int temp = configs ["configurations"]["display"]["fullscreen"].get_or ( temp );
        currents.fullscreen = temp;
      }
      catch (const std::exception & ex)
      {
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                  ex.what () );
      }

      // validation
      if ((currents.Width != 0) && (!valid))
      {
        valid = true;
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
                                                  "The configuration file is successfully read:\n\tResolution: (" +
                                                  std::to_string ( currents.Width ) + " x "
                                                  + std::to_string ( currents.Height ) + " )\t\t" +
                                                  "fullscreen: " + std::to_string ( currents.fullscreen ) );
        break;
      } else
      {
        // rewrite the configuration file with defaults
        if (!apply ( defaults ))
        {
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                                    "Rewriting the Configuration file using default settings failed." );
        }
      }
    }

  }
  catch (const std::exception & ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
  }
};


const bool& Configurations::isValid ( void )
{
  return valid;
};


const ConfigsContainer& Configurations::getDefaults ( void )
{
  return defaults;
};


const ConfigsContainer& Configurations::getSettings ( void )
{
  return currents;
};


const bool Configurations::apply ( const ConfigsContainer& object )
{
  try
  {

    std::ofstream writeStream ( path );
    if (writeStream.good ())
    {
      std::stringstream settingsLine;
      settingsLine << "configurations =\n\t{\n" <<
        "\t\tresolution = { width = " << std::to_string ( object.Width ) <<
        " , height = " << std::to_string ( object.Height ) << " },\n" <<
        "\t\tdisplay = { fullscreen = " << std::to_string ( object.fullscreen ) << " }\n\t}";
      writeStream << settingsLine.str ();
      writeStream.close ();

      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), "mainThread",
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
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), "mainThread",
                                              ex.what () );
    return false;
  }
};

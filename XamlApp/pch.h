// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,27.06.2020</created>
/// <state></state>
// ********************************************************************************

#ifndef GAMEENGINE_H
#define GAMEENGINE_H


// C++ standard libraries
#include <string> // C++ standard string class
#include <locale> // C++ standard locals (facets of the below header is declared here)
#include <codecvt> // C++ standard Unicode conversion facets

#include <iostream> // C++ standard input and output streams
#include <sstream> // C++ standard string streams
#include <fstream> // C++ standard file streams

#include <exception> // C++ standard exception

#include <atomic> // C++ standard atomic objects (no data races)
#include <thread> // C++ standard threads
#include <mutex> // C++ standard lockable objects

#include <list> // C++ standard list container

#include <future> // C++ future header


// Windows dependencies
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
// WinRt/C++ dependencies (Xaml)
#include <winrt/base.h> // COM smart pointer
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Navigation.h>

#include <windows.ui.xaml.media.dxinterop.h>

// WinRt/C++ dependencies (Core)
//#include <winrt/Windows.ApplicationModel.Core.h>


// Windows dependencies
//#include <windows.h> // Windows standard APIs
#include <ShlObj.h> // Windows standard control APIs
#include <Shlwapi.h> // Windows standard lightweight utility APIs
#pragma comment(lib, "Shlwapi.lib")

//#include <PathCch.h> // Windows standard shell APIs
//#pragma comment(lib, "PathCch.lib")


// DirectX dependencies
//#include <wrl/client.h> COM smart pointer
//#include <wrl.h>
//#include <d3d10_1.h> // standard DirectX3D 10 APIs (setting up and 3D drawing)
#include <d3d11_4.h> // standard DirectX3D 11 APIs (setting up and 3D drawing) // 11_3
//#pragma comment (lib, "d3d11.lib") // linkage to the 'd3d10_1' library
#if defined(NTDDI_WIN10_RS2)
#include <dxgi1_6.h> // standard DXGI APIs (tools to interface with installed hardware) // 1_6
#else
#include <dxgi1_5.h>
#endif // !defined(NTDDI_WIN10_RS2)
//#pragma comment (lib, "dxgi.lib") // linkage to the 'dxgi' library

#include <d2d1_3.h> // standard DirectX2D APIs (setting up and 2D drawing)
//#pragma comment (lib, "d2d1.lib") // linkage to the 'd2d1' library
#include <dwrite_3.h> // standard DirectXWrite APIs
#pragma comment (lib, "dwrite.lib") // linkage to the 'dwrite' library

#include <DirectXMath.h> // standard DirectX3D mathematics APIs
#include <DirectXColors.h> // standard DirectX3D colours APIs

#include <d3dcompiler.h> // standard DirectX3D compiler APIs (shader compiler)
#pragma comment (lib, "d3dcompiler.lib") // linkage to the 'd3dcompiler' library

//#define _DEBUG_SEVERITY // deeper debugging

//#define _NOT_DEBUGGING // shipping time definition

#ifndef _NOT_DEBUGGING
#include <dxgidebug.h> // additional DirectX debug APIs
#endif // _NOT_DEBUGGING


// Third party dependencies
//#include <lua.h> // Lua scripting language engine header file
#include <sol/sol.hpp>
#define SOL_ALL_SAFETIES_ON 1
#pragma comment (lib, "lua.lib") // Lua scripting language engine library


#endif // !GAMEENGINE_H

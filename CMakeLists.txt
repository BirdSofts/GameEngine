# *******************************************************************************************
#/ <summary>
#/
#/ </summary>
#/ <created>ʆϒʅ,01.11.2019</created>
#/ <changed>ʆϒʅ,02.11.2019</changed>
# *******************************************************************************************

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET_NAME LearningDirectX)

set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME ${TARGET_NAME})
project(${PROJECT_NAME}
  VERSION 1.0.0.0
  DESCRIPTION "Game Engine"
  HOMEPAGE_URL "https://github.com/BirdSofts/GameEngine"
  LANGUAGES CXX
  )

# Note: ISSUE: lua and sol libraries linkage

find_package(lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)

# win32
#list(APPEND RESOURCES
#  ${TARGET_NAME}.rc
#  resource.h
#  )

list(APPEND HEADER_FILES
  Camera.h
  Core.h
  Direct2D.h
  Direct3D.h
  Game.h
  Light.h
  ModelFormats.h
  Polygons.h
  Shader.h
  Shared.h
  Texture.h
  Timer.h
  Universe.h
  Utilities.h
  Window.h
  )

list(APPEND SOURCE_FILES
  Main.cpp
  Camera.cpp
  Core.cpp
  Direct2D.cpp
  Direct3D.cpp
  Game.cpp
  Light.cpp
  ModelFormats.cpp
  Polygons.cpp
  Shader.cpp
  Shared.cpp
  Texture.cpp
  Timer.cpp
  Universe.cpp
  Utilities.cpp
  Window.cpp
  )

if(WIN32)
  list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Lua and Sol third party library inclusion and linkage
#set(LUA_ROOT_DIR "C:/Lua/x64")
#if(NOT LUA_ROOT_DIR)
#  message(FATAL_ERROR "Lua root path couldn't be found!")
#endif()
#set(INCLUDE_DIRECTORY ${LUA_ROOT_DIR}/include)
#set(LIBRARY_DIRECTORY ${LUA_ROOT_DIR})
#message(${INCLUDE_DIRECTORY})
#message(${LIBRARY_DIRECTORY})

#find_file(LUA_LIBRARY liblua53.a HINTS ${LIBRARY_DIRECTORY} PATH_SUFFIXES lib)
#if (NOT LUA_LIBRARY)
#  message(FATAL_ERROR "liblua53.a couldn't be found!")
#endif()

add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(${TARGET_NAME}
#  PUBLIC lua
  PRIVATE sol2::sol2
  )

#target_include_directories(${TARGET_NAME} BEFORE
#  PUBLIC ${INCLUDE_DIRECTORY}
#  )

#target_link_libraries(${TARGET_NAME}
#  PUBLIC ${LUA_LIBRARY}
#  )

set_target_properties(${TARGET_NAME} PROPERTIES
  LINK_FLAGS /SUBSYSTEM:WINDOWS
  )

#add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
#  COMMAND cmake -E copy_if_different
#  ${LUA_ROOT_DIR}/lua53.dll
#  $<TARGET_FILE_DIR:${TARGET_NAME}>
#  )

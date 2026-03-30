cmake_minimum_required(VERSION 3.25)

project(Opac VERSION 1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Setup
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Project Files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)

file(GLOB_RECURSE UI_FILES
    ${CMAKE_SOURCE_DIR}/ui/*.ui
)

file(GLOB_RECURSE RESOURCE_FILES
    ${CMAKE_SOURCE_DIR}/resources/*.qrc
)

# Executable
add_executable(Opac
    ${SOURCES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

target_include_directories(Opac PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Qt 
target_link_libraries(Opac PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# MySQL Connector
set(MYSQL_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/lib/mysql/include/jdbc
)

set(MYSQL_LIBRARY
    ${CMAKE_SOURCE_DIR}/lib/mysql/lib64/vs14/mysqlcppconn.lib
)

target_include_directories(Opac PRIVATE
    ${MYSQL_INCLUDE_DIR}
)

target_link_libraries(Opac PRIVATE
    ${MYSQL_LIBRARY}
)

# Runtime Files 
# MySQL DLL
set(MYSQL_DLL
    ${CMAKE_SOURCE_DIR}/lib/mysql/lib64/mysqlcppconn-10-vs14.dll
)

if(EXISTS ${MYSQL_DLL})
    add_custom_command(TARGET Opac POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MYSQL_DLL}
        $<TARGET_FILE_DIR:Opac>
    )
else()
    message(WARNING "MySQL DLL not found at ${MYSQL_DLL}")
endif()

# .env file
set(ENV_FILE ${CMAKE_SOURCE_DIR}/.env)

if(EXISTS ${ENV_FILE})
    add_custom_command(TARGET Opac POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ENV_FILE}
        $<TARGET_FILE_DIR:Opac>
    )
else()
    message(WARNING ".env file not found!")
endif()

# Qt Deployment
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET Opac POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                --no-translations
                $<TARGET_FILE:Opac>
        COMMENT "Running windeployqt..."
    )
endif()

# Debug Info
message(STATUS "Source Dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build Dir: ${CMAKE_BINARY_DIR}")
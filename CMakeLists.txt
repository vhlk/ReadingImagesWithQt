cmake_minimum_required (VERSION 3.16)

# Enable C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ReadingImagesQt" VERSION 1.0 DESCRIPTION "Aplication for reading image files" LANGUAGES CXX)

# Setup Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()

# include files
include_directories(ReadingImagesQt "include")

# src files
set(SOURCE_FILES
	"src/main/Main.cpp"
	"src/main/ImageViewer.cpp"
)

# header files
set(HEADER_FILES
	"include/ImageViewer.hpp"
	"include/ImageViewerDefinitions.hpp"
)

add_executable(ReadingImagesQt ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(ReadingImagesQt PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)
cmake_minimum_required(VERSION 3.30)
project(LangulusCT
   VERSION         1.0.0
   DESCRIPTION     "Langulus concept library"
   HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else 
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
   #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
   #include(LangulusUtilities.cmake)
   #fetch_langulus_module(Core GIT_TAG wip)
endif()

# Build and install SIMD library                                            
add_library(LangulusCT INTERFACE)

target_include_directories(LangulusCT
   INTERFACE   include
)

#target_link_libraries(LangulusCT
#   PUBLIC   LangulusCore
#            $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:fmt>
#)

#target_compile_definitions(LangulusSIMD
#   PRIVATE  LANGULUS_EXPORT_ALL
#)

if(LANGULUS_OPTION_TESTING)
   #enable_testing()
   #add_subdirectory(test)
endif()
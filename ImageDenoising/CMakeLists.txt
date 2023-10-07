# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.8)
 
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(EdgeDetection)


# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)



add_executable(TotalVariationDenoisingImage TotalVariationDenoisingImage.cxx)
target_link_libraries(TotalVariationDenoisingImage ${ITK_LIBRARIES})
target_include_directories(TotalVariationDenoisingImage PRIVATE ../MITK/Modules/ImageDenoising)


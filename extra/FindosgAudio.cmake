# - Find a vsgAudio installation or build tree.
# The following variables are set if osgWorks is found.  If vsgAudio is not
# found, vsgAudio_FOUND is set to false.
#  vsgAudio_FOUND         - Set to true when vsgAudio is found.
#  vsgAudio_USE_FILE      - CMake file to use vsgAudio.
#  vsgAudio_MAJOR_VERSION - The vsgAudio major version number.
#  vsgAudio_MINOR_VERSION - The vsgAudio minor version number 
#                       (odd non-release).
#  vsgAudio_BUILD_VERSION - The vsgAudio patch level 
#                       (meaningless for odd minor).
#  vsgAudio_INCLUDE_DIRS  - Include directories for vsgAudio
#  vsgAudio_LIBRARY_DIRS  - Link directories for vsgAudio libraries

# The following cache entries must be set by the user to locate osgWorks:
#  vsgAudio_DIR  - The directory containing vsgAudioConfig.cmake.  
#             This is either the root of the build tree,
#             or the lib directory.  This is the 
#             only cache entry.

# To use vsgAudio in a 3rd party project something like this can be used:
#
# ======================================================
# vsgAudio setup and dependency setup

# find_package( vsgAudio )
# if( vsgAudio_FOUND )
#    include(${vsgAudio_USE_FILE})
# endif()

# OpenAL/OpenAL++ audio subsystem support

# IF(ENABLE_SUBSYSTEM_OPENAL)
#     SET(ALUT_USE_AL_SUBDIR ON)
#     FIND_PACKAGE(ALUT)
#     if(ALUT_FOUND)
#         include_directories(${ALUT_INCLUDE_DIR})
#     endif(ALUT_FOUND)
#     FIND_PACKAGE(OpenAL)
#     if(OPENAL_FOUND)
#         include_directories(${OPENAL_INCLUDE_DIR})
#     endif(OPENAL_FOUND)
#     FIND_PACKAGE(OGG)
#     FIND_PACKAGE(Vorbis)
#     if(VORBIS_FOUND)
#         include_directories(${VORBIS_INCLUDE_DIR})
#     endif(VORBIS_FOUND)
#     FIND_PACKAGE(VorbisFile)
# ENDIF(ENABLE_SUBSYSTEM_OPENAL)

# FMOD Ex support

# IF(ENABLE_SUBSYSTEM_FMOD)
#     FIND_PACKAGE(FMOD)
#     if( FMOD_FOUND )
#         include_directories(${FMOD_INCLUDE_DIR})
#     endif(FMOD_FOUND)
# ENDIF(ENABLE_SUBSYSTEM_FMOD)

# ======================================================

# Assume not found.
SET(vsgAudio_FOUND 0)

# Construct consitent error messages for use below.
SET(vsgAudio_DIR_DESCRIPTION "directory containing vsgAudioConfig.cmake.  This is either the root of the build tree, or PREFIX/lib for an installation.")
SET(vsgAudio_DIR_MESSAGE "vsgAudio not found.  Set the vsgAudio_DIR cmake cache entry to the ${vsgAudio_DIR_DESCRIPTION}")

# Use the Config mode of the find_package() command to find vsgAudioConfig.
# If this succeeds (possibly because vsgAudio_DIR is already set), the
# command will have already loaded vsgAudioConfig.cmake and set osgWorks_FOUND.
IF(NOT vsgAudio_FOUND)
  FIND_PACKAGE(vsgAudio QUIET NO_MODULE)
ENDIF(NOT vsgAudio_FOUND)

#-----------------------------------------------------------------------------
IF(NOT vsgAudio_FOUND)
  # vsgAudio not found, explain to the user how to specify its location.
  IF(vsgAudio_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR ${vsgAudio_DIR_MESSAGE})
  ELSE(vsgAudio_FIND_REQUIRED)
    IF(NOT vsgAudio_FIND_QUIETLY)
      MESSAGE(STATUS ${vsgAudio_DIR_MESSAGE})
    ENDIF(NOT vsgAudio_FIND_QUIETLY)
  ENDIF(vsgAudio_FIND_REQUIRED)
ENDIF(NOT vsgAudio_FOUND)

# This is part of the Findosg* suite used to find OpenSceneGraph components.
# Each component is separate and you must opt in to each module. You must 
# also opt into OpenGL and OpenThreads (and Producer if needed) as these 
# modules won't do it for you. This is to allow you control over your own 
# system piece by piece in case you need to opt out of certain components
# or change the Find behavior for a particular module (perhaps because the
# default FindOpenGL.cmake module doesn't work with your system as an
# example).
# If you want to use a more convenient module that includes everything,
# use the FindOpenSceneGraph.cmake instead of the Findosg*.cmake modules.
# 
# Locate OSGFX
# This module defines
# OSGFX_LIBRARY
# OSGFX_FOUND, if false, do not try to link to OSGFX
# OSGFX_INCLUDE_DIR, where to find the headers
#
# $OSGDIR is an environment variable that would
# correspond to the ./configure --prefix=$OSGDIR
# used in building osg.
#

# Header files are presumed to be included like
# #include <osg/PositionAttitudeTransform>
# #include <osgFX/QWebViewImage>

# Try the user's environment request before anything else.
FIND_PATH(OSGFX_INCLUDE_DIR osgFX/Effect
  HINTS
  $ENV{OSGFX_DIR}
  $ENV{OSG_DIR}
  $ENV{OSGDIR}
  PATH_SUFFIXES include
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OpenThreads_ROOT]
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]
)

FIND_LIBRARY(OSGFX_LIBRARY 
  NAMES osgFX
  HINTS
  $ENV{OSGFX_DIR}
  $ENV{OSG_DIR}
  $ENV{OSGDIR}
  PATH_SUFFIXES lib64 lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

FIND_LIBRARY(OSGFX_LIBRARY_DEBUG 
  NAMES osgFXd
  HINTS
  $ENV{OSGFX_DIR}
  $ENV{OSG_DIR}
  $ENV{OSGDIR}
  PATH_SUFFIXES lib64 lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(OSGFX_FOUND "NO")
IF(OSGFX_LIBRARY AND OSGFX_INCLUDE_DIR)
  SET(OSGFX_FOUND "YES")
ENDIF(OSGFX_LIBRARY AND OSGFX_INCLUDE_DIR)

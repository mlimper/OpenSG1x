
####################
# WARNING WARNING
#
# HIGHLY IN PROGRESS
####################

INCLUDE(OpenSGVersion)

INCLUDE(OSGCommonConfig)

SET(OSG2_PROJECT TRUE CACHE INTERNAL "" FORCE)
SET(OSG_EXTERNAL_PROJECT TRUE CACHE INTERNAL "" FORCE)

MACRO(OSG_INIT)

  INCLUDE(OSGCommonOptions)

  MESSAGE(STATUS "ARGS: ${ARGC} | ${ARGV} ")
  MESSAGE(STATUS "OPT : ${OSG_OPTIONAL_COMPONENTS}")

  SET(_OSG_COMPONENTS ${ARGV})

  OSG_COMMON_OPTIONS()
  OSG_COMMON_OPTIONS_CONFIGURED()

  SET(OSG_PLATFORM_64 0)
  SET(OSG_PLATFORM_32 0)
  SET(OSG_LIBDIR_SUFFIX "")

  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(OSG_PLATFORM_64 1)
    SET(OSG_LIBDIR_SUFFIX "64")
    IF(NOT WIN32)
      SET(OSG_LIBDIR_BASE_SUFFIX "64")
    ENDIF()
  ELSE()
    SET(OSG_PLATFORM_32 1)
  ENDIF()

  IF(${CMAKE_CXX_PLATFORM_ID} STREQUAL "Linux")
    SET(LINUX 1)
  ENDIF()

  IF(WIN32)
    SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugOpt;ReleaseNoOpt"
                                  CACHE STRING "OpenSG Build Types" FORCE )
  ELSE()
    SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugGV"
                                  CACHE STRING "OpenSG Build Types" FORCE )
  ENDIF(WIN32)

  IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR
     CMAKE_BUILD_TYPE STREQUAL "DebugGV")
    SET(OSG_LIBDIR_SUFFIX "${OSG_LIBDIR_SUFFIX}/debug")
    SET(OSG_LIBDIR_BUILD_TYPE_SUFFIX "debug")
  ENDIF()

  IF(OSG_DISABLE_MICROSOFT_SECURE_CXXX)
    OPTION(OSG_DISABLE_MS_ITERATOR_DEBUGGING "" ON)
  ELSE(OSG_DISABLE_MICROSOFT_SECURE_CXXX)
    OPTION(OSG_DISABLE_MS_ITERATOR_DEBUGGING "" OFF)
  ENDIF(OSG_DISABLE_MICROSOFT_SECURE_CXXX)


  IF(NOT WIN32 AND NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
  ENDIF(NOT WIN32 AND NOT CMAKE_BUILD_TYPE)

  # Disable boost cmake config, as it screws up OpenSG's boost config

  SET(Boost_NO_BOOST_CMAKE TRUE CACHE INTERNAL "" FORCE)
  SET(Boost_ADDITIONAL_VERSIONS "1.54.0" 
                                "1.54"
                                "1.53.0" 
                                "1.53"
                                "1.52.0" 
                                "1.52"
                                "1.51.0" 
                                "1.51"
                                "1.50.0"
                                "1.50"
                                "1.49.0" 
                                "1.49"
                                "1.48.0"
                                "1.48"
                                "1.47.0"
                                "1.47.0")

  SET(OSG_BUILD_ACTIVE TRUE)
  ADD_DEFINITIONS(-D OSG_BUILD_ACTIVE)

  IF(NOT WIN32)
    SET(OSG_SUPPORT_INC_SUBDIR "/OpenSG")
  ENDIF()

  OSG_COMMON_TARGETS()

  INCLUDE(OpenSGExtDep)

  #############
  #### OpenSG

  LIST(FIND _OSG_COMPONENTS OSGFileIO _OSG_USE_FILEIO)
  LIST(FIND _OSG_COMPONENTS OSGImageFileIO _OSG_USE_IMAGEFILEIO)
  LIST(FIND _OSG_COMPONENTS OSGContribComputeBase _OSG_USE_CONTRIBCOMPUTE)
  LIST(FIND _OSG_COMPONENTS OSGContribCSM _OSG_USE_CONTRIBCSM)
  LIST(FIND _OSG_COMPONENTS OSGCluster _OSG_USE_CLUSTER)
  LIST(FIND _OSG_COMPONENTS OSGWindowQT4 _OSG_USE_QT)
  LIST(FIND _OSG_COMPONENTS OSGSystem _OSG_USE_SYSTEM)
  LIST(FIND _OSG_COMPONENTS OSGContribWebInterface _OSG_USE_WEBINTERFACE)

  IF(_OSG_USE_CONTRIBCOMPUTE EQUAL -1 AND NOT _OSG_USE_SYSTEM EQUAL -1)
    LIST(FIND OSGSystem_DEP_LIB CUDA_CUDART_LIBRARY _OSG_USE_CONTRIBCOMPUTE)
  ENDIF()

  IF(NOT _OSG_USE_CONTRIBCSM EQUAL -1)
    IF(_OSG_USE_FILEIO EQUAL -1)
      LIST(APPEND _OSG_COMPONENTS OSGFileIO)
      SET(_OSG_USE_FILEIO 1)
    ENDIF(_OSG_USE_FILEIO EQUAL -1)

    IF(_OSG_USE_CLUSTER EQUAL -1)
      LIST(APPEND _OSG_COMPONENTS OSGCluster)
      SET(_OSG_USE_CLUSTER 1)
    ENDIF(_OSG_USE_CLUSTER EQUAL -1)

    IF(_OSG_USE_WEBINTERFACE EQUAL -1)
      LIST(FIND OSGContribCSM_DEP_OSG_LIB OSGContribWebInterface 
                _OSG_USE_WEBINTERFACE)

      IF(NOT _OSG_USE_WEBINTERFACE EQUAL -1)
        LIST(APPEND _OSG_COMPONENTS OSGContribWebInterface)
      ENDIF()
    ENDIF()

  ENDIF(NOT _OSG_USE_CONTRIBCSM EQUAL -1)

  FIND_PACKAGE(OpenSG REQUIRED COMPONENTS ${_OSG_COMPONENTS})

  IF(OpenSG_DIR)
    IF(NOT OSG_COMPILER_DEFAULTS)
      INCLUDE(OSGSetupCompiler)
      SET(OSG_COMPILER_DEFAULTS 1 CACHE INTERNAL "Defaults written" FORCE ) #INTERNAL
    ENDIF(NOT OSG_COMPILER_DEFAULTS)

    INCLUDE(OSGUpdateCompiler)

    INCLUDE(OSGBuildFunctions)
    INCLUDE(OSGConfigurePackages)

    INCLUDE_DIRECTORIES(${OpenSG_INCLUDE_DIRS})

    OSG_CONFIGURE_SUPPORT()

  ENDIF(OpenSG_DIR)

  #############
  #### Support
  #############

  IF(UNIX)
    FIND_LIBRARY(OSG_THREAD_LIB NAMES pthread)
    FIND_LIBRARY(OSG_DL_LIB     NAMES dl)
    FIND_LIBRARY(OSG_RT_LIB     NAMES rt)

    OSG_ADD_OPT(OSG_THREAD_LIB)
    OSG_ADD_OPT(OSG_DL_LIB)
    OSG_ADD_OPT(OSG_RT_LIB)
  ENDIF(UNIX)

  IF(APPLE)
    FIND_LIBRARY(OSG_APPLICATIONSERVICES_FRAMEWORK NAMES ApplicationServices)
    FIND_LIBRARY(OSG_CARBON_FRAMEWORK NAMES Carbon)
    FIND_LIBRARY(OSG_COCOA_FRAMEWORK NAMES Cocoa)
  ENDIF(APPLE)

  #############
  #### OpenGL
  #############

  FIND_PACKAGE(OpenGL REQUIRED)

  OSG_ADD_OPT(OPENGL_gl_LIBRARY)
  OSG_ADD_OPT(OPENGL_glu_LIBRARY)

  #############
  #### X11
  #############

  FIND_PACKAGE(X11)

  IF(X11_FOUND)
    OSG_SET(OSG_X_LIBS ${X11_X11_LIB})
    OSG_SET(OSG_X_INC_DIR ${X11_X11_INCLUDE_PATH})
  ENDIF(X11_FOUND)

  OSG_ADD_OPT(X11_FOUND)

  #############
  #### GLUT
  #############

  OSG_CONFIGURE_GLUT()

  IF(GLUT_FOUND)
    OSG_SET(OSG_WITH_GLUT 1)
    OSG_SET(OSG_GLUT_INC_DIR ${GLUT_INCLUDE_DIR})
    OSG_SET(OSG_GLUT_LIBS ${GLUT_LIBRARIES})
  ELSE(GLUT_FOUND)
    OSG_SET(OSG_WITH_GLUT 0)
    OSG_SET(OSG_GLUT_INC_DIR "")
    OSG_SET(OSG_GLUT_LIBS "")
  ENDIF(GLUT_FOUND)

  #############
  #### Python
  #############

  FIND_PACKAGE(PythonInterp)

  ##########
  #### boost
  ##########

  OSG_CONFIGURE_BOOST()
  OSG_CONFIGURE_ZLIB()

  MESSAGE(STATUS "Need fileio deps    : ${_OSG_USE_FILEIO}")
  MESSAGE(STATUS "Need imgfileio deps : ${_OSG_USE_IMAGEFILEIO}")
  MESSAGE(STATUS "Need cuda deps      : ${_OSG_USE_CONTRIBCOMPUTE}")
  MESSAGE(STATUS "Need cluster        : ${_OSG_USE_CLUSTER}")
  MESSAGE(STATUS "Need csm            : ${_OSG_USE_CONTRIBCSM}")
  MESSAGE(STATUS "Need qt             : ${_OSG_USE_QT}")

  IF(NOT _OSG_USE_IMAGEFILEIO EQUAL -1)

    MESSAGE(STATUS "Configure image file io")

    OSG_CONFIGURE_TIFF()
    OSG_CONFIGURE_PNG()
    OSG_CONFIGURE_JPEG()
    OSG_CONFIGURE_OPENEXR()

    IF(TIFF_FOUND)
      SET(OSG_EXTERNAL_IMAGELIBS ${OSG_EXTERNAL_IMAGELIBS} TIFF_LIBRARIES)
      SET(OSG_EXTERNAL_IMAGEINCS ${OSG_EXTERNAL_IMAGEINCS} TIFF_INCLUDE_DIR)
      OSG_SET(OSG_WITH_TIF 1)
      OSG_ADD_OPT(TIFF_LIBRARIES)
    ENDIF(TIFF_FOUND)

    IF(PNG_FOUND)
      SET(OSG_EXTERNAL_IMAGELIBS ${OSG_EXTERNAL_IMAGELIBS} PNG_LIBRARIES)
      SET(OSG_EXTERNAL_IMAGEINCS ${OSG_EXTERNAL_IMAGEINCS} PNG_INCLUDE_DIR)
      OSG_SET(OSG_WITH_PNG 1)
      OSG_ADD_OPT(PNG_INCLUDE_DIR)
      OSG_ADD_OPT(PNG_LIBRARIES)
    ENDIF(PNG_FOUND)

    IF(JPEG_FOUND)
      SET(OSG_EXTERNAL_IMAGELIBS ${OSG_EXTERNAL_IMAGELIBS} JPEG_LIBRARIES)
      SET(OSG_EXTERNAL_IMAGEINCS ${OSG_EXTERNAL_IMAGEINCS} JPEG_INCLUDE_DIR)
      OSG_SET(OSG_WITH_JPG 1)
      OSG_ADD_OPT(JPEG_LIBRARIES)
    ENDIF(JPEG_FOUND)

    IF(GDAL_FOUND)
      SET(OSG_EXTERNAL_IMAGELIBS ${OSG_EXTERNAL_IMAGELIBS} GDAL_LIBRARIES)
      SET(OSG_EXTERNAL_IMAGEINCS ${OSG_EXTERNAL_IMAGEINCS} GDAL_INCLUDE_DIR)
      OSG_SET(OSG_WITH_GDAL 1)
      OSG_ADD_OPT(GDAL_LIBRARIES)
    ENDIF(GDAL_FOUND)

    IF(OPENEXR_FOUND)
      SET(OSG_EXTERNAL_IMAGELIBS ${OSG_EXTERNAL_IMAGELIBS} OPENEXR_LIBRARIES)
      SET(OSG_EXTERNAL_IMAGEINCS ${OSG_EXTERNAL_IMAGEINCS} OPENEXR_INCLUDE_DIR)
      OSG_SET(OSG_WITH_IMF 1)

      OSG_ADD_OPT(OPENEXR_LIBRARIES)
      OSG_ADD_OPT(OPENEXR_INCLUDE_DIR)
    ENDIF(OPENEXR_FOUND)

  ENDIF(NOT _OSG_USE_IMAGEFILEIO EQUAL -1)

  IF(NOT _OSG_USE_FILEIO EQUAL -1)

    MESSAGE(STATUS "Configure file io")

    OSG_CONFIGURE_COLLADA()

    IF(COLLADA_FOUND)

      IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR 
         CMAKE_BUILD_TYPE STREQUAL "DebugOpt")

        OSG_SET(OSG_COLLADA_LIBS ${COLLADA_LIBRARY_DEBUG})

      ELSE()

        OSG_SET(OSG_COLLADA_LIBS ${COLLADA_LIBRARY_RELEASE})

      ENDIF()

      IF(WIN32)
        SET(OSG_COLLADA_LIBS ${OSG_COLLADA_TARGETS})
      ENDIF(WIN32)

    ELSE(COLLADA_FOUND)

        SET(OSG_COLLADA_LIBS "")

    ENDIF(COLLADA_FOUND)
  ENDIF(NOT _OSG_USE_FILEIO EQUAL -1)

  IF(NOT _OSG_USE_CONTRIBCOMPUTE EQUAL -1) # AND OSG_ENABLE_CUDA)

    MESSAGE(STATUS "Configure cuda deps")

    FIND_PACKAGE(CUDA QUIET REQUIRED)

    INCLUDE(CudaHelper)

    OSG_SETUP_CUDA_FLAGS()

    INCLUDE(ConfigurePackages.OSGContribComputeBase)

    IF(NOT OSG_BUILD_DEPENDEND)
      OSG_CONFIGURE_NVSDKCOMMON()
      OSG_CONFIGURE_NVOCLUTILS()
      OSG_CONFIGURE_CUDACOMMON()
      OSG_CONFIGURE_CUDAUTIL()
#      OSG_CONFIGURE_CUDPP()
      OSG_CONFIGURE_CUDAPARTSDK(${OSG_SUPPORT_ROOT})
      
      IF(NOT OSG_WITH_CUDAPARTSDK AND OSG_EXT_SUPPORT_ROOT)
        OSG_CONFIGURE_CUDAPARTSDK(${OSG_EXT_SUPPORT_ROOT})
      ENDIF()
    ENDIF(NOT OSG_BUILD_DEPENDEND)

    MESSAGE(STATUS "NV Status: NV:${OSG_NVSDKCOMMON_FOUND}")
    MESSAGE(STATUS "Cuda Status: CC:${OSG_CUDACOMMON_FOUND} CU:${OSG_CUDAUTIL_FOUND}")
    MESSAGE(STATUS "OCL Status : NV:${OSG_NVOCLUTILS_FOUND}")

    OSG_COMMON_CUDA()

  ENDIF(NOT _OSG_USE_CONTRIBCOMPUTE EQUAL -1) # AND OSG_ENABLE_CUDA)

  IF(NOT _OSG_USE_QT EQUAL -1)

    #############
    #### QT
    #############

    IF(WIN32)
      IF(EXISTS ${OSG_SUPPORT_ROOT}/qt5)
        SET(QT5_CMAKE_DIR "${OSG_SUPPORT_ROOT}/qt5/lib/cmake" CACHE PATH "" FORCE)
      ELSEIF(EXISTS ${OSG_SUPPORT_ROOT}/qt4)
        SET(QT_QMAKE_EXECUTABLE "${OSG_SUPPORT_ROOT}/qt4/bin/qmake.exe" CACHE FILEPATH "" FORCE)
      ENDIF()
    ENDIF()

    OSG_CONFIGURE_QT()
  ENDIF()

  #############
  #### Python
  #############

  FIND_PACKAGE(PythonLibs)

  OSG_ADD_OPT(PYTHON_INCLUDE_PATH)
  OSG_ADD_OPT(PYTHON_LIBRARY)

  IF(Boost_FOUND AND PYTHONLIBS_FOUND)

    FIND_PACKAGE(Boost COMPONENTS python)

    IF(Boost_FOUND AND WIN32)

      SET(OSG_BOOST_PYTHON_TARGETS )
      OSG_ADD_IMPORT_LIB(OSG_BOOST_PYTHON_TARGETS Boost_PYTHON_LIBRARY)
      SET(OSG_BOOST_PYTHON_LIBS ${OSG_BOOST_PYTHON_TARGETS})
      
    ELSE(Boost_FOUND AND WIN32)

      IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR 
         CMAKE_BUILD_TYPE STREQUAL "DebugOpt")

        SET(OSG_BOOST_PYTHON_LIBS ${Boost_PYTHON_LIBRARY_DEBUG})

      ELSE()
        SET(OSG_BOOST_PYTHON_LIBS ${Boost_PYTHON_LIBRARY_RELEASE})
      ENDIF()

    ENDIF(Boost_FOUND AND WIN32)

    # We ignore if boost python fails, we catch that somewhere else
    # so if we get here Boost_FOUND was true in the first place
    SET(Boost_FOUND TRUE)

  ENDIF(Boost_FOUND AND PYTHONLIBS_FOUND)


  CHECK_BUILD_DIR()

  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


  IF(WIN32)
    OSG_OPTION(OSG_USE_SEPARATE_LIBDIRS "" ON)
  ENDIF(WIN32)

  IF(OSGEXCLUDE_TESTS_FROM_ALL)
    SET(OSGEXCLUDE_TESTS EXCLUDE_FROM_ALL)
  ELSE()
    SET(OSGEXCLUDE_TESTS "")
  ENDIF()

  IF(OSGEXCLUDE_UNITTESTS_FROM_ALL)
    SET(OSGEXCLUDE_UNITTESTS EXCLUDE_FROM_ALL)
  ELSE()
    SET(OSGEXCLUDE_UNITTESTS "")
  ENDIF()

  IF(OSGEXCLUDE_EXAMPLES_SIMPLE_FROM_ALL)
    SET(OSGEXCLUDE_EXAMPLES_SIMPLE EXCLUDE_FROM_ALL)
  ELSE()
    SET(OSGEXCLUDE_EXAMPLES_SIMPLE "")
  ENDIF()

  IF(OSGEXCLUDE_EXAMPLES_ADVANCED_FROM_ALL)
    SET(OSGEXCLUDE_EXAMPLES_ADVANCED EXCLUDE_FROM_ALL)
  ELSE()
    SET(OSGEXCLUDE_EXAMPLES_ADVANCED "")
  ENDIF()

  INCLUDE(OSGSetupPython)

  MESSAGE("FOOOO ${CMAKE_PROJECT_NAME} ${CMAKE_BINARY_DIR}")

  FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cmake)

  STRING(TOUPPER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_UP)

  STRING(REPLACE "OpenSG" "OSG" CMAKE_PROJECT_NAME_SHORT ${CMAKE_PROJECT_NAME})
  STRING(TOUPPER ${CMAKE_PROJECT_NAME_SHORT} CMAKE_PROJECT_NAME_SHORT_UP)

  CONFIGURE_FILE(${OpenSG_DIR}/share/OpenSG/cmake/FindOpenSG.cmake.in
                 ${CMAKE_BINARY_DIR}/cmake/Find${CMAKE_PROJECT_NAME}.cmake
                 @ONLY                                      )

  INSTALL(FILES "${CMAKE_BINARY_DIR}/cmake/Find${CMAKE_PROJECT_NAME}.cmake"
          DESTINATION share/OpenSG/cmake)
#          COMPONENT development_configuration)

  OSG_SET_CACHE(OSG_EXTRA_EXTERNAL_MODULES "" STRING
                "Extra modules that OpenSG will try to import.")

  OSG_SET_CACHE(OSG_EXTERNAL_MODULES_INCLUDE "" FILEPATH 
                "Include file of extra modules")

  IF(EXISTS "${OSG_EXTERNAL_MODULES_INCLUDE}")
    INCLUDE("${OSG_EXTERNAL_MODULES_INCLUDE}")
  ENDIF()

  FOREACH(EXTERNAL ${OSG_EXTRA_EXTERNAL_MODULES})
    OPTION(OSGBUILD_${EXTERNAL}
           "Build using ${EXTERNAL} module" OFF)

    IF(OSGBUILD_${EXTERNAL})
      FIND_PATH(OSG_${EXTERNAL}_SOURCE_DIR
                CMakeLists.Lib.${EXTERNAL}.txt
                ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL})

      OSG_ADD_OPT(OSG_${EXTERNAL}_SOURCE_DIR)

      IF(NOT EXISTS
            ${OSG_${EXTERNAL}_SOURCE_DIR}/CMakeLists.Lib.${EXTERNAL}.txt)

        MESSAGE(SEND_ERROR "Cannot find ${EXTERNAL} sources directory: "
                           "${OSG_${EXTERNAL}_SOURCE_DIR}")              
      ELSE()
        LIST(APPEND OSG_EXTERNAL_CONFIGS "${EXTERNAL}")

        GET_FILENAME_COMPONENT(_EXT_SRC_DIR
                               "${OSG_${EXTERNAL}_SOURCE_DIR}/.."
                               REALPATH                         )

        IF(EXISTS "${OSG_${EXTERNAL}_SOURCE_DIR}/CMakeLists.PackagePrepare.cmake")
          LIST(APPEND OSG_EXTERNAL_PREPARE "${OSG_${EXTERNAL}_SOURCE_DIR}")
        ENDIF()

        IF(EXISTS "${_EXT_SRC_DIR}/CMakeLists.PackagePrepare.cmake")
          LIST(APPEND OSG_EXTERNAL_PREPARE "${_EXT_SRC_DIR}")
        ENDIF()
      ENDIF()
    ENDIF()
  ENDFOREACH(EXTERNAL)

IF(OSG_EXTERNAL_PREPARE)
  LIST(REMOVE_DUPLICATES OSG_EXTERNAL_PREPARE)
ENDIF()
ENDMACRO(OSG_INIT)


MACRO(_OSG_SETUP_BUILD)

  # when adding passes, make sure to add a corresponding pass directory variable
  # OSG_PASSDIR_${PASSNAME} - this variable may not be empty!
  SET(OSG_CMAKE_PASSES "OSGCOLLECT" "OSGSETUP")

  SET(OSG_PASSDIR_OSGCOLLECT "Collect")
  SET(OSG_PASSDIR_OSGSETUP   "Build")

  ###############
  # Clean
  ###############

  FILE(GLOB OSG_OLD_BUILD_FILES  "${CMAKE_BINARY_DIR}/*.cmake")

  IF(OSG_OLD_BUILD_FILES)
    FILE(REMOVE ${OSG_OLD_BUILD_FILES})
  ENDIF(OSG_OLD_BUILD_FILES)

#  FOREACH(_OSG_COMPONENT ${STORED_PROJECTS})
  FOREACH(_OSG_COMPONENT ${_OSG_COMPONENTS})
    MESSAGE(STATUS "FAKE : ${CMAKE_BINARY_DIR}/${_OSG_COMPONENT}.cmake")
    FILE(WRITE ${CMAKE_BINARY_DIR}/${_OSG_COMPONENT}.cmake "#dummy")

    SET(_OSG_COMPONENT_LI )

    FOREACH(_OSG_COMPONENT_DEP ${${_OSG_COMPONENT}_DEP_OSG_LIB})
      LIST(APPEND _OSG_COMPONENT_LI ${_OSG_COMPONENT_DEP})
    ENDFOREACH(_OSG_COMPONENT_DEP ${${_OSG_COMPONENT}_DEP_OSG_LIB})

    FOREACH(_OSG_COMPONENT_DEP ${${_OSG_COMPONENT}_DEP_LIB})
      IF(TARGET ${_OSG_COMPONENT_DEP})
        LIST(APPEND _OSG_COMPONENT_LI ${_OSG_COMPONENT_DEP})
      ELSE(TARGET ${_OSG_COMPONENT_DEP})
        LIST(APPEND _OSG_COMPONENT_LI ${${_OSG_COMPONENT_DEP}})
      ENDIF(TARGET ${_OSG_COMPONENT_DEP})
    ENDFOREACH(_OSG_COMPONENT_DEP ${${_OSG_COMPONENT}_DEP_LIB})

    IF(_OSG_COMPONENT_LI AND TARGET ${_OSG_COMPONENT})

      SET_TARGET_PROPERTIES(${_OSG_COMPONENT} PROPERTIES
                            IMPORTED_LINK_INTERFACE_LIBRARIES "${_OSG_COMPONENT_LI}")
    ENDIF(_OSG_COMPONENT_LI AND TARGET ${_OSG_COMPONENT})

    IF(EXISTS "${OSG_PYTHON_MODULE_BASE_DIR}/${_OSG_COMPONENT}/module")
        SET(OSG_PYTHON_${_OSG_COMPONENT}_MODULE_DIR "${OSG_PYTHON_MODULE_BASE_DIR}/${_OSG_COMPONENT}/module" CACHE INTERNAL "" FORCE)
    ENDIF()

  ENDFOREACH(_OSG_COMPONENT ${STORE_PROJECTS})

  ###############
  # Tests
  ###############

  # optional pass for test programs
  IF(OSGBUILD_TESTS)
    LIST(APPEND OSG_CMAKE_PASSES "OSGSETUPTEST")
    SET(OSG_PASSDIR_OSGSETUPTEST "Test")
  ENDIF()

  OSG_OPTION(OSG_ENABLE_FCD2CODE "" ON)
  OSG_OPTION(OSG_FCD2CODE_WRITE_CLASS "" OFF)
  OSG_OPTION(OSGBUILD_PYTHON_BINDINGS "Build python bindings" ON)

  SET(OSG_MAIN_LIB_TARGET OSGAllContribLibs)


  ###############
  # Python
  ###############

  SETUP_PYTHON_BUILD()

  ###############
  #### Doxygen
  ###############

  IF(EXISTS "${OSG_SUPPORT_ROOT}/bin/doxygen" AND NOT DOXYGEN_EXECUTABLE)
    SET(DOXYGEN_EXECUTABLE "${OSG_SUPPORT_ROOT}/bin/doxygen" 
                           CACHE FILEPATH "doxygen executable (custom)" FORCE)
  ENDIF()

  FIND_PACKAGE(Doxygen)

#  IF(NOT OSG_DOXY_INPUTS)
    # add a pass for doxygen to run
    LIST(APPEND OSG_CMAKE_PASSES "OSGDOXYDOC")
    SET(OSG_PASSDIR_OSGDOXYDOC "Doc/build")
#  ENDIF(NOT OSG_DOXY_INPUTS)

  # set up variables for the config file
  IF(DOXYGEN_DOT_EXECUTABLE)
    SET(OSG_DOXY_HAVE_DOT "YES")
  ENDIF(DOXYGEN_DOT_EXECUTABLE)

ENDMACRO(_OSG_SETUP_BUILD)

MACRO(_OSG_RUN_PASSES)

  FOREACH(LIBCONFIGFILE ${OSG_LIBRARY_CONFIG_FILES})
    GET_FILENAME_COMPONENT(LIBCONFIGDIR "${LIBCONFIGFILE}" PATH)
    GET_FILENAME_COMPONENT(LIBFILENAME  "${LIBCONFIGFILE}" NAME)
    STRING(REPLACE "CMakeLists.Lib." "" LIBFILENAME "${LIBFILENAME}")
    STRING(REPLACE ".txt"            "" LIBFILENAME "${LIBFILENAME}")

    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBCONFIGDIR}/${LIBFILENAME}.Prepare.cmake)
      INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/${LIBCONFIGDIR}/${LIBFILENAME}.Prepare.cmake)
    ENDIF()
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBCONFIGDIR}/${LIBFILENAME}.PrePasses.cmake)
      INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/${LIBCONFIGDIR}/${LIBFILENAME}.PrePasses.cmake)
    ENDIF()
  ENDFOREACH(LIBCONFIGFILE)

  FOREACH(EXTERNAL_PREPARE ${OSG_EXTERNAL_PREPARE})
    INCLUDE("${EXTERNAL_PREPARE}/CMakeLists.PackagePrepare.cmake")
  ENDFOREACH()

  FOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})
    IF(EXISTS ${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.Prepare.cmake)
      INCLUDE(${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.Prepare.cmake)
    ENDIF(EXISTS
    ${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.Prepare.cmake)
  ENDFOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})

  FOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})
    IF(EXISTS ${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.PrePasses.cmake)
      INCLUDE(${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.PrePasses.cmake)
    ENDIF(EXISTS ${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.PrePasses.cmake)
  ENDFOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})

  # run build passes
  FOREACH(PASS ${OSG_CMAKE_PASSES})
    SET(OSG_CMAKE_PASS ${PASS})

    MESSAGE(STATUS "\nPASS : ${OSG_CMAKE_PASS} in ${OSG_PASSDIR_${PASS}}\n")

    FOREACH(LIBCONFIGFILE ${OSG_LIBRARY_CONFIG_FILES})
        GET_FILENAME_COMPONENT(LIBCONFIGDIR "${LIBCONFIGFILE}" PATH)
        GET_FILENAME_COMPONENT(LIBFILENAME  "${LIBCONFIGFILE}" NAME)
        STRING(REPLACE "CMakeLists.Lib." "" LIBFILENAME "${LIBFILENAME}")
        STRING(REPLACE ".txt"            "" LIBFILENAME "${LIBFILENAME}")

        ADD_SUBDIRECTORY("${LIBCONFIGDIR}" "${OSG_PASSDIR_${PASS}}/${LIBFILENAME}")

    ENDFOREACH(LIBCONFIGFILE)

    FOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})
        ADD_SUBDIRECTORY("${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}"
                         "External/${EXTERNAL_CONFIG}/${OSG_PASSDIR_${PASS}}")
    ENDFOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})
   
  ENDFOREACH()

  IF(WIN32)
    SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugOpt;ReleaseNoOpt"
                                  CACHE STRING "OpenSG Build Types" FORCE )
  ELSE()
    SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugGV"
                                  CACHE STRING "OpenSG Build Types" FORCE )
  ENDIF(WIN32)

  EXECUTE_PROCESS(
      COMMAND "${CMAKE_COMMAND}" -E copy
                       "${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME_SHORT}Configured.h.cmake"
                       "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp.cmake")

  FOREACH(_OSG_ADD_CONFIGURE_LINE ${${CMAKE_PROJECT_NAME_UP}_ADDITIONAL_CONFIGURE_LINES})
    FILE(APPEND "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp.cmake" "\n${_OSG_ADD_CONFIGURE_LINE}\n")
  ENDFOREACH()

  FILE(APPEND "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp.cmake" "\n#endif // _${CMAKE_PROJECT_NAME_SHORT_UP}CONFIGURED_H_\n")

  CONFIGURE_FILE("${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp.cmake"
                 "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp")

  EXECUTE_PROCESS(
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                       "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h.tmp" 
                       "${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h")

  INSTALL(FILES ${CMAKE_BINARY_DIR}/Source/Base/Base/${CMAKE_PROJECT_NAME_SHORT}Configured.h
          DESTINATION include/OpenSG
          PERMISSIONS OWNER_WRITE OWNER_READ
                      GROUP_READ
                      WORLD_READ)

  FOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})
    IF(EXISTS ${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.Finalize.cmake)
      INCLUDE(${OSG_${EXTERNAL_CONFIG}_SOURCE_DIR}/${EXTERNAL_CONFIG}.Finalize.cmake)
    ENDIF()
  ENDFOREACH(EXTERNAL_CONFIG ${OSG_EXTERNAL_CONFIGS})

ENDMACRO(_OSG_RUN_PASSES)

MACRO(OSG_COLLECT_LIBS CURRENT_DIR_ONLY)

  _OSG_SETUP_BUILD()

  ###############
  # Passes
  ###############

  SET(_OSG_CURRDIR_ONLY ${CURRENT_DIR_ONLY})

  # Find files describing libraries to build, but make sure base
  # and system are processed first

  IF(_OSG_CURRDIR_ONLY)
    FILE(GLOB OSG_LIBRARY_CONFIG_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
         "./*CMakeLists.Lib.*.txt")
  ELSE()
    FILE(GLOB_RECURSE OSG_LIBRARY_CONFIG_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
         "./*CMakeLists.Lib.*.txt")
  ENDIF()

MESSAGE("FOOO ${OSG_LIBRARY_CONFIG_FILES}")

  IF(OSG_EXCLUDE_FROM_BUILD)
    LIST(REMOVE_ITEM OSG_LIBRARY_CONFIG_FILES ${OSG_EXCLUDE_FROM_BUILD})
  ENDIF()

  _OSG_RUN_PASSES()

ENDMACRO(OSG_COLLECT_LIBS CURRENT_DIR_ONLY)

MACRO(OSG_DOCOLLECT_LIBS)

  _OSG_SETUP_BUILD()

  ###############
  # Passes
  ###############

  SET(_OSG_CURRDIR_ONLY ${CURRENT_DIR_ONLY})

  SET(_OSG_TMPLIST ${ARGV})
  MATH(EXPR _OSG_LISTCOUNT ${ARGC}-1)

  # Find files describing libraries to build, but make sure base
  # and system are processed first

  SET(_OSG_TMPLIST ${ARGV})
  MATH(EXPR _OSG_LISTCOUNT ${ARGC}-1)

  FOREACH(_LOOPC RANGE 0 ${_OSG_LISTCOUNT} 2)

    MATH(EXPR _LOOPN ${_LOOPC}+1)

    LIST(GET _OSG_TMPLIST ${_LOOPC} OSGLIB_SOURCE_DIR)
    LIST(GET _OSG_TMPLIST ${_LOOPN} _OSG_CURRDIR_ONLY)

    IF(_OSG_CURRDIR_ONLY)
      FILE(GLOB _OSG_LIBRARY_CONFIG_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
           "${${OSGLIB_SOURCE_DIR}}/*CMakeLists.Lib.*.txt")
    ELSE()
      FILE(GLOB_RECURSE _OSG_LIBRARY_CONFIG_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
           "${${OSGLIB_SOURCE_DIR}}/*CMakeLists.Lib.*.txt")
    ENDIF()

    LIST(APPEND OSG_LIBRARY_CONFIG_FILES ${_OSG_LIBRARY_CONFIG_FILES})

    IF(EXISTS ${${OSGLIB_SOURCE_DIR}}/CMakeLists.PackagePrepare.cmake)
      INCLUDE(${${OSGLIB_SOURCE_DIR}}/CMakeLists.PackagePrepare.cmake)
    ENDIF()

  ENDFOREACH()

  IF(OSG_EXCLUDE_FROM_BUILD)
    LIST(REMOVE_ITEM OSG_LIBRARY_CONFIG_FILES ${OSG_EXCLUDE_FROM_BUILD})
  ENDIF()

  _OSG_RUN_PASSES()

ENDMACRO(OSG_DOCOLLECT_LIBS)
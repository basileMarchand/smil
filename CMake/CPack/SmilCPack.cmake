# Copyright (c) 2011, Matthieu FAESSEL and ARMINES
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FILE(GLOB NATIVE_LIB_DEPENDENCIES ${CMAKE_CURRENT_BINARY_DIR}/depends/*)

IF(NATIVE_LIB_DEPENDENCIES)
      INSTALL(FILES ${NATIVE_LIB_DEPENDENCIES} DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT base)
      SET(SMIL_INSTALL_COMPONENT_LIST base ${SMIL_INSTALL_COMPONENT_LIST})
ENDIF(NATIVE_LIB_DEPENDENCIES)


INCLUDE(InstallRequiredSystemLibraries)


SET(CPACK_PACKAGE_NAME "Smil")
SET(CPACK_PACKAGE_CONTACT "matthieu.faessel@mines-paristech.fr")

IF(WIN32)
      SET(DEFAULT_GENERATOR NSIS)
      IF(USE_64BIT_IDS)
	      SET(CPACK_SYSTEM_NAME win64)
      ELSE(USE_64BIT_IDS)
	      SET(CPACK_SYSTEM_NAME win32)
      ENDIF(USE_64BIT_IDS)
ELSE(WIN32)
      IF(CMAKE_CROSSCOMPILING)
	    SET(DEFAULT_GENERATOR TGZ)
      ELSE(CMAKE_CROSSCOMPILING)
	    IF(EXISTS "/etc/lsb-release")
		    SET(DEFAULT_GENERATOR DEB)
	    ELSEIF(EXISTS "/etc/redhat-release")
		    SET(DEFAULT_GENERATOR RPM)
	    ENDIF(EXISTS "/etc/lsb-release")
	    
	    IF(USE_64BIT_IDS)
		    SET(CPACK_SYSTEM_NAME amd64)
	    ELSE(USE_64BIT_IDS)
		    SET(CPACK_SYSTEM_NAME i386)
	    ENDIF(USE_64BIT_IDS)
      ENDIF(CMAKE_CROSSCOMPILING)
ENDIF(WIN32)

SET(CPACK_GENERATOR ${DEFAULT_GENERATOR} CACHE STRING "CPack generator type (DEB, RPM, NSIS, TGZ, TBZ2, ZIP, ...)")


SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple Morphological Image Library")
SET(CPACK_PACKAGE_VENDOR "Matthieu Faessel and ARMINES")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
SET(CPACK_PACKAGE_VERSION ${SMIL_VERSION})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Smil")

SET(CPACK_SOURCE_IGNORE_FILES ".kdev*;.git;notes.txt;todo.txt")


SET(CPACK_COMPONENTS_ALL)

FOREACH(COMP ${SMIL_INSTALL_COMPONENT_LIST})
	LIST_CONTAINS(IS_IN_LIST ${COMP} ${CPACK_COMPONENTS_ALL})
	IF(NOT IS_IN_LIST)
		LIST(APPEND CPACK_COMPONENTS_ALL ${COMP})
		
		STRING(REGEX REPLACE "-.*$" "" COMP_PREFIX ${COMP})
		LIST_CONTAINS(IS_ADDON ${COMP_PREFIX} ${AVAILABLE_ADDONS})
		IF(IS_ADDON)
			STRING(TOUPPER ${COMP} COMP_UP)
			STRING(REGEX REPLACE ".*-" "" COMP_SUFFIX ${COMP})
			SET(CPACK_COMPONENT_${COMP_UP}_DEPENDS ${COMP_SUFFIX})
		ENDIF(IS_ADDON)
	ENDIF(NOT IS_IN_LIST)
ENDFOREACH(COMP ${SMIL_COMPONENT_LIST})


SET(CPACK_COMPONENT_BASE_DISPLAY_NAME "Smil base libraries")
SET(CPACK_COMPONENT_PYTHON_DISPLAY_NAME "Smil python libraries")
SET(CPACK_COMPONENT_JAVA_DISPLAY_NAME "Smil java libraries")

# SET(CPACK_COMPONENTS_IGNORE_GROUPS TRUE)

IF(BUILD_SHARED_LIBS OR NATIVE_LIB_DEPENDENCIES)
	SET(CPACK_COMPONENT_BASE_REQUIRED TRUE)
	SET(CPACK_COMPONENT_PYTHON_DEPENDS base)
	SET(CPACK_COMPONENT_JAVA_DEPENDS base)
ENDIF(BUILD_SHARED_LIBS OR NATIVE_LIB_DEPENDENCIES)


#### NSIS ####
IF(CPACK_GENERATOR MATCHES "NSIS")
      #       SET(CPACK_GENERATOR NSIS)
      SET(CPACK_NSIS_COMPONENT_INSTALL ON)
      SET(CPACK_NSIS_PACKAGE_COMPONENT TRUE)
      SET(CPACK_NSIS_DISPLAY_NAME "Smil")
      #       SET(CPACK_TOPLEVEL_DIRECTORY "${CMAKE_BINARY_DIR}/_CPack_Packages/win32/NSIS")
      SET(CPACK_NSIS_HELP_LINK "http://smil.cmm.mines-paristech.fr/doc/")
      SET(CPACK_NSIS_URL_INFO_ABOUT "http://smil.cmm.mines-paristech.fr/doc/")

      IF(WRAP_PYTHON)
	      IF(USE_64BIT_IDS)
		      # allow the installer to access keys in the x64 view of the registry
		      SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} 
			      SetRegView 64
		      ")
	      ENDIF(USE_64BIT_IDS)
	      SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
			WriteRegStr HKLM \\\"SOFTWARE\\\\Python\\\\PythonCore\\\\${PYTHON_VERSION}\\\\PythonPath\\\\Smil\\\" \\\"\\\" \\\"$INSTDIR\\\\${SMIL_LIBRARIES_INSTALL_PATH}\\\"
	      ")

	      SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
			DeleteRegValue HKLM \\\"SOFTWARE\\\\Python\\\\PythonCore\\\\${PYTHON_VERSION}\\\\PythonPath\\\" \\\"Smil\\\"
	      ")
      ENDIF(WRAP_PYTHON)
ENDIF(CPACK_GENERATOR MATCHES "NSIS")



#### DEB ####
IF(CPACK_GENERATOR MATCHES "DEB")

      OPTION(CPACK_USE_SHLIBDEPS "Use shlibdeps to determine package dependencies. 
	  This requires to add the libraries path to LD_LIBRARY_PATH" TRUE)
	  
      SET(CPACK_DEB_PACKAGE_COMPONENT TRUE)
      SET(CPACK_DEB_COMPONENT_INSTALL TRUE)

      SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matthieu Faessel") #required
      IF(USE_64BIT_IDS)
	      SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
      ELSE(USE_64BIT_IDS)
	      SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
      ENDIF(USE_64BIT_IDS)

      SET(CPACK_DEB_COMPONENT_INSTALL ON)

      IF(CPACK_USE_SHLIBDEPS)
		SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
      ELSE(CPACK_USE_SHLIBDEPS)

	      # lucid
	      IF(LINUX-CODENAME STREQUAL "lucid")
		      SET(CPACK_DEB_COMMON_DEPENDS
			    "libc6 (>= 2.11), libgcc1 (>= 1:4.1.1), 
			    libqtcore4 (>= 4:4.6), 
			    libqtgui4 (>= 4:4.6), 
			    libqwt6, 
			    libstdc++6 (>= 4.4.0)")
	      ENDIF(LINUX-CODENAME STREQUAL "lucid")

	      SET(CPACK_DEBIAN_COMPONENT_base_DEPENDS "${CPACK_DEB_COMMON_DEPENDS}")
	      
	      SET(CPACK_DEBIAN_COMPONENT_python_DEPENDS "${CPACK_DEB_COMMON_DEPENDS}, python${PYTHON_VERSION}")
	      
	      
      ENDIF(CPACK_USE_SHLIBDEPS)
      
ENDIF(CPACK_GENERATOR MATCHES "DEB")      


#### RPM ####

IF(CPACK_GENERATOR MATCHES "RPM")

      SET(LIB_DEPS)
      IF(USE_QT)
#	    SET(LIB_DEPS "${LIB_DEPS}, qt >= ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")
	    IF(USE_QWT)
#		  SET(LIB_DEPS "${LIB_DEPS}, qwt")
	    ENDIF(USE_QWT)
      ENDIF(USE_QT)

      SET(CPACK_RPM_PACKAGE_REQUIRES ${LIB_DEPS})
      
ENDIF(CPACK_GENERATOR MATCHES "RPM")      



#### TGZ ####
IF(CPACK_GENERATOR MATCHES "TGZ")
ENDIF(CPACK_GENERATOR MATCHES "TGZ")


      

INCLUDE(CPack)

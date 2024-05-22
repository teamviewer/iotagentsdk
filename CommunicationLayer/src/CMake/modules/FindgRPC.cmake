#********************************************************************************#
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2021 TeamViewer Germany GmbH                                     #
#                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy   #
# of this software and associated documentation files (the "Software"), to deal  #
# in the Software without restriction, including without limitation the rights   #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      #
# copies of the Software, and to permit persons to whom the Software is          #
# furnished to do so, subject to the following conditions:                       #
#                                                                                #
# The above copyright notice and this permission notice shall be included in all #
# copies or substantial portions of the Software.                                #
#                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  #
# SOFTWARE.                                                                      #
#********************************************************************************#
##
# Module to find and configure the gRPC library
# This module tries to find gRPC in config mode first to favour system/envirnoment provided installations.
# Otherwise it will define its own targets and tries to find their dependencies.
#
# The following targets are added:
#  gRPC::grpc              - C library
#  gRPC::grpc++            - C++ library
#  gRPC::grpc++_reflection - C++ reflection library
#
# NOTE: This find script requires the protobuf (protoc) and zlib libraries to be present.
# Either the executable target protobuf::protoc is avialable or Protobuf_PROTOC_EXECUTABLE is set or this script tries to locate protoc.

if(gRPC_FOUND)
	return()
endif()

# try to find gRPC in config mode to find system/environment installations
find_package(gRPC CONFIG QUIET)
if(gRPC_FOUND)
	return()
endif()

find_package(ZLIB)
find_package(Threads)

# Find gRPC include directory
find_path(GRPC_INCLUDE_DIR grpc/grpc.h)
mark_as_advanced(GRPC_INCLUDE_DIR)

# Find gpr library
find_library(GRPC_GPR_LIBRARY NAMES gpr)
mark_as_advanced(GRPC_GPR_LIBRARY)

add_library(gRPC::gpr UNKNOWN IMPORTED)
set_target_properties(gRPC::gpr PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
	IMPORTED_LOCATION ${GRPC_GPR_LIBRARY}
)

# Find C library
find_library(GRPC_LIBRARY NAMES grpc)
mark_as_advanced(GRPC_LIBRARY)

add_library(gRPC::grpc UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
	INTERFACE_LINK_LIBRARIES "gRPC::gpr;ZLIB::ZLIB;Threads::Threads"
	IMPORTED_LOCATION ${GRPC_LIBRARY}
)

# Find C++ library
find_library(GRPC_GRPC++_LIBRARY NAMES grpc++)
mark_as_advanced(GRPC_GRPC++_LIBRARY)

add_library(gRPC::grpc++ UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc++ PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
	INTERFACE_LINK_LIBRARIES gRPC::grpc
	IMPORTED_LOCATION ${GRPC_GRPC++_LIBRARY}
)

# Find C++ reflection library
find_library(GRPC_GRPC++_REFLECTION_LIBRARY NAMES grpc++_reflection)
mark_as_advanced(GRPC_GRPC++_REFLECTION_LIBRARY)

add_library(gRPC::grpc++_reflection UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc++_reflection PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
	INTERFACE_LINK_LIBRARIES gRPC::grpc++
	IMPORTED_LOCATION ${GRPC_GRPC++_REFLECTION_LIBRARY}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gRPC DEFAULT_MSG GRPC_LIBRARY GRPC_INCLUDE_DIR GRPC_GRPC++_REFLECTION_LIBRARY ZLIB_LIBRARIES)

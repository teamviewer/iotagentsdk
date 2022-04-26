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
#
# The following targets are added:
#  gRPC::grpc              - C library
#  gRPC::grpc++            - C++ library
#  gRPC::grpc++_reflection - C++ reflection library
#  gRPC::grpc_cpp_plugin   - C++ generator plugin for .proto files used by protoc
#
# NOTE: This find script requires the protobuf (protoc) and zlib libraries to be present.
# Either the executable target protobuf::protoc is avialable or Protobuf_PROTOC_EXECUTABLE is set or this script tries to locate protoc.

find_package(ZLIB)

##
# Generates C++ sources from the .proto files
#
# grpc_generate_cpp(<sources> <headers> [INCLUDES <include_directory> [include_directory...]] [DESTINATION <path>] <proto_source1> [proto_source2 ...])
#
#  <sources>                        - variable for generated source files
#  <headers>                        - variable for generated header files
#  INCLUDES <include_directory> ... - use provided directories as search paths for protoc
#  DESTINATION <path>               - directory where the source files will be created
#  <proto_source1> ...              - .proto source files to process
#
function(grpc_generate_cpp _sources _headers)
	cmake_parse_arguments(_GRPC_GEN_CPP "APPEND_INCLUDES" "DESTINATION" "INCLUDES" ${ARGN})

	if(NOT _GRPC_GEN_CPP_UNPARSED_ARGUMENTS)
		message(SEND_ERROR "grpc_generate_cpp() called without any input files")
		return()
	endif()

	if(NOT _GRPC_GEN_CPP_DESTINATION)
		set(_GRPC_GEN_CPP_DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	set(_protobuf_include_paths -I${CMAKE_CURRENT_SOURCE_DIR})

	foreach(_item IN LISTS _GRPC_GEN_CPP_INCLUDES PROTOBUF_IMPORT_DIRS)
		get_filename_component(_directory_absolute ${_item} ABSOLUTE)
		if(NOT ("-I${_directory_absolute}" IN_LIST _protobuf_include_paths))
			list(APPEND _protobuf_include_paths -I${_directory_absolute})
		endif()
	endforeach()

	unset(${_sources})
	unset(${_headers})
	foreach(_file IN LISTS _GRPC_GEN_CPP_UNPARSED_ARGUMENTS)
		get_filename_component(_file_absolute ${_file} ABSOLUTE)
		get_filename_component(_directory_absolute ${_file_absolute} PATH)
		get_filename_component(_file_name_base ${_file} NAME_WE)
		set(_file_cc "${_GRPC_GEN_CPP_DESTINATION}/${_file_name_base}.grpc.pb.cc")
		set(_file_h  "${_GRPC_GEN_CPP_DESTINATION}/${_file_name_base}.grpc.pb.h")

		list(APPEND ${_sources} ${_file_cc})
		list(APPEND ${_headers} ${_file_h})

		add_custom_command(
			OUTPUT ${_file_cc} ${_file_h}
			COMMAND gRPC::protoc
			ARGS --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
				--grpc_out=${_GRPC_GEN_CPP_DESTINATION}
				-I${_directory_absolute}
				${_protobuf_include_paths}
				${_file_absolute}
			DEPENDS gRPC::protoc gRPC::grpc_cpp_plugin ${_file_absolute}
			COMMENT "Running protoc with gRPC C++ plugin on ${_file}"
			VERBATIM
		)
	endforeach()

	set_property(SOURCE ${${_sources}} ${${_headers}} PROPERTY GENERATED TRUE)
	set(${_sources} ${${_sources}} PARENT_SCOPE)
	set(${_headers} ${${_headers}} PARENT_SCOPE)
endfunction()

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
	INTERFACE_LINK_LIBRARIES "gRPC::gpr;${ZLIB_LIBRARIES};-pthread"
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

# Find protoc C++ generator plugin
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)
mark_as_advanced(GRPC_CPP_PLUGIN)

add_executable(gRPC::grpc_cpp_plugin IMPORTED)
set_target_properties(gRPC::grpc_cpp_plugin PROPERTIES
	IMPORTED_LOCATION ${GRPC_CPP_PLUGIN}
)

# Find protoc
add_executable(gRPC::protoc IMPORTED)

if(TARGET protobuf::protoc)
	get_property(_protoc_location TARGET protobuf::protoc PROPERTY IMPORTED_LOCATION)
elseif(Protobuf_PROTOC_EXECUTABLE)
	set(_protoc_location ${Protobuf_PROTOC_EXECUTABLE})
endif()

if(_protoc_location)
	set(GRPC_PROTOC ${_protoc_location} CACHE PATH "location of protoc used by gRPC")
	unset(_protoc_location)
else()
	find_program(GRPC_PROTOC NAMES protoc protoc.exe)
endif()
mark_as_advanced(GRPC_PROTOC)

set_target_properties(gRPC::protoc PROPERTIES
	IMPORTED_LOCATION ${GRPC_PROTOC}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gRPC DEFAULT_MSG GRPC_LIBRARY GRPC_INCLUDE_DIR GRPC_GRPC++_REFLECTION_LIBRARY GRPC_CPP_PLUGIN GRPC_PROTOC ZLIB_LIBRARIES)

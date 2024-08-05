# Find protoc C++ generator plugin
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)
mark_as_advanced(GRPC_CPP_PLUGIN)

if(TARGET protobuf::protoc)
	set(_protoc_location protobuf::protoc)
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
			COMMAND ${GRPC_PROTOC}
			ARGS --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
				--grpc_out=${_GRPC_GEN_CPP_DESTINATION}
				-I${_directory_absolute}
				${_protobuf_include_paths}
				${_file_absolute}
			DEPENDS ${GRPC_PROTOC} ${GRPC_CPP_PLUGIN} ${_file_absolute}
			COMMENT "Running protoc with gRPC C++ plugin on ${_file}"
			VERBATIM
		)
	endforeach()

	set_property(SOURCE ${${_sources}} ${${_headers}} PROPERTY GENERATED TRUE)
	set(${_sources} ${${_sources}} PARENT_SCOPE)
	set(${_headers} ${${_headers}} PARENT_SCOPE)
endfunction()
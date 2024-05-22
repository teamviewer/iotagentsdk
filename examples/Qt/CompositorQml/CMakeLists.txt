// Forked from https://code.qt.io/cgit/qt/qtwayland.git/tree/examples/wayland/minimal-qml?h=6.2 
// addjustment happend only for the integration of the TVQTRC Plugin

project(example_Qt6_CompositorQml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml WaylandCompositor)

qt_add_executable(example_Qt6_CompositorQml
    main.cpp
)

set_target_properties(example_Qt6_CompositorQml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(example_Qt6_CompositorQml PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
	Qt::WaylandCompositor
)

# Resources:
set(minimal-qml_resource_files
    "main.qml"
)

qt6_add_resources(example_Qt6_CompositorQml "minimal-qml"
    PREFIX
        "/"
    FILES
        ${minimal-qml_resource_files}
)

if(BUILD_SHARED_LIBS)
	target_link_libraries(example_Qt6_CompositorQml PRIVATE TVQtRC.interface)
	add_dependencies(example_Qt6_CompositorQml TVQtRC)
	add_custom_command(TARGET example_Qt6_CompositorQml POST_BUILD
		COMMAND [ -L $<TARGET_FILE_NAME:TVQtRC> ] || ln -s $<TARGET_FILE:TVQtRC> $<TARGET_FILE_NAME:TVQtRC>
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
	target_link_libraries(example_Qt6_CompositorQml TVQtRC)
endif()

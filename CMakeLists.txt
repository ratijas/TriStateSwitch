cmake_minimum_required(VERSION 3.16)

project(TriStateSwitchQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_NO_PRIVATE_MODULE_WARNING ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickPrivate QuickTemplates2 QuickTemplates2Private)

qt_standard_project_setup(REQUIRES 6.9)

qt_add_executable(appTriStateSwitchQt
    main.cpp
)

qt_add_qml_module(appTriStateSwitchQt
    URI TriStateSwitchQt
    VERSION 1.0
    QML_FILES
        Main.qml
        TriStateSwitchBasic.qml
        TriStateSwitchIndicatorBasic.qml
    SOURCES
        tristateswitch.h tristateswitch.cpp
        geometryutils.h geometryutils.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appTriStateSwitchQt PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTriStateSwitchQt
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appTriStateSwitchQt
    PRIVATE
        Qt6::Quick Qt6::QuickPrivate
        Qt6::QuickTemplates2 Qt6::QuickTemplates2Private
)

include(GNUInstallDirs)
install(TARGETS appTriStateSwitchQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

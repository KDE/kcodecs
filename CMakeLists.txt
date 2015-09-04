cmake_minimum_required(VERSION 2.8.12)

project(KCodecs)

include(FeatureSummary)
find_package(ECM 5.14.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} )

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.3.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)


include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(ECMPoQmTools)

set(KF5_VERSION "5.14.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KCODECS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfigVersion.cmake"
                        SOVERSION 5)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

add_subdirectory(src)

add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Codecs")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5CodecsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5CodecsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5CodecsTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

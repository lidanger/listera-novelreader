cmake_minimum_required (VERSION 3.5)

project(listera-novelreader 
    VERSION 1.1.6.2
    DESCRIPTION "A Chinese novel reader for txt files"
    HOMEPAGE_URL "https://gitee.com/lidanger/listera-novelreader"
    LANGUAGES "CXX")

# build targets

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)

aux_source_directory(src SRC)

file(GLOB RCC_FILES resources/*.qrc)

add_executable(${PROJECT_NAME} ${SRC} ${RCC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets Qt5::Core Qt5::Gui 
    dtkcore dtkgui dtkwidget)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(src)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")

include_directories(
    /usr/include/libdtk-5.2.2/DCore
    /usr/include/libdtk-5.2.1/DGui
    /usr/include/libdtk-5.2.2/DWidget)

endif()

#include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_custom_command(TARGET ${PROJECT_NAME}
    PRE_LINK
    COMMAND lrelease *.ts
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/translations)

# export targets

set(RESOURCE_FILES
    LICENSE
    README.md
)
set_target_properties(${PROJECT_NAME} PROPERTIES
  RESOURCE "${RESOURCE_FILES}")

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")

set(CMAKE_INSTALL_PREFIX "/usr")

install(TARGETS ${PROJECT_NAME}
    #CONFIGURATIONS Release
    RUNTIME DESTINATION bin 
    RESOURCE DESTINATION share/doc/${PROJECT_NAME})

file(GLOB DESKTOP_FILES resources/*.desktop)

install(FILES ${DESKTOP_FILES} 
    DESTINATION share/applications)

install(FILES resources/images/logo.png
    DESTINATION share/icons/hicolor/128x128/apps
    RENAME ${PROJECT_NAME}.png)

file(GLOB TRANSLATION_FILES translations/*.qm)

install(FILES ${TRANSLATION_FILES} 
    DESTINATION share/${PROJECT_NAME}/translations)

endif()

# build packages

set(CPACK_PACKAGE_DESCRIPTION 
    "A Chinese novel reader for txt files.")

set(CPACK_PACKAGE_HOMEPAGE_URL ${PROJECT_HOMEPAGE_URL})
set(CPACK_PACKAGE_CONTACT "lidanger <lidanger@outlook.com>")

set(CPACK_RESOURCE_FILE_LICENSE, ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README, ${PROJECT_SOURCE_DIR}/README.md)

set(CPACK_PACKAGE_CHECKSUM "MD5")

set(CPACK_SET_DESTDIR true)

include(InstallRequiredSystemLibraries)

# source package

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;/.git/;.swp$;.#;/#;/build/;~") 

# targets package

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")

# build deb packages 

set(CPACK_GENERATOR "DEB")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "text")

set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_HOMEPAGE_URL})

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

else()

set(CPACK_GENERATOR "TGZ")

endif()

include(CPack)

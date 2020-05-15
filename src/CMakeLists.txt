set(krunner_pidgin_SRCS
        PidginRunner.cpp PidginClient.cpp Buddy.cpp)
set(client_test_SRCS
        client_test.cpp PidginClient.cpp Buddy.cpp)

add_library(krunner_pidgin MODULE ${krunner_pidgin_SRCS})
target_link_libraries(krunner_pidgin KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
        KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

install(TARGETS krunner_pidgin DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasma-runner-pidgin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

add_executable(client_test ${client_test_SRCS})
target_link_libraries(client_test KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
        KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Israel Rios")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.14), libkf5plasma5 (>= 4.96.0), libkf5runner5 (>= 5.24.0), libqt5dbus5 (>= 5.4), libstdc++6 (>= 4.1.1)")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Krunner plugin that provides the list of buddies in Pidgin making it possible to start a conversation from Krunner.")
SET(CPACK_DEBIAN_PACKAGE_SECTION "kde")
SET(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE, "https://github.com/israelrios/plasma-runner-pidgin")
INCLUDE(CPack)
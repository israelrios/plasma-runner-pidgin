set(krunner_pidgin_SRCS
        PidginRunner.cpp PidginClient.cpp Buddy.cpp)
set(client_test_SRCS
        client_test.cpp PidginClient.cpp Buddy.cpp)

cmake_policy(SET CMP0063 NEW)

add_library(krunner_pidgin MODULE ${krunner_pidgin_SRCS})
target_link_libraries(krunner_pidgin KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
        KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

install(TARGETS krunner_pidgin DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasma-runner-pidgin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

add_executable(client_test ${client_test_SRCS})
target_link_libraries(client_test KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
        KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

# lampswitch.desktop

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop" @ONLY)
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop" DESTINATION "${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart")

# in.tari.lampswitch.services.policy

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/in.tari.${CMAKE_PROJECT_NAME}.services.policy.in" "${CMAKE_CURRENT_BINARY_DIR}/in.tari.${CMAKE_PROJECT_NAME}.services.policy" @ONLY)
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/in.tari.${CMAKE_PROJECT_NAME}.services.policy" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/polkit-1/actions")

# services.sh

install (PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/services.sh" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME}")

# lampswitch.svg
# lampswitch-active.svg
# lampswitch-attention.svg

install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons/" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor")

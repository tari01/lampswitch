# lampswitch

add_executable ("lampswitch" main.c)
target_compile_definitions ("lampswitch" PUBLIC G_LOG_DOMAIN="lampswitch" LOCALEDIR="${CMAKE_INSTALL_FULL_LOCALEDIR}" DATADIR="${CMAKE_INSTALL_FULL_DATADIR}")
target_link_libraries ("lampswitch" ${DEPS_LIBRARIES})
target_include_directories ("lampswitch" PUBLIC ${DEPS_INCLUDE_DIRS})
install (TARGETS "lampswitch" RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

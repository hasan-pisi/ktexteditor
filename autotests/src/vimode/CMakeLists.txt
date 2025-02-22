set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
  ${CMAKE_SOURCE_DIR}/src/vimode
)

add_library(vimode_base STATIC
    base.cpp
    fakecodecompletiontestmodel.cpp
    emulatedcommandbarsetupandteardown.cpp
)
target_link_libraries(vimode_base PRIVATE KF6TextEditor KF6::Codecs Qt6::Test)

set (VIMODE_TEST_LINK_LIBS KF6TextEditor
  vimode_base
  KF6::I18n
  KF6::SyntaxHighlighting
  KF6::Codecs
  KF6::Completion
  Qt6::Qml
  Qt6::Test
)

macro(vimode_unit_test)
  ecm_add_test(${ARGN} TEST_NAME "vimode_${ARGV0}"
               LINK_LIBRARIES ${VIMODE_TEST_LINK_LIBS})
endmacro()


vimode_unit_test(modes modes.cpp)
vimode_unit_test(view view.cpp)
vimode_unit_test(completion completion.cpp)
vimode_unit_test(keys keys.cpp)
vimode_unit_test(emulatedcommandbar emulatedcommandbar.cpp)
vimode_unit_test(hlsearch hlsearch.cpp)

add_custom_target(vimode_tests ${CMAKE_CTEST_COMMAND} --force-new-ctest-process -R "^vimode_")

# CMake generated Testfile for 
# Source directory: /home/naiqi/repos/AL_USDMaya/translators/tests
# Build directory: /home/naiqi/repos/AL_USDMaya/2018build/translators/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TestAdditionalTranslators "testTranslators.sh" "/home/naiqi/AL2018" "USD_LIBRARY_DIR" "/home/naiqi/repos/AL_USDMaya/translators/tests")
set_tests_properties(TestAdditionalTranslators PROPERTIES  WORKING_DIRECTORY "/home/naiqi/repos/AL_USDMaya/translators/tests")

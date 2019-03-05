# CMake generated Testfile for 
# Source directory: /home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/tests
# Build directory: /home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TestAdditionalSchemas "RunTests.sh" "/home/naiqi/AL2018" "USD_LIBRARY_DIR")
set_tests_properties(TestAdditionalSchemas PROPERTIES  WORKING_DIRECTORY "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/tests")

# - Config file for the AL_USDMaya package
# It defines the following variables

#  ALUSDMAYA_ROOT - top directory of the package
#  ALUSDMAYA_INCLUDE_DIRS - include directories
#  ALUSDMAYA_LIBRARY    - libraries to link against
#  ALUSDMAYA_PYTHON_LIBRARY    - python libraries to link against
#  ALUSDMAYA_LIBRARIES - all of the libraries created.

# Compute paths
set(ALUSDMAYA_ROOT "/home/naiqi/AL2018")
set(ALUSDMAYA_INCLUDE_DIRS "/home/naiqi/AL2018/include")

set(ALUSDMAYA_LIBRARY "/home/naiqi/AL2018/lib/libAL_USDMaya.so")
set(ALUSDMAYA_PYTHON_LIBRARY "/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so")
list(APPEND ALUSDMAYACONFIG_LIBRARIES 
            ${ALUSDMAYA_LIBRARY} 
            ${ALUSDMAYA_PYTHON_LIBRARY})


#
# Main component of TF Micro project 'hello_world'.
#

idf_component_register(
    SRCS esp/main.cc main_functions.cc model.cc output_handler.cc constants.cc wave/wavelib.c 
wave/conv.c wave/cwt.c wave/cwtmath.c 
wave/hsfft.c wave/real.c wave/wavefilt.c 
wave/wavefunc.c wave/wtmath.c 
max30003/max30003.c 
signal/normalize.c 
signal/aaa.cpp 
signal/aaa_c_connector.cpp 
wifi/rest.c 
    INCLUDE_DIRS )

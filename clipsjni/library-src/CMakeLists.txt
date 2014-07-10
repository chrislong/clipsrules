include(UseJava)

include_directories(SYSTEM ${JNI_INCLUDE_DIRS})
include_directories(../../core)

get_property(CLIPS_JARFILE TARGET CLIPSJNI_jar PROPERTY JAR_FILE)
add_custom_command(
  OUTPUT CLIPSJNI_Environment.h
  COMMAND ${Java_JAVAH_EXECUTABLE} -o CLIPSJNI_Environment.h -classpath ${CLIPS_JARFILE} CLIPSJNI.Environment
  DEPENDS ${CLIPS_JARFILE}
  )

add_library(CLIPSJNI_native MODULE CLIPSJNI_Environment.cpp CLIPSJNI_Environment.h)
set_target_properties(CLIPSJNI_native PROPERTIES OUTPUT_NAME CLIPSJNI)
if (APPLE)
  # In java you want:
  #      System.loadLibrary("LIBRARY");
  # then JNI will look for a library whose name is platform dependent, namely
  #   MacOS  : libLIBRARY.jnilib
  #   Windows: LIBRARY.dll
  #   Linux  : libLIBRARY.so
  set_target_properties (CLIPSJNI_native PROPERTIES SUFFIX ".jnilib")
endif ()

target_link_libraries(CLIPSJNI_native libclips --no-allow-shlib-undefined --no-undefined)

# --no-undefined
# --no-allow-shlib-undefined

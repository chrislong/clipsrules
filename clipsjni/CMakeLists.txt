enable_language(Java)

find_package(Java COMPONENTS Development)
find_package(JNI)

if(JNI_FOUND)
  add_subdirectory(java-src)
  add_subdirectory(library-src)
else()
  message(WARNING "Could not find Java JNI support. Will not build CLIPSJNI library.")
endif()

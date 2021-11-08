################################################################################
##Copyright 2020 Antoine HOARAU <antoine [at] flr.io>
## part of this script is copied from Antoine's HOARAU's code on github
################################################################################


cmake_minimum_required(VERSION 3.13)

message(STATUS "Start configuration")

SET(XENOMAI_XENO_CONFIG /usr/xenomai/bin/xeno-config)
SET(MODELS Controller;Plant)
project(realtimecontroller)


function(find_xeno_skin_variables prefix skin_name)
    set(${prefix}_FOUND "")
    set(${prefix}_INCLUDE_DIRS "")
    set(${prefix}_LIBRARIES "")
    set(${prefix}_DEFINITIONS "")
    set(${prefix}_CFLAGS_OTHER "")
    set(${prefix}_LDFLAGS_OTHER "")
    set(${prefix}_LDFLAGS "")
    set(${prefix}_CFLAGS "")
    
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}
                    OUTPUT_VARIABLE ${prefix}_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_VARIABLE ${prefix}_LDFLAGS_ERROR)
    execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --cflags
                    OUTPUT_VARIABLE ${prefix}_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_VARIABLE ${prefix}_CFLAGS_ERROR)

    if(${prefix}_LDFLAGS_ERROR)
        message(FATAL_ERROR "Could not determine ldflags with command ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --ldflags ${XENO_CONFIG_LDFLAGS_EXTRA_ARGS}")
    endif()

    if(${prefix}_CFLAGS_ERROR)
        message(FATAL_ERROR "Could not determine cflags with command ${XENOMAI_XENO_CONFIG} --skin=${skin_name} --cflags")
    endif()

    set(${prefix}_FOUND TRUE)

    string(STRIP ${prefix}_LDFLAGS ${${prefix}_LDFLAGS})
    string(STRIP ${prefix}_CFLAGS ${${prefix}_CFLAGS})
    string(REPLACE " " ";" _${prefix}_LDFLAGS ${${prefix}_LDFLAGS})
    string(REPLACE " " ";" _${prefix}_CFLAGS ${${prefix}_CFLAGS})
    
    foreach(_entry ${_${prefix}_LDFLAGS})
      string(REGEX MATCH "^-L(.+)|^-l(.+)|^(-Wl,.+)|^(.*bootstrap(-pic)?.o)" _lib ${_entry})
      if(_lib)
        list(APPEND ${prefix}_LIBRARY ${_lib})
      else()
        list(APPEND ${prefix}_OTHER_LDFLAGS ${_entry})
      endif()
    endforeach()
    foreach(_entry ${_${prefix}_CFLAGS})
      string(REGEX MATCH "^-I.+" _include_dir ${_entry})
      string(REGEX MATCH "^-D.+" _definition ${_entry})
      if(_include_dir)
        string(REGEX REPLACE "^-I" "" _include_dir ${_include_dir})
        list(APPEND ${prefix}_INCLUDE_DIR ${_include_dir})
      elseif(_definition)
        string(REGEX REPLACE "^-D" "" _definition ${_definition})
        list(APPEND ${prefix}_DEFINITIONS ${_definition})
      else()
        list(APPEND ${prefix}_OTHER_CFLAGS ${_entry})
      endif()
    endforeach()
    string(REPLACE ";" " " ${prefix}_OTHER_LDFLAGS "${${prefix}_OTHER_LDFLAGS}")
    string(REPLACE ";" " " ${prefix}_OTHER_CFLAGS "${${prefix}_OTHER_CFLAGS}")

    message(STATUS "
    ==========================================
    Xenomai ${XENOMAI_VERSION} ${skin_name} skin
        libs          : ${${prefix}_LIBRARY}
        include       : ${${prefix}_INCLUDE_DIR}
        definitions   : ${${prefix}_DEFINITIONS}
        ldflags       : ${${prefix}_LDFLAGS}
        cflags        : ${${prefix}_CFLAGS}
        other ldflags : ${${prefix}_OTHER_LDFLAGS}
        other cflags  : ${${prefix}_OTHER_CFLAGS}
    ==========================================
    ")

    set(${prefix}_INCLUDE_DIRS ${${prefix}_INCLUDE_DIR} CACHE INTERNAL "")
    set(${prefix}_LIBRARIES ${${prefix}_LIBRARY} CACHE INTERNAL "")
    set(${prefix}_DEFINITIONS ${${prefix}_DEFINITIONS} CACHE INTERNAL "")
    set(${prefix}_LDFLAGS ${${prefix}_LDFLAGS} CACHE INTERNAL "")
    set(${prefix}_CFLAGS ${${prefix}_CFLAGS} CACHE INTERNAL "")
    set(${prefix}_OTHER_LDFLAGS ${${prefix}_LDFLAGS} CACHE INTERNAL "")
    set(${prefix}_OTHER_CFLAGS ${${prefix}_CFLAGS} CACHE INTERNAL "")
    set(${prefix}_FOUND ${${prefix}_FOUND} CACHE INTERNAL "")

    mark_as_advanced(${prefix}_LIBRARIES ${prefix}_INCLUDE_DIRS ${prefix}_DEFINITIONS ${prefix}_CFLAGS ${prefix}_LDFLAGS ${prefix}_OTHER_CFLAGS ${prefix}_OTHER_LDFLAGS)
endfunction()


FOREACH(model ${MODELS})
	FILE(GLOB model_files ${CMAKE_CURRENT_SOURCE_DIR}/${model}/common/*.cpp)
	list(APPEND lib_list ${model_files})
	list(APPEND lib_list ${CMAKE_CURRENT_SOURCE_DIR}/${model}/${model}.cpp)
endforeach()


FILE(GLOB class_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp)
list(APPEND class_list ${class_files})


#FOREACH(file ${lib_list})
#	message(STATUS "X=${file}")
#endforeach()


find_xeno_skin_variables(XENOMAI_POSIX posix)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XENO_CFLAGS}")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${XENO_LDFLAGS}")

add_library(model_libraries ${lib_list})

add_library(class_libaries ${class_list})
target_include_directories(class_libaries PRIVATE ${XENOMAI_POSIX_INCLUDE_DIRS})
target_link_libraries(class_libaries PRIVATE ${XENOMAI_POSIX_LIBRARIES})
target_compile_definitions(class_libaries PRIVATE ${XENOMAI_POSIX_DEFINITIONS})

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(main PRIVATE ${XENOMAI_POSIX_INCLUDE_DIRS})
target_link_libraries(main PRIVATE model_libraries class_libaries)
target_compile_definitions(main PRIVATE ${XENOMAI_POSIX_DEFINITIONS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(non-real-time ${CMAKE_CURRENT_SOURCE_DIR}/src/NRT20.cpp)
target_link_libraries(non-real-time PRIVATE Threads::Threads)

add_executable(logging ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.cpp)
target_link_libraries(non-real-time PRIVATE Threads::Threads)




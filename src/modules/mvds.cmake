cmake_minimum_required(VERSION 3.7)

find_package(GTest REQUIRED)

SET(CMAKE_CXX_COMPILER /usr/bin/g++-8)
set(CMAKE_CXX_FLAGS "-Werror -std=c++17 -ggdb")

function(mvds_add_package name)

    set(project_SRC "")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package/members" members)
    STRING(REGEX REPLACE "\n" ";" members "${members}")

    foreach(member ${members})
        set(source "${member}.cpp")
        list(APPEND project_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
    endforeach(member)

    message("Sources: ${project_SRC}")

    add_library (${name} ${project_SRC})

    target_include_directories (${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/package/dependencies")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package/dependencies" dependencies)
        STRING(REGEX REPLACE "\n" ";" dependencies "${dependencies}")

        message("Dependencies: ${dependencies}")

	foreach(dep ${dependencies})
	    message("Dependency: ${dep}")
	    target_link_libraries(${name} ${dep})
	endforeach(dep)
    endif()






    foreach(member ${members})
        set(src "${CMAKE_CURRENT_SOURCE_DIR}/${member}.t.cpp")

        if(EXISTS ${src})
            add_executable("${member}.t.tsk" ${src})
            target_include_directories("${member}.t.tsk" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
            target_link_libraries("${member}.t.tsk" ${name} pthread ${GTEST_LIBRARIES})
        endif()
    endforeach(member)

endfunction(mvds_add_package)

function(mvds_add_application name)

    set(project_SRC "")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/application/members" members)
    STRING(REGEX REPLACE "\n" ";" members "${members}")

    foreach(member ${members})
        set(source "${member}.cpp")
        list(APPEND project_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
    endforeach(member)

    message("Sources: ${project_SRC}")

    add_library("${name}" ${project_SRC})

    target_include_directories ("${name}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/application/dependencies")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/application/dependencies" dependencies)
        STRING(REGEX REPLACE "\n" ";" dependencies "${dependencies}")

        message("Dependencies: ${dependencies}")

	foreach(dep ${dependencies})
	    message("Dependency: ${dep}")
	    target_link_libraries("${name}" ${dep})
	endforeach(dep)
    endif()


    foreach(member ${members})
        set(src "${CMAKE_CURRENT_SOURCE_DIR}/${member}.t.cpp")

        if(EXISTS ${src})
            add_executable("${member}.t.tsk" ${src})
            target_include_directories("${member}.t" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
            target_link_libraries("${member}.t" ${name} pthread)
        endif()
    endforeach(member)

    add_executable("${name}.tsk" "${CMAKE_CURRENT_SOURCE_DIR}/${name}.m.cpp")
    target_include_directories("${name}.tsk" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries("${name}.tsk" "${name}")
    
endfunction(mvds_add_application)

function(mvds_add_adapter name)

    set(project_SRC "")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/adapter/members" members)
    STRING(REGEX REPLACE "\n" ";" members "${members}")

    foreach(member ${members})
        set(source "${member}.cpp")
        list(APPEND project_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
    endforeach(member)

    message("Sources: ${project_SRC}")

    add_library("${name}" ${project_SRC})

    target_include_directories ("${name}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/adapter/dependencies")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/adapter/dependencies" dependencies)
        STRING(REGEX REPLACE "\n" ";" dependencies "${dependencies}")

        message("Dependencies: ${dependencies}")

	foreach(dep ${dependencies})
	    message("Dependency: ${dep}")
	    target_link_libraries("${name}" ${dep})
	endforeach(dep)
    endif()


    foreach(member ${members})
        set(src "${CMAKE_CURRENT_SOURCE_DIR}/${member}.t.cpp")

        if(EXISTS ${src})
            add_executable("${member}.t.tsk" ${src})
            target_include_directories("${member}.t" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
            target_link_libraries("${member}.t" ${name} pthread)
        endif()
    endforeach(member)

endfunction(mvds_add_adapter)


function(mvds_build)

 
  set(MVDS_PACKAGE_DIRS "adapters;applications")

  # All packages in groups
  FILE(GLOB packages RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/groups/*")  
  foreach(pkg ${packages})
    IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${pkg})
      LIST(APPEND MVDS_PACKAGE_DIRS ${pkg})
    ENDIF()
  endforeach(pkg)
  
  foreach(pkgtype ${MVDS_PACKAGE_DIRS})
    FILE(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${pkgtype}/*")

    message("Package type: ${pkgtype}")
    
    foreach(pkg ${children})
      if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${pkg}/CMakeLists.txt")
    	message("Package: ${pkg}")
	set(TEMP_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	set(CMAKE_CURRENT_SOURCE_DIR "${TEMP_CURRENT_SOURCE_DIR}/${pkg}")
	include("${pkg}/CMakeLists.txt")
	set(CMAKE_CURRENT_SOURCE_DIR ${TEMP_CURRENT_SOURCE_DIR})
      endif()
    endforeach(pkg)
    
  endforeach(pkgtype)

endfunction(mvds_build)

cmake_minimum_required(VERSION 3.22)

set(CPM_DOWNLOAD_VERSION 0.35.5)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(chaven)

CPMAddPackage("gh:Tencent/rapidjson#v1.1.0")

find_package(CURL REQUIRED)

set(CHAVEN_INCLUDE_DIRS
    include
    ${rapidjson_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

file(GLOB_RECURSE CHAVEN_INCLUDE include/*.h include/*.hpp)
set(CHAVEN_SRC 
    src/abominations.cpp
    src/act_comm.cpp
    src/act_info.cpp
    src/act_move.cpp
    src/act_obj.cpp
    src/act_wiz.cpp
    src/alias.cpp
    src/attract.cpp
    src/ban.cpp
    src/bit.cpp
    src/bitarray.cpp
    src/clans.cpp
    src/comm.cpp
    src/creation.cpp
    src/db.cpp
    src/db2.cpp
    src/events.cpp
    src/flags.cpp
    src/fight.cpp
    src/ghosts.cpp
    src/handler.cpp
    src/hedit.cpp
    src/houses.cpp
    src/institute.cpp
    src/interp.cpp
    src/lookup.cpp
    src/map.cpp
    src/mem.cpp
    src/music.cpp
    src/note_new.cpp
    src/olc.cpp
    src/olc_act.cpp
    src/olc_mpcode.cpp
    src/olc_save.cpp
    src/patrols.cpp
    src/recycle.cpp
    src/save.cpp
    src/sex.cpp
    src/skills.cpp
    src/social-edit.cpp
    src/social.cpp
    src/stories.cpp
    src/shifters.cpp
    src/string.cpp
    src/sys.cpp
    src/system.cpp
    src/tables.cpp
    src/travel.cpp
    src/update.cpp
    src/world.cpp
    src/Buffer.cpp
    src/Note.cpp
    src/Util.cpp
    src/MudIo.cpp
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "-fpermissive -w")

# this is the core library we're making.
add_executable(chaven ${CHAVEN_INCLUDE} ${CHAVEN_SRC})
# everything we make will link to it.
target_include_directories(chaven PUBLIC ${CHAVEN_INCLUDE_DIRS})
target_link_libraries(chaven crypt)

SET(chaven ${CMAKE_INSTALL_PREFIX}/bin/)

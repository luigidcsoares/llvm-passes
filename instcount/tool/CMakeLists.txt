add_executable(instcount
    Main.cpp
    ../lib/InstCount.cpp
    )

target_link_libraries(instcount LLVMPasses)
target_include_directories(instcount PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

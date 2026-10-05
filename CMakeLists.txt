cmake_minimum_required(VERSION 3.20)

project(File_Encryption_Program
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(LOG "Enable logging features" ON)

# ============================================================
# Main Program
# ============================================================

add_executable(File_Encryption_Program

    main.cpp

    # DES
    src/cryptologic/DES/DES.cpp
    src/cryptologic/DES/Triple_DES.cpp
    src/cryptologic/DES/feistel.cpp

    # Operation Modes
    src/cryptologic/mode/mode.cpp
    src/cryptologic/mode/CTR.cpp

    # interface
    src/interface/menu.cpp

)

if(LOG)
	target_compile_definitions(File_Encryption_Program PRIVATE LOG)
	target_compile_options(File_Encryption_Program PRIVATE -O0 -g)
else()
	target_compile_options(File_Encryption_Program PRIVATE -O3)
endif()

target_include_directories(File_Encryption_Program
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)


# ============================================================
# Test Program
# ============================================================

add_executable(Test_Program

	test_generator/test.cpp
)

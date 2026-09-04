cmake_minimum_required(VERSION 3.20)

project(File_Encryption_Program
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Main Program
# ============================================================

add_executable(File_Encryption_Program

    main.cpp

    # Cryptologic
    src/cryptologic/crypto.cpp

    # DES
    src/cryptologic/DES/DES.cpp
    src/cryptologic/DES/feistel.cpp

    # Operation Modes
    src/cryptologic/mode/mode.cpp
    src/cryptologic/mode/CTR.cpp

    # Interface
    src/interface/consolePrinter.cpp
)

target_include_directories(File_Encryption_Program
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)



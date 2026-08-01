set(TARGET PrimeStatus)
project(${TARGET})

add_library(${TARGET} SHARED
    src/PrimeStatus.cpp
    src/Subsystem.cpp
)

target_include_directories(${TARGET} PRIVATE "include")
target_link_libraries(${TARGET} PUBLIC UE4SS)
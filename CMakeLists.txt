project(foo C)
set(CMAKE_C_FLAGS "-std=c99 -g -O0 -Wall -Wextra")
add_executable(pt pt.c)
target_link_libraries(pt -lrt)
add_executable(ad ad.c)
target_link_libraries(ad -lrt)

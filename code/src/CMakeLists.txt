# 声明当前目录下的源文件
file(GLOB SOURCES "*.cpp")

set(LIB_NAME log)
message("LIB_NAME: ${LIB_NAME}")

# 创建可执行文件
add_library(${LIB_NAME} ${SOURCES})

# 包含头文件目录
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/code/include)

# 链接my_class.h所需的其他库或源文件
target_link_libraries(${LIB_NAME} pthread)

# message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


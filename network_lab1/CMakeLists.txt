# ##############################################################################
# 基础项目配置
# ##############################################################################
# 最低CMake版本，3.16为稳定基线，FATAL_ERROR：版本不足直接终止
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# 项目定义：名称、版本号、编程语言
project(
    NetworkLab1   # 项目名称 不能带空格 如果带空格要➕“” “Network Lab-1”
    VERSION 1.0.1
    LANGUAGES CXX
)

# C++标准配置：标准C++17，禁用编译器扩展，保证跨平台一致性
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ##############################################################################
# 编译警告开关：用OPTION控制，编译时自由切换，商业项目常用
# ##############################################################################
# 选项：BUILD_STRICT_MODE，默认OFF，开启=警告全部当成错误
option(BUILD_STRICT_MODE "Enable strict compile warnings (warning as error)" OFF)

if(BUILD_STRICT_MODE)
    if(MSVC)
        # Windows MSVC：最高警告等级，警告=错误
        add_compile_options(/W4 /WX)
    else()
        # Mac Clang / Linux GCC：全套严格警告，警告=错误
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
else()
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

# ##############################################################################
# 默认构建类型：没指定就默认Release发布版
# ##############################################################################
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ========== 新增：打印项目配置信息，配置阶段直接看参数 ==========
message(STATUS "========================================")
message(STATUS "Project Name:        ${PROJECT_NAME}")
message(STATUS "Project Version:     ${PROJECT_VERSION}")
message(STATUS "C++ Standard:        ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "Strict Warning Mode: ${BUILD_STRICT_MODE}")
message(STATUS "========================================")

# ##############################################################################
# 第三方库示例：SFML
# 如果你不用SFML，注释掉这一段，替换成别的库（fmt/spdlog等写法类似）
# ##############################################################################
# find_package：寻找系统安装的SFML库，REQUIRED找不到就直接报错终止
# COMPONENTS：指定需要的组件，不需要的组件删掉
find_package(SFML REQUIRED COMPONENTS Graphics Window System Network)

# ##############################################################################
# 编译目标：可执行程序 client server
# ##############################################################################
add_executable(client
    src/client.cpp
)

add_executable(server
    src/server.cpp
)

# 头文件目录：PRIVATE 仅当前DemoApp可用，不会传递给其他依赖
target_include_directories(client
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# 链接第三方库：PRIVATE，只给DemoApp链接SFML
target_link_libraries(client
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Network
)

target_link_libraries(server
    PRIVATE
        SFML::System
        SFML::Network
)

# ##############################################################################
# 安装规则：打包发布用，cmake --install build
# ##############################################################################
install(TARGETS client server
    RUNTIME DESTINATION bin
)

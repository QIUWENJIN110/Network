# ##############################################################################
# 基础项目配置
# ##############################################################################
# 最低CMake版本，3.16为稳定基线，FATAL_ERROR：版本不足直接终止
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# 项目定义：名称、版本号、编程语言
project(
    TankGame   # 项目名称 不能带空格 如果带空格要➕“” “Network Lab”
    VERSION 1.0.0 # 项目版本号
    LANGUAGES CXX
)

# 替换全局 include_directories，移到target级别
# include_directories(${PROJECT_SOURCE_DIR})

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
find_package(SFML REQUIRED COMPONENTS Graphics Window System Network Audio)

# ##############################################################################
# 编译目标：可执行程序 
# ##############################################################################
file(GLOB_RECURSE GAME_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/SFML-ECS-Networking/*.cpp"
    "${CMAKE_SOURCE_DIR}/SFML-ECS-Networking/*.h"
)

add_executable(TankGame ${GAME_SRCS})

# 头文件路径绑定到目标（替代全局include_directories）
target_include_directories(TankGame PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(TankGame PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    SFML::Network
)

# 复制资源文件夹
# file(COPY ${CMAKE_SOURCE_DIR}/SFML-ECS-Networking/Assets
#     DESTINATION ${CMAKE_BINARY_DIR})

# 构建阶段复制，绑定目标，每次build自动同步资源
add_custom_command(TARGET TankGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/SFML-ECS-Networking/Assets
        $<TARGET_FILE_DIR:TankGame>/Assets
)

# ##############################################################################
# 安装规则：打包发布用，cmake --install build
# ##############################################################################
install(TARGETS TankGame
    RUNTIME DESTINATION bin
)

#最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8) #3.13

# 项目名称
PROJECT(shared_bike)

#将指定目录添加到编译器的头文件搜索路径下
#指定包含的头文件
INCLUDE_DIRECTORIES(../third/include)  #第三方的
INCLUDE_DIRECTORIES(./common)

LINK_DIRECTORIES(../third/lib/iniparser)
LINK_DIRECTORIES(./common)

# 收集指定路径下的源文件 文件名 
#将输出列表保持到指定变量中   
#内置变量：CMAKE_SOURCE_DIR 定义了顶级CMakeLists.txt 所在的文件夹(src)
#PROJECT_SOURCE_DIR定义了包含最近的project()命令的CMakeLists.txt 所在的文件夹
aux_source_directory(${PROJECT_SOURCE_DIR}  SOURCE_FILES)

# 使用给定的源文件 为工程生成shared_bike可执行文件 
ADD_EXECUTABLE(shared_bike  ${SOURCE_FILES})

#用来显示的定义变量
SET(CMAME_CXX_FLAGS "${CMAME_CXX_FLAGS}-rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations")

TARGET_LINK_LIBRARIES(shared_bike iniparser)
TARGET_LINK_LIBRARIES(shared_bike common)

# 增加子目录
ADD_SUBDIRECTORY(common)

#设置cmake安装的路劲
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
#安装
INSTALL(TARGETS shared_bike DESTINATION bin)




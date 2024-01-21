#CMakeLists.txt 基本玩法
#CMake 最低版本要求
cmake_minimum_required(VERSION 2.8)  #3.13

# 收集指定路径下的源文件 文件名 
#将输出列表保持到指定变量中   
aux_source_directory(.  SOURCE_COMMON_FILES)
# 最终将common下的源文件打包成库 给其他地方进行使用

#add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] source1 [source2 ...])
 #构建库供其他模块使用
ADD_LIBRARY(common ${SOURCE_COMMON_FILES}) 

#用来显示的定义变量
SET(CMAME_CXX_FLAGS "${CMAME_CXX_FLAGS}-rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations")


#指定使用第三方的头文件(.h) 库文件
#将指定目录添加到编译器的头文件搜索路径下
INCLUDE_DIRECTORIES(../../third/include)

#库文件
#将指定目录添加到需要连接的库文件目录下
LINK_DIRECTORIES(../../third/lib/iniparser) #指定全部路径
LINK_DIRECTORIES(../../third/lib/log4cpp) #指定全部路径

#指定连接
#将目标文件与库文件进行链接
TARGET_LINK_LIBRARIES(common iniparser)
TARGET_LINK_LIBRARIES(common log4cpp)
TARGET_LINK_LIBRARIES(common pthread) #将pthread添加进来
TARGET_LINK_LIBRARIES(common dl)


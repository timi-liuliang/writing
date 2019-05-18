# CMake 常用宏分享

## 介绍
在本人多年的CMake使用过程中，有两个自定义宏被广泛且频繁使用。分别是：  
#### 1. GROUP_FILES
根据项目目录结构，分组显示c++文件，实现目录结构与项目结构的对应。
```cmake
# MACRO GROUP_FILES
MACRO(GROUP_FILES src_files root_path)
        SET(cur_dir ${root_path})
        FOREACH(group_files ${${src_files}})
            STRING(REGEX REPLACE ${cur_dir}/\(.*\) \\1 sgbd_fpath ${group_files})
            STRING(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            STRING(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            STRING(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            IF(sgbd_nogroup)
				SOURCE_GROUP("" FILES ${group_files})
			ELSE()
				INCLUDE_DIRECTORIES(${sgbd_group_name})
				SOURCE_GROUP(${sgbd_group_name} FILES ${group_files})
            ENDIF(sgbd_nogroup)
        ENDFOREACH(group_files)
ENDMACRO(GROUP_FILES)
```
#### 2. EXCLUDE_FILES
根据匹配规则移除项目中的文件，在跨平台开发中，主要用于移除其它平台相关的代码文件。
```cmake
MACRO(EXCLUDE_FILES src_files rescure exclude_dir)
	FILE(${rescure} excludefiles  ${exclude_dir})
	FOREACH(excludefile ${excludefiles})
		LIST(REMOVE_ITEM ${src_files} ${excludefile})
	ENDFOREACH(excludefile)
ENDMACRO(EXCLUDE_FILES)
```

## 应用
这里我们以[glslang](https://github.com/timi-liuliang/echo/blob/master/thirdparty/glslang/CMakeLists.txt)库为例
```cmake
# Set Module Name
SET(MODULE_NAME glslang)

# message
MESSAGE( STATUS "Configuring module: ${MODULE_NAME} ...")

# Include Directories
INCLUDE_DIRECTORIES( ${ECHO_SRC_PATH})
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR})

# Link Directories
LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# set module path
SET(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Recursive get all files
FILE( GLOB_RECURSE ALL_FILES *.h *.inl *.c *.cpp *.mm)

# remove files by platform
IF(ECHO_PLATFORM_WIN32)
    EXCLUDE_FILES(ALL_FILES GLOB_RECURSE ${MODULE_PATH}/glslang/OSDependent/Unix/*.*)
ELSE()
    EXCLUDE_FILES(ALL_FILES GLOB_RECURSE ${MODULE_PATH}/glslang/OSDependent/Windows/*.*)
ENDIF()

# Group
GROUP_FILES(ALL_FILES ${CMAKE_CURRENT_SOURCE_DIR})

# Add library
ADD_LIBRARY(${MODULE_NAME} ${ALL_FILES} CMakeLists.txt)

# Set Folder
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES FOLDER "thirdparty")

# Message
MESSAGE(STATUS "Configure ${MODULE_NAME} success!")

```

![Image of group files](https://github.com/timi-liuliang/writing/blob/master/2019/8.CMake%20%E5%B8%B8%E7%94%A8%E5%AE%8F/group_files.png?raw=true)

## 结论
通过对GROUP_FILES 与 EXCLUDE_FILES的使用，极端情况下只需要修改 ${MODULE_NAME}，就可以实现一个c++库的CMake化。

![Image of group files](https://github.com/timi-liuliang/writing/blob/master/2019/8.CMake%20%E5%B8%B8%E7%94%A8%E5%AE%8F/diff.png?raw=true)   
[spine](https://github.com/timi-liuliang/echo/blob/master/thirdparty/spine/CMakeLists.txt) 与 [spirv-cross](https://github.com/timi-liuliang/echo/blob/master/thirdparty/spirv-cross/CMakeLists.txt) 只有库名称存存在差异
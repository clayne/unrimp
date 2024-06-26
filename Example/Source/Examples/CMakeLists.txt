#/*********************************************************\
# * Copyright (c) 2012-2022 The Unrimp Team
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# * and associated documentation files (the "Software"), to deal in the Software without
# * restriction, including without limitation the rights to use, copy, modify, merge, publish,
# * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all copies or
# * substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#\*********************************************************/


##################################################
## CMake
##################################################
cmake_minimum_required(VERSION 3.14.0)

# Optional "Simple DirectMedia Layer" (SDL, https://www.libsdl.org/ ) support inside the example framework
if(EXAMPLE_SDL2_ENABLED)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Build/")
	set(SDL2_BUILDING_LIBRARY "1")	# Ignore "SDL2main" since the example framework has an own main program entry point
	find_package(SDL2)
endif()
if(SDL2_FOUND)
	message("Found SDL2, SDL2 example enabled")
endif()


##################################################
## Source codes
##################################################
set(EXTERNAL_SOURCE_CODES
	# DeviceInput external library
	${CMAKE_SOURCE_DIR}/External/Example/DeviceInput/DeviceInput.cpp
)
if(EXAMPLES_MIMALLOC)
	set(EXTERNAL_SOURCE_CODES
		${EXTERNAL_SOURCE_CODES}
		# mimalloc external library
		${CMAKE_SOURCE_DIR}/External/Rhi/mimalloc/MimallocUnityBuild.cpp
	)
endif()
if(NOT ANDROID)
	set(EXTERNAL_SOURCE_CODES
		${EXTERNAL_SOURCE_CODES}
		# PhysicsFS external library
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_archiver_zip.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_archiver_dir.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_archiver_unpacked.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_byteorder.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_unicode.c
	)
endif()
if(WIN32)
	set(EXTERNAL_SOURCE_CODES
		${EXTERNAL_SOURCE_CODES}
		# PhysicsFS external library
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_platform_windows.c
	)
elseif(UNIX AND NOT ANDROID)
	set(EXTERNAL_SOURCE_CODES
		${EXTERNAL_SOURCE_CODES}
		# PhysicsFS external library
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_platform_unix.c
		${CMAKE_SOURCE_DIR}/External/Renderer/PhysicsFS/physfs_platform_posix.c
	)
endif()
if(RENDERER_PROFILER)
	set(EXTERNAL_SOURCE_CODES
		${EXTERNAL_SOURCE_CODES}
		# Remotery external library
		${CMAKE_SOURCE_DIR}/External/Renderer/Remotery/Remotery.c
	)
endif()
set(SOURCE_CODES
	${EXTERNAL_SOURCE_CODES}
	ExamplesUnityBuild.cpp
	Private/ExampleRunner.cpp
	Private/Framework/CommandLineArguments.cpp
	Private/Framework/Color4.cpp
	Private/Framework/ExampleBase.cpp
	Private/Framework/IApplication.cpp
	Private/Framework/IApplicationRhi.cpp
	Private/Framework/ExampleBase.cpp
	Private/Main.cpp
)
if(WIN32)
	if(SDL2_FOUND)
		set(SOURCE_CODES
			${SOURCE_CODES}
			Private/Framework/ApplicationImplSdl2.cpp
		)
	else()
		set(SOURCE_CODES
			${SOURCE_CODES}
			Private/Framework/ApplicationImplWindows.cpp
		)
	endif()
elseif(ANDROID)
	message("TODO(co) Examples: Add Android support")
elseif(UNIX)
	set(SOURCE_CODES
		${SOURCE_CODES}
		Private/Framework/Linux/ApplicationImplLinux.cpp
		Private/Framework/Linux/X11Window.cpp
		Private/Framework/Linux/X11Application.cpp
	)
endif()
if(RENDERER)
	set(SOURCE_CODES
		${SOURCE_CODES}
		Private/Framework/IApplicationRenderer.cpp
	)
endif()

# Add a natvis file for better debug support in Visual Studio ( https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects?view=vs-2019 )
if(MSVC)
	set(SOURCE_CODES ${SOURCE_CODES} ${CMAKE_SOURCE_DIR}/Source/Rhi/Rhi.natvis ${CMAKE_SOURCE_DIR}/Source/Renderer/Renderer.natvis)
endif(MSVC)

# Disable warnings in external libraries, we can't fix them
if(WIN32)
	# Used options:
	# - Disable "warning C4018: '<': signed/unsigned mismatch"
	# - Disable "warning C4061: enumerator 'MsgID_None' in switch of enum 'MessageID' is not explicitly handled by a case label"
	# - Disable "warning C4100: 'len': unreferenced formal parameter"
	# - Disable "warning C4101: 'retval': unreferenced local variable"
	# - Disable "warning C4127: conditional expression is constant"
	# - Disable "warning C4191: 'type cast': unsafe conversion from 'void (__cdecl *)(Sample *)' to 'ObjDestructor'"
	# - Disable "warning C4201: nonstandard extension used: nameless struct/union"
	# - Disable "warning C4242: '=': conversion from 'PHYSFS_uint32' to 'PHYSFS_uint16', possible loss of data"
	# - Disable "warning C4244: 'initializing': conversion from 'const PHYSFS_uint32' to 'const PHYSFS_uint16', possible loss of data"
	# - Disable "warning C4255: '__slwpcb': no function prototype given: converting '()' to '(void)'"
	# - Disable "warning C4267: '=': conversion from 'size_t' to 'int', possible loss of data"
	# - Disable "warning C4388: '<': signed/unsigned mismatch"
	# - Disable "warning C4389: '==': signed/unsigned mismatch"
	# - Disable "warning C4456: declaration of 'len' hides previous local declaration"
	# - Disable "warning C4548: expression before comma has no effect; expected expression with side-effect"
	# - Disable "warning C4668: '_USING_V110_SDK71_' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'"
	# - Disable "warning C4701: potentially uninitialized local variable 'timestamp' used"
	set_source_files_properties(${EXTERNAL_SOURCE_CODES} PROPERTIES COMPILE_DEFINITIONS "PHYSFS_SUPPORTS_7Z=0;PHYSFS_SUPPORTS_GRP=0;PHYSFS_SUPPORTS_HOG=0;;PHYSFS_SUPPORTS_MVL=0;PHYSFS_SUPPORTS_WAD=0;PHYSFS_SUPPORTS_QPAK=0;PHYSFS_SUPPORTS_SLB=0;PHYSFS_SUPPORTS_ISO9660=0;PHYSFS_SUPPORTS_VDF=0")
	set_source_files_properties(${EXTERNAL_SOURCE_CODES} PROPERTIES COMPILE_FLAGS "/wd4018 /wd4061 /wd4100 /wd4101 /wd4127 /wd4191 /wd4201 /wd4242 /wd4244 /wd4255 /wd4267 /wd4388 /wd4389 /wd4456 /wd4548 /wd4668 /wd4701")
endif()


##################################################
## Executables
##################################################
if(WIN32)
	add_executable(Examples WIN32 ${SOURCE_CODES})

	# Set Visual Studio debugger working directory (see https://stackoverflow.com/a/42973332 )
	set_target_properties(Examples PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Binary/${OS_ARCHITECTURE}/")
elseif(ANDROID)
	add_library(Examples SHARED ${SOURCE_CODES})
else()
	add_executable(Examples ${SOURCE_CODES})
	set(LIBRARIES ${LIBRARIES} X11 dl)
endif()
if(SHARED_LIBRARY)
	if(RENDERER)
		if(WIN32)
			target_link_libraries(Examples Renderer.lib)
		else()
			target_link_libraries(Examples Renderer)
		endif()
		add_dependencies(Examples Renderer)
	endif()
	set_target_properties(Examples PROPERTIES COMPILE_FLAGS -DSHARED_LIBRARIES)
else()
	unrimp_add_conditional_library_dependency(Examples RHI_NULL NullRhi)
	unrimp_add_conditional_library_dependency(Examples RHI_VULKAN VulkanRhi)
	unrimp_add_conditional_library_dependency(Examples RHI_OPENGL OpenGLRhi)
	unrimp_add_conditional_library_dependency(Examples RHI_OPENGLES3 OpenGLES3Rhi)
	unrimp_add_conditional_library_dependency(Examples RHI_DIRECT3D9 Direct3D9Rhi)
	unrimp_add_conditional_library_dependency(Examples RHI_DIRECT3D10 Direct3D10Rhi)
	unrimp_add_conditional_library_dependency(Examples RHI_DIRECT3D11 Direct3D11Rhi)
	unrimp_add_conditional_library_dependency(Examples RHI_DIRECT3D12 Direct3D12Rhi)
	unrimp_add_conditional_library_dependency(Examples RENDERER Renderer)
	if(UNIX AND NOT ANDROID)
		set(LIBRARIES ${LIBRARIES} pthread)
	endif()
	target_link_libraries(Examples ${LIBRARIES})
endif()
if(SDL2_FOUND)
	target_link_libraries(Examples ${SDL2_LIBRARY})
endif()


##################################################
## Preprocessor definitions
##################################################
unrimp_add_conditional_definition(Examples ARCHITECTURE_X64)
if(RHI_DEBUG)
	target_compile_definitions(Examples PRIVATE RHI_DEBUG)
	target_compile_definitions(Examples PRIVATE RHI_STATISTICS)
endif()
target_compile_definitions(Examples PRIVATE GLM_FORCE_CXX2A GLM_FORCE_INLINE GLM_FORCE_AVX2 GLM_FORCE_QUAT_DATA_XYZW GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_ENABLE_EXPERIMENTAL GLM_FORCE_SILENT_WARNINGS)
if(WIN32)
	target_compile_definitions(Examples PRIVATE UNICODE)
endif()
unrimp_add_conditional_rhi_definitions(Examples)
if(EXAMPLES_MIMALLOC)
	target_compile_definitions(Examples PRIVATE EXAMPLES_MIMALLOC)
endif()
unrimp_add_conditional_definition(Examples RENDERER)
unrimp_add_conditional_definition(Examples RENDERER_GRAPHICS_DEBUGGER)
unrimp_add_conditional_definition(Examples RENDERER_PROFILER)
if(RENDERER_PROFILER)
	if(RHI_DIRECT3D11)
		target_compile_definitions(Examples PRIVATE RMT_USE_D3D11=1)
	endif()
	if(RHI_OPENGL)
		target_compile_definitions(Examples PRIVATE RMT_USE_OPENGL=1)
	endif()
endif()
unrimp_add_conditional_definition(Examples RENDERER_IMGUI)
unrimp_add_conditional_definition(Examples RENDERER_OPENVR)
unrimp_add_conditional_definition(Examples SDL2_FOUND)

# Static builds are usually used for shipping builds, so disable the renderer toolkit for static builds
if(SHARED_LIBRARY)
	unrimp_add_conditional_definition(Examples RENDERER_TOOLKIT)
endif()


##################################################
## Includes
##################################################
target_include_directories(Examples PRIVATE ${CMAKE_SOURCE_DIR}/Example/Source
											${CMAKE_SOURCE_DIR}/Source
											${CMAKE_SOURCE_DIR}/External/Example	# For "DeviceInput" and "ini.h"
											${CMAKE_SOURCE_DIR}/External/Renderer)	# "glm", "PhysicsFS", "ImGui", "OpenVR", "RenderDoc" and "Remotery"
if(ANDROID)
	target_include_directories(Examples PRIVATE ${CMAKE_SOURCE_DIR}/External/Example/Android)
endif()
if(EXAMPLES_MIMALLOC)
	target_include_directories(Examples PRIVATE ${CMAKE_SOURCE_DIR}/External/Rhi/mimalloc/include)
endif()
if(RENDERER_TOOLKIT)
	target_include_directories(Examples PRIVATE ${CMAKE_SOURCE_DIR}/Renderer/RendererToolkit/include)
endif()
if(SDL2_FOUND)
	target_include_directories(Examples PRIVATE ${SDL2_INCLUDE_DIR})
endif()
target_link_directories(Examples PRIVATE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if(RHI_DIRECT3D11)
	target_link_directories(Examples PRIVATE ${CMAKE_SOURCE_DIR}/External/Rhi/AGS_SDK/Windows_${ARCHITECTURE}/Library)
endif()


##################################################
## Post build
##################################################
# Copy the SDL2 shared library, if necessary
if(WIN32 AND SDL2_FOUND)
	message("SDL copy " "${CMAKE_SOURCE_DIR}/External/Example/SDL2/lib/${ARCHITECTURE}/SDL2.dll")
	add_custom_command(TARGET Examples COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/External/Example/SDL2/lib/${ARCHITECTURE}/SDL2.dll" ${OUTPUT_BINARY_DIRECTORY})
endif()


##################################################
## Install
##################################################
install(TARGETS Examples RUNTIME DESTINATION "${OUTPUT_BINARY_DIRECTORY}")

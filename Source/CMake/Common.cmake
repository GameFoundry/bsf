# Adds a library as a dependency to a target. The dependency is assumed to have separate libraries for release and debug builds.
function(add_library_per_config target_name lib_name release_name debug_name)
	add_library(${lib_name} STATIC IMPORTED)
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_DEBUG ${PROJECT_SOURCE_DIR}/../Dependencies/${lib_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${debug_name}${BS_LIBRARY_EXTENSION})
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${PROJECT_SOURCE_DIR}/../Dependencies/${lib_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${release_name}${BS_LIBRARY_EXTENSION})
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_RELEASE ${PROJECT_SOURCE_DIR}/../Dependencies/${lib_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${release_name}${BS_LIBRARY_EXTENSION})	
	
	target_link_libraries(${target_name} PRIVATE ${lib_name})	
endfunction()

# Adds a library as a dependency to a target. The dependency is assumed to have separate libraries for release and debug builds. The libraries
# are assumed to be named differently from its dependency folder (which happens if a single dependency consists of multiple libraries).
function(add_library_per_config_multi target_name lib_name file_name release_name debug_name)
	add_library(${lib_name} STATIC IMPORTED)
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_DEBUG ${PROJECT_SOURCE_DIR}/../Dependencies/${file_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${debug_name}${BS_LIBRARY_EXTENSION})
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${PROJECT_SOURCE_DIR}/../Dependencies/${file_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${release_name}${BS_LIBRARY_EXTENSION})
	set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_RELEASE ${PROJECT_SOURCE_DIR}/../Dependencies/${file_name}/lib/${BS_OUTPUT_DIR_PREFIX}/${release_name}${BS_LIBRARY_EXTENSION})	
	
	target_link_libraries(${target_name} PRIVATE ${lib_name})	
endfunction()

function(add_engine_dependencies target_name)
	if(RENDER_API_MODULE MATCHES "DirectX 11")
		add_dependencies(${target_name} BansheeD3D11RenderAPI)
	elseif(RENDER_API_MODULE MATCHES "Vulkan")
		add_dependencies(${target_name} BansheeVulkanRenderAPI)
	else()
		add_dependencies(${target_name} BansheeGLRenderAPI)
	endif()

	if(AUDIO_MODULE MATCHES "FMOD")
		add_dependencies(${target_name} BansheeFMOD)
	else() # Default to OpenAudio
		add_dependencies(${target_name} BansheeOpenAudio)
	endif()
	
	add_dependencies(${target_name} BansheeMono BansheeSL BansheeOISInput BansheePhysX RenderBeast SBansheeEngine)
endfunction()

function(add_subdirectory_optional subdir_name)
	get_filename_component(fullPath ${subdir_name} ABSOLUTE)
	if(EXISTS ${fullPath}/CMakeLists.txt)
		add_subdirectory(${subdir_name})
	endif()
endfunction()

MACRO(add_imported_library LIB_NAME RELEASE_NAME DEBUG_NAME)
	add_library(${LIB_NAME} STATIC IMPORTED)
	
	if(CMAKE_CONFIGURATION_TYPES) # Multiconfig generator?
		set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION_DEBUG "${DEBUG_NAME}")
		set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG "${RELEASE_NAME}")
		set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION_RELEASE "${RELEASE_NAME}")
	else()
		set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION "${RELEASE_NAME}")
	endif()
ENDMACRO()

MACRO(find_imported_library2 FOLDER_NAME LIB_NAME DEBUG_LIB_NAME)
	find_library(${LIB_NAME}_LIBRARY_RELEASE NAMES ${LIB_NAME} PATHS ${${FOLDER_NAME}_LIBRARY_RELEASE_SEARCH_DIRS})
	
	if(${FOLDER_NAME}_LIBRARY_DEBUG_SEARCH_DIRS)
		find_library(${LIB_NAME}_LIBRARY_DEBUG NAMES ${DEBUG_LIB_NAME} PATHS ${${FOLDER_NAME}_LIBRARY_DEBUG_SEARCH_DIRS})
	else()
		find_library(${LIB_NAME}_LIBRARY_DEBUG NAMES ${DEBUG_LIB_NAME} PATHS ${${FOLDER_NAME}_LIBRARY_RELEASE_SEARCH_DIRS})
	endif()
		
	if(${LIB_NAME}_LIBRARY_RELEASE AND ${LIB_NAME}_LIBRARY_DEBUG)
		add_imported_library(${FOLDER_NAME}::${LIB_NAME} "${${LIB_NAME}_LIBRARY_RELEASE}" "${${LIB_NAME}_LIBRARY_DEBUG}")
	else()
		set(${FOLDER_NAME}_FOUND FALSE)
		message("Cannot find imported library: ${LIB_NAME} ${${LIB_NAME}_LIBRARY_RELEASE}")
	endif()

	list(APPEND ${FOLDER_NAME}_LIBRARIES ${FOLDER_NAME}::${LIB_NAME})
	mark_as_advanced(${LIB_NAME}_LIBRARY_RELEASE ${LIB_NAME}_LIBRARY_DEBUG)
ENDMACRO()

MACRO(find_imported_library FOLDER_NAME LIB_NAME)
	find_imported_library2(${FOLDER_NAME} ${LIB_NAME} ${LIB_NAME})
ENDMACRO()

function(update_binary_deps)
	# Clean and create a temporary folder
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/../Temp)	
	execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/../Temp)	
	
	set(BINARY_DEPENDENCIES_URL http://data.banshee3d.com/BansheeDependencies_VS2015_Master.zip)
	file(DOWNLOAD ${BINARY_DEPENDENCIES_URL} ${PROJECT_SOURCE_DIR}/../Temp/Dependencies.zip SHOW_PROGRESS)
	
	message(STATUS "Exacting files. Please wait...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf ${PROJECT_SOURCE_DIR}/../Temp/Dependencies.zip
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../Temp
	)
	
	# Copy executables and dynamic libraries
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/../bin)	
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../Temp/Built/bin ${PROJECT_SOURCE_DIR}/../bin)	
	
	# Copy static libraries, headers and tools
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/../Dependencies)	
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../Temp/Built/Dependencies ${PROJECT_SOURCE_DIR}/../Dependencies)
	
	# Clean up
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/../Temp)	
endfunction()
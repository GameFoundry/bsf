include(FetchContent)


# if INCLUDE_IMGUI_PLUGIN is true, then git clone the imgui plugin (with
# recursive submodules). it will be cloned into the build directory where it
# will be configured as bsfImgui target. other targets can then set bsfImgui
# as their dependency in their respective cmake files.

set(FETCHCONTENT_QUIET CACHE INTERNAL 0)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/pgruenbacher/bsfimgui.git
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imgui)


# mark as advanced so that FetchContent variables aren't being shown in the
# cmake gui options, which we want to keep clean for user
mark_as_advanced(
	FETCHCONTENT_BASE_DIR
	FETCHCONTENT_FULLY_DISCONNECTED
	FETCHCONTENT_SOURCE_DIR_IMGUI
	FETCHCONTENT_UPDATES_DISCONNECTED
)

if (TARGET bsfImgui)
	# Do something.
	message( STATUS "bsfImgui target available")
else()		
		message( SEND_ERROR "bsfImgui target not available" )
endif()

# mark as advanced so that FetchContent variables aren't being shown in the
# cmake gui options, which we want to keep clean for user
mark_as_advanced(imgui)

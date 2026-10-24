if(B3D_GPU_BACKEND MATCHES "DirectX12" OR B3D_BUILD_ALL_PLUGINS)
	# Source files
	B3DGlobSourceFiles(${CMAKE_CURRENT_SOURCE_DIR} "" "" BS_D3D12_TESTS_SRC)

	# Compile backend-private utilities directly into the test plugin.
	list(APPEND BS_D3D12_TESTS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../B3DD3D12BarrierUtility.cpp")
	list(APPEND BS_D3D12_TESTS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../B3DD3D12Utility.cpp")
	list(APPEND BS_D3D12_TESTS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../Utility/B3DD3D12BarrierBatch.cpp")

	add_library(bsfD3D12GpuBackendTests SHARED ${BS_D3D12_TESTS_SRC})

	# Common flags
	B3DSetDefaultLinkAndCompileFlags(bsfD3D12GpuBackendTests)

	# Framework public headers and compile flags
	target_link_libraries(bsfD3D12GpuBackendTests PRIVATE bsfHeaders)

	# Plugin-private and shader compiler headers
	target_include_directories(bsfD3D12GpuBackendTests PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
		"${CMAKE_CURRENT_SOURCE_DIR}/../../ShaderBackends/bsfShaderBackendHLSL"
		"${B3D_FRAMEWORK_SOURCE_FOLDER}/Tests")

	# Match the backend's Direct3D headers.
	B3DUseD3D12AgilitySDKHeaders(bsfD3D12GpuBackendTests)

	# Libraries
	target_link_libraries(bsfD3D12GpuBackendTests PRIVATE bsf)

	# IDE specific
	set_property(TARGET bsfD3D12GpuBackendTests PROPERTY FOLDER Tests)

	# Install / post-build copy alongside UnitTestRunner so plugin discovery finds the tests.
	B3DSetUpPostBuildAndInstallSteps(bsfD3D12GpuBackendTests)

	# Register the test plugin as a build dependency of UnitTestRunner.
	set_property(GLOBAL APPEND PROPERTY B3D_PLUGIN_TEST_TARGETS bsfD3D12GpuBackendTests)
endif()

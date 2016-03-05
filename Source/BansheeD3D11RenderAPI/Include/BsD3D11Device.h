//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"

namespace BansheeEngine
{
		/**
		 * @brief	Available DX11 error levels.
		 */
		enum BS_D3D11_ERROR_LEVEL
		{
			D3D11ERR_NO_EXCEPTION,
			D3D11ERR_CORRUPTION,
			D3D11ERR_ERROR,
			D3D11ERR_WARNING,
			D3D11ERR_INFO
		};

	/**
	 * @brief	Wrapper around DirectX 11 device object.
	 */
	class BS_D3D11_EXPORT D3D11Device
	{
	public:
		/**
		 * @brief	Constructs the object with a previously created DX11 device.
		 */
		D3D11Device(ID3D11Device* device);
		~D3D11Device();

		/**
		 * @brief	Shuts down the device any releases any internal resources.
		 */
		void shutdown();

		/**
		 * @brief	Returns DX11 immediate context object.
		 */
		ID3D11DeviceContext* getImmediateContext() const { return mImmediateContext; }

		/**
		 * @brief	Returns DX11 class linkage object.
		 */
		ID3D11ClassLinkage* getClassLinkage() const { return mClassLinkage; }

		/**
		 * @brief	Returns internal DX11 device.
		 */
		ID3D11Device* getD3D11Device() const { return mD3D11Device; }		

		/**
		 * @brief	Resets error state & error messages.
		 */
		void clearErrors();

		/**
		 * @brief	Query if error occurred at any point since last "clearError" call.
		 * 			Use getErrorDescription to get a string describing the error.
		 */
		bool hasError() const;

		/**
		 * @brief	Returns a string describing an error if one occurred.
		 */
		String getErrorDescription(bool clearErrors = true);
		
		/**
		 * @brief	Sets the level for which we want to receive errors for. Errors will be reported
		 *			for the provided level and any higher priority level.
		 */
		void setExceptionsErrorLevel(const BS_D3D11_ERROR_LEVEL exceptionsErrorLevel);

	private:
		D3D11Device();

		ID3D11Device* mD3D11Device;
		ID3D11DeviceContext* mImmediateContext;
		ID3D11InfoQueue* mInfoQueue; 
		ID3D11ClassLinkage* mClassLinkage;
	};
}
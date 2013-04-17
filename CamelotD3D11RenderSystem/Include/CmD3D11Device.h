#pragma once

#include "CmD3D11Prerequisites.h"

namespace CamelotFramework
{
	class CM_D3D11_EXPORT D3D11Device
	{
	public:
		D3D11Device(ID3D11Device* device);
		~D3D11Device();

		void shutdown();

		ID3D11DeviceContext* getImmediateContext() const { return mImmediateContext; }
		ID3D11ClassLinkage* getClassLinkage() const { return mClassLinkage; }
		ID3D11Device* getD3D11Device() const { return mD3D11Device; }		

		/**
		 * @brief	Resets error state & error messages.
		 */
		void clearErrors();

		/**
		 * @brief	Query if error occurred at any point since class "clearError" call.
		 * 			Use getErrorDescription to get a string describing the error.
		 */
		bool hasError() const;

		/**
		 * @brief	Returns a string describing an error, if one occurred.
		 */
		String getErrorDescription(bool clearErrors = true);
		
		enum CM_D3D11_ERROR_LEVEL
		{
			D3D11ERR_NO_EXCEPTION,
			D3D11ERR_CORRUPTION,
			D3D11ERR_ERROR,
			D3D11ERR_WARNING,
			D3D11ERR_INFO
		};

		void setExceptionsErrorLevel(const CM_D3D11_ERROR_LEVEL exceptionsErrorLevel);

	private:
		ID3D11Device*			mD3D11Device;
		ID3D11DeviceContext*	mImmediateContext;
		ID3D11InfoQueue*		mInfoQueue; 

		// Storing class linkage
		ID3D11ClassLinkage* mClassLinkage;

		D3D11Device();
	};
}
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuBufferView.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a specific view of a GpuBuffer. Different views all of the same buffer be used
	 *			in different situations (e.g. for reading from a shader, or for a unordered read/write operation).
	 */
	class BS_D3D11_EXPORT D3D11GpuBufferView : public GpuBufferView
	{
	public:
		D3D11GpuBufferView();
		virtual ~D3D11GpuBufferView();

		/**
		 * @copydoc	GpuBufferView::initialize
		 */
		void initialize(GpuBufferPtr buffer, GPU_BUFFER_DESC& desc);

	private:
		/**
		 * @brief	Creates a DX11 shader resource view that allows a buffer to be bound to a shader for reading (the most
		 *			common option). 
		 *
		 * @param	buffer			Buffer to create the view for.
		 * @param	firstElement	Index of the first element the view manages. Interpretation of this value depends on
		 *							exact buffer type. It may be byte offset for raw buffers, or number of elements for
		 *							structured buffers.
		 * @param	elementWidth	Width of a single element in the buffer. Size of the structure in structured buffers and
		 *							ignored for raw buffers as they always operate on single byte basis.
		 * @param	numElements		Number of elements the view manages, starting after the "firstElement". This means number of
		 *							bytes for raw buffers or number of structures for structured buffers.
		 *
		 * @returns	Constructed DX11 shader resource view object.
		 */
		ID3D11ShaderResourceView* createSRV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements);

		/**
		 * @brief	Creates a DX11 unordered access view that allows a buffer to be bound to a shader for random reading or writing.
		 *
		 * @param	buffer			Buffer to create the view for.
		 * @param	firstElement	Index of the first element the view manages. Interpretation of this value depends on
		 *							exact buffer type. It may be byte offset for raw buffers, or number of elements for
		 *							structured buffers.
		 * @param	numElements		Number of elements the view manages, starting after the "firstElement". This means number of
		 *							bytes for raw buffers or number of structures for structured buffers.
		 *
		 * @returns	Constructed DX11 unordered access view object.
		 */
		ID3D11UnorderedAccessView* createUAV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 numElements, bool useCounter);

		ID3D11ShaderResourceView* mSRV;
		ID3D11UnorderedAccessView* mUAV;
	};
}
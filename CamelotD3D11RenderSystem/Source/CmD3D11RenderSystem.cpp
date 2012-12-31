#include "CmD3D11RenderSystem.h"
#include "CmD3D11DriverList.h"
#include "CmD3D11Driver.h"
#include "CmD3D11Device.h"
#include "CmD3D11TextureManager.h"
#include "CmD3D11HardwareBufferManager.h"
#include "CmD3D11GpuProgramManager.h"
#include "CmD3D11RenderWindowManager.h"
#include "CmD3D11HLSLProgramFactory.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11RenderSystem::D3D11RenderSystem()
		: mDXGIFactory(nullptr), mDevice(nullptr), mDriverList(nullptr)
		, mActiveD3DDriver(nullptr), mFeatureLevel(D3D_FEATURE_LEVEL_9_1)
		, mHLSLFactory(nullptr)
	{

	}

	D3D11RenderSystem::~D3D11RenderSystem()
	{
		destroy_internal();
	}

	const String& D3D11RenderSystem::getName() const
	{
		static String strName("D3D11RenderSystem");
		return strName;
	}

	void D3D11RenderSystem::initialize_internal()
	{
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&mDXGIFactory);
		if(FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Failed to create Direct3D11 DXGIFactory");

		mDriverList = new D3D11DriverList(mDXGIFactory);
		mActiveD3DDriver = mDriverList->item(0); // TODO: Always get first driver, for now

		IDXGIAdapter* selectedAdapter = mActiveD3DDriver->getDeviceAdapter();

		D3D_FEATURE_LEVEL requestedLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		UINT32 numRequestedLevel = sizeof(requestedLevels) / sizeof(requestedLevels[0]);

		ID3D11Device* device;
		hr = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, 
			requestedLevels, numRequestedLevel, D3D11_SDK_VERSION, &device, &mFeatureLevel, 0);

		if(FAILED(hr))         
			CM_EXCEPT(RenderingAPIException, "Failed to create Direct3D11 object. D3D11CreateDeviceN returned this error code: " + toString(hr));

		mDevice = new D3D11Device(device);

		LARGE_INTEGER driverVersion;
		if(SUCCEEDED(selectedAdapter->CheckInterfaceSupport(IID_ID3D10Device /* intentionally D3D10, not D3D11 */, &driverVersion)))
		{
			mDriverVersion.major = HIWORD(driverVersion.HighPart);
			mDriverVersion.minor = LOWORD(driverVersion.HighPart);
			mDriverVersion.release = HIWORD(driverVersion.LowPart);
			mDriverVersion.build = LOWORD(driverVersion.LowPart);
		}

		// Create the texture manager for use by others		
		TextureManager::startUp(new D3D11TextureManager());

		// Also create hardware buffer manager		
		HardwareBufferManager::startUp(new D3D11HardwareBufferManager(*mDevice));

		// Create the GPU program manager		
		GpuProgramManager::startUp(new D3D11GpuProgramManager(*mDevice));

		// Create render window manager
		RenderWindowManager::startUp(new D3D11RenderWindowManager(this));

		// Create & register HLSL factory		
		mHLSLFactory = new D3D11HLSLProgramFactory();

		RenderSystem::initialize_internal();
	}

    void D3D11RenderSystem::destroy_internal()
	{
		SAFE_DELETE(mHLSLFactory);

		RenderWindowManager::shutDown();
		GpuProgramManager::shutDown();
		HardwareBufferManager::shutDown();
		TextureManager::shutDown();

		SAFE_RELEASE(mDXGIFactory);
		SAFE_DELETE(mDevice);
		SAFE_DELETE(mDriverList);
		mActiveD3DDriver = nullptr;

		RenderSystem::destroy_internal();
	}

	void D3D11RenderSystem::setSamplerState(UINT16 texUnit, const SamplerState& samplerState)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setBlendState(const BlendState& blendState)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setRasterizerState(const RasterizerState& rasterizerState)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setDepthStencilState(const DepthStencilState& depthStencilState)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setStencilRefValue(UINT32 refValue)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::disableTextureUnit(UINT16 texUnit)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::beginFrame()
	{
		// Not used
	}

	void D3D11RenderSystem::endFrame()
	{
		// Not used
	}

	void D3D11RenderSystem::setViewport(const Viewport& vp)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setVertexDeclaration(VertexDeclarationPtr decl)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setVertexBufferBinding(VertexBufferBinding* binding)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::bindGpuProgram(GpuProgramHandle prg)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setScissorRect(UINT32 left /*= 0*/, UINT32 top /*= 0*/, UINT32 right /*= 800*/, UINT32 bottom /*= 600 */)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::clearFrameBuffer(unsigned int buffers, const Color& color /*= Color::Black*/, float depth /*= 1.0f*/, unsigned short stencil /*= 0 */)
	{
		//mDevice->getImmediateContext()->c

		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setRenderTarget(RenderTarget* target)
	{
		//if(target != nullptr)
		//{
		//	mRenderViews
		//}
		//else
		//{

		//}

		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
	{
		LOGWRN("This call will be ignored. DX11 uses shaders for setting clip planes.");
	}

	RenderSystemCapabilities* D3D11RenderSystem::createRenderSystemCapabilities() const
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void D3D11RenderSystem::initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	String D3D11RenderSystem::getErrorDescription(long errorNumber) const
	{
		return mDevice->getErrorDescription();
	}

	void D3D11RenderSystem::determineFSAASettings(UINT32 fsaa, const String& fsaaHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings)
	{
		bool ok = false;
		bool qualityHint = fsaaHint.find("Quality") != String::npos;
		size_t origFSAA = fsaa;
		bool tryCSAA = false;
		// NVIDIA, prefer CSAA if available for 8+
		// it would be tempting to use getCapabilities()->getVendor() == GPU_NVIDIA but
		// if this is the first window, caps will not be initialised yet
		
		if (mActiveD3DDriver->getAdapterIdentifier().VendorId == 0x10DE && 
			fsaa >= 8)
		{
			tryCSAA	 = true;
		}

		while (!ok)
		{
			// Deal with special cases
			if (tryCSAA)
			{
				// see http://developer.nvidia.com/object/coverage-sampled-aa.html
				switch(fsaa)
				{
				case 8:
					if (qualityHint)
					{
						outFSAASettings->Count = 8;
						outFSAASettings->Quality = 8;
					}
					else
					{
						outFSAASettings->Count = 4;
						outFSAASettings->Quality = 8;
					}
					break;
				case 16:
					if (qualityHint)
					{
						outFSAASettings->Count = 8;
						outFSAASettings->Quality = 16;
					}
					else
					{
						outFSAASettings->Count = 4;
						outFSAASettings->Quality = 16;
					}
					break;
				}
			}
			else // !CSAA
			{
				outFSAASettings->Count = fsaa == 0 ? 1 : fsaa;
				outFSAASettings->Quality = 0;
			}


			HRESULT hr;
			UINT outQuality;
			hr = mDevice->getD3D11Device()->CheckMultisampleQualityLevels(format, outFSAASettings->Count, &outQuality);

			if (SUCCEEDED(hr) && (!tryCSAA || outQuality > outFSAASettings->Quality))
			{
				ok = true;
			}
			else
			{
				// downgrade
				if (tryCSAA && fsaa == 8)
				{
					// for CSAA, we'll try downgrading with quality mode at all samples.
					// then try without quality, then drop CSAA
					if (qualityHint)
					{
						// drop quality first
						qualityHint = false;
					}
					else
					{
						// drop CSAA entirely 
						tryCSAA = false;
					}
					// return to original requested samples
					fsaa = static_cast<UINT32>(origFSAA);
				}
				else
				{
					// drop samples
					--fsaa;

					if (fsaa == 1)
					{
						// ran out of options, no FSAA
						fsaa = 0;
						ok = true;
					}
				}
			}

		} // while !ok
	}

	bool D3D11RenderSystem::checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage)
	{
		return true;
	}

	VertexElementType D3D11RenderSystem::getColorVertexElementType() const
	{
		return VET_COLOR_ABGR;
	}

	void D3D11RenderSystem::convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram /*= false */)
	{
		dest = matrix;

		// Convert depth range from [-1,+1] to [0,1]
		dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
		dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
		dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
		dest[2][3] = (dest[2][3] + dest[3][3]) / 2;

		if (!forGpuProgram)
		{
			// Convert right-handed to left-handed
			dest[0][2] = -dest[0][2];
			dest[1][2] = -dest[1][2];
			dest[2][2] = -dest[2][2];
			dest[3][2] = -dest[3][2];
		}
	}

	float D3D11RenderSystem::getHorizontalTexelOffset()
	{
		return 0.0f;
	}

	float D3D11RenderSystem::getVerticalTexelOffset()
	{
		return 0.0f;
	}

	float D3D11RenderSystem::getMinimumDepthInputValue()
	{
		return 0.0f;
	}

	float D3D11RenderSystem::getMaximumDepthInputValue()
	{
		return -1.0f;
	}
}
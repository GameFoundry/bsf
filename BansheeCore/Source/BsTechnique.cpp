#include "BsTechnique.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsRendererManager.h"
#include "BsPass.h"
#include "BsRenderer.h"
#include "BsFrameAlloc.h"
#include "BsTechniqueRTTI.h"

namespace BansheeEngine
{
	TechniqueBase::TechniqueBase(const String& renderSystem, const String& renderer)
		:mRenderSystem(renderSystem), mRenderer(renderer)
	{

	}

	bool TechniqueBase::isSupported() const
	{
		if (RenderSystem::instancePtr()->getName() == mRenderSystem &&
			(RendererManager::instance().getActive()->getName() == mRenderer ||
			RendererManager::getCoreRendererName() == mRenderer))
		{
			return true;
		}

		return false;
	}

	void TechniqueBase::checkBounds(UINT32 idx, UINT32 bound) const
	{
		if (idx < 0 || idx >= bound)
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));
	}

	TechniqueCore::TechniqueCore(const String& renderSystem, const String& renderer)
		:TTechnique(renderSystem, renderer)
	{ }

	SPtr<PassCore> TechniqueCore::addPass()
	{
		SPtr<PassCore> newPass = PassCore::create();

		mPasses.push_back(newPass);
		_markCoreDirty();

		return newPass;
	}

	void TechniqueCore::syncToCore(const CoreSyncData& data)
	{
		UINT8* buffer = data.getBuffer();

		UINT32 numElements = 0;
		memcpy(&numElements, buffer, sizeof(UINT32));
		buffer += sizeof(UINT32);

		mPasses.clear();
		for (UINT32 i = 0; i < numElements; i++)
		{
			SPtr<PassCore>* pass = (SPtr<PassCore>*)buffer;

			mPasses.push_back(*pass);
			buffer += sizeof(SPtr<PassCore>);

			pass->~SPtr<PassCore>();
		}
	}

	SPtr<TechniqueCore> TechniqueCore::create(const String& renderSystem, const String& renderer)
	{
		TechniqueCore* technique = new (bs_alloc<TechniqueCore>()) TechniqueCore(renderSystem, renderer);
		SPtr<TechniqueCore> techniquePtr = bs_shared_ptr<TechniqueCore, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);
		techniquePtr->initialize();

		return techniquePtr;
	}

	Technique::Technique(const String& renderSystem, const String& renderer)
		:TTechnique(renderSystem, renderer)
	{ }

	Technique::Technique()
		: TTechnique("", "")
	{ }

	SPtr<Pass> Technique::addPass()
	{
		SPtr<Pass> newPass = Pass::create();

		mPasses.push_back(newPass);
		_markCoreDirty();

		return newPass;
	}

	SPtr<TechniqueCore> Technique::getCore() const
	{
		return std::static_pointer_cast<TechniqueCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Technique::createCore() const
	{
		TechniqueCore* technique = new (bs_alloc<TechniqueCore>()) TechniqueCore(mRenderSystem, mRenderer);
		SPtr<TechniqueCore> techniquePtr = bs_shared_ptr<TechniqueCore, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);

		return techniquePtr;
	}

	void Technique::_markCoreDirty()
	{
		markCoreDirty();
	}

	CoreSyncData Technique::syncToCore(FrameAlloc* alloc)
	{
		UINT32 numElements = (UINT32)mPasses.size();

		UINT32 size = sizeof(UINT32) + sizeof(SPtr<PassCore>) * numElements;
		UINT8* data = alloc->alloc(size);

		UINT8* dataPtr = data;
		memcpy(dataPtr, &numElements, sizeof(UINT32));
		dataPtr += sizeof(UINT32);

		for (UINT32 i = 0; i < numElements; i++)
		{
			SPtr<PassCore>* passPtr = new (dataPtr)SPtr<PassCore>();

			if (mPasses[i] != nullptr)
				*passPtr = mPasses[i]->getCore();
			else
				*passPtr = nullptr;

			dataPtr += sizeof(SPtr<PassCore>);
		}

		return CoreSyncData(data, size);
	}

	TechniquePtr Technique::create(const String& renderSystem, const String& renderer)
	{
		Technique* technique = new (bs_alloc<Technique>()) Technique(renderSystem, renderer);
		TechniquePtr techniquePtr = bs_core_ptr<Technique, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);
		techniquePtr->initialize();

		return techniquePtr;
	}

	TechniquePtr Technique::createEmpty()
	{
		Technique* technique = new (bs_alloc<Technique>()) Technique();
		TechniquePtr techniquePtr = bs_core_ptr<Technique, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);

		return techniquePtr;
	}

	RTTITypeBase* Technique::getRTTIStatic()
	{
		return TechniqueRTTI::instance();
	}

	RTTITypeBase* Technique::getRTTI() const
	{
		return Technique::getRTTIStatic();
	}
}
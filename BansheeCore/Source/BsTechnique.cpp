#include "BsTechnique.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsRendererManager.h"
#include "BsPass.h"
#include "BsCoreRenderer.h"
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

	template<bool Core>
	TTechnique<Core>::TTechnique(const String& renderSystem, const String& renderer, const Vector<SPtr<PassType>>& passes)
		: TechniqueBase(renderSystem, renderer), mPasses(passes)
	{ }

	template<bool Core>
	TTechnique<Core>::TTechnique()
		: TechniqueBase("", "")
	{ }

	template<bool Core>
	SPtr<typename TTechnique<Core>::PassType> TTechnique<Core>::getPass(UINT32 idx) const
	{
		if (idx < 0 || idx >= (UINT32)mPasses.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		return mPasses[idx];
	}

	template class TTechnique < false > ;
	template class TTechnique < true >;

	TechniqueCore::TechniqueCore(const String& renderSystem, const String& renderer, const Vector<SPtr<PassCore>>& passes)
		:TTechnique(renderSystem, renderer, passes)
	{ }

	SPtr<TechniqueCore> TechniqueCore::create(const String& renderSystem, const String& renderer, const Vector<SPtr<PassCore>>& passes)
	{
		TechniqueCore* technique = new (bs_alloc<TechniqueCore>()) TechniqueCore(renderSystem, renderer, passes);
		SPtr<TechniqueCore> techniquePtr = bs_shared_ptr<TechniqueCore, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);
		techniquePtr->initialize();

		return techniquePtr;
	}

	Technique::Technique(const String& renderSystem, const String& renderer, const Vector<SPtr<Pass>>& passes)
		:TTechnique(renderSystem, renderer, passes)
	{ }

	Technique::Technique()
		: TTechnique()
	{ }

	SPtr<TechniqueCore> Technique::getCore() const
	{
		return std::static_pointer_cast<TechniqueCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Technique::createCore() const
	{
		Vector<SPtr<PassCore>> passes;
		for (auto& pass : mPasses)
			passes.push_back(pass->getCore());

		TechniqueCore* technique = new (bs_alloc<TechniqueCore>()) TechniqueCore(mRenderSystem, mRenderer, passes);
		SPtr<TechniqueCore> techniquePtr = bs_shared_ptr<TechniqueCore, GenAlloc>(technique);
		techniquePtr->_setThisPtr(techniquePtr);

		return techniquePtr;
	}

	TechniquePtr Technique::create(const String& renderSystem, const String& renderer, const Vector<SPtr<Pass>>& passes)
	{
		Technique* technique = new (bs_alloc<Technique>()) Technique(renderSystem, renderer, passes);
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
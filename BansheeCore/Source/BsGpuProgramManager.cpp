//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuProgramManager.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
	String sNullLang = "null";

	/**
	 * @brief	Null GPU program used in place of GPU programs we cannot create.
	 *			Null programs don't do anything.
	 */
	class NullProgram : public GpuProgram
	{
	public:
		NullProgram()
			:GpuProgram("", "", GPT_VERTEX_PROGRAM, GPP_NONE, nullptr)
		{ }

		~NullProgram() { }

		bool isSupported() const { return false; }
		const String& getLanguage() const { return sNullLang; }

	protected:
		void loadFromSource() {}

		void buildConstantDefinitions() const
		{ }
	};

	/**
	 * @brief	Factory that creates null GPU programs. 
	 */
	class NullProgramFactory : public GpuProgramFactory
	{
	public:
		NullProgramFactory() {}
		~NullProgramFactory() {}

		const String& getLanguage() const 
		{ 
			return sNullLang;
		}

		GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacencyInformation)
		{
			return bs_core_ptr<NullProgram, PoolAlloc>();
		}

		GpuProgramPtr create(GpuProgramType type)
		{
			return bs_core_ptr<NullProgram, PoolAlloc>();
		}
	};

	GpuProgramManager::GpuProgramManager()
	{
		mNullFactory = bs_new<NullProgramFactory>();
		addFactory(mNullFactory);
	}

	GpuProgramManager::~GpuProgramManager()
	{
		bs_delete((NullProgramFactory*)mNullFactory);
	}

	void GpuProgramManager::addFactory(GpuProgramFactory* factory)
	{
		mFactories[factory->getLanguage()] = factory;
	}

    void GpuProgramManager::removeFactory(GpuProgramFactory* factory)
    {
        FactoryMap::iterator it = mFactories.find(factory->getLanguage());
        if (it != mFactories.end() && it->second == factory)
        {
            mFactories.erase(it);
        }
    }

	GpuProgramFactory* GpuProgramManager::getFactory(const String& language)
	{
		FactoryMap::iterator i = mFactories.find(language);

		if (i == mFactories.end())
			i = mFactories.find(sNullLang);

		return i->second;
	}

	bool GpuProgramManager::isLanguageSupported(const String& lang)
	{
		FactoryMap::iterator i = mFactories.find(lang);

		return i != mFactories.end();
	}

    GpuProgramPtr GpuProgramManager::create(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes,
		bool requiresAdjacencyInformation)
    {
		GpuProgramFactory* factory = getFactory(language);
		GpuProgramPtr ret = factory->create(source, entryPoint, gptype, profile, includes, requiresAdjacencyInformation);
		ret->_setThisPtr(ret);
		ret->initialize();

        return ret;
    }

	GpuProgramPtr GpuProgramManager::createEmpty(const String& language, GpuProgramType type)
	{
		GpuProgramFactory* factory = getFactory(language);
		GpuProgramPtr ret = factory->create(type);
		ret->_setThisPtr(ret);

		return ret;
	}

    GpuProgramFactory::~GpuProgramFactory() 
    {
    }
}

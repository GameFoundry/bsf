#include "CmGpuProgramManager.h"
#include "CmRenderSystem.h"

namespace BansheeEngine 
{
	String sNullLang = "null";
	class NullProgram : public GpuProgram
	{
	protected:
		/** Internal load implementation, must be implemented by subclasses.
		*/
		void loadFromSource(void) {}
		/// Populate the passed parameters with name->index map, must be overridden
		void populateParameterNames(GpuProgramParametersSharedPtr params)
		{
			// Skip the normal implementation
			// Ensure we don't complain about missing parameter names
			params->setIgnoreMissingParams(true);

		}
		void buildConstantDefinitions() const
		{
			// do nothing
		}

	public:
		NullProgram()
			: GpuProgram("", "", GPT_VERTEX_PROGRAM, GPP_NONE, nullptr){}
		~NullProgram() {}
		/// Overridden from GpuProgram - never supported
		bool isSupported(void) const { return false; }
		/// Overridden from GpuProgram
		const String& getLanguage(void) const { return sNullLang; }

		/// Overridden from StringInterface
		bool setParameter(const String& name, const String& value)
		{
			// always silently ignore all parameters so as not to report errors on
			// unsupported platforms
			return true;
		}
	};

	class NullProgramFactory : public GpuProgramFactory
	{
	public:
		NullProgramFactory() {}
		~NullProgramFactory() {}
		/// Get the name of the language this factory creates programs for
		const String& getLanguage(void) const 
		{ 
			return sNullLang;
		}
		GpuProgramPtr create(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, 
			bool requiresAdjacencyInformation)
		{
			return cm_core_ptr<NullProgram, PoolAlloc>();
		}
		GpuProgramPtr create(GpuProgramType type)
		{
			return cm_core_ptr<NullProgram, PoolAlloc>();
		}
	};

	GpuProgramManager::GpuProgramManager()
	{
		mNullFactory = cm_new<NullProgramFactory>();
		addFactory(mNullFactory);
	}

	GpuProgramManager::~GpuProgramManager()
	{
		cm_delete((NullProgramFactory*)mNullFactory);
	}

	void GpuProgramManager::addFactory(GpuProgramFactory* factory)
	{
		// deliberately allow later plugins to override earlier ones
		mFactories[factory->getLanguage()] = factory;
	}

    void GpuProgramManager::removeFactory(GpuProgramFactory* factory)
    {
        // Remove only if equal to registered one, since it might overridden
        // by other plugins
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
		{
			// use the null factory to create programs that will never be supported
			i = mFactories.find(sNullLang);
		}
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

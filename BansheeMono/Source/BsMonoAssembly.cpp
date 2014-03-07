#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "CmUtil.h"
#include "BsMonoUtil.h"
#include "CmException.h"

#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/tokentype.h>

using namespace CamelotFramework;

namespace BansheeEngine
{
	inline size_t MonoAssembly::ClassId::Hash::operator()(const MonoAssembly::ClassId& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.namespaceName);
		hash_combine(seed, v.name);
		return seed;
	}

	inline bool MonoAssembly::ClassId::Equals::operator()(const MonoAssembly::ClassId& a, const MonoAssembly::ClassId& b) const
	{
		return a.name == b.name && a.namespaceName == b.namespaceName;
	}

	MonoAssembly::ClassId::ClassId(const String& namespaceName, String name)
		:namespaceName(namespaceName), name(name)
	{

	}

	MonoAssembly::MonoAssembly()
		:mIsLoaded(false), mMonoImage(nullptr), mMonoAssembly(nullptr)
	{

	}

	MonoAssembly::~MonoAssembly()
	{
		unload();
	}

	void MonoAssembly::load(const CM::String& path, const CM::String& name)
	{
		::MonoAssembly* monoAssembly = mono_domain_assembly_open (MonoManager::instance().getDomain(), path.c_str());
		if(monoAssembly == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot load Mono assembly: " + path);
		}

		mName = name;
		mMonoAssembly = monoAssembly;
		mMonoImage = mono_assembly_get_image(mMonoAssembly);
		if(mMonoImage == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get script assembly image.");
		}

		// Load all classes
		int numRows = mono_image_get_table_rows (mMonoImage, MONO_TABLE_TYPEDEF);

		for(int i = 1; i < numRows; i++)
		{
			::MonoClass* monoClass = mono_class_get (mMonoImage, i | MONO_TOKEN_TYPE_DEF);

			String ns = mono_class_get_namespace(monoClass);
			String type = mono_class_get_name(monoClass);

			String fullClassName = ns + "." + type;
			MonoClass* newClass = new (cm_alloc<MonoClass>()) MonoClass(fullClassName, monoClass, this);
			mClasses[ClassId(ns, type)] = newClass;
		}

		mIsLoaded = true;
	}

	void MonoAssembly::unload()
	{
		if(!mIsLoaded)
			return;

		for(auto& entry : mClasses)
			cm_delete(entry.second);

		mClasses.clear();

		if(mMonoImage != nullptr)
		{
			mono_image_close(mMonoImage);
			mMonoImage = nullptr;
		}

		mIsLoaded = false;
		mMonoAssembly = nullptr;
	}

	void MonoAssembly::initialize(const CM::String& entryPoint)
	{
		MonoMethodDesc* methodDesc = mono_method_desc_new(entryPoint.c_str(), false);
		::MonoMethod* entry = mono_method_desc_search_in_image(methodDesc, mMonoImage);

		if(entry != nullptr)
		{
			MonoObject* exception = nullptr;
			mono_runtime_invoke(entry, nullptr, nullptr, &exception);

			MonoUtil::throwIfException(exception);
		}
	}

	MonoClass* MonoAssembly::getClass(const String& namespaceName, const String& name) const
	{
		if(!mIsLoaded)
			CM_EXCEPT(InvalidStateException, "Trying to use an unloaded assembly.");

		ClassId classId(namespaceName, name);
		auto iterFind = mClasses.find(classId);

		if(iterFind != mClasses.end())
			return iterFind->second;

		return nullptr;
	}

	CM::Vector<MonoClass*>::type MonoAssembly::getAllClasses() const
	{
		CM::Vector<MonoClass*>::type classes;
		classes.reserve(classes.size());

		for(auto& curClass : mClasses)
			classes.push_back(curClass.second);

		return classes;
	}
}
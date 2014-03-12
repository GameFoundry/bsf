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
		:mIsLoaded(false), mMonoImage(nullptr), mMonoAssembly(nullptr), mIsDependency(false)
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

		mIsLoaded = true;
		mIsDependency = false;
	}

	void MonoAssembly::loadAsDependency(MonoImage* image, const CM::String& name)
	{
		::MonoAssembly* monoAssembly = mono_image_get_assembly(image);
		if(monoAssembly == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot get assembly from image.");
		}

		mName = name;
		mMonoAssembly = monoAssembly;
		mMonoImage = image;

		mIsLoaded = true;
		mIsDependency = true;
	}

	void MonoAssembly::unload()
	{
		if(!mIsLoaded)
			return;

		for(auto& entry : mClasses)
			cm_delete(entry.second);

		mClasses.clear();

		if(mMonoImage != nullptr && !mIsDependency)
		{
			mono_image_close(mMonoImage);
			mMonoImage = nullptr;
		}

		mIsLoaded = false;
		mMonoAssembly = nullptr;
		mHaveCachedClassList = false;
	}

	void MonoAssembly::initialize(const CM::String& entryPoint)
	{
		MonoMethodDesc* methodDesc = mono_method_desc_new(entryPoint.c_str(), false);

		if(methodDesc != nullptr)
		{
			::MonoMethod* entry = mono_method_desc_search_in_image(methodDesc, mMonoImage);

			if(entry != nullptr)
			{
				MonoObject* exception = nullptr;
				mono_runtime_invoke(entry, nullptr, nullptr, &exception);

				MonoUtil::throwIfException(exception);
			}
		}
	}

	MonoClass* MonoAssembly::getClass(const String& namespaceName, const String& name) const
	{
		if(!mIsLoaded)
			CM_EXCEPT(InvalidStateException, "Trying to use an unloaded assembly.");

		MonoAssembly::ClassId classId(namespaceName, name);
		auto iterFind = mClasses.find(classId);

		if(iterFind != mClasses.end())
			return iterFind->second;

		::MonoClass* monoClass = mono_class_from_name(mMonoImage, namespaceName.c_str(), name.c_str());
		if(monoClass == nullptr)
			return nullptr;

		MonoClass* newClass = new (cm_alloc<MonoClass>()) MonoClass(namespaceName, name, monoClass, this);
		mClasses[classId] = newClass;

		return newClass;
	}

	const CM::Vector<MonoClass*>::type& MonoAssembly::getAllClasses() const
	{
		if(mHaveCachedClassList)
			return mCachedClassList;

		mCachedClassList.clear();

		int numRows = mono_image_get_table_rows (mMonoImage, MONO_TABLE_TYPEDEF);

		for(int i = 1; i < numRows; i++) // Skip Module
		{
			::MonoClass* monoClass = mono_class_get (mMonoImage, (i + 1) | MONO_TOKEN_TYPE_DEF);

			String ns = mono_class_get_namespace(monoClass);
			String type = mono_class_get_name(monoClass);

			MonoClass* curClass = getClass(ns, type);

			if(curClass != nullptr)
				mCachedClassList.push_back(curClass);
		}

		mHaveCachedClassList = true;

		return mCachedClassList;
	}
}
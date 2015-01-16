#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace BansheeEngine
{
	/**
	 * @brief	Contains information about a single Mono (i.e. managed) assembly
	 */
	class BS_MONO_EXPORT MonoAssembly
	{
	public:
		/**
		 * @brief	Used for uniquely identifying a managed class, normally for use
		 *			in containers.
		 */
		struct ClassId
		{
			struct Hash
			{
				inline size_t operator()(const ClassId& v) const;
			};

			struct Equals
			{
				inline bool operator()(const ClassId &a, const ClassId &b) const;
			};

			ClassId();
			ClassId(const String& namespaceName, String name, ::MonoClass* genericInstance = nullptr);

			String namespaceName;
			String name;
			::MonoClass* genericInstance;
		};

	public:
		virtual ~MonoAssembly();

		/**
		 * @brief	Returns the name of this assembly.
		 */
		const String& getName() const { return mName; }

		/**
	     * @brief	Attempts to find a managed class with the specified namespace and name
		 *			in this assembly. Returns null if one cannot be found.
	     */
		MonoClass* getClass(const String& namespaceName, const String& name) const;

		/**
		 * @brief	Converts an internal mono representation of a class into engine class.
		 */
		MonoClass* getClass(::MonoClass* rawMonoClass) const;

		/**
		 * @brief	Returns a list of all classes in the assembly.
		 */
		const Vector<MonoClass*>& getAllClasses() const;

		/**
	     * @brief	Invokes a zero-parameter static method in the format
		 *			"Class::Method". Used primarily for invoking an assembly entry point.
	     */
		void invoke(const String& functionName);

	private:
		friend class MonoManager;

		MonoAssembly(const String& path, const String& name);

		/**
	     * @brief	Attempts to find a managed class with the specified namespace and name
		 *			in this assembly. Registers a new class using the provided raw class if
		 *			one cannot be found. Returns null provided raw class is null.
	     */
		MonoClass* getClass(const String& namespaceName, const String& name, ::MonoClass* rawMonoClass) const;

		/**
		 * @brief	Loads an assembly into the specified domain.
		 */
		void load(MonoDomain* domain);

		/**
		 * @brief	Initializes an assembly from an internal mono image.
		 *
		 * @note	Normally used for assemblies that were already loaded by the managed runtime
		 *			as dependencies.
		 */
		void loadFromImage(MonoImage* image);

		/**
		 * @brief	Unloads the assembly and all the types associated with it.
		 *			Caller must ensure not to use any types from this assembly after
		 *			it has been unloaded.
		 */
		void unload();

		/**
	     * @brief	Returns true if the provided name represents a generic class.
		 *
		 * @note	This method only analyzes the name to determine if it is in generic class format.
	     */
		bool isGenericClass(const String& name) const;

		String mName;
		String mPath;
		MonoImage* mMonoImage;
		::MonoAssembly* mMonoAssembly;
		bool mIsLoaded;
		bool mIsDependency;
		
		mutable UnorderedMap<ClassId, MonoClass*, ClassId::Hash, ClassId::Equals> mClasses;
		mutable UnorderedMap<::MonoClass*, MonoClass*> mClassesByRaw;

		mutable bool mHaveCachedClassList;
		mutable Vector<MonoClass*> mCachedClassList;
	};
}
//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsMonoUtil.h"
#include "BsScriptRRefBase.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Base class for all resource interop objects. */
	class BS_SCR_BE_EXPORT ScriptResourceBase : public PersistentScriptObjectBase
	{
	public:
		/**	Returns a generic handle to the internal wrapped resource. */
		virtual HResource getGenericHandle() const = 0;

		/**	Sets the internal resource this object wraps. */
		virtual void setResource(const HResource& resource) = 0;

		/** Returns the managed version of this resource. */
		MonoObject* getManagedInstance() const;

		/**
		 * Returns a reference wrapper for the provided resource.
		 *
		 * @param[in]	resource	Handle to the resource to retrieve the reference wrapper for.
		 * @param[in]	rttiId		Type ID for the resource type to create a RRef for.
		 */
		static MonoObject* getRRef(const HResource& resource, UINT32 rttiId);

		/**
		 * Maps a RTTI ID to a class representing the specified resource type in managed code. Returns null if the ID
		 * cannot be mapped to a managed resource class.
		 */
		static ::MonoClass* getManagedResourceClass(UINT32 rttiId);

		/**
		 * Returns a RRef<T> type that can be used for wrapping a resource of the type represented by the provided
		 * RTTI ID.
		 */
		static ::MonoClass* getRRefClass(UINT32 rttiId);
	protected:
		friend class ScriptResourceManager;

		ScriptResourceBase(MonoObject* instance);
		virtual ~ScriptResourceBase();

		/**
		 * Makes the object reference the specific managed instance. Internally this allocates a GC handle that keeps a
		 * reference to the object and allows getManagedInstance to retrieve the managed instance when requested. Should
		 * be called on initial creation and whenever the managed instance changes (e.g. after assembly refresh). This
		 * creates a strong reference to the managed instance, and you need to make sure to release it with
		 * freeManagedInstance() when no longer required.
		 */
		void setManagedInstance(MonoObject* instance);

		/**
		 * Frees a managed instace assigned with setManagedInstance(). Should be called before the object is destroyed or
		 * when you changing the managed instance it points to (in order to release the previous instance).
		 */
		void freeManagedInstance();

		/**	
		 * Triggered by the script resource managed when the native resource handle this object point to has been destroyed.
		 */
		virtual void notifyResourceDestroyed() { }

		/** Destroys the interop object, unless refresh is in progress in which case it is just prepared for re-creation. */
		void destroy();

		UINT32 mGCHandle = 0;
	};

	/**	Base class for a specific resource's interop object. */
	template<class ScriptClass, class ResType, class BaseType = ScriptResourceBase>
	class BS_SCR_BE_EXPORT TScriptResource : public ScriptObject <ScriptClass, BaseType>
	{
	public:
		/**	Returns a generic handle to the internal wrapped resource. */
		HResource getGenericHandle() const override { return mResource; }

		/**	Sets the internal resource this object wraps. */
		void setResource(const HResource& resource) override { mResource = static_resource_cast<ResType>(resource); }

		/**	Returns a handle to the internal wrapped resource. */
		const ResourceHandle<ResType>& getHandle() const { return mResource; }

		/** Returns a reference wrapper for this resource. */
		MonoObject* getRRef() const
		{
			return ScriptResourceBase::getRRef(mResource, ResType::getRTTIStatic()->getRTTIId());
		}

	protected:
		friend class ScriptResourceManager;

		TScriptResource(MonoObject* instance, const ResourceHandle<ResType>& resource)
			:ScriptObject<ScriptClass, BaseType>(instance), mResource(resource)
		{
			this->setManagedInstance(instance);
		}

		virtual ~TScriptResource() {}

		/** @copydoc ScriptObject::_createManagedInstance */
		MonoObject* _createManagedInstance(bool construct) override
		{
			MonoObject* managedInstance = ScriptClass::metaData.scriptClass->createInstance(construct);
			this->setManagedInstance(managedInstance);

			return managedInstance;
		}

		/** @copydoc ScriptObjectBase::_clearManagedInstance */
		void _clearManagedInstance() override
		{
			this->freeManagedInstance();
		}

		/**	
		 * Triggered by the script resource managed when the native resource handle this object point to has been destroyed.
		 */
		void notifyResourceDestroyed() override
		{
			this->freeManagedInstance();
		}

		/**	Called when the managed instance gets finalized by the CLR. */
		void _onManagedInstanceDeleted(bool assemblyRefresh) override
		{
			this->freeManagedInstance();
			this->destroy();
		}

		ResourceHandle<ResType> mResource;
	};

	/**	Interop class between C++ & CLR for Resource. */
	class BS_SCR_BE_EXPORT ScriptResource : public ScriptObject<ScriptResource, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "Resource")

	private:
		ScriptResource(MonoObject* instance)
			:ScriptObject(instance)
		{ }

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoString* internal_getName(ScriptResourceBase* nativeInstance);
		static void internal_getUUID(ScriptResourceBase* nativeInstance, UUID* uuid);
		static void internal_release(ScriptResourceBase* nativeInstance);
	};

	/**	Interop class between C++ & CLR for UUID. */
	class BS_SCR_BE_EXPORT ScriptUUID : public ScriptObject<ScriptUUID>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "UUID")

		/**	Unboxes a boxed managed UUID struct and returns the native version of the structure. */
		static UUID unbox(MonoObject* obj);

		/**	Boxes a native UUID struct and returns a managed object containing it. */
		static MonoObject* box(const UUID& value);

	private:
		ScriptUUID(MonoObject* instance);
	};

	/** @} */
}

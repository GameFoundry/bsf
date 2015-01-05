#pragma once

#include "BsPrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"
#include "BsIResourceListener.h"
#include "BsBounds.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Signals which portion of a RenderableHandler is dirty.
	 */
	enum class RenderableDirtyFlag
	{
		Transform = 0x01,
		Everything = 0x02
	};

	/**
	 * @brief	Renderable represents any visible object in the scene. It has a mesh,
	 *			bounds and a set of materials. Renderer will render any Renderable objects
	 *			visible by the camera.
	 */
	template<bool Core>
	class BS_EXPORT TRenderableHandler
	{
		template<bool Core> struct TMeshType {};
		template<> struct TMeshType < false > { typedef HMesh Type; };
		template<> struct TMeshType < true > { typedef SPtr<MeshCore> Type; };

		template<bool Core> struct TMaterialType {};
		template<> struct TMaterialType < false > { typedef HMaterial Type; };
		template<> struct TMaterialType < true > { typedef SPtr<MaterialCore> Type; };

		typedef typename TMeshType<Core>::Type MeshType;
		typedef typename TMaterialType<Core>::Type MaterialType;

	public:
		TRenderableHandler();
		virtual ~TRenderableHandler();

		/**
		 * @brief	Sets the mesh to render. All sub-meshes of the mesh will be rendered,
		 *			and you may set individual materials for each sub-mesh.
		 */
		void setMesh(const MeshType& mesh);

		/**
		 * @brief	Sets a material that will be used for rendering a sub-mesh with
		 *			the specified index. If a sub-mesh doesn't have a specific material set
		 *			then the primary material will be used.
		 */
		void setMaterial(UINT32 idx, const MaterialType& material);

		/**
		 * @brief	Sets the primary material to use for rendering. Any sub-mesh that
		 *			doesn't have an explicit material set will use this material.
		 *
		 * @note	This is equivalent to calling setMaterial(0, material).
		 */
		void setMaterial(const MaterialType& material);

		/**
		 * @brief	Sets the layer bitfield. Renderable layer must match camera layer
		 *			in order for the camera to render the component.
		 */
		void setLayer(UINT64 layer);

		/**
		 * @brief	Sets the transform matrix that is applied to the object
		 *			when its being rendered.
		 */
		void setTransform(const Matrix4& transform);

		/**
		 * @brief	Sets the world position of the renderable.
		 */
		void setPosition(const Vector3& position);

		/**
		 * @brief	Sets whether the object should be rendered or not.
		 */
		void setIsActive(bool active);

		/**
		 * @brief	Gets the layer bitfield. Renderable layer must match camera layer
		 *			in order for the camera to render the component.
		 */
		UINT64 getLayer() const { return mLayer; }

		/**
		 * @brief	Returns the mesh used for rendering.
		 */
		MeshType getMesh() const { return mMesh; }

		/**
		 * @brief	Returns the material used for rendering a sub-mesh with
		 *			the specified index.
		 */
		MaterialType getMaterial(UINT32 idx) const { return mMaterials[idx]; }

		/**
		 * @brief	Returns the transform matrix that is applied to the object
		 *			when its being rendered.
		 */
		Matrix4 getTransform() const { return mTransform; }

		/**
		 * @brief	Gets whether the object should be rendered or not.
		 */
		bool getIsActive() const { return mIsActive; }

		/**
		 * @brief	Retrieves the world position of the renderable.
		 */
		Vector3 getPosition() const { return mPosition; }

	protected:
		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty(RenderableDirtyFlag flag = RenderableDirtyFlag::Everything) { }

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 */
		virtual void _markResourcesDirty() { }

		MeshType mMesh;
		Vector<MaterialType> mMaterials;
		UINT64 mLayer;
		Vector<AABox> mWorldBounds;
		Vector3 mPosition;
		Matrix4 mTransform;
		bool mIsActive;
	};

	/**
	 * @copydoc	TRenderableHandler
	 */
	class BS_EXPORT RenderableHandlerCore : public CoreObjectCore, public TRenderableHandler<true>
	{
	public:
		~RenderableHandlerCore();

		/**
		 * @brief	Gets world bounds of the mesh rendered by this object.
		 */
		Bounds getBounds() const;

		/**
		 * @brief	Returns the type that controls how is this object rendered.
		 */
		RenderableType getRenderableType() const { return RenType_LitTextured; }

		/**
		 * @brief	Sets an ID that can be used for uniquely identifying this handler by the renderer.
		 */
		void setRendererId(UINT32 id) { mRendererId = id; }

		/**
		 * @brief	Retrieves an ID that can be used for uniquely identifying this handler by the renderer.
		 */
		UINT32 getRendererId() const { return mRendererId; }

	protected:
		friend class RenderableHandler;

		RenderableHandlerCore();

		/**
		 * @copydoc	RenderableHandlerCore::initialize
		 */
		void initialize();

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		void syncToCore(const CoreSyncData& data);

		UINT32 mRendererId;
	};

	/**
	 * @copydoc	TRenderableHandler
	 */
	class BS_EXPORT RenderableHandler : public IReflectable, public CoreObject, public TRenderableHandler<false>, public IResourceListener
	{
	public:
		/**
		 * @brief	Gets world bounds of the mesh rendered by this object.
		 */
		Bounds getBounds() const;

		/**
		 * @brief	Retrieves an implementation of a renderable handler usable only from the
		 *			core thread.
		 */
		SPtr<RenderableHandlerCore> getCore() const;

		/**
		 * @brief	Creates a new renderable handler instance.
		 */
		static RenderableHandlerPtr create();

	protected:
		RenderableHandler() { }

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty(RenderableDirtyFlag flag = RenderableDirtyFlag::Everything);

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 */
		void _markResourcesDirty();

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies);

		/**
		 * @copydoc	IResourceListener::getResourceDependencies
		 */
		void getListenerResources(Vector<HResource>& resources);

		/**
		 * @copydoc IResourceListener::notifyResourceLoaded
		 */
		void notifyResourceLoaded(const HResource& resource) { markCoreDirty(); }

		/**
		 * @copydoc IResourceListener::notifyResourceDestroyed
		 */
		void notifyResourceDestroyed(const HResource& resource) { markCoreDirty(); }

		/**
		 * @copydoc IResourceListener::notifyResourceChanged
		 */
		void notifyResourceChanged(const HResource& resource) { markCoreDirty(); }

		/**
		 * @brief	Creates a new renderable handler instance without initializing it.
		 */
		static RenderableHandlerPtr createEmpty();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableHandlerRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}
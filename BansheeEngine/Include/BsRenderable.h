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
	 * @brief	Signals which portion of a Renderable is dirty.
	 */
	enum class RenderableDirtyFlag
	{
		Transform = 0x01,
		Everything = 0x02
	};

	/**
	 * @brief	Renderable represents any visible object in the scene. It has a mesh,
	 *			bounds and a set of materials. Renderer will render any Renderable objects
	 *			visible by a camera.
	 */
	template<bool Core>
	class BS_EXPORT TRenderable
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
		TRenderable();
		virtual ~TRenderable();

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
		 * @brief	Returns all materials used for rendering this renderable. Each of the materials is used for rendering
		 * 			a single sub-mesh.
		 */
		const Vector<MaterialType>& getMaterials() { return mMaterials; }

		/**
		 * @brief	Sets all materials used for rendering this renderable. Each of the materials is used for rendering
		 * 			a single sub-mesh. If number of materials is larger than number of sub-meshes, they will be ignored.
		 * 			If lower, the remaining materials will be removed.
		 */
		void setMaterials(const Vector<MaterialType>& materials);

		/**
		 * @brief	Sets the layer bitfield that controls whether a renderable is considered 
		 *			visible in a specific camera. Renderable layer must match camera layer
		 *			in order for the camera to render the component.
		 */
		void setLayer(UINT64 layer);

		/**
		 * @brief	Sets the transform matrix that is applied to the object
		 *			when its being rendered.
		 */
		void setTransform(const Matrix4& transform, const Matrix4& transformNoScale);

		/**
		 * @brief	Sets the world position of the renderable.
		 */
		void setPosition(const Vector3& position);

		/**
		 * @brief	Sets whether the object should be rendered or not.
		 */
		void setIsActive(bool active);

		/**
		 * @brief	Gets the layer bitfield that controls whether a renderable is considered 
		 *			visible in a specific camera. Renderable layer must match camera layer
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
		 * @brief	Returns the transform matrix that is applied to the object
		 *			when its being rendered. This transform matrix does not
		 *			include scale values.
		 */
		Matrix4 getTransformNoScale() const { return mTransformNoScale; }

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
		 * @copydoc	CoreObject::markDependenciesDirty
		 */
		virtual void _markDependenciesDirty() { }

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
		Matrix4 mTransformNoScale;
		bool mIsActive;
	};

	/**
	 * @copydoc	TRenderable
	 */
	class BS_EXPORT RenderableCore : public CoreObjectCore, public TRenderable<true>
	{
	public:
		~RenderableCore();

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
		friend class Renderable;

		RenderableCore();

		/**
		 * @copydoc	CoreObject::initialize
		 */
		void initialize() override;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		void syncToCore(const CoreSyncData& data) override;

		UINT32 mRendererId;
	};

	/**
	 * @copydoc	TRenderable
	 */
	class BS_EXPORT Renderable : public IReflectable, public CoreObject, public TRenderable<false>, public IResourceListener
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
		SPtr<RenderableCore> getCore() const;

		/**
	     * @brief	Returns the hash value that can be used to identify if the internal data needs an update.
		 */
		UINT32 _getLastModifiedHash() const { return mLastUpdateHash; }

		/**
	     * @brief	Sets the hash value that can be used to identify if the internal data needs an update.
		 */
		void _setLastModifiedHash(UINT32 hash) { mLastUpdateHash = hash; }

		/**
		 * @brief	Creates a new renderable handler instance.
		 */
		static RenderablePtr create();

	protected:
		Renderable();

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const override;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty(RenderableDirtyFlag flag = RenderableDirtyFlag::Everything) override;

		/**
		* @copydoc	IResourceListener::markResourcesDirty
		*/
		void _markResourcesDirty() override;

		/**
		 * @copydoc	CoreObject::markDependenciesDirty
		 */
		void _markDependenciesDirty() override;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<CoreObject*>& dependencies) override;

		/**
		 * @copydoc	IResourceListener::getListenerResources
		 */
		void getListenerResources(Vector<HResource>& resources) override;

		/**
		 * @copydoc IResourceListener::notifyResourceLoaded
		 */
		void notifyResourceLoaded(const HResource& resource) override;

		/**
		 * @copydoc IResourceListener::notifyResourceDestroyed
		 */
		void notifyResourceDestroyed(const HResource& resource) override;

		/**
		 * @copydoc IResourceListener::notifyResourceChanged
		 */
		void notifyResourceChanged(const HResource& resource) override;

		/**
		 * @brief	Creates a new renderable handler instance without initializing it.
		 */
		static RenderablePtr createEmpty();

		UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}
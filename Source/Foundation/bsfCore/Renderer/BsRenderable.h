//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsIReflectable.h"
#include "CoreThread/BsCoreObject.h"
#include "Resources/BsIResourceListener.h"
#include "Math/BsBounds.h"
#include "Math/BsAABox.h"
#include "Scene/BsSceneActor.h"

namespace bs
{
	struct EvaluatedAnimationData;

	/** @addtogroup Implementation
	 *  @{
	 */

	/** Type of animation that can be applied to a renderable object. */
	enum class RenderableAnimType
	{
		None,
		Skinned,
		Morph,
		SkinnedMorph,
		Count // Keep at end
	};

	/**
	 * Renderable represents any visible object in the scene. It has a mesh, bounds and a set of materials. Renderer will
	 * render any Renderable objects visible by a camera.
	 */
	template<bool Core>
	class BS_CORE_EXPORT TRenderable : public SceneActor
	{
		using MeshType = CoreVariantHandleType<Mesh, Core>;
		using MaterialType = CoreVariantHandleType<Material, Core>;

	public:
		TRenderable();
		virtual ~TRenderable() = default;

		/** @copydoc bs::SceneActor::setTransform */
		void setTransform(const Transform& transform) override;

		/**
		 * Determines the mesh to render. All sub-meshes of the mesh will be rendered, and you may set individual materials
		 * for each sub-mesh.
		 */
		void setMesh(const MeshType& mesh);

		/**
		 * Sets a material that will be used for rendering a sub-mesh with the specified index. If a sub-mesh doesn't have
		 * a specific material set then the primary material will be used.
		 */
		void setMaterial(UINT32 idx, const MaterialType& material);

		/**
		 * Sets the primary material to use for rendering. Any sub-mesh that doesn't have an explicit material set will use
		 * this material.
		 *
		 * @note	This is equivalent to calling setMaterial(0, material).
		 */
		void setMaterial(const MaterialType& material);

		/** @copydoc setMaterials() */
		const Vector<MaterialType>& getMaterials() { return mMaterials; }

		/**
		 * Determines all materials used for rendering this renderable. Each of the materials is used for rendering a single
		 * sub-mesh. If number of materials is larger than number of sub-meshes, they will be ignored. If lower, the
		 * remaining materials will be removed.
		 */
		void setMaterials(const Vector<MaterialType>& materials);

		/**
		 * Determines the layer bitfield that controls whether a renderable is considered visible in a specific camera. 
		 * Renderable layer must match camera layer in order for the camera to render the component.
		 */
		void setLayer(UINT64 layer);

		/** 
		 * Sets bounds that will be used when determining if object is visible. Only relevant if setUseOverrideBounds() is
		 * set to true.
		 *
		 * @param[in]	bounds	Bounds in local space.
		 */
		void setOverrideBounds(const AABox& bounds);

		/**
		 * Enables or disables override bounds. When enabled the bounds provided to setOverrideBounds() will be used for
		 * determining object visibility, otherwise the bounds from the object's mesh will be used. Disabled by default.
		 */
		void setUseOverrideBounds(bool enable);

		/** @copydoc setLayer() */
		UINT64 getLayer() const { return mLayer; }

		/**	@copydoc setMesh() */
		MeshType getMesh() const { return mMesh; }

		/**	Returns the material used for rendering a sub-mesh with the specified index. */
		MaterialType getMaterial(UINT32 idx) const { return mMaterials[idx]; }

		/**	Returns the transform matrix that is applied to the object when its being rendered. */
		Matrix4 getMatrix() const { return mTfrmMatrix; }

		/**
		 * Returns the transform matrix that is applied to the object when its being rendered. This transform matrix does 
		 * not include scale values.
		 */
		Matrix4 getMatrixNoScale() const { return mTfrmMatrixNoScale; }

	protected:
		/**
		 * Notifies the core object manager that this object is dependant on some other CoreObject(s), and the dependencies
		 * changed since the last call to this method. This will trigger a call to getCoreDependencies() to collect the 
		 * new dependencies.
		 */
		virtual void _markDependenciesDirty() { }

		/**	Marks the resource dependencies list as dirty and schedules it for rebuild. */
		virtual void _markResourcesDirty() { }

		/** Triggered whenever the renderable's mesh changes. */
		virtual void onMeshChanged() { }

		MeshType mMesh;
		Vector<MaterialType> mMaterials;
		UINT64 mLayer = 1;
		AABox mOverrideBounds;
		bool mUseOverrideBounds = false;
		Matrix4 mTfrmMatrix = BsIdentity;
		Matrix4 mTfrmMatrixNoScale = BsIdentity;
		RenderableAnimType mAnimType = RenderableAnimType::None;
	};

	/** @} */

	/** @addtogroup Renderer-Internal
	 *  @{
	 */

	/** @copydoc TRenderable */
	class BS_CORE_EXPORT Renderable : public IReflectable, public CoreObject, public TRenderable<false>, public IResourceListener
	{
	public:
		/**	Gets world bounds of the mesh rendered by this object. */
		Bounds getBounds() const;

		/** Determines the animation that will be used for animating the attached mesh. */
		void setAnimation(const SPtr<Animation>& animation);

		/** @copydoc setAnimation */
		const SPtr<Animation>& getAnimation() const { return mAnimation; }

		/** Checks is the renderable animated or static. */
		bool isAnimated() const { return mAnimation != nullptr; }

		/**	Retrieves an implementation of a renderable handler usable only from the core thread. */
		SPtr<ct::Renderable> getCore() const;

		/**	Creates a new renderable handler instance. */
		static SPtr<Renderable> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc SceneActor::_updateState */
		void _updateState(const SceneObject& so, bool force = false) override;

		/** @copydoc CoreObject::initialize() */
		void initialize() override;

		/** @} */
	protected:
		/** @copydoc CoreObject::createCore */
		SPtr<ct::CoreObject> createCore() const override;

		/** @copydoc TRenderable::onMeshChanged */
		void onMeshChanged() override;

		/** Updates animation properties depending on the current mesh. */
		void refreshAnimation();

		/** @copydoc TRenderable::_markCoreDirty */
		void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

		/** @copydoc TRenderable::_markResourcesDirty */
		void _markResourcesDirty() override;

		/** @copydoc CoreObject::markDependenciesDirty */
		void _markDependenciesDirty() override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/** @copydoc CoreObject::getCoreDependencies */
		void getCoreDependencies(Vector<CoreObject*>& dependencies) override;

		/** @copydoc CoreObject::onDependencyDirty */
		void onDependencyDirty(CoreObject* dependency, UINT32 dirtyFlags) override;

		/** @copydoc IResourceListener::getListenerResources */
		void getListenerResources(Vector<HResource>& resources) override;

		/** @copydoc IResourceListener::notifyResourceLoaded */
		void notifyResourceLoaded(const HResource& resource) override;

		/** @copydoc IResourceListener::notifyResourceChanged */
		void notifyResourceChanged(const HResource& resource) override;

		/**	Creates a new renderable instance without initializing it. */
		static SPtr<Renderable> createEmpty();

		SPtr<Animation> mAnimation;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	namespace ct
	{
	/** @copydoc TRenderable */
	class BS_CORE_EXPORT Renderable : public CoreObject, public TRenderable<true>
	{
	public:
		~Renderable();

		/**	Gets world bounds of the mesh rendered by this object. */
		Bounds getBounds() const;

		/**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
		void setRendererId(UINT32 id) { mRendererId = id; }

		/**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
		UINT32 getRendererId() const { return mRendererId; }

		/** Returns the type of animation influencing this renderable, if any. */
		RenderableAnimType getAnimType() const { return mAnimType; }

		/** Returns the identifier of the animation, if this object is animated using skeleton or blend shape animation. */
		UINT64 getAnimationId() const { return mAnimationId; }

		/** 
		 * Updates internal animation buffers from the contents of the provided animation data object. Does nothing if
		 * renderable is not affected by animation.
		 */
		void updateAnimationBuffers(const EvaluatedAnimationData& animData);

		/** Returns the GPU buffer containing element's bone matrices, if it has any. */
		const SPtr<GpuBuffer>& getBoneMatrixBuffer() const { return mBoneMatrixBuffer; }

		/** Returns the vertex buffer containing element's morph shape vertices, if it has any. */
		const SPtr<VertexBuffer>& getMorphShapeBuffer() const { return mMorphShapeBuffer; }

		/** Returns vertex declaration used for rendering meshes containing morph shape information. */
		const SPtr<VertexDeclaration>& getMorphVertexDeclaration() const { return mMorphVertexDeclaration; }

	protected:
		friend class bs::Renderable;

		Renderable();

		/** @copydoc CoreObject::initialize */
		void initialize() override;

		/** @copydoc CoreObject::syncToCore */
		void syncToCore(const CoreSyncData& data) override;

		/** Creates any buffers required for renderable animation. Should be called whenever animation properties change. */
		void createAnimationBuffers();

		UINT32 mRendererId;
		UINT64 mAnimationId;
		UINT32 mMorphShapeVersion;

		SPtr<GpuBuffer> mBoneMatrixBuffer;
		SPtr<VertexBuffer> mMorphShapeBuffer;
		SPtr<VertexDeclaration> mMorphVertexDeclaration;
	};
	}

	/** @} */
}

#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class GizmoManagerCore;

	/**
	 * @brief	Handles the rendering and picking of gizmo elements. Gizmos are icons and 3D objects
	 *			usually rendered in scene view for various visualization purposes (e.g. a Camera component
	 *			will have a gizmo that draws a Camera icon since otherwise it has no visual representation).
	 *			Aside from being rendered, gizmos can also be selected by the user as if they were normal
	 *			scene elements.
	 */
	class BS_ED_EXPORT GizmoManager : public Module<GizmoManager>
	{
	public:
		GizmoManager();
		~GizmoManager();

		/**
		 * @brief	Starts gizmo creation. All further call will be referencing this gizmo. 
		 *			Must be followed by a matching "endGizmo".
		 *
		 * @note	gizmoParent	Scene object this gizmo is attached to. Selecting the gizmo will
		 *			select this scene object.
		 */
		void startGizmo(const HSceneObject& gizmoParent);

		/**
		 * @brief	Ends gizmo creation. Must be called after a matching "startGizmo".
		 */
		void endGizmo();

		/**
		 * @brief	Changes the color of any further gizmo draw calls. 
		 */
		void setColor(const Color& color);

		/**
		 * @brief	Changes the transform that will be applied to meshes of any further gizmo draw calls. 
		 */
		void setTransform(const Matrix4& transform);

		/**
		 * @brief	If pickable is set to true, gizmo can be selected by the user, otherwise it will be
		 *			drawn but cannot be interacted with.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void setPickable(bool pickable) { mPickable = pickable; }

		/**
		 * @brief	Returns the currently set gizmo color.
		 */
		Color getColor() const { return mColor; }

		/**
		 * @brief	Returns the currently set gizmo transform.
		 */
		Matrix4 getTransform() const { return mTransform; }

		/**
		 * @brief	Draws an axis aligned cuboid.
		 *
		 * @param	position	Center of the cuboid.
		 * @param	extents		Radius of the cuboid in each axis.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawCube(const Vector3& position, const Vector3& extents);

		/**
		 * @brief	Draws a sphere.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawSphere(const Vector3& position, float radius);

		/**
		 * @brief	Draws a wireframe axis aligned cuboid.
		 *
		 * @param	position	Center of the cuboid.
		 * @param	extents		Radius of the cuboid in each axis.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawWireCube(const Vector3& position, const Vector3& extents);

		/**
		 * @brief	Draws a wireframe sphere represented by three discs.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawWireSphere(const Vector3& position, float radius);

		/**
		 * @brief	Draws a line between two points.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawLine(const Vector3& start, const Vector3& end);

		/**
		 * @brief	Draws a wireframe frustum.
		 *
		 * @param	position	Origin of the frustum, or the eye point.
		 * @param	aspect		Ratio of frustum width over frustum height.
		 * @param	FOV			Horizontal field of view in degrees.
		 * @param	near		Distance to the near frustum plane.
		 * @param	far			Distance to the far frustum plane.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far);

		/**
		 * @brief	Draws an icon that always faces the camera.
		 *
		 * @param	position	Position of the center of the icon.
		 * @param	image		Sprite image for the icon to draw.
		 * @param	fixedScale	If true then the icon will remain the same size
		 *						regardless of the distance from camera.
		 *
		 * @note	Must be called between startGizmo/endGizmo calls.
		 */
		void drawIcon(Vector3 position, HSpriteTexture image, bool fixedScale);

		/**
		 * @brief	Updates all the gizmo meshes to reflect all draw calls submitted since "clearGizmos".
		 *
		 * @note	Internal method.
		 */
		void update(const CameraHandlerPtr& camera);

		/**
		 * @brief	Queues all gizmos to be rendered for picking. Each gizmo is draw with a separate
		 *			color so we can identify them later.
		 *
		 * @param	camera				Camera to draw the gizmos on.
		 * @param	idxToColorCallback	Callback that assigns a unique color to each gizmo index.
		 *
		 * @note	Internal method.
		 */
		void renderForPicking(const CameraHandlerPtr& camera, std::function<Color(UINT32)> idxToColorCallback);

		/**
		 * @brief	Clears all gizmo data, but doesn't update the meshes
		 *			or the render data. (Calling update would create empty
		 *			meshes, but before calling update gizmos will still be drawn).
		 */
		void clearGizmos();

		/**
		 * @brief	Clears gizmo render data like meshes, but doesn't clear the original gizmo data.
		 *			(Calling update would just recreate the render data).
		 */
		void clearRenderData();

		/**
		 * @brief	Returns a scene object that was attached to a specific gizmo.
		 *
		 * @param	gizmoIdx	Index of the gizmo to look for.
		 */
		HSceneObject getSceneObject(UINT32 gizmoIdx);

	private:
		friend class GizmoManagerCore;

		/**
		 * @brief	Supported types of gizmo materials (i.e. shaders)
		 */
		enum class GizmoMaterial
		{
			Solid, Wire, Picking
		};

		/**
		 * @brief	Common data shared by all gizmo types.
		 */
		struct CommonData
		{
			UINT32 idx;
			Color color;
			Matrix4 transform;
			HSceneObject sceneObject;
			bool pickable;
		};

		/**
		 * @brief	Data required for rendering a cuboid gizmo.
		 */
		struct CubeData : public CommonData
		{
			Vector3 position;
			Vector3 extents;
		};

		/**
		 * @brief	Data required for rendering a sphere gizmo.
		 */
		struct SphereData : public CommonData
		{
			Vector3 position;
			float radius;
		};

		/**
		 * @brief	Data required for rendering a line gizmo.
		 */
		struct LineData : public CommonData
		{
			Vector3 start;
			Vector3 end;
		};

		/**
		 * @brief	Data required for rendering a frustum gizmo.
		 */
		struct FrustumData : public CommonData
		{
			Vector3 position;
			float aspect;
			Degree FOV;
			float near;
			float far;
		};

		/**
		 * @brief	Data required for rendering an icon gizmo.
		 */
		struct IconData : public CommonData
		{
			Vector3 position;
			bool fixedScale;
			HSpriteTexture texture;
		};

		/**
		 * @brief	Stores how many icons use a specific texture.
		 */
		struct IconRenderData
		{
			UINT32 count;
			SPtr<TextureCore> texture;
		};

		/**
		 * @brief	Data used for initializing the core thread equivalent of the gizmo manager.
		 */
		struct CoreInitData
		{
			SPtr<MaterialCore> solidMat;
			SPtr<MaterialCore> wireMat;
			SPtr<MaterialCore> iconMat;
			SPtr<MaterialCore> pickingMat;
			SPtr<MaterialCore> alphaPickingMat;
		};

		typedef Vector<IconRenderData> IconRenderDataVec;
		typedef std::shared_ptr<IconRenderDataVec> IconRenderDataVecPtr;

		/**
		 * @brief	Builds a brand new mesh that can be used for rendering all icon gizmos.
		 *
		 * @param	camera		Camera the mesh will be rendered to.
		 * @param	iconData	A list of all icons and their properties.
		 * @param	forPicking	Whether the icons will be rendered normally, or with a special material for picking.
		 * @param	renderData	Output data that outlines the structure of the returned mesh. It tells us which portions of the
		 *						mesh use which icon texture.
		 *
		 * @returns	A mesh containing all of the visible icons. Mesh is allocated using the icon mesh heap and should be deallocated manually.
		 */
		TransientMeshPtr buildIconMesh(const CameraHandlerPtr& camera, const Vector<IconData>& iconData, bool forPicking, IconRenderDataVecPtr& renderData);

		/**
		 * @brief	Resizes the icon width/height so it is always scaled to optimal size (with preserved aspect).
		 */
		void limitIconSize(UINT32& width, UINT32& height);

		/**
		 * @brief	Calculates colors for an icon based on its position in the camera. e.g. icons too close to too far might be faded.
		 *
		 * @param	tint		Primary tint for the icon.
		 * @param	camera		Camera in which the icon will be rendered in.
		 * @param	iconHeight	Height of the icon in pixels.
		 * @param	fixedScale	Whether the icon size changes depending on distance from the camera.
		 * @param	normalColor	Normal color of the icon.
		 * @param	fadedColor	Faded color to be used when icon is occluded by geometry.
		 */
		void calculateIconColors(const Color& tint, const CameraHandlerPtr& camera, UINT32 iconHeight, bool fixedScale,
			Color& normalColor, Color& fadedColor);

		/**
		 * @brief	Initializes the core thread version of the gizmo manager.
		 */
		void initializeCore(const CoreInitData& initData);

		/**
		 * @brief	Destroys the core thread version of the gizmo manager.
		 */
		void destroyCore(GizmoManagerCore* core);

		static const UINT32 VERTEX_BUFFER_GROWTH;
		static const UINT32 INDEX_BUFFER_GROWTH;
		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const float MAX_ICON_RANGE;
		static const UINT32 OPTIMAL_ICON_SIZE;
		static const float ICON_TEXEL_WORLD_SIZE;

		typedef Set<IconData, std::function<bool(const IconData&, const IconData&)>> IconSet;

		Color mColor;
		Matrix4 mTransform;
		HSceneObject mActiveSO;
		bool mPickable;
		UINT32 mCurrentIdx;

		DrawHelper* mDrawHelper;
		DrawHelper* mPickingDrawHelper;

		Vector<CubeData> mSolidCubeData;
		Vector<CubeData> mWireCubeData;
		Vector<SphereData> mSolidSphereData;
		Vector<SphereData> mWireSphereData;
		Vector<LineData> mLineData;
		Vector<FrustumData> mFrustumData;
		Vector<IconData> mIconData;
		Map<UINT32, HSceneObject> mIdxToSceneObjectMap;

		MeshHeapPtr mIconMeshHeap;

		TransientMeshPtr mIconMesh;

		std::atomic<GizmoManagerCore*> mCore;

		// Immutable
		VertexDataDescPtr mIconVertexDesc;

		// Transient
		struct SortedIconData
		{
			float distance;
			Vector2I screenPosition;
			UINT32 iconIdx;
		};

		Vector<SortedIconData> mSortedIconData;
	};

	/**
	 * @brief	Core thread version of the gizmo manager that handles most of the rendering
	 *			of meshes provided by the gizmo manager.
	 */
	class GizmoManagerCore
	{
		friend class GizmoManager;

		/**
		 * @brief	Solid gizmo material and parameter handles.
		 */
		struct SolidMaterialData
		{
			SPtr<MaterialCore> mat;
			GpuParamMat4Core mViewProj;
			GpuParamVec4Core mViewDir;
		};

		/**
		 * @brief	Wire gizmo material and parameter handles.
		 */
		struct WireMaterialData
		{
			SPtr<MaterialCore> mat;
			GpuParamMat4Core mViewProj;
		};

		/**
		 * @brief	Icon gizmo material and parameter handles.
		 */
		struct IconMaterialData
		{
			SPtr<MaterialCore> mat;
			SPtr<GpuParamsCore> mFragParams[2];
			GpuParamMat4Core mViewProj[2];
			GpuParamTextureCore mTexture[2];
		};

		/**
		 * @brief	Gizmo material and parameter handles used for picking.
		 */
		struct PickingMaterialData
		{
			SPtr<MaterialCore> mat;
			GpuParamMat4Core mViewProj;
		};

		/**
		 * @brief	Gizmo material and parameter handles used for picking, with
		 *			blending support (generally used for icon picking).
		 */
		struct AlphaPickingMaterialData
		{
			SPtr<MaterialCore> mat;
			SPtr<GpuParamsCore> mFragParams;
			GpuParamMat4Core mViewProj;
			GpuParamTextureCore mTexture;
		};

		struct PrivatelyConstuct { };

	public:
		GizmoManagerCore(const PrivatelyConstuct& dummy);

	private:
		/**
		 * @brief	Initializes the core gizmo manager. Must be called right after creation.
		 */
		void initialize(const GizmoManager::CoreInitData& initData);

		/**
		 * @brief	Renders all gizmos in the parent camera.
		 */
		void render();

		/**
		 * @brief	Renders a non-icon gizmo mesh using the provided parameters.
		 *
		 * @param	viewMatrix	View matrix of the camera we are rendering with.
		 * @param	projMatrix	Projection matrix of the camera we are rendering with.
		 * @param	viewDir		View direction of the camera we are rendering with.
		 * @param	mesh		Mesh to render. This is normally the solid or wireframe gizmo mesh.
		 * @param	material	Material to use for rendering. This is normally the solid, wireframe or picking material.
		 */
		void renderGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, Vector3 viewDir, SPtr<MeshCoreBase> mesh, GizmoManager::GizmoMaterial material);

		/**
		 * @brief	Renders the icon gizmo mesh using the provided parameters.
		 *
		 * @param	screenArea			Area of the viewport to render the gizmos in, in pixels.
		 * @param	mesh				Mesh containing the icons.
		 * @param	renderData			Icon render data outlining which parts of the icon mesh use which textures.
		 * @param	usePickingMaterial	Should the icons be rendered normally or for picking.
		 */
		void renderIconGizmos(Rect2I screenArea, SPtr<MeshCoreBase> mesh, GizmoManager::IconRenderDataVecPtr renderData, bool usePickingMaterial);

		/**
		 * @brief	Updates the internal data that is used for rendering. Normally you would call this after 
		 *			updating the camera or meshes on the sim thread.
		 *
		 * @param	camera			Sets the camera all rendering will be performed to.
		 * @param	solidMesh		Mesh containing solid gizmos.
		 * @param	wireMesh		Mesh containing wireframe gizmos.
		 * @param	iconMesh		Mesh containing icon meshes.
		 * @param	iconRenderData	Icon render data outlining which parts of the icon mesh use which textures.
		 */
		void updateData(const SPtr<CameraHandlerCore>& camera, const SPtr<MeshCoreBase>& solidMesh, const SPtr<MeshCoreBase>& wireMesh,
			const SPtr<MeshCoreBase>& iconMesh, const GizmoManager::IconRenderDataVecPtr& iconRenderData);

		static const float PICKING_ALPHA_CUTOFF;

		SPtr<CameraHandlerCore> mCamera;

		SPtr<MeshCoreBase> mSolidMesh;
		SPtr<MeshCoreBase> mWireMesh;
		SPtr<MeshCoreBase> mIconMesh;
		GizmoManager::IconRenderDataVecPtr mIconRenderData;

		// Immutable
		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;
		IconMaterialData mIconMaterial;

		PickingMaterialData mPickingMaterial;
		AlphaPickingMaterialData mAlphaPickingMaterial;
	};
}
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	class HandleDrawManagerCore;

	/**
	 * @brief	Allows you to easily draw various kinds of simple shapes, primarily
	 *			used for drawing handles in the scene view.
	 *
	 *			Drawn elements only persist for a single draw call and need to be re-queued
	 *			after.
	 */
	class BS_ED_EXPORT HandleDrawManager
	{
	public:
		HandleDrawManager();
		~HandleDrawManager();

		/**
		 * @brief	Sets the color of all the following draw* calls.
		 */
		void setColor(const Color& color);

		/**
		 * @brief	Sets the transform matrix that will be applied to all
		 *			following draw* calls.
		 */
		void setTransform(const Matrix4& transform);

		/**
		 * Sets the layer bitfield that controls whether a handle is considered visible in a specific camera. Handle layer 
		 * must match camera layer in order for the camera to render it
		 */
		void setLayer(UINT64 layer);

		/**
		 * @brief	Draws a solid cuboid.
		 *
		 * @param	position	Center of the cuboid.
		 * @param	extents		Radius of the cuboid in all directions.
		 * @param	size		Uniform scale of the object.
		 */
		void drawCube(const Vector3& position, const Vector3& extents, float size = 1.0f);

		/**
		 * @brief	Draws a solid sphere.
		 *
		 * @param	position	Center of the sphere.
		 * @param	radius		Radius of the sphere.
		 * @param	size		Uniform scale of the object.
		 */
		void drawSphere(const Vector3& position, float radius, float size = 1.0f);

		/**
		 * @brief	Draws a wireframe cuboid.
		 *
		 * @param	position	Center of the cuboid.
		 * @param	extents		Radius of the cuboid in all directions.
		 * @param	size		Uniform scale of the object.
		 */
		void drawWireCube(const Vector3& position, const Vector3& extents, float size = 1.0f);

		/**
		 * @brief	Draws a wireframe sphere.
		 *
		 * @param	position	Center of the sphere.
		 * @param	radius		Radius of the sphere.
		 * @param	size		Uniform scale of the object.
		 */
		void drawWireSphere(const Vector3& position, float radius, float size = 1.0f);

		/**
		 * @brief	Draws a solid cone.
		 *
		 * @param	base		Position of the center of the base of the cone.
		 * @param	normal		Orientation of the cone, pointing from center base to the tip of the cone.
		 * @param	height		Height of the cone (along the normal).
		 * @param	radius		Radius of the base of the cone.
		 * @param	size		Uniform scale of the object.
		 */
		void drawCone(const Vector3& base, const Vector3& normal, float height, float radius, float size = 1.0f);

		/**
		 * @brief	Draws a line.
		 *
		 * @param	start		Starting point for the line.
		 * @param	end			Ending point for the line.
		 * @param	size		Uniform scale of the object.
		 */
		void drawLine(const Vector3& start, const Vector3& end, float size = 1.0f);

		/**
		 * @brief	Draws a double-sided solid disc.
		 *
		 * @param	position	Center of the disc.
		 * @param	normal		Orientation of the disc, pointing in the direction the disc is visible in.
		 * @param	radius		Radius of the disc.
		 * @param	size		Uniform scale of the object.
		 */
		void drawDisc(const Vector3& position, const Vector3& normal, float radius, float size = 1.0f);

		/**
		 * @brief	Draws a wireframe disc.
		 *
		 * @param	position	Center of the disc.
		 * @param	normal		Orientation of the disc, pointing in the direction the disc is visible in.
		 * @param	radius		Radius of the disc.
		 * @param	size		Uniform scale of the object.
		 */
		void drawWireDisc(const Vector3& position, const Vector3& normal, float radius, float size = 1.0f);

		/**
		 * @brief	Draws a double-sided solid arc.
		 *
		 * @param	position	Center of the arc.
		 * @param	normal		Orientation of the arc, pointing in the direction the arc is visible in.
		 * @param	radius		Radius of the arc.
		 * @param	startAngle	Angle at which to start the arc.
		 * @param	amountAngle	Length of the arc.
		 * @param	size		Uniform scale of the object.
		 */
		void drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f);

		/**
		 * @brief	Draws a wireframe arc.
		 *
		 * @param	position	Center of the arc.
		 * @param	normal		Orientation of the arc, pointing in the direction the arc is visible in.
		 * @param	radius		Radius of the arc.
		 * @param	startAngle	Angle at which to start the arc.
		 * @param	amountAngle	Length of the arc.
		 * @param	size		Uniform scale of the object.
		 */
		void drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f);

		/**
		 * @brief	Draws a double-sided solid rectangle.
		 *
		 * @param	area		Position and size of the rectangle.
		 * @param	size		Uniform scale of the object.
		 */
		void drawRect(const Rect3& area, float size = 1.0f);

		/**
		 * @brief	Queues all the handle draw commands executed so far for rendering. All commands
		 *			are cleared and will need to be called again to draw them again.
		 */
		void draw(const CameraPtr& camera);

	private:
		friend class HandleDrawManagerCore;

		/**
		 * @brief	Initializes the core thread portion of the draw manager.
		 *
		 * @param	wireMat		Material used for drawing the wireframe objects.
		 * @param	solidMat	Material used for drawing the solid objects.
		 */
		void initializeCore(const SPtr<MaterialCore>& wireMat, const SPtr<MaterialCore>& solidMat);

		/**
		 * @brief	Destroys the core thread portion of the draw manager.
		 */
		void destroyCore(HandleDrawManagerCore* core);

		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const UINT32 ARC_QUALITY;

		Matrix4 mTransform;
		std::atomic<HandleDrawManagerCore*> mCore;
		DrawHelper* mDrawHelper;
	};

	/**
	 * @brief	Core thread specific portion of the HandleDrawManager that
	 *			handles actual rendering.
	 */
	class BS_ED_EXPORT HandleDrawManagerCore
	{
		/**
		 * @brief	Contains information about the material used for 
		 *			drawing solid objects and its parameters.
		 */
		struct SolidMaterialData
		{
			SPtr<MaterialCore> mat;
			GpuParamMat4Core mViewProj;
			GpuParamVec4Core mViewDir;
		};

		/**
		 * @brief	Contains information about the material used for 
		 *			drawing wireframe objects and its parameters.
		 */
		struct WireMaterialData
		{
			SPtr<MaterialCore> mat;
			GpuParamMat4Core mViewProj;
		};

		/**
		 * @brief	Type of mesh that can be drawn.
		 */
		enum class MeshType
		{
			Solid, Wire
		};

		/**
		 * @brief	Contains data about a render mesh.
		 */
		struct MeshData
		{
			MeshData(const SPtr<MeshCoreBase>& mesh, MeshType type)
				:mesh(mesh), type(type)
			{ }

			SPtr<MeshCoreBase> mesh;
			MeshType type;
		};

		struct PrivatelyConstruct { };

	public:
		HandleDrawManagerCore(const PrivatelyConstruct& dummy) { }
		~HandleDrawManagerCore();

	private:
		friend class HandleDrawManager;

		/**
		 * @brief	Initializes the object. Must be called right after construction.
		 *
		 * @param	wireMat		Material used for drawing the wireframe objects.
		 * @param	solidMat	Material used for drawing the solid objects.
		 */
		void initialize(const SPtr<MaterialCore>& wireMat, const SPtr<MaterialCore>& solidMat);

		/**
		 * @brief	Updates the data that will be used for rendering the new frame.
		 *
		 * @param	camera	Camera to render to.
		 * @param	meshes	Meshes to render.
		 */
		void updateData(const SPtr<CameraCore>& camera, const Vector<MeshData>& meshes);

		/**
		 * @brief	Callback triggered by the renderer. Draws the stored meshes.
		 */
		void render();

		SPtr<CameraCore> mCamera;
		Vector<MeshData> mMeshes;

		// Immutable
		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;
	};
}
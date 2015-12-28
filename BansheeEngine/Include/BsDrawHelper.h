#pragma once

#include "BsPrerequisites.h"
#include "BsMatrix4.h"
#include "BsVector3.h"
#include "BsColor.h"
#include "BsRect3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class for immediate drawing of common geometric shapes.
	 */
	class BS_EXPORT DrawHelper
	{
	public:
		/**
		 * @brief	Controls in what order will elements be rendered,
		 *			depending on some reference point
		 */
		enum class SortType
		{
			BackToFront,
			FrontToBack,
			None
		};

		/**
		 * @brief	Type of meshes that are output by DrawHelper.
		 */
		enum class MeshType
		{
			Solid, Wire, Text
		};

		/**
		 * @brief	Container for mesh of a specific type output by
		 *			the DrawHelper.
		 */
		struct ShapeMeshData
		{
			TransientMeshPtr mesh;
			MeshType type;
		};

		DrawHelper();
		~DrawHelper();

		/**
		 * @brief	Sets a color that will be used for any shapes recorded after this call.
		 */
		void setColor(const Color& color);

		/**
		 * @brief	Sets a transform matrix that will be used for any shapes recorded after this call.
		 */
		void setTransform(const Matrix4& transform);

		/**
		 * @brief	Records a solid cuboid with the specified properties in the internal draw queue.
		 */
		void cube(const Vector3& position, const Vector3& extents);

		/**
		 * @brief	Records a solid sphere with the specified properties in the internal draw queue.
		 */
		void sphere(const Vector3& position, float radius, UINT32 quality = 1);

		/**
		 * @brief	Records a wireframe cube with the specified properties in the internal draw queue.
		 */
		void wireCube(const Vector3& position, const Vector3& extents);

		/**
		 * @brief	Records a wireframe sphere with the specified properties in the internal draw queue.
		 */
		void wireSphere(const Vector3& position, float radius, UINT32 quality = 10);

		/**
		 * @brief	Records a line with the specified properties in the internal draw queue.
		 */
		void line(const Vector3& start, const Vector3& end);

		/**
		 * @brief	Records a wireframe frustum with the specified properties in the internal draw queue.
		 */
		void frustum(const Vector3& position, float aspect, Degree FOV, float near, float far);

		/**
		 * @brief	Records a solid cone with the specified properties in the internal draw queue.
		 */
		void cone(const Vector3& base, const Vector3& normal, float height, float radius, UINT32 quality = 10);

		/**
		 * @brief	Records a solid disc with the specified properties in the internal draw queue.
		 */
		void disc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality = 10);

		/**
		 * @brief	Records a wireframe disc with the specified properties in the internal draw queue.
		 */
		void wireDisc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality = 10);

		/**
		 * @brief	Records a solid arc with the specified properties in the internal draw queue.
		 */
		void arc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, 
			UINT32 quality = 10);

		/**
		 * @brief	Records a wireframe arc with the specified properties in the internal draw queue.
		 */
		void wireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, 
			UINT32 quality = 10);

		/**
		 * @brief	Records a solid rectangle with the specified properties in the internal draw queue.
		 */
		void rectangle(const Rect3& area);

		/**
		 * Records a mesh representing 2D text with the specified properties in the internal draw queue. 
		 *
		 * @param[in]	position		Position to render the text at. Text will be centered around this point.
		 * @param[in]	text			Text to draw.
		 * @param[in]	font			Font to use for rendering the text's characters.
		 * @param[in]	size			Size of the text, in points.
		 */
		void text(const Vector3& position, const WString& text, const HFont& font, UINT32 size = 10);

		/**
		 * @brief	Clears all recorded shapes.
		 */
		void clear();

		/**
		 * @brief	Generates a set of meshes from all the recorded solid and wireframe shapes.
		 *			The meshes can be accessed via getMeshes() and released via clearMeshes(). 
		 *			Any previously active meshes will be cleared when this method is called.
		 *
		 * @param	sorting		Determines how (and if) should elements be sorted
		 *						based on their distance from the reference point.
		 * @param	reference	Reference point to use for determining distance when
		 *						sorting.
		 *
		 * @note	You must call releaseSolidMesh() when done.
		 */
		void buildMeshes(SortType sorting = SortType::None, const Vector3& reference = Vector3::ZERO);

		/**
		 * @brief	Returns a set of meshes you have previously built using buildMeshes().
		 */
		const Vector<ShapeMeshData>& getMeshes() const { return mMeshes; }

		/**
		 * @brief	Deallocates all active meshes.
		 */
		void clearMeshes();

	private:
		struct CommonData
		{
			Color color;
			Matrix4 transform;
			Vector3 center;
		};

		struct CubeData : public CommonData
		{
			Vector3 position;
			Vector3 extents;
		};

		struct SphereData : public CommonData
		{
			Vector3 position;
			float radius;
			UINT32 quality;
		};

		struct LineData : public CommonData
		{
			Vector3 start;
			Vector3 end;
		};

		struct Rect3Data : public CommonData
		{
			Rect3 area;
		};

		struct FrustumData : public CommonData
		{
			Vector3 position;
			float aspect;
			Degree FOV;
			float near;
			float far;
		};

		struct ConeData : public CommonData
		{
			Vector3 base;
			Vector3 normal;
			float height;
			float radius;
			UINT32 quality;
		};

		struct DiscData : public CommonData
		{
			Vector3 position;
			Vector3 normal;
			float radius;
			UINT32 quality;
		};

		struct ArcData : public CommonData
		{
			Vector3 position;
			Vector3 normal;
			float radius;
			Degree startAngle;
			Degree amountAngle;
			UINT32 quality;
		};

		struct Text2DData : public CommonData
		{
			Vector3 position;
			WString text;
			HFont font;
			UINT32 size;
		};

		static const UINT32 VERTEX_BUFFER_GROWTH;
		static const UINT32 INDEX_BUFFER_GROWTH;

		Color mColor;
		Matrix4 mTransform;

		Vector<CubeData> mSolidCubeData;
		Vector<CubeData> mWireCubeData;
		Vector<SphereData> mSolidSphereData;
		Vector<SphereData> mWireSphereData;
		Vector<LineData> mLineData;
		Vector<Rect3Data> mRect3Data;
		Vector<FrustumData> mFrustumData;
		Vector<ConeData> mConeData;
		Vector<DiscData> mDiscData;
		Vector<DiscData> mWireDiscData;
		Vector<ArcData> mArcData;
		Vector<ArcData> mWireArcData;
		Vector<Text2DData> mText2DData;

		Vector<ShapeMeshData> mMeshes;

		MeshHeapPtr mSolidMeshHeap;
		MeshHeapPtr mWireMeshHeap;
		MeshHeapPtr mTextMeshHeap;

		VertexDataDescPtr mSolidVertexDesc;
		VertexDataDescPtr mWireVertexDesc;
		VertexDataDescPtr mTextVertexDesc;
	};
}
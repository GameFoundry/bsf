//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "Math/BsMatrix4.h"
#include "Math/BsVector3.h"
#include "Math/BsVector2.h"
#include "Image/BsColor.h"
#include "Math/BsRect3.h"
#include "RenderAPI/BsSubMesh.h"
#include "Scene/BsTransform.h"

namespace bs
{
	/** @addtogroup Utility-Engine
	 *  @{
	 */

	/**	Helper class for immediate drawing of common geometric shapes. */
	class BS_EXPORT DrawHelper
	{
	public:
		/** Controls in what order will elements be rendered, depending on some reference point. */
		enum class SortType
		{
			BackToFront,
			FrontToBack,
			None
		};

		/**	Type of meshes that are output by DrawHelper. */
		enum class MeshType
		{
			Solid, Wire, Line, Text
		};

		/**	Container for mesh of a specific type output by the DrawHelper. */
		struct ShapeMeshData
		{
			SPtr<Mesh> mesh;
			SubMesh subMesh;
			MeshType type;
			HTexture texture;
		};

		DrawHelper();

		/**	Sets a color that will be used for any shapes recorded after this call. */
		void setColor(const Color& color);

		/**	Sets a transform matrix that will be used for any shapes recorded after this call. */
		void setTransform(const Matrix4& transform);

		/** Sets the layer bitfield that can be used for filtering which objects are output into the final mesh. */
		void setLayer(UINT64 layer);

		/**	Records a solid cuboid with the specified properties in the internal draw queue. */
		void cube(const Vector3& position, const Vector3& extents);

		/**	Records a solid sphere with the specified properties in the internal draw queue. */
		void sphere(const Vector3& position, float radius, UINT32 quality = 1);

		/**	Records a wireframe cube with the specified properties in the internal draw queue. */
		void wireCube(const Vector3& position, const Vector3& extents);

		/**	Records a wireframe sphere with the specified properties in the internal draw queue. */
		void wireSphere(const Vector3& position, float radius, UINT32 quality = 10);

		/**	Records a wireframe hemisphere with the specified properties in the internal draw queue. */
		void wireHemisphere(const Vector3& position, float radius, UINT32 quality = 10);

		/**	Records a line with the specified properties in the internal draw queue. */
		void line(const Vector3& start, const Vector3& end);

		/**	
		 * Records a list of lines in the internal draw queue. The list must contain lines as pair of vertices, starting
		 * point followed by an end point, and so on.
		 */
		void lineList(const Vector<Vector3>& lines);

		/**	Records a wireframe frustum with the specified properties in the internal draw queue. */
		void frustum(const Vector3& position, float aspect, Degree FOV, float near, float far);

		/**	Records a solid cone with the specified properties in the internal draw queue. */
		void cone(const Vector3& base, const Vector3& normal, float height, float radius,
			const Vector2& scale = Vector2::ONE, UINT32 quality = 10);

		/**	Records a wire cone with the specified properties in the internal draw queue. */
		void wireCone(const Vector3& base, const Vector3& normal, float height, float radius,
			const Vector2& scale = Vector2::ONE, UINT32 quality = 10);

		/**	Records a solid disc with the specified properties in the internal draw queue. */
		void disc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality = 10);

		/**	Records a wireframe disc with the specified properties in the internal draw queue. */
		void wireDisc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality = 10);

		/**	Records a solid arc with the specified properties in the internal draw queue. */
		void arc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle,
			UINT32 quality = 10);

		/**	Records a wireframe arc with the specified properties in the internal draw queue. */
		void wireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle,
			UINT32 quality = 10);

		/** Records a 3D mesh to be drawn as wireframe in the internal draw queue. */
		void wireMesh(const SPtr<MeshData>& meshData);

		/**	Records a solid rectangle with the specified properties in the internal draw queue. */
		void rectangle(const Rect3& area);

		/**
		 * Records a mesh representing 2D text with the specified properties in the internal draw queue.
		 *
		 * @param[in]	position	Position to render the text at. Text will be centered around this point.
		 * @param[in]	text		Text to draw.
		 * @param[in]	font		Font to use for rendering the text's characters.
		 * @param[in]	size		Size of the characters, in points.
		 */
		void text(const Vector3& position, const String& text, const HFont& font, UINT32 size = 10);

		/**	Clears all recorded shapes. */
		void clear();

		/**
		 * Generates a set of meshes from all the recorded solid and wireframe shapes. The meshes can be accessed via
		 * getMeshes() and released via clearMeshes().
		 *
		 * @param	sorting		(optional) Determines how (and if) should elements be sorted
		 *						based on their distance from the reference point.
		 * @param	camera		(optional) Camera through which the meshes will be rendered.
		 * @param	layers		(optional) Layers bitfield that can be used for controlling which shapes will be included
		 *						in the mesh. This bitfield will be ANDed with the layer specified when recording the shape.
		 * @return				Generated mesh data.
		 */
		Vector<ShapeMeshData> buildMeshes(SortType sorting = SortType::None,
			const Camera* camera = nullptr, UINT64 layers = 0xFFFFFFFFFFFFFFFF);

	private:
		struct CommonData
		{
			Color color;
			Matrix4 transform;
			Vector3 center;
			UINT64 layer;
		};

		struct CubeData : CommonData
		{
			Vector3 position;
			Vector3 extents;
		};

		struct SphereData : CommonData
		{
			Vector3 position;
			float radius;
			UINT32 quality;
		};

		struct LineData : CommonData
		{
			Vector3 start;
			Vector3 end;
		};

		struct LineListData : CommonData
		{
			Vector<Vector3> lines;
		};

		struct Rect3Data : CommonData
		{
			Rect3 area;
		};

		struct FrustumData : CommonData
		{
			Vector3 position;
			float aspect;
			Degree FOV;
			float nearDist;
			float farDist;
		};

		struct ConeData : CommonData
		{
			Vector3 base;
			Vector3 normal;
			float height;
			float radius;
			Vector2 scale;
			UINT32 quality;
		};

		struct DiscData : CommonData
		{
			Vector3 position;
			Vector3 normal;
			float radius;
			UINT32 quality;
		};

		struct ArcData : CommonData
		{
			Vector3 position;
			Vector3 normal;
			float radius;
			Degree startAngle;
			Degree amountAngle;
			UINT32 quality;
		};

		struct Text2DData : CommonData
		{
			Vector3 position;
			String text;
			HFont font;
			UINT32 size;
		};

		struct WireMeshData : CommonData
		{
			SPtr<MeshData> meshData;
		};

		static const UINT32 VERTEX_BUFFER_GROWTH;
		static const UINT32 INDEX_BUFFER_GROWTH;

		Color mColor;
		Matrix4 mTransform;
		UINT64 mLayer;

		Vector<CubeData> mSolidCubeData;
		Vector<CubeData> mWireCubeData;
		Vector<SphereData> mSolidSphereData;
		Vector<SphereData> mWireSphereData;
		Vector<SphereData> mWireHemisphereData;
		Vector<LineData> mLineData;
		Vector<LineListData> mLineListData;
		Vector<Rect3Data> mRect3Data;
		Vector<FrustumData> mFrustumData;
		Vector<ConeData> mConeData;
		Vector<ConeData> mWireConeData;
		Vector<DiscData> mDiscData;
		Vector<DiscData> mWireDiscData;
		Vector<ArcData> mArcData;
		Vector<ArcData> mWireArcData;
		Vector<Text2DData> mText2DData;
		Vector<WireMeshData> mWireMeshData;

		SPtr<VertexDataDesc> mSolidVertexDesc;
		SPtr<VertexDataDesc> mWireVertexDesc;
		SPtr<VertexDataDesc> mLineVertexDesc;
		SPtr<VertexDataDesc> mTextVertexDesc;
	};

	/** @} */
}

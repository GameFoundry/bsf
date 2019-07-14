//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Particles/BsParticleModule.h"
#include "Math/BsDegree.h"
#include "Math/BsVector2.h"
#include "Math/BsVector3.h"
#include "Math/BsMatrix4.h"
#include "BsParticleDistribution.h"

namespace bs
{
	class Random;
	class ParticleSet;

	/** @addtogroup Particles
	 *  @{
	 */

	/** Types of emission modes. */
	enum class BS_SCRIPT_EXPORT(m:Particles) ParticleEmissionModeType
	{
		/** Position will be picked randomly on a shape. */
		Random,

		/** Positions will loop around the shape in a predictable fashion. */
		Loop,

		/** Similar to Loop, except the order will be reversed when one loop iteration finishes. */
		PingPong,

		/**
		* All particles spawned on the shape at some instant (usually a frame) will be spread around the shape equally.
		*/
		Spread
	};

	/** Controls how are particle positions on a shape chosen. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true) ParticleEmissionMode
	{
		/** Type that determines general behaviour. */
		ParticleEmissionModeType type = ParticleEmissionModeType::Random;

		/**
		 * Speed along which particle generation should move around the shape, relevant for Loop and PingPing emission
		 * modes.
		 */
		float speed = 1.0f;

		/**
		 * Determines the minimum interval allowed between the generated particles. 0 specifies the particles can be
		 * generated anywhere on the shape.
		 */
		float interval = 0.0f;
	};

	/**
	 * Base class from all emitter shapes. Emitter shapes determine the position and direction of newly created particles.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterShape : public IReflectable
	{
	public:
		virtual ~ParticleEmitterShape() = default;

		/**
		 * @name Internal
		 * @{
		 */

		/**
		 * Spawns a new set of particles using the current shape's distribution.
		 *
		 * @param[in]	random		Random number generator.
		 * @param[in]	particles	Particle set in which to insert new particles.
		 * @param[in]	count		Number of particles to spawn.
		 * @param[in]	state		Optional state that can contain various per-frame information required for spawning
		 *							the particles.
		 * @return					Index at which the first of the particles was inserted, with other particles following
		 *							sequentially.
		 */
		virtual UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const = 0;

		/** @} */
	protected:
		friend class ParticleEmitter;

		ParticleEmitterShape() = default;

		/**
		 * Calculates the bounds of the emitter shape.
		 *
		 * @param[in]	shape		AABB for the emitter shape itself.
		 * @param[in]	velocity	AABB for the generated normals.
		 */
		virtual void calcBounds(AABox& shape, AABox& velocity) const = 0;

		/**
		 * Checks has the emitter been initialized properly. If the emitter is not valid then the spawn() method is
		 * not allowed to be called.
		 */
		bool isValid() const { return mIsValid; }

		bool mIsValid = true;
	};

	/** Determines the emission type for the cone particle emitter shape. */
	enum class BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterConeType
	{
		/** Emit particles only from the cone base. */
		Base,
		/** Emit particles from the entire cone volume. */
		Volume
	};

	/** Information describing a ParticleEmitterConeShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleConeShapeOptions) PARTICLE_CONE_SHAPE_DESC
	{
		/** Determines where on the cone are the particles emitter from. */
		ParticleEmitterConeType type = ParticleEmitterConeType::Base;

		/** Radius of the cone base. */
		float radius = 0.0f;

		/** Angle of the cone. */
		Degree angle = Degree(45.0f);

		/** Length of the cone. Irrelevant if emission type is Base. */
		float length = 1.0f;

		/**
		 * Proportion of the volume that can emit particles. Thickness of 0 results in particles being emitted only from the
		 * edge of the cone, while thickness of 1 results in particles being emitted from the entire volume. In-between
		 * values will use a part of the volume.
		 */
		float thickness = 1.0f;

		/** Angular portion of the cone from which to emit particles from, in degrees. */
		Degree arc = Degree(360.0f);

		/** Determines how will particle positions on the shape be generated. */
		ParticleEmissionMode mode;
	};

	/**
	 * Particle emitter shape that emits particles from a cone. Particles can be created on cone base or volume, while
	 * controling the radial arc of the emitted portion of the volume, as well as thickness of the cone emission volume.
	 * All particles will have random normals within the distribution of the cone.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterConeShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterConeShape(const PARTICLE_CONE_SHAPE_DESC& desc);
		ParticleEmitterConeShape() = default;
		virtual ~ParticleEmitterConeShape() = default;

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_CONE_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_CONE_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter cone shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterConeShape> create(const PARTICLE_CONE_SHAPE_DESC& desc);

		/** Creates a new particle emitter cone shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterConeShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle randomly, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** Spawns a single particle on the specified point on the cone, generating its position and normal. */
		void _spawn(float t, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		/** Generates a position and normal of a particle based on the input 2D position on the cone circle base. */
		void getPointInCone(const Vector2& pos2D, float distance, Vector3& position, Vector3& normal) const;

		PARTICLE_CONE_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterConeShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterSphereShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleSphereShapeOptions) PARTICLE_SPHERE_SHAPE_DESC
	{
		/** Radius of the sphere. */
		float radius = 1.0f;

		/**
		 * Proportion of the volume that can emit particles. Thickness of 0 results in particles being emitted only from the
		 * edge of the volume, while thickness of 1 results in particles being emitted from the entire volume. In-between
		 * values will use a part of the volume.
		 */
		float thickness = 0.0f;
	};

	/**
	 * Particle emitter shape that emits particles from a sphere. Particles can be emitted from sphere surface, the entire
	 * volume or a proportion of the volume depending on the thickness parameter. All particles will have normals pointing
	 * outwards in a spherical direction.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterSphereShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterSphereShape() = default;
		ParticleEmitterSphereShape(const PARTICLE_SPHERE_SHAPE_DESC& desc);

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_SPHERE_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_SPHERE_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter sphere shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterSphereShape> create(const PARTICLE_SPHERE_SHAPE_DESC& desc);

		/** Creates a new particle emitter sphere shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterSphereShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_SPHERE_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterSphereShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterHemisphereShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleHemisphereShapeOptions) PARTICLE_HEMISPHERE_SHAPE_DESC
	{
		/** Radius of the hemisphere. */
		float radius = 1.0f;

		/**
		 * Proportion of the volume that can emit particles. Thickness of 0 results in particles being emitted only from the
		 * edge of the volume, while thickness of 1 results in particles being emitted from the entire volume. In-between
		 * values will use a part of the volume.
		 */
		float thickness = 0.0f;
	};

	/**
	 * Particle emitter shape that emits particles from a hemisphere. Particles can be emitted from the hemisphere surface,
	 * the entire volume or a proportion of the volume depending on the thickness parameter. All particles will have
	 * normals pointing outwards in a spherical direction.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterHemisphereShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterHemisphereShape() = default;
		ParticleEmitterHemisphereShape(const PARTICLE_HEMISPHERE_SHAPE_DESC& desc);

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_HEMISPHERE_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_HEMISPHERE_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter sphere shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterHemisphereShape> create(const PARTICLE_HEMISPHERE_SHAPE_DESC& desc);

		/** Creates a new particle emitter sphere shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterHemisphereShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_HEMISPHERE_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterHemisphereShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Determines the emission type for the cone particle emitter shape. */
	enum class BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterBoxType
	{
		/** Particles will be emitted from the entire volume. */
		Volume,
		/** Particles will be emitted only from box surface. */
		Surface,
		/** Particles will be emitted only from box edge. */
		Edge
	};

	/** Information describing a ParticleEmitterBoxShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleBoxShapeOptions) PARTICLE_BOX_SHAPE_DESC
	{
		/** Determines from which portion of the box should particles be emitted from. */
		ParticleEmitterBoxType type = ParticleEmitterBoxType::Volume;

		/** Extends of the box. */
		Vector3 extents = Vector3::ONE;
	};

	/**
	 * Particle emitter shape that emits particles from an axis aligned box. Particles can be emitted from box volume,
	 * surface or edges. All particles have their normals set to positive Z direction.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterBoxShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterBoxShape() = default;
		ParticleEmitterBoxShape(const PARTICLE_BOX_SHAPE_DESC& desc);

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_BOX_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_BOX_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter box shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterBoxShape> create(const PARTICLE_BOX_SHAPE_DESC& desc);

		/** Creates a new particle emitter box shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterBoxShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_BOX_SHAPE_DESC mInfo;

		float mSurfaceArea[3];
		float mEdgeLengths[3];

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterBoxShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterLineShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleLineShapeOptions) PARTICLE_LINE_SHAPE_DESC
	{
		/** Length of the line. */
		float length = 1.0f;

		/** Determines how will particle positions on the shape be generated. */
		ParticleEmissionMode mode;
	};

	/** Particle emitter shape that emits particles from a line segment. */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterLineShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterLineShape() = default;
		ParticleEmitterLineShape(const PARTICLE_LINE_SHAPE_DESC& desc);

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_LINE_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_LINE_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter edge shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterLineShape> create(const PARTICLE_LINE_SHAPE_DESC& desc);

		/** Creates a new particle emitter edge shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterLineShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle randomly, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** Spawns a single particle on the specified point on the line, generating its position and normal. */
		void _spawn(float t, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_LINE_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterLineShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterCircleShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleCircleShapeOptions) PARTICLE_CIRCLE_SHAPE_DESC
	{
		/** Radius of the circle. */
		float radius = 1.0f;

		/**
		 * Proportion of the surface that can emit particles. Thickness of 0 results in particles being emitted only from
		 * the edge of the circle, while thickness of 1 results in particles being emitted from the entire surface.
		 * In-between values will use a part of the surface.
		 */
		float thickness = 0.0f;

		/** Angular portion of the cone from which to emit particles from, in degrees. */
		Degree arc = Degree(360.0f);

		/** Determines how will particle positions on the shape be generated. */
		ParticleEmissionMode mode;
	};

	/**
	 * Particle emitter shape that emits particles from a circle. Using the thickness parameter you can control whether to
	 * emit only from circle edge, the entire surface or just a part of the surface. Using the arc parameter you can emit
	 * from a specific angular portion of the circle.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterCircleShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterCircleShape() = default;
		ParticleEmitterCircleShape(const PARTICLE_CIRCLE_SHAPE_DESC& desc);
		virtual ~ParticleEmitterCircleShape() = default;

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_CIRCLE_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_CIRCLE_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter circle shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterCircleShape> create(const PARTICLE_CIRCLE_SHAPE_DESC& desc);

		/** Creates a new particle emitter circle shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterCircleShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle randomly, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** Spawns a single particle on the specified point on the circle, generating its position and normal. */
		void _spawn(float t, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_CIRCLE_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterCircleShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterRectShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleRectShapeOptions) PARTICLE_RECT_SHAPE_DESC
	{
		/** Extents of the rectangle. */
		Vector2 extents = Vector2::ONE;
	};

	/** Particle emitter shape that emits particles from the surface of a rectangle. */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterRectShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterRectShape() = default;
		ParticleEmitterRectShape(const PARTICLE_RECT_SHAPE_DESC& desc);

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_RECT_SHAPE_DESC& options) { mInfo = options; }

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_RECT_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter rectangle shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterRectShape> create(const PARTICLE_RECT_SHAPE_DESC& desc);

		/** Creates a new particle emitter rectangle shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterRectShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** Spawns a single particle, generating its position and normal. */
		void _spawn(const Random& random, Vector3& position, Vector3& normal) const;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_RECT_SHAPE_DESC mInfo;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterRectShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Determines the emission type for the mesh particle emitter shape. */
	enum class BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterMeshType
	{
		/** Particles will be emitted from mesh vertices. */
		Vertex,
		/** Particles will be emitted from mesh edges. */
		Edge,
		/** Particles will be emitted from mesh triangles. */
		Triangle
	};

	/** Information describing a ParticleEmitterStaticMeshShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleStaticMeshShapeOptions) PARTICLE_STATIC_MESH_SHAPE_DESC
	{
		/** Determines from which portion of the mesh are the particles emitted from. */
		ParticleEmitterMeshType type = ParticleEmitterMeshType::Triangle;

		/**
		 * When enabled the particles will be emitted sequentially from mesh vertices in the order they are defined.
		 * Only relevant for the Vertex emit mode.
		 */
		bool sequential = false;

		/**
		 * Mesh to spawn particles on. Must at least contain per-vertex position data encoded as 3D float vectors. Can
		 * optionally contain per-vertex normals encoded as 3D float vectors or as 4-byte unsigned-normalized format.
		 */
		HMesh mesh;
	};

	/**
	 * Calculates and stores per-triangle weights that can be used for easily picking a random triangle on a mesh, ensuring
	 * larger triangles are picked more likely.
	 */
	class MeshWeightedTriangles
	{
		/** Contains the cumulative, normalized weight of the triangle and its vertex indices. */
		struct TriangleWeight
		{
			float cumulativeWeight;
			UINT32 indices[3];
		};

	public:
		MeshWeightedTriangles() = default;
		MeshWeightedTriangles(const MeshData& meshData);

		/** Updates the weights from the provided mesh data. */
		void calculate(const MeshData& meshData);

		/** Find a random triangle on the mesh and outputs its vertex indices. */
		void getTriangle(const Random& random, std::array<UINT32, 3>& indices) const;

	private:
		Vector<TriangleWeight> mWeights;
	};

	/** Contains common functionality for particle mesh emitters. */
	class MeshEmissionHelper
	{
	public:
		/**
		 * Initializes the emission helper if the provided mesh contains necessary data for particle emission. Otherwise
		 * reports any issues in the log.
		 *
		 * @param[in]	mesh		Mesh to validate.
		 * @param[in]	perVertex	Set to true if particle emission is happening on mesh vertices.
		 * @param[in]	skinning	Set to true if the mesh will be animated using skinning.
		 * @return					True if initialized, or false if issues were detected.
		 */
		bool initialize(const HMesh& mesh, bool perVertex, bool skinning);

		/**
		 * Returns the next sequential vertex on the mesh and increments the internal counter so the next vertex is
		 * returned on the following call. Loops around if end is reached. Returns vertex position, normal and index.
		 */
		void getSequentialVertex(Vector3& position, Vector3& normal, UINT32& idx) const;

		/** Randomly picks a vertex on the mesh and returns its position, normal and index. */
		void getRandomVertex(const Random& random, Vector3& position, Vector3& normal, UINT32& idx) const;

		/** Randomly picks an edge on the mesh and returns the position, normal and indices of its vertices. */
		void getRandomEdge(const Random& random, std::array<Vector3, 2>& position, std::array<Vector3, 2>& normal,
			std::array<UINT32, 2>& idx) const;

		/** Randomly picks an triangle on the mesh and returns the position, normal and indices of its vertices. */
		void getRandomTriangle(const Random& random, std::array<Vector3, 3>& position, std::array<Vector3, 3>& normal,
			std::array<UINT32, 3>& idx) const;

		/** Evaluates a blend matrix for a vertex at the specified index. */
		Matrix4 getBlendMatrix(const Matrix4* bones, UINT32 vertexIdx) const;

	private:
		MeshWeightedTriangles mWeightedTriangles;

		UINT8* mVertices = nullptr;
		UINT8* mNormals = nullptr;
		UINT32 mNumVertices = 0;
		UINT32 mVertexStride = 0;
		bool m32BitNormals = true;

		UINT8* mBoneIndices = nullptr;
		UINT8* mBoneWeights = nullptr;

		SPtr<MeshData> mMeshData;

		// Transient
		mutable UINT32 mNextSequentialIdx = 0;
	};

	/**
	 * Particle emitter shape that emits particles from a surface of a static (non-animated) mesh. Particles can be
	 * emitted from mesh vertices, edges or triangles. If information about normals exists, particles will also inherit
	 * the normals.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterStaticMeshShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterStaticMeshShape(const PARTICLE_STATIC_MESH_SHAPE_DESC& desc);
		ParticleEmitterStaticMeshShape();
		virtual ~ParticleEmitterStaticMeshShape() = default;

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_STATIC_MESH_SHAPE_DESC& options);

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_STATIC_MESH_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter static mesh shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterStaticMeshShape> create(const PARTICLE_STATIC_MESH_SHAPE_DESC& desc);

		/** Creates a new particle emitter static mesh shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterStaticMeshShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_STATIC_MESH_SHAPE_DESC mInfo;
		MeshEmissionHelper mMeshEmissionHelper;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterStaticMeshShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Information describing a ParticleEmitterSkinnedMeshShape. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true,n:ParticleSkinnedMeshShapeOptions) PARTICLE_SKINNED_MESH_SHAPE_DESC
	{
		/** Determines from which portion of the mesh are the particles emitted from. */
		ParticleEmitterMeshType type = ParticleEmitterMeshType::Triangle;

		/**
		 * When enabled the particles will be emitted sequentially from mesh vertices in the order they are defined.
		 * Only relevant for the Vertex emit mode.
		 */
		bool sequential = false;

		/**
		 * Renderable object containing a mesh to spawn particles on, as well as the attached Animation object resposible
		 * for performing skinned animation. Mesh must at least contain per-vertex position data encoded as 3D float
		 * vectors, blend indices encoded in 4-byte format, and blend weights encoded a 4D float vectors. Can optionally
		 * contain per-vertex normals encoded as 3D float vectors or as 4-byte unsigned-normalized format.
		 */
		ComponentOrActor<Renderable> renderable;
	};

	/**
	 * Particle emitter shape that emits particles from a surface of a skinned (animated) mesh. Particles can be
	 * emitted from mesh vertices, edges or triangles. If information about normals exists, particles will also inherit
	 * the normals.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitterSkinnedMeshShape : public ParticleEmitterShape
	{
	public:
		ParticleEmitterSkinnedMeshShape(const PARTICLE_SKINNED_MESH_SHAPE_DESC& desc);
		ParticleEmitterSkinnedMeshShape();
		virtual ~ParticleEmitterSkinnedMeshShape() = default;

		/** Options describing the shape. */
		BS_SCRIPT_EXPORT(pr:setter,n:Options,inline)
		void setOptions(const PARTICLE_SKINNED_MESH_SHAPE_DESC& options);

		/** @copydoc setOptions */
		BS_SCRIPT_EXPORT(pr:getter,n:Options)
		const PARTICLE_SKINNED_MESH_SHAPE_DESC& getOptions() const { return mInfo; }

		/** Creates a new particle emitter skinned mesh shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterSkinnedMeshShape> create(const PARTICLE_SKINNED_MESH_SHAPE_DESC& desc);

		/** Creates a new particle emitter skinned mesh shape. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitterSkinnedMeshShape> create();

		/**
		 * @name Internal
		 * @{
		 */

		/** @copydoc ParticleEmitterShape::_spawn */
		UINT32 _spawn(const Random& random, ParticleSet& particles, UINT32 count,
			const ParticleSystemState& state) const override;

		/** @} */
	protected:
		/** @copydoc ParticleEmitterShape::calcBounds */
		void calcBounds(AABox& shape, AABox& velocity) const override;

		PARTICLE_SKINNED_MESH_SHAPE_DESC mInfo;
		MeshEmissionHelper mMeshEmissionHelper;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterSkinnedMeshShapeRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** Specifies a burst of particles that occurs at a certain time point. */
	struct BS_SCRIPT_EXPORT(m:Particles,pl:true) ParticleBurst
	{
		ParticleBurst() = default;
		ParticleBurst(float time, FloatDistribution count, UINT32 cycles = 1, float interval = 1.0f)
			:time(time), count(std::move(count)), cycles(cycles), interval(interval)
		{ }

		/** Time at which to trigger the burst, in seconds. */
		float time = 0.0f;

		/** Number of particles to emit when the burst triggers. */
		FloatDistribution count = 0;

		/**
		 * Determines how many times to trigger the burst. If 0 the burst will trigger infinitely. Use @p interval to
		 * to control the time between each cycle.
		 */
		UINT32 cycles = 1;

		/** Controls how much time needs to pass before triggering another burst cycle, in seconds. */
		float interval = 1.0f;
	};

	/** Handles spawning of new particles using the specified parameters and shape. */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Particles) ParticleEmitter : public ParticleModule
	{
	public:
		/** Shape over which to emit the particles. */
		BS_SCRIPT_EXPORT(pr:setter,n:Shape)
		void setShape(SPtr<ParticleEmitterShape> shape) { mShape = std::move(shape); }

		/** @copydoc setShape */
		BS_SCRIPT_EXPORT(pr:getter,n:Shape)
		const SPtr<ParticleEmitterShape>& getShape() const { return mShape; }

		/** Determines the number of particles that are emitted every second. */
		BS_SCRIPT_EXPORT(pr:setter,n:EmissionRate)
		void setEmissionRate(FloatDistribution value) { mEmissionRate = std::move(value); }

		/** @copydoc setEmissionRate */
		BS_SCRIPT_EXPORT(pr:getter,n:EmissionRate)
		const FloatDistribution& getEmissionRate() const { return mEmissionRate; }

		/** Determines discrete intervals to emit particles. */
		BS_SCRIPT_EXPORT(pr:setter,n:EmissionBursts)
		void setEmissionBursts(Vector<ParticleBurst> bursts);

		/** @copydoc setEmissionBursts */
		BS_SCRIPT_EXPORT(pr:getter,n:EmissionBursts)
		const Vector<ParticleBurst>& getEmissionBursts() const { return mBursts; }

		/** Determines the lifetime of particles when they are initially spawned, in seconds. */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialLifetime)
		void setInitialLifetime(FloatDistribution value) { mInitialLifetime = std::move(value); }

		/** @copydoc setInitialLifetime */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialLifetime)
		const FloatDistribution& getInitialLifetime() const { return mInitialLifetime; }

		/**
		 * Sets the initial speed of the particles, in meters/second. The speed is applied along the particle's velocity
		 * direction, which is determined by the emission shape and potentially other properties.
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialSpeed)
		void setInitialSpeed(FloatDistribution value) { mInitialSpeed = std::move(value); }

		/** @copydoc setInitialSpeed */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialSpeed)
		const FloatDistribution& getInitialSpeed() const { return mInitialSpeed; }

		/**
		 * Determines the size of the particles when initially spawned. The size is applied uniformly in all dimensions.
		 * Only used if 3D size is disabled.
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialSize)
		void setInitialSize(FloatDistribution value) { mInitialSize = std::move(value); }

		/** @copydoc setInitialSize */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialSize)
		const FloatDistribution& getInitialSize() const { return mInitialSize; }

		/**
		 * Determines the size of the particles when initially spawned. Size can be specified for each dimension separately.
		 * Only used if 3D size is enabled.
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialSize3D)
		void setInitialSize3D(Vector3Distribution value) { mInitialSize3D = std::move(value); }

		/** @copydoc setInitialSize3D */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialSize3D)
		const Vector3Distribution& getInitialSize3D() const { return mInitialSize3D; }

		/**
		 * Determines should the initial particle size be applied uniformly (if disabled), or evaluated separately for each
		 * dimension (if enabled).
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:Use3DSize)
		void setUse3DSize(bool value) { mUse3DSize = value; }

		/** @copydoc setUse3DSize */
		BS_SCRIPT_EXPORT(pr:getter,n:Use3DSize)
		bool getUse3DSize() const { return mUse3DSize; }

		/**
		 * Determines the rotation of the particles when initially spawned, in degrees. The rotation is applied around the
		 * particle's local Z axis. Only used if 3D rotation is disabled.
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialRotation)
		void setInitialRotation(FloatDistribution value) { mInitialRotation = std::move(value); }

		/** @copydoc setInitialRotation */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialRotation)
		const FloatDistribution& getInitialRotation() const { return mInitialRotation; }

		/**
		 * Determines the rotation of the particles when initially spawned, in Euler angles. Only used if 3D rotation is
		 * enabled.
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialRotation3D)
		void setInitialRotation3D(Vector3Distribution value) { mInitialRotation3D = std::move(value); }

		/** @copydoc setInitialRotation3D */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialRotation3D)
		const Vector3Distribution& getInitialRotation3D() const { return mInitialRotation3D; }

		/**
		 * Determines should the initial particle rotation be a single angle applied around a Z axis (if disabled), or a
		 * set of Euler angles that allow you to rotate around every axis (if enabled).
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:Use3DRotation)
		void setUse3DRotation(bool value) { mUse3DRotation = value; }

		/** @copydoc setUse3DRotation */
		BS_SCRIPT_EXPORT(pr:getter,n:Use3DRotation)
		bool getUse3DRotation() const { return mUse3DRotation; }

		/** Determines the initial color (in RGB channels) and transparency (in A channel) of particles. */
		BS_SCRIPT_EXPORT(pr:setter,n:InitialColor)
		void setInitialColor(const ColorDistribution& value) { mInitialColor = value; }

		/** @copydoc setInitialColor */
		BS_SCRIPT_EXPORT(pr:getter,n:InitialColor)
		const ColorDistribution& getInitialColor() const { return mInitialColor; }

		/**
		 * Determines a range of values determining a random offset to apply to particle position after it has been emitted.
		 * Offset will be randomly selected in all three axes in range [-value, value].
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:RandomOffset)
		void setRandomOffset(float value) { mRandomOffset = value; }

		/** @copydoc setRandomOffset */
		BS_SCRIPT_EXPORT(pr:getter,n:RandomOffset)
		float getRandomOffset() const { return mRandomOffset; }

		/**
		 * Determines should particle U texture coordinate be randomly flipped, mirroring the image. The value represents
		 * a percent of particles that should be flipped, in range [0, 1].
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:FlipU)
		void setFlipU(float value) { mFlipU = Math::clamp01(value); }

		/** @copydoc setFlipU */
		BS_SCRIPT_EXPORT(pr:getter,n:FlipU)
		float getFlipU() const { return mFlipU; }

		/**
		 * Determines should particle V texture coordinate be randomly flipped, mirroring the image. The value represents
		 * a percent of particles that should be flipped, in range [0, 1].
		 */
		BS_SCRIPT_EXPORT(pr:setter,n:FlipV)
		void setFlipV(float value) { mFlipV = Math::clamp01(value); }

		/** @copydoc setFlipV */
		BS_SCRIPT_EXPORT(pr:getter,n:FlipV)
		float getFlipV() const { return mFlipV; }

		/** Creates a new emitter. */
		BS_SCRIPT_EXPORT(ec:T)
		static SPtr<ParticleEmitter> create();
	private:
		friend class ParticleSystem;

		/**
		 * Spawns new particles in the specified time increment (if any).
		 *
		 * @param[in]	random			Random number generator.
		 * @param[in]	state			Various per-frame information provided by the parent particle system.
		 * @param[in]	set				Set to which to append new particles to.
		 */
		void spawn(Random& random, const ParticleSystemState& state, ParticleSet& set) const;

		/**
		 * Spawns the specified number of particles.
		 *
		 * @param[in]	count			Number of particles to spawn.
		 * @param[in]	random			Random number generator.
		 * @param[in]	state			Various per-frame information provided by the parent particle system.
		 * @param[in]	set				Set to which to append new particles to.
		 * @param[in]	spacing			When false all particles will use the current emitter time. When true the particles
		 *								will be assigned a time between current time and time step end time, so they are
		 *								unifomly distributed in this time range.
		 * @return						Actual number of spawned particles.
		 */
		UINT32 spawn(UINT32 count, Random& random, const ParticleSystemState& state, ParticleSet& set, bool spacing) const;

		// User-visible properties
		SPtr<ParticleEmitterShape> mShape;

		FloatDistribution mEmissionRate = 50.0f;
		Vector<ParticleBurst> mBursts;

		FloatDistribution mInitialLifetime = 10.0f;
		FloatDistribution mInitialSpeed = 1.0f;

		FloatDistribution mInitialSize = 0.1f;
		Vector3Distribution mInitialSize3D = Vector3::ONE;
		bool mUse3DSize = false;

		FloatDistribution mInitialRotation = 0.0f;
		Vector3Distribution mInitialRotation3D = Vector3::ZERO;
		bool mUse3DRotation = false;

		ColorDistribution mInitialColor = Color::White;

		float mFlipU = 0.0f;
		float mFlipV = 0.0f;
		
		float mRandomOffset = 0.0f;

		// Internal state
		mutable float mEmitAccumulator = 0.0f;
		mutable Vector<float> mBurstAccumulator;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ParticleEmitterRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */

}

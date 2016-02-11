#include "BsPhysX.h"
#include "PxPhysicsAPI.h"
#include "BsPhysXMaterial.h"
#include "BsPhysXMesh.h"
#include "BsPhysXRigidbody.h"
#include "BsPhysXBoxCollider.h"
#include "BsPhysXSphereCollider.h"
#include "BsPhysXPlaneCollider.h"
#include "BsPhysXCapsuleCollider.h"
#include "BsTaskScheduler.h"
#include "BsTime.h"
#include "Bsvector3.h"

using namespace physx;

namespace BansheeEngine
{
	struct PHYSICS_INIT_DESC
	{
		float typicalLength = 1.0f;
		float typicalSpeed = 9.81f;
		Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
		bool initCooking = true; // TODO: Disable this for Game build
		float timeStep = 1.0f / 60.0f;
	};

	class PhysXAllocator : public PxAllocatorCallback
	{
	public:
		void* allocate(size_t size, const char*, const char*, int) override
		{
			void* ptr = bs_alloc_aligned16((UINT32)size);
			PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
			return ptr;
		}

		void deallocate(void* ptr) override
		{
			bs_free_aligned16(ptr);
		}
	};

	class PhysXErrorCallback : public PxErrorCallback
	{
	public:
		void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			{
				const char* errorCode = nullptr;

				UINT32 severity = 0;
				switch (code)
				{
				case PxErrorCode::eNO_ERROR:
					errorCode = "No error";
					break;
				case PxErrorCode::eINVALID_PARAMETER:
					errorCode = "Invalid parameter";
					severity = 2;
					break;
				case PxErrorCode::eINVALID_OPERATION:
					errorCode = "Invalid operation";
					severity = 2;
					break;
				case PxErrorCode::eOUT_OF_MEMORY:
					errorCode = "Out of memory";
					severity = 2;
					break;
				case PxErrorCode::eDEBUG_INFO:
					errorCode = "Info";
					break;
				case PxErrorCode::eDEBUG_WARNING:
					errorCode = "Warning";
					severity = 1;
					break;
				case PxErrorCode::ePERF_WARNING:
					errorCode = "Performance warning";
					severity = 1;
					break;
				case PxErrorCode::eABORT:
					errorCode = "Abort";
					severity = 2;
					break;
				case PxErrorCode::eINTERNAL_ERROR:
					errorCode = "Internal error";
					severity = 2;
					break;
				case PxErrorCode::eMASK_ALL:
				default:
					errorCode = "Unknown error";
					severity = 2;
					break;
				}

				StringStream ss;

				switch(severity)
				{
				case 0:
					ss << "PhysX info (" << errorCode << "): " << message << " at " << file << ":" << line;
					LOGDBG(ss.str());
					break;
				case 1:
					ss << "PhysX warning (" << errorCode << "): " << message << " at " << file << ":" << line;
					LOGWRN(ss.str());
					break;
				case 2:
					ss << "PhysX error (" << errorCode << "): " << message << " at " << file << ":" << line;
					LOGERR(ss.str());
					BS_ASSERT(false); // Halt execution on debug builds when error occurrs
					break;
				}
			}
		}
	};

	class PhysXEventCallback : public PxSimulationEventCallback
	{
		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override { /* Do nothing */ }
		void onWake(PxActor** actors, PxU32 count) override { /* Do nothing */ }
		void onSleep(PxActor** actors, PxU32 count) override { /* Do nothing */ }

		void onTrigger(PxTriggerPair* pairs, PxU32 count) override
		{
			for (PxU32 i = 0; i < count; i++)
			{
				const PxTriggerPair& pair = pairs[i];

				PhysX::ContactEventType type;
				bool ignoreContact = false;
				switch ((UINT32)pair.status)
				{
				case PxPairFlag::eNOTIFY_TOUCH_FOUND:
					type = PhysX::ContactEventType::ContactBegin;
					break;
				case PxPairFlag::eNOTIFY_TOUCH_PERSISTS:
					type = PhysX::ContactEventType::ContactStay;
					break;
				case PxPairFlag::eNOTIFY_TOUCH_LOST:
					type = PhysX::ContactEventType::ContactEnd;
					break;
				default:
					ignoreContact = true;
					break;
				}

				if (ignoreContact)
					continue;

				PhysX::TriggerEvent event;
				event.trigger = (Collider*)pair.triggerShape->userData;
				event.other = (Collider*)pair.otherShape->userData;
				event.type = type;

				gPhysX()._reportTriggerEvent(event);
			}
		}

		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count) override
		{
			for (PxU32 i = 0; i < count; i++)
			{
				const PxContactPair& pair = pairs[i];

				PhysX::ContactEventType type;
				bool ignoreContact = false;
				switch((UINT32)pair.events)
				{
				case PxPairFlag::eNOTIFY_TOUCH_FOUND:
					type = PhysX::ContactEventType::ContactBegin;
					break;
				case PxPairFlag::eNOTIFY_TOUCH_PERSISTS:
					type = PhysX::ContactEventType::ContactStay;
					break;
				case PxPairFlag::eNOTIFY_TOUCH_LOST:
					type = PhysX::ContactEventType::ContactEnd;
					break;
				default:
					ignoreContact = true;
					break;
				}

				if (ignoreContact)
					continue;

				PhysX::ContactEvent event;
				event.colliderA = (Collider*)pair.shapes[0]->userData;
				event.colliderB = (Collider*)pair.shapes[1]->userData;
				event.type = type;

				PxU32 contactCount = pair.contactCount;
				const PxU8* stream = pair.contactStream;
				PxU16 streamSize = pair.contactStreamSize;

				if (contactCount > 0 && streamSize > 0)
				{
					PxU32 contactIdx = 0;
					PxContactStreamIterator iter((PxU8*)stream, streamSize);

					stream += ((streamSize + 15) & ~15);

					const PxReal* impulses = reinterpret_cast<const PxReal*>(stream);
					PxU32 hasImpulses = (pair.flags & PxContactPairFlag::eINTERNAL_HAS_IMPULSES);

					while (iter.hasNextPatch())
					{
						iter.nextPatch();
						while (iter.hasNextContact())
						{
							iter.nextContact();

							ContactPoint point;
							point.position = fromPxVector(iter.getContactPoint());
							point.separation = iter.getSeparation();
							point.normal = fromPxVector(iter.getContactNormal());

							if (hasImpulses)
								point.impulse = impulses[contactIdx];
							else
								point.impulse = 0.0f;

							event.points.push_back(point);

							contactIdx++;
						}
					}
				}

				gPhysX()._reportContactEvent(event);
			}
		}
	};

	class PhysXCPUDispatcher : public PxCpuDispatcher
	{
	public:
		void submitTask(PxBaseTask& physxTask) override
		{
			// Note: Banshee's task scheduler is pretty low granularity. Consider a better task manager in case PhysX ends
			// up submitting many tasks.
			// - PhysX's task manager doesn't seem much lighter either. But perhaps I can at least create a task pool to 
			//   avoid allocating them constantly.

			auto runTask = [&]() { physxTask.run(); physxTask.release(); };
			TaskPtr task = Task::create("PhysX", runTask);

			TaskScheduler::instance().addTask(task);
		}

		PxU32 getWorkerCount() const override
		{
			return (PxU32)TaskScheduler::instance().getNumWorkers();
		}
	};

	PxFilterFlags PhysXFilterShader(PxFilterObjectAttributes attr0, PxFilterData data0, PxFilterObjectAttributes attr1, 
		PxFilterData data1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		if (PxFilterObjectIsTrigger(attr0) || PxFilterObjectIsTrigger(attr1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		UINT64 groupA = *(UINT64*)&data0.word0;
		UINT64 groupB = *(UINT64*)&data1.word0;

		bool canCollide = gPhysics().isCollisionEnabled(groupA, groupB);
		if (!canCollide)
			return PxFilterFlag::eSUPPRESS;

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlags();
	}

	static PhysXAllocator gPhysXAllocator;
	static PhysXErrorCallback gPhysXErrorHandler;
	static PhysXCPUDispatcher gPhysXCPUDispatcher;
	static PhysXEventCallback gPhysXEventCallback;

	PhysX::PhysX()
	{
		PHYSICS_INIT_DESC input; // TODO - Make this an input parameter.

		PxTolerancesScale scale;
		scale.length = input.typicalLength;
		scale.speed = input.typicalSpeed;

		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gPhysXAllocator, gPhysXErrorHandler);
		mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation, scale);

		PxRegisterArticulations(*mPhysics);

		if (input.initCooking)
		{
			// Note: PhysX supports cooking for specific platforms to make the generated results better. Consider
			// allowing the meshes to be re-cooked when target platform is changed. Right now we just use the default value.

			PxCookingParams cookingParams(scale);
			mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, cookingParams);
		}

		PxSceneDesc sceneDesc(scale); // TODO - Test out various other parameters provided by scene desc
		sceneDesc.gravity = toPxVector(input.gravity);
		sceneDesc.cpuDispatcher = &gPhysXCPUDispatcher;
		sceneDesc.filterShader = PhysXFilterShader;
		sceneDesc.simulationEventCallback = &gPhysXEventCallback;
		sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

		mScene = mPhysics->createScene(sceneDesc);
		mSimulationStep = input.timeStep;
		mDefaultMaterial = mPhysics->createMaterial(0.0f, 0.0f, 0.0f);
	}

	PhysX::~PhysX()
	{
		mScene->release();

		if (mCooking != nullptr)
			mCooking->release();

		mPhysics->release();
		mFoundation->release();
	}

	void PhysX::update()
	{
		mUpdateInProgress = true;

		float nextFrameTime = mLastSimulationTime + mSimulationStep;
		float curFrameTime = gTime().getTime();
		if(curFrameTime < nextFrameTime)
		{
			// TODO - Interpolate rigidbodies but perform no actual simulation

			return;
		}

		float simulationAmount = curFrameTime - mLastSimulationTime;
		while (simulationAmount >= mSimulationStep) // In case we're running really slow multiple updates might be needed
		{
			// Note: Consider delaying fetchResults one frame. This could improve performance because Physics update would be
			//       able to run parallel to the simulation thread, but at a cost to input latency.
			// TODO - Provide a scratch buffer for the simulation (use the frame allocator, but I must extend it so it allocates
			//	      on a 16 byte boundary).
			mScene->simulate(mSimulationStep);
			mScene->fetchResults(true);

			// Update rigidbodies with new transforms
			PxU32 numActiveTransforms;
			const PxActiveTransform* activeTransforms = mScene->getActiveTransforms(numActiveTransforms);

			for (PxU32 i = 0; i < numActiveTransforms; i++)
			{
				Rigidbody* rigidbody = static_cast<Rigidbody*>(activeTransforms[i].userData);
				const PxTransform& transform = activeTransforms[i].actor2World;

				// Note: Make this faster, avoid dereferencing Rigidbody and attempt to access pos/rot destination directly,
				//       use non-temporal writes
				rigidbody->_setTransform(fromPxVector(transform.p), fromPxQuaternion(transform.q));
			}

			simulationAmount -= mSimulationStep;
		}

		// TODO - Consider extrapolating for the remaining "simulationAmount" value

		mLastSimulationTime = curFrameTime; 
		mUpdateInProgress = false;

		triggerEvents();
	}

	void PhysX::_reportContactEvent(const ContactEvent& event)
	{
		mContactEvents.push_back(event);
	}

	void PhysX::_reportTriggerEvent(const TriggerEvent& event)
	{
		mTriggerEvents.push_back(event);
	}

	void PhysX::triggerEvents()
	{
		CollisionData data;

		for(auto& entry : mTriggerEvents)
		{
			data.collider = entry.other;

			switch (entry.type)
			{
			case ContactEventType::ContactBegin:
				entry.trigger->onCollisionBegin(data);
				break;
			case ContactEventType::ContactStay:
				entry.trigger->onCollisionStay(data);
				break;
			case ContactEventType::ContactEnd:
				entry.trigger->onCollisionEnd(data);
				break;
			}
		}

		auto notifyContact = [&](Collider* obj, Collider* other, ContactEventType type, 
			const Vector<ContactPoint>& points, bool flipNormals = false)
		{
			data.collider = other;
			data.contactPoints = points;

			if(flipNormals)
			{
				for (auto& point : data.contactPoints)
					point.normal = -point.normal;
			}

			SPtr<Rigidbody> rigidbody = obj->getRigidbody();
			if(rigidbody != nullptr)
			{
				switch (type)
				{
				case ContactEventType::ContactBegin:
					rigidbody->onCollisionBegin(data);
					break;
				case ContactEventType::ContactStay:
					rigidbody->onCollisionStay(data);
					break;
				case ContactEventType::ContactEnd:
					rigidbody->onCollisionEnd(data);
					break;
				}
			}
			else
			{
				switch (type)
				{
				case ContactEventType::ContactBegin:
					obj->onCollisionBegin(data);
					break;
				case ContactEventType::ContactStay:
					obj->onCollisionStay(data);
					break;
				case ContactEventType::ContactEnd:
					obj->onCollisionEnd(data);
					break;
				}
			}
		};

		for (auto& entry : mContactEvents)
		{
			notifyContact(entry.colliderA, entry.colliderB, entry.type, entry.points, true);
			notifyContact(entry.colliderB, entry.colliderA, entry.type, entry.points, false);
		}

		mTriggerEvents.clear();
		mContactEvents.clear();
	}

	SPtr<PhysicsMaterial> PhysX::createMaterial(float staticFriction, float dynamicFriction, float restitution)
	{
		return bs_shared_ptr_new<PhysXMaterial>(mPhysics, staticFriction, dynamicFriction, restitution);
	}

	SPtr<PhysicsMesh> PhysX::createMesh(const MeshDataPtr& meshData, PhysicsMeshType type)
	{
		return bs_shared_ptr_new<PhysXMesh>(meshData, type);
	}

	SPtr<Rigidbody> PhysX::createRigidbody(const HSceneObject& linkedSO)
	{
		return bs_shared_ptr_new<PhysXRigidbody>(mPhysics, mScene, linkedSO);
	}

	SPtr<BoxCollider> PhysX::createBoxCollider(const Vector3& extents, const Vector3& position,
		const Quaternion& rotation)
	{
		return bs_shared_ptr_new<PhysXBoxCollider>(mPhysics, position, rotation, extents);
	}

	SPtr<SphereCollider> PhysX::createSphereCollider(float radius, const Vector3& position, const Quaternion& rotation)
	{
		return bs_shared_ptr_new<PhysXSphereCollider>(mPhysics, position, rotation, radius);
	}

	SPtr<PlaneCollider> PhysX::createPlaneCollider(const Vector3& position, const Quaternion& rotation)
	{
		return bs_shared_ptr_new<PhysXPlaneCollider>(mPhysics, position, rotation);
	}

	SPtr<CapsuleCollider> PhysX::createCapsuleCollider(float radius, float halfHeight, const Vector3& position, 
		const Quaternion& rotation)
	{
		return bs_shared_ptr_new<PhysXCapsuleCollider>(mPhysics, position, rotation, radius, halfHeight);
	}

	PhysX& gPhysX()
	{
		return static_cast<PhysX&>(PhysX::instance());
	}
}
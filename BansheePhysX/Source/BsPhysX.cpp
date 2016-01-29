#include "BsPhysX.h"
#include "PxPhysicsAPI.h"
#include "BsPhysXMaterial.h"
#include "BsPhysXRigidbody.h"
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
		bool initCooking = false;
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

	class PhysXCPUDispatcher : public PxCpuDispatcher
	{
	public:
		void submitTask(PxBaseTask& physxTask) override
		{
			// TODO - Banshee's task scheduler is pretty low granularity. Consider a better task manager in case PhysX ends
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

		bool canCollide = Physics::instance().isCollisionEnabled(groupA, groupB);
		if (!canCollide)
			return PxFilterFlag::eSUPPRESS;

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlags();
	}

	static PhysXAllocator gPhysXAllocator;
	static PhysXErrorCallback gPhysXErrorHandler;
	static PhysXCPUDispatcher gPhysXCPUDispatcher;
	
	PhysX::PhysX()
	{
		PHYSICS_INIT_DESC input; // TODO - Make this an input parameter.

		PxTolerancesScale scale; // TODO - Use these same values for cooking, physx init and scene desc
		scale.length = input.typicalLength;
		scale.speed = input.typicalSpeed;

		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gPhysXAllocator, gPhysXErrorHandler);
		mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation, scale);

		PxRegisterArticulations(*mPhysics);

		if (input.initCooking)
		{
			PxCookingParams cookingParams(scale); // TODO - Potentially allow more customization to set up cooking params
			mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, cookingParams);
		}

		PxSceneDesc sceneDesc(scale); // TODO - Test out various other parameters provided by scene desc
		sceneDesc.gravity = toPxVector(input.gravity);
		sceneDesc.cpuDispatcher = &gPhysXCPUDispatcher;
		sceneDesc.filterShader = PhysXFilterShader;

		// TODO - Hook up triggers
		// TODO - Allow for continuous collision detection, and regions of interest stuff
		// TODO - Set up various performance limits, call flushCache when needed
		// TODO - Probably many more startup settings I'm missing

		mScene = mPhysics->createScene(sceneDesc);
		mSimulationStep = input.timeStep;
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
			// TODO - Consider delaying fetchResults one frame. This could improve performance but at a cost to input latency.
			// TODO - Provide a scratch buffer for the simulation (use the frame allocator, but I must extend it so it allocates
			//	      on a 16 byte boundary).
			mScene->simulate(mSimulationStep);
			mScene->fetchResults(true);

			// TODO - Update all rigidbody transfroms from their PhsyX state

			simulationAmount -= mSimulationStep;
		}

		// TODO - Consider extrapolating for the remaining "simulationAmount" value

		mLastSimulationTime = curFrameTime; 
	}

	SPtr<PhysicsMaterial> PhysX::createMaterial(float staticFriction, float dynamicFriction, float restitution)
	{
		return bs_shared_ptr_new<PhysXMaterial>(mPhysics, staticFriction, dynamicFriction, restitution);
	}

	SPtr<Rigidbody> PhysX::createRigidbody(const Vector3& position, const Quaternion& rotation)
	{
		return bs_shared_ptr_new<PhysXRigidbody>(mPhysics, position, rotation);
	}
}
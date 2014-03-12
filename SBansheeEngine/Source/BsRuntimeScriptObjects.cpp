#include "BsRuntimeScriptObjects.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	SerializableObjectInfo::SerializableObjectInfo()
		:mMonoClass(nullptr)
	{

	}

	SerializableObjectInfo::~SerializableObjectInfo()
	{
		for(auto& field : mFields)
		{
			cm_delete(field.second);
		}
	}

	SerializableFieldInfo::SerializableFieldInfo()
		:mMonoField(nullptr), mType(ScriptFieldType::Primitive), mFlags((ScriptFieldFlags)0)
	{

	}

	SerializableFieldInfo::~SerializableFieldInfo()
	{

	}

	RuntimeScriptObjects::~RuntimeScriptObjects()
	{
		clearScriptObjects();
	}

	void RuntimeScriptObjects::refreshScriptObjects()
	{
		clearScriptObjects();

		// Scan all loaded assemblies
		//  - find classes deriving from Component (make sure even non-direct descendants from Component are considered)
		//  - find classes implementing [Serializable] attribute
		//  - If multiple copies of the same class are found ignore them
		// Create dummy SerializableObjectInfo entries in the map so I can quickly look up serializable objects
		//  SerializableObjectInfo should know if object is Component or [Serializable]
		// Go through all of their fields
		//  Depending on attributes and visibility, mark them as inspectable and/or serializable
		//  Ensure to only get fields of that EXACT class, not its base
		//  Detect field type:
		//   - Primitive - straightforward just check for primitive types
		//   - GameObjectHandle - check if object is SceneObject or derives from Component
		//   - SerializableObject - check if object is SerializableObject
		//   - ResourceHandle - derives from Resource
		
		// TODO - How will I serialize SerializableObjects?
		//  - We don't do any instance tracking for SerializableObjects. Each one will be deserialized and serialized
		//    as its own separate instance.
		
		// TODO - Each class will need an unique ID
		//      - So will each field within a class
		//      - Will likely need two maps
		//        - ns.type -> id
		//        - id -> SerializableObjectInfo*

		// TODO - For each class search its base class (if it has one) and see if it is Component
		//  or child of COmponent. If it is, set up the parent/child fields in SerializableObjectInfo.

		// TODO - SerializableObjectInfo needs to be IReflectable and its referenced children
		// should be shared pointers
	}

	void RuntimeScriptObjects::clearScriptObjects()
	{
		for(auto& scriptObject : mObjectInfos)
		{
			cm_delete(scriptObject.second);
		}
	}
}
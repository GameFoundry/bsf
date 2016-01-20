//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsPixelData.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for PixelData.
	 */
	class BS_SCR_BE_EXPORT ScriptPixelData : public ScriptObject <ScriptPixelData>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "PixelData")

		/**
		 * @brief	Returns the internal wrapped pixel data.
		 */
		PixelDataPtr getInternalValue() const { return mPixelData; }

		/**
		 * @brief	Creates a new managed pixel data instance that wraps
		 *			the provided native pixel data instance.
		 */
		static MonoObject* create(const PixelDataPtr& pixelData);

	private:
		ScriptPixelData(MonoObject* managedInstance);
		~ScriptPixelData();

		/**
		 * @brief	Initializes the object. Must be called after construction
		 *			and before use.
		 */
		void initialize(const PixelDataPtr& pixelData);

		/**
		 * @brief	Checks is the underlying pixel data of the provided object locked.
		 *			When locked pixel data cannot be accessed.
		 */
		static bool checkIsLocked(ScriptPixelData* thisPtr);

		PixelDataPtr mPixelData;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, PixelVolume* volume, PixelFormat format);
		static void internal_getPixel(ScriptPixelData* thisPtr, int x, int y, int z, Color* value);
		static void internal_setPixel(ScriptPixelData* thisPtr, int x, int y, int z, Color* value);
		static void internal_getPixels(ScriptPixelData* thisPtr, MonoArray** value);
		static void internal_setPixels(ScriptPixelData* thisPtr, MonoArray* value);
		static void internal_getRawPixels(ScriptPixelData* thisPtr, MonoArray** value);
		static void internal_setRawPixels(ScriptPixelData* thisPtr, MonoArray* value);
		static void internal_getExtents(ScriptPixelData* thisPtr, PixelVolume* value);
		static void internal_getFormat(ScriptPixelData* thisPtr, PixelFormat* value);
		static void internal_getRowPitch(ScriptPixelData* thisPtr, int* value);
		static void internal_getSlicePitch(ScriptPixelData* thisPtr, int* value);
		static void internal_getSize(ScriptPixelData* thisPtr, int* value);
		static void internal_getIsConsecutive(ScriptPixelData* thisPtr, bool* value);
	};
}
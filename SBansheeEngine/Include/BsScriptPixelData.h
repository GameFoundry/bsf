#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsPixelData.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptPixelData : public ScriptObject <ScriptPixelData>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "PixelData")

		PixelDataPtr getInternalValue() const { return mPixelData; }

		static MonoObject* create(const PixelDataPtr& pixelData);

	private:
		ScriptPixelData(MonoObject* managedInstance);
		~ScriptPixelData();

		void initialize(const PixelDataPtr& pixelData);

		static void internal_createInstance(MonoObject* instance, PixelVolume volume, PixelFormat format);
		static void internal_getPixel(ScriptPixelData* thisPtr, int x, int y, int z, Color* value);
		static void internal_setPixel(ScriptPixelData* thisPtr, int x, int y, int z, Color value);
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

		static bool checkIsLocked(ScriptPixelData* thisPtr);

		PixelDataPtr mPixelData;
	};
}
#pragma once

#include "BsCorePrerequisites.h"
#include "BsStaticAlloc.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsVector2I.h"
#include "BsVectorNI.h"
#include "BsColor.h"
#include "BsMatrix3.h"
#include "BsMatrix4.h"
#include "BsMatrixNxM.h"
#include "BsMaterial.h"
#include "BsShader.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Material
	 *  @{
	 */

	/** 
	 * Contains all parameter values set in a Material. This is similar to GpuParams which also stores parameter values,
	 * however GpuParams are built for use on the GPU-side and don't store parameters that don't exist in a compiled GPU
	 * program. This object on the other hand stores all parameters defined in a shader, regardless or not if they actually
	 * exist in the GPU program. Additionally GpuParams are defined per-program (e.g. vertex, fragment) while this object
	 * exists for the entire material.
	 *
	 * @note
	 * This introduces redundancy as parameters stored by GpuParams and this object are duplicated. If this is an issue the
	 * implementation can be modified to only store parameters not included in GpuParams.
	 * @note
	 * The reason why parameters in this class and GpuParams differ is most often compiler optimizations. If a compiler
	 * optimizes out a variable in a GPU program we should still be able to store it, either for later when the variable
	 * will be introduced, or for other techniques that might have that variable implemented.
	 */
	class BS_CORE_EXPORT __MaterialParams
	{
		/** Raw data for a single structure parameter. */
		class BS_CORE_EXPORT StructParamData
		{
		public:
			UINT8* data;
			UINT32 dataSize;
		};

		/** Data for a single texture parameter. */
		class BS_CORE_EXPORT TextureParamData
		{
		public:
			HTexture value;
			bool isLoadStore;
			TextureSurface surface;
		};

	public:
		/** Type of material parameter. */
		enum class ParamType
		{
			Data, Texture, Sampler
		};

		/** Meta-data about a parameter. */
		struct ParamData
		{
			ParamType type;
			GpuParamDataType dataType;
			UINT32 index;
			UINT32 arraySize;
		};

		/** Creates a new material params object and initializes enough room for parameters from the provided shader. */
		__MaterialParams(const HShader& shader);
		~__MaterialParams();

		/** 
		 * Returns the value of a shader data parameter with the specified name at the specified array index. If the
		 * parameter name, index or type is not valid a warning will be logged and output value will not be retrieved.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	arrayIdx	If the parameter is an array, index of the entry to access.
		 * @param[out]	output		If successful, value of the parameter.
		 *
		 * @tparam		T			Native type of the parameter.
		 */
		template <typename T>
		void getDataParam(const String& name, UINT32 arrayIdx, T& output) const
		{
			const ParamData* param = getParamData(name, ParamType::Data, getDataType(output), arrayIdx);
			if (param == nullptr)
				return;

			getDataParam(param->index + arrayIdx, output);
		}

		/** 
		 * Sets the value of a shader data parameter with the specified name at the specified array index. If the
		 * parameter name, index or type is not valid a warning will be logged and output value will not be set.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	arrayIdx	If the parameter is an array, index of the entry to access.
		 * @param[in]	input		New value of the parameter.
		 *
		 * @tparam		T			Native type of the parameter.
		 */
		template <typename T>
		void setDataParam(const String& name, UINT32 arrayIdx, const T& input) const
		{
			const ParamData* param = getParamData(name, ParamType::Data, getDataType(input), arrayIdx);
			if (param == nullptr)
				return;

			setDataParam(param->index + arrayIdx, input);
		}

		/** 
		 * Returns the value of a shader structure parameter with the specified name at the specified array index. If the
		 * parameter name, index or type is not valid a warning will be logged and output value will not be retrieved.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[out]	value		Pre-allocated buffer of @p size bytes where the value will be retrieved.
		 * @param[in]	size		Size of the buffer into which to write the value. Must match parameter struct's size.
		 * @param[in]	arrayIdx	If the parameter is an array, index of the entry to access.
		 */
		void getStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx);

		/** 
		 * Sets the value of a shader structure parameter with the specified name at the specified array index. If the
		 * parameter name, index or type is not valid a warning will be logged and output value will not be retrieved.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	value		Buffer of @p size bytes containing the new value of the structure.
		 * @param[in]	size		Size of the buffer from which to retrieve the value. Must match parameter struct's size.
		 * @param[in]	arrayIdx	If the parameter is an array, index of the entry to access.
		 */
		void setStructData(const String& name, const void* value, UINT32 size, UINT32 arrayIdx);

		/** 
		 * Returns the value of a shader texture parameter with the specified name. If the parameter name or type is not 
		 * valid a warning will be logged and output value will not be retrieved.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[out]	value		Output value of the parameter.
		 */
		void getTexture(const String& name, HTexture& value);

		/** 
		 * Sets the value of a shader texture parameter with the specified name. If the parameter name or type is not 
		 * valid a warning will be logged and output value will not be set.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	value		New value of the parameter.
		 */
		void setTexture(const String& name, const HTexture& value);

		/** 
		 * Returns the value of a shader load/store texture parameter with the specified name. If the parameter name or 
		 * type is not valid a warning will be logged and output value will not be retrieved.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[out]	value		Output value of the parameter.
		 * @param[out]	surface		Surface describing which part of the texture is being accessed.
		 */
		void getLoadStoreTexture(const String& name, HTexture& value, TextureSurface& surface);

		/** 
		 * Sets the value of a shader load/store texture parameter with the specified name. If the parameter name or 
		 * type is not valid a warning will be logged and the value will not be set.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	value		New value of the parameter.
		 * @param[in]	surface		Surface describing which part of the texture is being accessed.
		 */
		void setLoadStoreTexture(const String& name, const HTexture& value, const TextureSurface& surface);

		/** 
		 * Sets the value of a shader sampler state parameter with the specified name. If the parameter name or type is not 
		 * valid a warning will be logged and output value will not be set.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[out]	value		Output value of the parameter.
		 */
		void getSamplerState(const String& name, SamplerStatePtr& value);

		/** 
		 * Sets the value of a shader sampler state parameter with the specified name. If the parameter name or type is not 
		 * valid a warning will be logged and output value will not be set.
		 * 
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	value		New value of the parameter.
		 */
		void setSamplerState(const String& name, const SamplerStatePtr& value);

		/** 
		 * Returns data about a parameter and reports an error if there is a type or size mismatch, or if the parameter
		 * does exist.
		 *
		 * @param[in]	name		Name of the shader parameter.
		 * @param[in]	type		Type of the parameter retrieve. Error will be logged if actual type of the parameter 
		 *							doesn't match.
		 * @param[in]	dataType	Only relevant if the parameter is a data type. Determines exact data type of the parameter
		 *							to retrieve. Error will be logged if the type of the parameter doesn't match.
		 * @param[in]	arrayIdx	Array index of the entry to retrieve. Error will be logged if the array index is out
		 *							side of the valid range.
		 *
		 * @return					If successful, object describing the parameter with an index to its data. Otherwise
		 *							null.
		 */
		const ParamData* getParamData(const String& name, ParamType type, GpuParamDataType dataType, UINT32 arrayIdx);

		/** Returns a value of a parameter using its iternal array index as returned by getParamData(). */
		void getDataParam(UINT32 index, float& output) const { output = mFloatParams[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Vector2& output) const { output = mVec2Params[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Vector3& output) const { output = mVec3Params[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Vector4& output) const { output = mVec4Params[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Matrix3& output) const { output = mMat3Params[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Matrix4& output) const { output = mMat4Params[index]; }

		/** @copydoc getDataParam(UINT32, float&) */
		void getDataParam(UINT32 index, Color& output) const { output = mColorParams[index]; }

		/** Sets a value of a parameter using its iternal array index as returned by getParamData(). */
		void setDataParam(UINT32 index, float input) { mFloatParams[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Vector2 input) { mVec2Params[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Vector3 input) { mVec3Params[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Vector4 input) { mVec4Params[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Matrix3 input) { mMat3Params[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Matrix4 input) { mMat4Params[index] = input; }

		/** @copydoc setDataParam(UINT32, float&) */
		void setDataParam(UINT32 index, Color input) { mColorParams[index] = input; }

		/** 
		 * Equivalent to getStructData(const String&, UINT32, void*, UINT32) except it uses the internal parameter index
		 * directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void getStructData(UINT32 index, void* value, UINT32 size);

		/** 
		 * Equivalent to setStructData(const String&, UINT32, void*, UINT32) except it uses the internal parameter index
		 * directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void setStructData(UINT32 index, const void* value, UINT32 size);

		/** 
		 * Equivalent to getTexture(const String&, HTexture&) except it uses the internal parameter index directly, 
		 * avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void getTexture(UINT32 index, HTexture& value);

		/** 
		 * Equivalent to setTexture(const String&, HTexture&) except it uses the internal parameter index directly, 
		 * avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void setTexture(UINT32 index, const HTexture& value);

		/** 
		 * Equivalent to getLoadStoreTexture(const String&, HTexture&, TextureSurface&) except it uses the internal 
		 * parameter index directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void getLoadStoreTexture(UINT32 index, HTexture& value, TextureSurface& surface);

		/** 
		 * Equivalent to setLoadStoreTexture(const String&, HTexture&, TextureSurface&) except it uses the internal 
		 * parameter index directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void setLoadStoreTexture(UINT32 index, const HTexture& value, const TextureSurface& surface);

		/** 
		 * Equivalent to getSamplerState(const String&, SamplerStatePtr&) except it uses the internal parameter index 
		 * directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void getSamplerState(UINT32 index, SamplerStatePtr& value);

		/** 
		 * Equivalent to setSamplerState(const String&, SamplerStatePtr&) except it uses the internal parameter index 
		 * directly, avoiding the name lookup. Caller must guarantee the index is valid.
		 */
		void setSamplerState(UINT32 index, const SamplerStatePtr& value);

		static GpuParamDataType getDataType(const float& dummy) { return GPDT_FLOAT1; }
		static GpuParamDataType getDataType(const Vector2& dummy) { return GPDT_FLOAT2; }
		static GpuParamDataType getDataType(const Vector3& dummy) { return GPDT_FLOAT3; }
		static GpuParamDataType getDataType(const Vector4& dummy) { return GPDT_FLOAT4; }
		static GpuParamDataType getDataType(const int& dummy) { return GPDT_INT1; }
		static GpuParamDataType getDataType(const Vector2I& dummy) { return GPDT_INT1; }
		static GpuParamDataType getDataType(const Vector3I& dummy) { return GPDT_INT1; }
		static GpuParamDataType getDataType(const Vector4I& dummy) { return GPDT_INT1; }
		static GpuParamDataType getDataType(const Matrix2& dummy) { return GPDT_MATRIX_2X2; }
		static GpuParamDataType getDataType(const Matrix2x3& dummy) { return GPDT_MATRIX_2X3; }
		static GpuParamDataType getDataType(const Matrix2x4& dummy) { return GPDT_MATRIX_2X4; }
		static GpuParamDataType getDataType(const Matrix3& dummy) { return GPDT_MATRIX_3X3; }
		static GpuParamDataType getDataType(const Matrix3x2& dummy) { return GPDT_MATRIX_3X2; }
		static GpuParamDataType getDataType(const Matrix3x4& dummy) { return GPDT_MATRIX_3X4; }
		static GpuParamDataType getDataType(const Matrix4& dummy) { return GPDT_MATRIX_4X4; }
		static GpuParamDataType getDataType(const Matrix4x2& dummy) { return GPDT_MATRIX_4X2; }
		static GpuParamDataType getDataType(const Matrix4x3& dummy) { return GPDT_MATRIX_4X3; }
		static GpuParamDataType getDataType(const Color& dummy) { return GPDT_COLOR; }
	private:
		const static UINT32 STATIC_BUFFER_SIZE = 256;

		UnorderedMap<String, ParamData> mParams;

		float* mFloatParams = nullptr;
		Vector2* mVec2Params = nullptr;
		Vector3* mVec3Params = nullptr;
		Vector4* mVec4Params = nullptr;
		int* mIntParams = nullptr;
		Vector2I* mVec2IParams = nullptr;
		Vector3I* mVec3IParams = nullptr;
		Vector4I* mVec4IParams = nullptr;
		Matrix2* mMat2Params = nullptr;
		Matrix2x3* mMat2x3Params = nullptr;
		Matrix2x4* mMat2x4Params = nullptr;
		Matrix3* mMat3Params = nullptr;
		Matrix3x2* mMat3x2Params = nullptr;
		Matrix3x4* mMat3x4Params = nullptr;
		Matrix4* mMat4Params = nullptr;
		Matrix4x2* mMat4x2Params = nullptr;
		Matrix4x3* mMat4x3Params = nullptr;
		Color* mColorParams = nullptr;
		StructParamData* mStructParams = nullptr;
		TextureParamData* mTextureParams = nullptr;
		SamplerStatePtr* mSamplerStateParams = nullptr;

		UINT32 mNumFloatParams = 0;
		UINT32 mNumVec2Params = 0;
		UINT32 mNumVec3Params = 0;
		UINT32 mNumVec4Params = 0;
		UINT32 mNumIntParams = 0;
		UINT32 mNumVec2IParams = 0;
		UINT32 mNumVec3IParams = 0;
		UINT32 mNumVec4IParams = 0;
		UINT32 mNumMat2Params = 0;
		UINT32 mNumMat2x3Params = 0;
		UINT32 mNumMat2x4Params = 0;
		UINT32 mNumMat3Params = 0;
		UINT32 mNumMat3x2Params = 0;
		UINT32 mNumMat3x4Params = 0;
		UINT32 mNumMat4Params = 0;
		UINT32 mNumMat4x2Params = 0;
		UINT32 mNumMat4x3Params = 0;
		UINT32 mNumColorParams = 0;
		UINT32 mNumStructParams = 0;
		UINT32 mNumTextureParams = 0;
		UINT32 mNumSamplerParams = 0;

		mutable StaticAlloc<STATIC_BUFFER_SIZE, STATIC_BUFFER_SIZE> mAlloc;
	};

	/** @} */
	/** @endcond */
}

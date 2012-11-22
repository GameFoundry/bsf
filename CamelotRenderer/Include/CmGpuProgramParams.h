/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __GpuProgramParams_H_
#define __GpuProgramParams_H_

// Precompiler options
#include "CmPrerequisites.h"
#include "CmRenderCommand.h"
#include "CmSamplerState.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Materials
	*  @{
	*/
	/** Enumeration of the types of constant we may encounter in programs. 
	@note Low-level programs, by definition, will always use either
	float4 or int4 constant types since that is the fundamental underlying
	type in assembler.
	*/
	enum GpuConstantType
	{
		GCT_FLOAT1 = 1,
		GCT_FLOAT2 = 2,
		GCT_FLOAT3 = 3,
		GCT_FLOAT4 = 4,
		GCT_SAMPLER1D = 5,
		GCT_SAMPLER2D = 6,
		GCT_SAMPLER3D = 7,
		GCT_SAMPLERCUBE = 8,
		GCT_SAMPLER1DSHADOW = 9,
		GCT_SAMPLER2DSHADOW = 10,
		GCT_MATRIX_2X2 = 11,
		GCT_MATRIX_2X3 = 12,
		GCT_MATRIX_2X4 = 13,
		GCT_MATRIX_3X2 = 14,
		GCT_MATRIX_3X3 = 15,
		GCT_MATRIX_3X4 = 16,
		GCT_MATRIX_4X2 = 17,
		GCT_MATRIX_4X3 = 18,
		GCT_MATRIX_4X4 = 19,
		GCT_INT1 = 20,
		GCT_INT2 = 21,
		GCT_INT3 = 22,
		GCT_INT4 = 23,
		GCT_UNKNOWN = 99
	};

	/** The variability of a GPU parameter, as derived from auto-params targetting it.
	These values must be powers of two since they are used in masks.
	*/
	enum GpuParamVariability
	{
		/// No variation except by manual setting - the default
		GPV_GLOBAL = 1, 
		/// Varies per object (based on an auto param usually), but not per light setup
		GPV_PER_OBJECT = 2, 
		/// Varies with light setup
		GPV_LIGHTS = 4, 

		/// Full mask (16-bit)
		GPV_ALL = 0xFFFF

	};

	/** Information about predefined program constants. 
	@note Only available for high-level programs but is referenced generically
	by GpuProgramParameters.
	*/
	struct CM_EXPORT GpuConstantDefinition
	{
		/// Data type
		GpuConstantType constType;
		/// Physical start index in buffer (either float or int buffer)
		size_t physicalIndex;
		/// Logical index - used to communicate this constant to the rendersystem
		size_t logicalIndex;
		/** Number of raw buffer slots per element 
		(some programs pack each array element to float4, some do not) */
		size_t elementSize;
		/// Length of array
		size_t arraySize;
		/// How this parameter varies (bitwise combination of GpuProgramVariability)
		mutable UINT16 variability;

		bool isFloat() const
		{
			return isFloat(constType);
		}

		static bool isFloat(GpuConstantType c)
		{
			switch(c)
			{
			case GCT_INT1:
			case GCT_INT2:
			case GCT_INT3:
			case GCT_INT4:
			case GCT_SAMPLER1D:
			case GCT_SAMPLER2D:
			case GCT_SAMPLER3D:
			case GCT_SAMPLERCUBE:
			case GCT_SAMPLER1DSHADOW:
			case GCT_SAMPLER2DSHADOW:
				return false;
			default:
				return true;
			};

		}

		bool isSampler() const
		{
			return isSampler(constType);
		}

		static bool isSampler(GpuConstantType c)
		{
			switch(c)
			{
			case GCT_SAMPLER1D:
			case GCT_SAMPLER2D:
			case GCT_SAMPLER3D:
			case GCT_SAMPLERCUBE:
			case GCT_SAMPLER1DSHADOW:
			case GCT_SAMPLER2DSHADOW:
				return true;
			default:
				return false;
			};

		}


		/** Get the element size of a given type, including whether to pad the 
			elements into multiples of 4 (e.g. SM1 and D3D does, GLSL doesn't)
		*/
		static size_t getElementSize(GpuConstantType ctype, bool padToMultiplesOf4)
		{
			if (padToMultiplesOf4)
			{
				switch(ctype)
				{
				case GCT_FLOAT1:
				case GCT_INT1:
				case GCT_SAMPLER1D:
				case GCT_SAMPLER2D:
				case GCT_SAMPLER3D:
				case GCT_SAMPLERCUBE:
				case GCT_SAMPLER1DSHADOW:
				case GCT_SAMPLER2DSHADOW:
				case GCT_FLOAT2:
				case GCT_INT2:
				case GCT_FLOAT3:
				case GCT_INT3:
				case GCT_FLOAT4:
				case GCT_INT4:
					return 4;
				case GCT_MATRIX_2X2:
				case GCT_MATRIX_2X3:
				case GCT_MATRIX_2X4:
					return 8; // 2 float4s
				case GCT_MATRIX_3X2:
				case GCT_MATRIX_3X3:
				case GCT_MATRIX_3X4:
					return 12; // 3 float4s
				case GCT_MATRIX_4X2:
				case GCT_MATRIX_4X3:
				case GCT_MATRIX_4X4:
					return 16; // 4 float4s
				default:
					return 4;
				};
			}
			else
			{
				switch(ctype)
				{
				case GCT_FLOAT1:
				case GCT_INT1:
				case GCT_SAMPLER1D:
				case GCT_SAMPLER2D:
				case GCT_SAMPLER3D:
				case GCT_SAMPLERCUBE:
				case GCT_SAMPLER1DSHADOW:
				case GCT_SAMPLER2DSHADOW:
					return 1;
				case GCT_FLOAT2:
				case GCT_INT2:
					return 2;
				case GCT_FLOAT3:
				case GCT_INT3:
					return 3;
				case GCT_FLOAT4:
				case GCT_INT4:
					return 4;
				case GCT_MATRIX_2X2:
					return 4;
				case GCT_MATRIX_2X3:
				case GCT_MATRIX_3X2:
					return 6;
				case GCT_MATRIX_2X4:
				case GCT_MATRIX_4X2:
					return 8; 
				case GCT_MATRIX_3X3:
					return 9;
				case GCT_MATRIX_3X4:
				case GCT_MATRIX_4X3:
					return 12; 
				case GCT_MATRIX_4X4:
					return 16; 
				default:
					return 4;
				};

			}
		}

		GpuConstantDefinition()
			: constType(GCT_UNKNOWN)
			, physicalIndex((std::numeric_limits<size_t>::max)())
			, elementSize(0)
			, arraySize(1)
			, variability(GPV_GLOBAL) {}
	};
	typedef map<String, GpuConstantDefinition>::type GpuConstantDefinitionMap;
	typedef GpuConstantDefinitionMap::const_iterator GpuConstantDefinitionIterator;

	/// Struct collecting together the information for named constants.
	struct CM_EXPORT GpuNamedConstants
	{
		/// Total size of the float buffer required
		size_t floatBufferSize;
		/// Total size of the int buffer required
		size_t intBufferSize;
		/// Total number of samplers referenced
		size_t samplerCount;
		/// Map of parameter names to GpuConstantDefinition
		GpuConstantDefinitionMap map;

		GpuNamedConstants() : floatBufferSize(0), intBufferSize(0), samplerCount(0) {}

		/** Generate additional constant entries for arrays based on a base definition.
		@remarks
		Array uniforms will be added just with their base name with no array
		suffix. This method will add named entries for array suffixes too
		so individual array entries can be addressed. Note that we only 
		individually index array elements if the array size is up to 16
		entries in size. Anything larger than that only gets a [0] entry
		as well as the main entry, to save cluttering up the name map. After
		all, you can address the larger arrays in a bulk fashion much more
		easily anyway. 
		*/
		void generateConstantDefinitionArrayEntries(const String& paramName, 
			const GpuConstantDefinition& baseDef);

		/// Indicates whether all array entries will be generated and added to the definitions map
		static bool getGenerateAllConstantDefinitionArrayEntries();

		/** Sets whether all array entries will be generated and added to the definitions map.
		@remarks
		Usually, array entries can only be individually indexed if they're up to 16 entries long,
		to save memory - arrays larger than that can be set but only via the bulk setting
		methods. This option allows you to choose to individually index every array entry. 
		*/
		static void setGenerateAllConstantDefinitionArrayEntries(bool generateAll);

	protected:
		/** Indicates whether all array entries will be generated and added to the definitions map
		@remarks
		Normally, the number of array entries added to the definitions map is capped at 16
		to save memory. Setting this value to <code>true</code> allows all of the entries
		to be generated and added to the map.
		*/
		static bool msGenerateAllConstantDefinitionArrayEntries;
	};
	typedef std::shared_ptr<GpuNamedConstants> GpuNamedConstantsPtr;

	/** Structure recording the use of a physical buffer by a logical parameter
	index. Only used for low-level programs.
	*/
	struct CM_EXPORT GpuLogicalIndexUse
	{
		/// Physical buffer index
		size_t physicalIndex;
		/// Current physical size allocation
		size_t currentSize;
		/// How the contents of this slot vary
		mutable UINT16 variability;

		GpuLogicalIndexUse() 
			: physicalIndex(99999), currentSize(0), variability(GPV_GLOBAL) {}
		GpuLogicalIndexUse(size_t bufIdx, size_t curSz, UINT16 v) 
			: physicalIndex(bufIdx), currentSize(curSz), variability(v) {}
	};
	typedef map<size_t, GpuLogicalIndexUse>::type GpuLogicalIndexUseMap;
	/// Container struct to allow params to safely & update shared list of logical buffer assignments
	struct CM_EXPORT GpuLogicalBufferStruct
	{
		CM_MUTEX(mutex)
			/// Map from logical index to physical buffer location
			GpuLogicalIndexUseMap map;
		/// Shortcut to know the buffer size needs
		size_t bufferSize;
		GpuLogicalBufferStruct() : bufferSize(0) {}
	};
	typedef std::shared_ptr<GpuLogicalBufferStruct> GpuLogicalBufferStructPtr;

	/**
	 * @brief	Contains a reference to a texture together with it's sampler properties
	 */
	struct GpuTextureEntry
	{
		TextureRef texture;
		SamplerState samplerState;
	};

	typedef std::shared_ptr<GpuTextureEntry> GpuTextureEntryPtr;

	/** Definition of container that holds the current float constants.
	@note Not necessarily in direct index order to constant indexes, logical
	to physical index map is derived from GpuProgram
	*/
	typedef vector<float>::type FloatConstantList;
	/** Definition of container that holds the current float constants.
	@note Not necessarily in direct index order to constant indexes, logical
	to physical index map is derived from GpuProgram
	*/
	typedef vector<int>::type IntConstantList;
	/** Definition of container that holds the current float constants.
	@note Not necessarily in direct index order to constant indexes, logical
	to physical index map is derived from GpuProgram
	*/
	typedef vector<GpuTextureEntryPtr>::type TextureList;

	/** Collects together the program parameters used for a GpuProgram.
	@remarks
	Gpu program state includes constant parameters used by the program, and
	bindings to render system state which is propagated into the constants 
	by the engine automatically if requested.
	@par
	GpuProgramParameters objects should be created through the GpuProgram and
	may be shared between multiple Pass instances. For this reason they
	are managed using a shared pointer, which will ensure they are automatically
	deleted when no Pass is using them anymore. 
	@par
	High-level programs use named parameters (uniforms), low-level programs 
	use indexed constants. This class supports both, but you can tell whether 
	named constants are supported by calling hasNamedParameters(). There are
	references in the documentation below to 'logical' and 'physical' indexes;
	logical indexes are the indexes used by low-level programs and represent 
	indexes into an array of float4's, some of which may be settable, some of
	which may be predefined constants in the program. We only store those
	constants which have actually been set, therefore our buffer could have 
	gaps if we used the logical indexes in our own buffers. So instead we map
	these logical indexes to physical indexes in our buffer. When using 
	high-level programs, logical indexes don't necessarily exist, although they
	might if the high-level program has a direct, exposed mapping from parameter
	names to logical indexes. In addition, high-level languages may or may not pack
	arrays of elements that are smaller than float4 (e.g. float2/vec2) contiguously.
	This kind of information is held in the ConstantDefinition structure which 
	is only populated for high-level programs. You don't have to worry about
	any of this unless you intend to read parameters back from this structure
	rather than just setting them.
	*/
	class CM_EXPORT GpuProgramParameters
	{
	public:
		/** Defines the type of the extra data item used by the auto constant.

		*/
		enum ACDataType {
			/// no data is required
			ACDT_NONE,
			/// the auto constant requires data of type int
			ACDT_INT,
			/// the auto constant requires data of type real
			ACDT_REAL
		};

		/** Defines the base element type of the auto constant
		*/
		enum ElementType {
			ET_INT,
			ET_REAL
		};

	protected:
		/// Packed list of floating-point constants (physical indexing)
		FloatConstantList mFloatConstants;
		/// Packed list of integer constants (physical indexing)
		IntConstantList mIntConstants;
		/// List of all texture parameters
		TextureList mTextures;
		/** Logical index to physical index map - for low-level programs
		or high-level programs which pass params this way. */
		GpuLogicalBufferStructPtr mFloatLogicalToPhysical;
		/** Logical index to physical index map - for low-level programs
		or high-level programs which pass params this way. */
		GpuLogicalBufferStructPtr mIntLogicalToPhysical;
		/** Logical index to physical index map - for low-level programs
		or high-level programs which pass params this way. */
		GpuLogicalBufferStructPtr mSamplerLogicalToPhysical;
		/// Mapping from parameter names to def - high-level programs are expected to populate this
		GpuNamedConstantsPtr mNamedConstants;
		/// The combined variability masks of all parameters
		UINT16 mCombinedVariability;
		/// Do we need to transpose matrices?
		bool mTransposeMatrices;
		/// flag to indicate if names not found will be ignored
		bool mIgnoreMissingParams;

		/** Gets the low-level structure for a logical index. 
		*/
		GpuLogicalIndexUse* _getFloatConstantLogicalIndexUse(size_t logicalIndex, size_t requestedSize, UINT16 variability);
		/** Gets the physical buffer index associated with a logical int constant index. 
		*/
		GpuLogicalIndexUse* _getIntConstantLogicalIndexUse(size_t logicalIndex, size_t requestedSize, UINT16 variability);

	public:
		GpuProgramParameters();
		~GpuProgramParameters() {}

		/// Copy constructor
		GpuProgramParameters(const GpuProgramParameters& oth);
		/// Operator = overload
		GpuProgramParameters& operator=(const GpuProgramParameters& oth);

		/** Internal method for providing a link to a name->definition map for parameters. */
		void _setNamedConstants(const GpuNamedConstantsPtr& constantmap);

		/** Internal method for providing a link to a logical index->physical index map for parameters. */
		void _setLogicalIndexes(const GpuLogicalBufferStructPtr& floatIndexMap, 
			const GpuLogicalBufferStructPtr&  intIndexMap, const GpuLogicalBufferStructPtr& samplerIndexMap);


		/// Does this parameter set include named parameters?
		bool hasNamedParameters() const { return mNamedConstants != nullptr; }
		/** Does this parameter set include logically indexed parameters?
		@note Not mutually exclusive with hasNamedParameters since some high-level
		programs still use logical indexes to set the parameters on the 
		rendersystem.
		*/
		bool hasLogicalIndexedParameters() const { return mFloatLogicalToPhysical != nullptr; }

		/** Sets a 4-element floating-point parameter to the program.
		@param index The logical constant index at which to place the parameter 
		(each constant is a 4D float)
		@param vec The value to set
		*/
		void setConstant(size_t index, const Vector4& vec);
		/** Sets a single floating-point parameter to the program.
		@note This is actually equivalent to calling 
		setConstant(index Vector4(val, 0, 0, 0)) since all constants are 4D.
		@param index The logical constant index at which to place the parameter (each constant is
		a 4D float)
		@param val The value to set
		*/
		void setConstant(size_t index, float val);
		/** Sets a 4-element floating-point parameter to the program via Vector3.
		@param index The logical constant index at which to place the parameter (each constant is
		a 4D float).
		Note that since you're passing a Vector3, the last element of the 4-element
		value will be set to 1 (a homogeneous vector)
		@param vec The value to set
		*/
		void setConstant(size_t index, const Vector3& vec);
		/** Sets a Matrix4 parameter to the program.
		@param index The logical constant index at which to place the parameter (each constant is
		a 4D float).
		NB since a Matrix4 is 16 floats long, this parameter will take up 4 indexes.
		@param m The value to set
		*/
		void setConstant(size_t index, const Matrix4& m);
		/** Sets a list of Matrix4 parameters to the program.
		@param index The logical constant index at which to start placing the parameter (each constant is
		a 4D float).
		NB since a Matrix4 is 16 floats long, so each entry will take up 4 indexes.
		@param m Pointer to an array of matrices to set
		@param numEntries Number of Matrix4 entries
		*/
		void setConstant(size_t index, const Matrix4* m, size_t numEntries);
		/** Sets a multiple value constant floating-point parameter to the program.
		@param index The logical constant index at which to start placing parameters (each constant is
		a 4D float)
		@param val Pointer to the values to write, must contain 4*count floats
		@param count The number of groups of 4 floats to write
		*/
		void setConstant(size_t index, const float *val, size_t count);
		/** Sets a multiple value constant floating-point parameter to the program.
		@param index The logical constant index at which to start placing parameters (each constant is
		a 4D float)
		@param val Pointer to the values to write, must contain 4*count floats
		@param count The number of groups of 4 floats to write
		*/
		void setConstant(size_t index, const double *val, size_t count);
		/** Sets a ColourValue parameter to the program.
		@param index The logical constant index at which to place the parameter (each constant is
		a 4D float)
		@param colour The value to set
		*/
		void setConstant(size_t index, const Color& colour);

		/** Sets a multiple value constant integer parameter to the program.
		@remarks
		Different types of GPU programs support different types of constant parameters.
		For example, it's relatively common to find that vertex programs only support
		floating point constants, and that fragment programs only support integer (fixed point)
		parameters. This can vary depending on the program version supported by the
		graphics card being used. You should consult the documentation for the type of
		low level program you are using, or alternatively use the methods
		provided on RenderSystemCapabilities to determine the options.
		@param index The logical constant index at which to place the parameter (each constant is
		a 4D integer)
		@param val Pointer to the values to write, must contain 4*count ints
		@param count The number of groups of 4 ints to write
		*/
		void setConstant(size_t index, const int *val, size_t count);

		/** Write a series of floating point values into the underlying float 
		constant buffer at the given physical index.
		@param physicalIndex The buffer position to start writing
		@param val Pointer to a list of values to write
		@param count The number of floats to write
		*/
		void _writeRawConstants(size_t physicalIndex, const float* val, size_t count);
		/** Write a series of floating point values into the underlying float 
		constant buffer at the given physical index.
		@param physicalIndex The buffer position to start writing
		@param val Pointer to a list of values to write
		@param count The number of floats to write
		*/
		void _writeRawConstants(size_t physicalIndex, const double* val, size_t count);
		/** Write a series of integer values into the underlying integer
		constant buffer at the given physical index.
		@param physicalIndex The buffer position to start writing
		@param val Pointer to a list of values to write
		@param count The number of ints to write
		*/
		void _writeRawConstants(size_t physicalIndex, const int* val, size_t count);
		/** Read a series of floating point values from the underlying float 
		constant buffer at the given physical index.
		@param physicalIndex The buffer position to start reading
		@param count The number of floats to read
		@param dest Pointer to a buffer to receive the values
		*/
		void _readRawConstants(size_t physicalIndex, size_t count, float* dest);
		/** Read a series of integer values from the underlying integer 
		constant buffer at the given physical index.
		@param physicalIndex The buffer position to start reading
		@param count The number of ints to read
		@param dest Pointer to a buffer to receive the values
		*/
		void _readRawConstants(size_t physicalIndex, size_t count, int* dest);
		/** Read a texture from the underlying texture 
		array at the given physical index.
		@param physicalIndex The array position of the texture
		@param dest Reference of the texture to store
		*/
		void _readTexture(size_t physicalIndex, TextureRef& dest);
		/** Write a 4-element floating-point parameter to the program directly to 
		the underlying constants buffer.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param vec The value to set
		@param count The number of floats to write; if for example
		the uniform constant 'slot' is smaller than a Vector4
		*/
		void _writeRawConstant(size_t physicalIndex, const Vector4& vec, 
			size_t count = 4);
		/** Write a single floating-point parameter to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param val The value to set
		*/
		void _writeRawConstant(size_t physicalIndex, float val);
		/** Write a single integer parameter to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param val The value to set
		*/
		void _writeRawConstant(size_t physicalIndex, int val);
		/** Write a 3-element floating-point parameter to the program via Vector3.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param vec The value to set
		*/
		void _writeRawConstant(size_t physicalIndex, const Vector3& vec);
		/** Write a 2-element floating-point parameter to the program via Vector2.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param vec The value to set
		*/
		void _writeRawConstant(size_t physicalIndex, const Vector2& vec);
		/** Write a Matrix4 parameter to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param m The value to set
		@param elementCount actual element count used with shader
		*/
		void _writeRawConstant(size_t physicalIndex, const Matrix4& m, size_t elementCount);
		/** Write a list of Matrix4 parameters to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param numEntries Number of Matrix4 entries
		*/
		void _writeRawConstant(size_t physicalIndex, const Matrix4* m, size_t numEntries);
		/** Write a Matrix3 parameter to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param m The value to set
		@param elementCount actual element count used with shader
		*/
		void _writeRawConstant(size_t physicalIndex, const Matrix3& m, size_t elementCount);
		/** Write a ColourValue parameter to the program.
		@note You can use these methods if you have already derived the physical
		constant buffer location, for a slight speed improvement over using
		the named / logical index versions.
		@param physicalIndex The physical buffer index at which to place the parameter 
		@param colour The value to set
		@param count The number of floats to write; if for example
		the uniform constant 'slot' is smaller than a Vector4
		*/
		void _writeRawConstant(size_t physicalIndex, const Color& colour, 
			size_t count = 4);


		/** Gets an iterator over the named GpuConstantDefinition instances as defined
		by the program for which these parameters exist.
		@note
		Only available if this parameters object has named parameters.
		*/
		GpuConstantDefinitionIterator getConstantDefinitionIterator(void) const;

		/** Get a specific GpuConstantDefinition for a named parameter.
		@note
		Only available if this parameters object has named parameters.
		*/
		const GpuConstantDefinition& getConstantDefinition(const String& name) const;

		/** Get the full list of GpuConstantDefinition instances.
		@note
		Only available if this parameters object has named parameters.
		*/
		const GpuNamedConstants& getConstantDefinitions() const;

		/** Get the current list of mappings from low-level logical param indexes
		to physical buffer locations in the float buffer.
		@note
		Only applicable to low-level programs.
		*/
		const GpuLogicalBufferStructPtr& getFloatLogicalBufferStruct() const { return mFloatLogicalToPhysical; }

		/** Retrieves the logical index relating to a physical index in the float
		buffer, for programs which support that (low-level programs and 
		high-level programs which use logical parameter indexes).
		@returns std::numeric_limits<size_t>::max() if not found
		*/
		size_t getFloatLogicalIndexForPhysicalIndex(size_t physicalIndex);
		/** Retrieves the logical index relating to a physical index in the int
		buffer, for programs which support that (low-level programs and 
		high-level programs which use logical parameter indexes).
		@returns std::numeric_limits<size_t>::max() if not found
		*/
		size_t getIntLogicalIndexForPhysicalIndex(size_t physicalIndex);

		/** Get the current list of mappings from low-level logical param indexes
		to physical buffer locations in the integer buffer.
		@note
		Only applicable to low-level programs.
		*/
		const GpuLogicalBufferStructPtr& getIntLogicalBufferStruct() const { return mIntLogicalToPhysical; }
		/// Get a reference to the list of float constants
		const FloatConstantList& getFloatConstantList() const { return mFloatConstants; }
		/// Get a pointer to the 'nth' item in the float buffer
		float* getFloatPointer(size_t pos) { return &mFloatConstants[pos]; }
		/// Get a pointer to the 'nth' item in the float buffer
		const float* getFloatPointer(size_t pos) const { return &mFloatConstants[pos]; }
		/// Get a reference to the list of int constants
		const IntConstantList& getIntConstantList() const { return mIntConstants; }
		/// Get a pointer to the 'nth' item in the int buffer
		int* getIntPointer(size_t pos) { return &mIntConstants[pos]; }
		/// Get a pointer to the 'nth' item in the int buffer
		const int* getIntPointer(size_t pos) const { return &mIntConstants[pos]; }
		const GpuLogicalBufferStructPtr& getSamplerLogicalBufferStruct() const { return mSamplerLogicalToPhysical; }
		TextureRef getTexture(size_t pos) const;
		const SamplerState& getSamplerState(size_t pos) const;
		/// Get a reference to the list of textures
		const TextureList& getTextureList() const { return mTextures; }

		/** Tells the program whether to ignore missing parameters or not.
		*/
		void setIgnoreMissingParams(bool state) { mIgnoreMissingParams = state; }

		/** Sets a texture parameter to the program.
		@remarks
		Different types of GPU programs support different types of constant parameters.
		For example, it's relatively common to find that vertex programs only support
		floating point constants, and that fragment programs only support integer (fixed point)
		parameters. This can vary depending on the program version supported by the
		graphics card being used. You should consult the documentation for the type of
		low level program you are using, or alternatively use the methods
		provided on RenderSystemCapabilities to determine the options.
		@par
		Another possible limitation is that some systems only allow constants to be set
		on certain boundaries, e.g. in sets of 4 values for example. Again, see
		RenderSystemCapabilities for full details.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val The value to set
		*/
		void setNamedConstant(const String& name, TextureRef val);

		/** Sets a sampler state to the program. Name of the sampler should be the same
		as the name of the texture parameter it is being set for.
		@remarks
		Different types of GPU programs support different types of constant parameters.
		For example, it's relatively common to find that vertex programs only support
		floating point constants, and that fragment programs only support integer (fixed point)
		parameters. This can vary depending on the program version supported by the
		graphics card being used. You should consult the documentation for the type of
		low level program you are using, or alternatively use the methods
		provided on RenderSystemCapabilities to determine the options.
		@par
		Another possible limitation is that some systems only allow constants to be set
		on certain boundaries, e.g. in sets of 4 values for example. Again, see
		RenderSystemCapabilities for full details.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val The value to set
		*/
		void setNamedConstant(const String& name, const SamplerState& val);

		/** Sets a single value constant floating-point parameter to the program.
		@remarks
		Different types of GPU programs support different types of constant parameters.
		For example, it's relatively common to find that vertex programs only support
		floating point constants, and that fragment programs only support integer (fixed point)
		parameters. This can vary depending on the program version supported by the
		graphics card being used. You should consult the documentation for the type of
		low level program you are using, or alternatively use the methods
		provided on RenderSystemCapabilities to determine the options.
		@par
		Another possible limitation is that some systems only allow constants to be set
		on certain boundaries, e.g. in sets of 4 values for example. Again, see
		RenderSystemCapabilities for full details.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val The value to set
		*/
		void setNamedConstant(const String& name, float val);
		/** Sets a single value constant integer parameter to the program.
		@remarks
		Different types of GPU programs support different types of constant parameters.
		For example, it's relatively common to find that vertex programs only support
		floating point constants, and that fragment programs only support integer (fixed point)
		parameters. This can vary depending on the program version supported by the
		graphics card being used. You should consult the documentation for the type of
		low level program you are using, or alternatively use the methods
		provided on RenderSystemCapabilities to determine the options.
		@par
		Another possible limitation is that some systems only allow constants to be set
		on certain boundaries, e.g. in sets of 4 values for example. Again, see
		RenderSystemCapabilities for full details.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val The value to set
		*/
		void setNamedConstant(const String& name, int val);
		/** Sets a Vector4 parameter to the program.
		@param name The name of the parameter
		@param vec The value to set
		*/
		void setNamedConstant(const String& name, const Vector4& vec);
		/** Sets a Vector3 parameter to the program.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param index The index at which to place the parameter
		NB this index refers to the number of floats, so a Vector3 is 3. Note that many 
		rendersystems & programs assume that every floating point parameter is passed in
		as a vector of 4 items, so you are strongly advised to check with 
		RenderSystemCapabilities before using this version - if in doubt use Vector4
		or ColourValue instead (both are 4D).
		@param vec The value to set
		*/
		void setNamedConstant(const String& name, const Vector3& vec);
		/** Sets a Vector2 parameter to the program.
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param vec The value to set
		*/
		void setNamedConstant(const String& name, const Vector2& vec);
		/** Sets a Matrix4 parameter to the program.
		@param name The name of the parameter
		@param m The value to set
		*/
		void setNamedConstant(const String& name, const Matrix4& m);
		/** Sets a list of Matrix4 parameters to the program.
		@param name The name of the parameter; this must be the first index of an array,
		for examples 'matrices[0]'
		NB since a Matrix4 is 16 floats long, so each entry will take up 4 indexes.
		@param m Pointer to an array of matrices to set
		@param numEntries Number of Matrix4 entries
		*/
		void setNamedConstant(const String& name, const Matrix4* m, size_t numEntries);
		/** Sets a Matrix3 parameter to the program.
		@param name The name of the parameter
		@param m The value to set
		*/
		void setNamedConstant(const String& name, const Matrix3& m);
		/** Sets a multiple value constant floating-point parameter to the program.
		@par
		Some systems only allow constants to be set on certain boundaries, 
		e.g. in sets of 4 values for example. The 'multiple' parameter allows
		you to control that although you should only change it if you know
		your chosen language supports that (at the time of writing, only
		GLSL allows constants which are not a multiple of 4).
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val Pointer to the values to write
		@param count The number of 'multiples' of floats to write
		@param multiple The number of raw entries in each element to write, 
		the default is 4 so count = 1 would write 4 floats.
		*/
		void setNamedConstant(const String& name, const float *val, size_t count, 
			size_t multiple = 4);
		/** Sets a multiple value constant floating-point parameter to the program.
		@par
		Some systems only allow constants to be set on certain boundaries, 
		e.g. in sets of 4 values for example. The 'multiple' parameter allows
		you to control that although you should only change it if you know
		your chosen language supports that (at the time of writing, only
		GLSL allows constants which are not a multiple of 4).
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val Pointer to the values to write
		@param count The number of 'multiples' of floats to write
		@param multiple The number of raw entries in each element to write, 
		the default is 4 so count = 1 would write 4 floats.
		*/
		void setNamedConstant(const String& name, const double *val, size_t count, 
			size_t multiple = 4);
		/** Sets a ColourValue parameter to the program.
		@param name The name of the parameter
		@param colour The value to set
		*/
		void setNamedConstant(const String& name, const Color& colour);

		/** Sets a multiple value constant floating-point parameter to the program.
		@par
		Some systems only allow constants to be set on certain boundaries, 
		e.g. in sets of 4 values for example. The 'multiple' parameter allows
		you to control that although you should only change it if you know
		your chosen language supports that (at the time of writing, only
		GLSL allows constants which are not a multiple of 4).
		@note
		This named option will only work if you are using a parameters object created
		from a high-level program (HighLevelGpuProgram).
		@param name The name of the parameter
		@param val Pointer to the values to write
		@param count The number of 'multiples' of floats to write
		@param multiple The number of raw entries in each element to write, 
		the default is 4 so count = 1 would write 4 floats.
		*/
		void setNamedConstant(const String& name, const int *val, size_t count, 
			size_t multiple = 4);

		/**
		 * @brief	Returns true if a named constant with the specified name exists.
		 */
		bool hasNamedConstant(const String& name) const;

		/** Find a constant definition for a named parameter.
		@remarks
		This method returns null if the named parameter did not exist, unlike
		getConstantDefinition which is more strict; unless you set the 
		last parameter to true.
		@param name The name to look up
		@param throwExceptionIfMissing If set to true, failure to find an entry
		will throw an exception.
		*/
		const GpuConstantDefinition* _findNamedConstantDefinition(
			const String& name, bool throwExceptionIfMissing = false) const;
		/** Gets the physical buffer index associated with a logical float constant index. 
		@note Only applicable to low-level programs.
		@param logicalIndex The logical parameter index
		@param requestedSize The requested size - pass 0 to ignore missing entries
		and return std::numeric_limits<size_t>::max() 
		*/
		size_t _getFloatConstantPhysicalIndex(size_t logicalIndex, size_t requestedSize, UINT16 variability);
		/** Gets the physical buffer index associated with a logical int constant index. 
		@note Only applicable to low-level programs.
		@param logicalIndex The logical parameter index
		@param requestedSize The requested size - pass 0 to ignore missing entries
		and return std::numeric_limits<size_t>::max() 
		*/
		size_t _getIntConstantPhysicalIndex(size_t logicalIndex, size_t requestedSize, UINT16 variability);

		/** Sets whether or not we need to transpose the matrices passed in from the rest of OGRE.
		@remarks
		D3D uses transposed matrices compared to GL and OGRE; this is not important when you
		use programs which are written to process row-major matrices, such as those generated
		by Cg, but if you use a program written to D3D's matrix layout you will need to enable
		this flag.
		*/
		void setTransposeMatrices(bool val) { mTransposeMatrices = val; } 
		/// Gets whether or not matrices are to be transposed when set
		bool getTransposeMatrices(void) const { return mTransposeMatrices; } 

		/** Copies the values of all constants (including auto constants) from another
		GpuProgramParameters object.
		@note This copes the internal storage of the paarameters object and therefore
		can only be used for parameters objects created from the same GpuProgram.
		To merge parameters that match from different programs, use copyMatchingNamedConstantsFrom.
		*/
		void copyConstantsFrom(const GpuProgramParameters& source);
	};

	/// Shared pointer used to hold references to GpuProgramParameters instances
	typedef std::shared_ptr<GpuProgramParameters> GpuProgramParametersSharedPtr;

	/** @} */
	/** @} */
}
#endif


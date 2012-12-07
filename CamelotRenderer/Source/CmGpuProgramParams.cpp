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
#include "CmGpuProgramParams.h"
#include "CmMatrix4.h"
#include "CmMatrix3.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmTexture.h"
#include "CmRenderSystemCapabilities.h"
#include "CmException.h"


namespace CamelotEngine
{

	bool GpuNamedConstants::msGenerateAllConstantDefinitionArrayEntries = false;

	//---------------------------------------------------------------------
	void GpuNamedConstants::generateConstantDefinitionArrayEntries(
		const String& paramName, const GpuConstantDefinition& baseDef)
	{
		// Copy definition for use with arrays
		GpuConstantDefinition arrayDef = baseDef;
		arrayDef.arraySize = 1;
		String arrayName;

		// Add parameters for array accessors
		// [0] will refer to the same location, [1+] will increment
		// only populate others individually up to 16 array slots so as not to get out of hand,
		// unless the system has been explicitly configured to allow all the parameters to be added

		// paramName[0] version will always exist 
		UINT32 maxArrayIndex = 1;
		if (baseDef.arraySize <= 16 || msGenerateAllConstantDefinitionArrayEntries)
			maxArrayIndex = baseDef.arraySize;

		for (UINT32 i = 0; i < maxArrayIndex; i++)
		{
			arrayName = paramName + "[" + toString(i) + "]";
			map.insert(GpuConstantDefinitionMap::value_type(arrayName, arrayDef));
			// increment location
			arrayDef.physicalIndex += arrayDef.elementSize;
		}
		// note no increment of buffer sizes since this is shared with main array def

	}

	//---------------------------------------------------------------------
	bool GpuNamedConstants::getGenerateAllConstantDefinitionArrayEntries()
	{
		return msGenerateAllConstantDefinitionArrayEntries;
	}

	//---------------------------------------------------------------------
	void GpuNamedConstants::setGenerateAllConstantDefinitionArrayEntries(bool generateAll)
	{
		msGenerateAllConstantDefinitionArrayEntries = generateAll;
	}    
	//-----------------------------------------------------------------------------
	//      GpuProgramParameters Methods
	//-----------------------------------------------------------------------------
	GpuProgramParameters::GpuProgramParameters() :
		mCombinedVariability(GPV_GLOBAL)
		, mTransposeMatrices(false)
		, mIgnoreMissingParams(false)
	{
	}
	//-----------------------------------------------------------------------------

	GpuProgramParameters::GpuProgramParameters(const GpuProgramParameters& oth)
	{
		*this = oth;
	}

	//-----------------------------------------------------------------------------
	GpuProgramParameters& GpuProgramParameters::operator=(const GpuProgramParameters& oth)
	{
		// let compiler perform shallow copies of structures 
		// RealConstantEntry, IntConstantEntry
		mFloatConstants = oth.mFloatConstants;
		mIntConstants  = oth.mIntConstants;
		mTextures = oth.mTextures;
		mFloatLogicalToPhysical = oth.mFloatLogicalToPhysical;
		mIntLogicalToPhysical = oth.mIntLogicalToPhysical;
		mSamplerLogicalToPhysical = oth.mSamplerLogicalToPhysical;
		mNamedConstants = oth.mNamedConstants;

		mCombinedVariability = oth.mCombinedVariability;
		mTransposeMatrices = oth.mTransposeMatrices;
		mIgnoreMissingParams  = oth.mIgnoreMissingParams;

		return *this;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_setNamedConstants(
		const GpuNamedConstantsPtr& namedConstants)
	{
		mNamedConstants = namedConstants;

		// Determine any extension to local buffers

		// Size and reset buffer (fill with zero to make comparison later ok)
		if (namedConstants->floatBufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				namedConstants->floatBufferSize - mFloatConstants.size(), 0.0f);
		}
		if (namedConstants->intBufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				namedConstants->intBufferSize - mIntConstants.size(), 0);
		}
		if (namedConstants->samplerCount > mTextures.size())
		{
			mTextures.insert(mTextures.end(), 
				namedConstants->samplerCount - mTextures.size(), nullptr);
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_setLogicalIndexes(
		const GpuLogicalBufferStructPtr& floatIndexMap, 
		const GpuLogicalBufferStructPtr& intIndexMap,
		const GpuLogicalBufferStructPtr& samplerIndexMap)
	{
		mFloatLogicalToPhysical = floatIndexMap;
		mIntLogicalToPhysical = intIndexMap;
		mSamplerLogicalToPhysical = samplerIndexMap;

		// resize the internal buffers
		// Note that these will only contain something after the first parameter
		// set has set some parameters

		// Size and reset buffer (fill with zero to make comparison later ok)
		if ((floatIndexMap != nullptr) && floatIndexMap->bufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				floatIndexMap->bufferSize - mFloatConstants.size(), 0.0f);
		}
		if ((intIndexMap != nullptr) &&  intIndexMap->bufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				intIndexMap->bufferSize - mIntConstants.size(), 0);
		}

		if ((samplerIndexMap != nullptr) &&  samplerIndexMap->bufferSize > mTextures.size())
		{
			mTextures.insert(mTextures.end(), 
				samplerIndexMap->bufferSize - mTextures.size(), nullptr);
		}
	}
	//---------------------------------------------------------------------()
	void GpuProgramParameters::setConstant(UINT32 index, const Vector4& vec)
	{
		setConstant(index, vec.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, float val)
	{
		setConstant(index, Vector4(val, 0.0f, 0.0f, 0.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const Vector3& vec)
	{
		setConstant(index, Vector4(vec.x, vec.y, vec.z, 1.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const Matrix4& m)
	{
		// set as 4x 4-element floats
		if (mTransposeMatrices)
		{
			Matrix4 t = m.transpose();
			GpuProgramParameters::setConstant(index, t[0], 4);
		}
		else
		{
			GpuProgramParameters::setConstant(index, m[0], 4);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const Matrix4* pMatrix, 
		UINT32 numEntries)
	{
		if (mTransposeMatrices)
		{
			for (UINT32 i = 0; i < numEntries; ++i)
			{
				Matrix4 t = pMatrix[i].transpose();
				GpuProgramParameters::setConstant(index, t[0], 4);
				index += 4;
			}
		}
		else
		{
			GpuProgramParameters::setConstant(index, pMatrix[0][0], 4 * numEntries);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const Color& colour)
	{
		setConstant(index, colour.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const float *val, UINT32 count)
	{
		// Raw buffer size is 4x count
		UINT32 rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		UINT32 physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);

		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const double *val, UINT32 count)
	{
		// Raw buffer size is 4x count
		UINT32 rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		UINT32 physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		assert(physicalIndex + rawCount <= mFloatConstants.size());
		// Copy manually since cast required
		for (UINT32 i = 0; i < rawCount; ++i)
		{
			mFloatConstants[physicalIndex + i] = 
				static_cast<float>(val[i]);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(UINT32 index, const int *val, UINT32 count)
	{
		// Raw buffer size is 4x count
		UINT32 rawCount = count * 4;
		// get physical index
		assert((mIntLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		UINT32 physicalIndex = _getIntConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Vector4& vec,
		UINT32 count)
	{
		// remember, raw content access uses raw float count rather than float4
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, vec.ptr(), std::min(count, (UINT32)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, float val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, int val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Vector3& vec)
	{
		_writeRawConstants(physicalIndex, vec.ptr(), 3);		
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Vector2& vec)
	{
		_writeRawConstants(physicalIndex, vec.ptr(), 2);		
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Matrix4& m, UINT32 elementCount)
	{

		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			Matrix4 t = m.transpose();
			_writeRawConstants(physicalIndex, t[0], elementCount>16?16:elementCount);
		}
		else
		{
			_writeRawConstants(physicalIndex, m[0], elementCount>16?16:elementCount);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Matrix4* pMatrix, UINT32 numEntries)
	{
		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			for (UINT32 i = 0; i < numEntries; ++i)
			{
				Matrix4 t = pMatrix[i].transpose();
				_writeRawConstants(physicalIndex, t[0], 16);
				physicalIndex += 16;
			}
		}
		else
		{
			_writeRawConstants(physicalIndex, pMatrix[0][0], 16 * numEntries);
		}


	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, const Matrix3& m, UINT32 elementCount)
	{
		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			Matrix3 t = m.Transpose();
			_writeRawConstants(physicalIndex, t[0], elementCount>9?9:elementCount);
		}
		else
		{
			_writeRawConstants(physicalIndex, m[0], elementCount>9?9:elementCount);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(UINT32 physicalIndex, 
		const Color& colour, UINT32 count)
	{
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, colour.ptr(), std::min(count, (UINT32)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(UINT32 physicalIndex, const double* val, UINT32 count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		for (UINT32 i = 0; i < count; ++i)
		{
			mFloatConstants[physicalIndex+i] = static_cast<float>(val[i]);
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(UINT32 physicalIndex, const float* val, UINT32 count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(&mFloatConstants[physicalIndex], val, sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(UINT32 physicalIndex, const int* val, UINT32 count)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(&mIntConstants[physicalIndex], val, sizeof(int) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(UINT32 physicalIndex, UINT32 count, float* dest)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(dest, &mFloatConstants[physicalIndex], sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(UINT32 physicalIndex, UINT32 count, int* dest)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(dest, &mIntConstants[physicalIndex], sizeof(int) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readTexture(UINT32 physicalIndex, TextureHandle& dest)
	{
		assert(physicalIndex < mTextures.size());
		dest = mTextures[physicalIndex]->texture;
	}
	//---------------------------------------------------------------------
	GpuLogicalIndexUse* GpuProgramParameters::_getFloatConstantLogicalIndexUse(
		UINT32 logicalIndex, UINT32 requestedSize, UINT16 variability)
	{
		if (mFloatLogicalToPhysical == nullptr)
			return 0;

		GpuLogicalIndexUse* indexUse = 0;
		CM_LOCK_MUTEX(mFloatLogicalToPhysical->mutex)

			GpuLogicalIndexUseMap::iterator logi = mFloatLogicalToPhysical->map.find(logicalIndex);
		if (logi == mFloatLogicalToPhysical->map.end())
		{
			if (requestedSize)
			{
				UINT32 physicalIndex = (UINT32)mFloatConstants.size();

				// Expand at buffer end
				mFloatConstants.insert(mFloatConstants.end(), requestedSize, 0.0f);

				// Record extended size for future GPU params re-using this information
				mFloatLogicalToPhysical->bufferSize = (UINT32)mFloatConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				UINT32 currPhys = physicalIndex;
				UINT32 count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;

				for (UINT32 logicalNum = 0; logicalNum < count; ++logicalNum)
				{
					GpuLogicalIndexUseMap::iterator it = 
						mFloatLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(
						logicalIndex + logicalNum, 
						GpuLogicalIndexUse(currPhys, requestedSize, variability))).first;
					currPhys += 4;

					if (logicalNum == 0)
						insertedIterator = it;
				}

				indexUse = &(insertedIterator->second);
			}
			else
			{
				// no match & ignore
				return 0;
			}

		}
		else
		{
			UINT32 physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);
			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				UINT32 insertCount = requestedSize - logi->second.currentSize;
				FloatConstantList::iterator insertPos = mFloatConstants.begin();
				std::advance(insertPos, physicalIndex);
				mFloatConstants.insert(insertPos, insertCount, 0.0f);
				// shift all physical positions after this one
				for (GpuLogicalIndexUseMap::iterator i = mFloatLogicalToPhysical->map.begin();
					i != mFloatLogicalToPhysical->map.end(); ++i)
				{
					if (i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				mFloatLogicalToPhysical->bufferSize += insertCount;
				if (mNamedConstants != nullptr)
				{
					for (GpuConstantDefinitionMap::iterator i = mNamedConstants->map.begin();
						i != mNamedConstants->map.end(); ++i)
					{
						if (i->second.isFloat() && i->second.physicalIndex > physicalIndex)
							i->second.physicalIndex += insertCount;
					}
					mNamedConstants->floatBufferSize += insertCount;
				}

				logi->second.currentSize += insertCount;
			}
		}

		if (indexUse)
			indexUse->variability = variability;

		return indexUse;

	}
	//---------------------------------------------------------------------()
	GpuLogicalIndexUse* GpuProgramParameters::_getIntConstantLogicalIndexUse(UINT32 logicalIndex, UINT32 requestedSize, UINT16 variability)
	{
		if (mIntLogicalToPhysical == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, 
			"This is not a low-level parameter parameter object");
		}

		GpuLogicalIndexUse* indexUse = 0;
		CM_LOCK_MUTEX(mIntLogicalToPhysical->mutex)

			GpuLogicalIndexUseMap::iterator logi = mIntLogicalToPhysical->map.find(logicalIndex);
		if (logi == mIntLogicalToPhysical->map.end())
		{
			if (requestedSize)
			{
				UINT32 physicalIndex = (UINT32)mIntConstants.size();

				// Expand at buffer end
				mIntConstants.insert(mIntConstants.end(), requestedSize, 0);

				// Record extended size for future GPU params re-using this information
				mIntLogicalToPhysical->bufferSize = (UINT32)mIntConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				UINT32 currPhys = physicalIndex;
				UINT32 count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;
				for (UINT32 logicalNum = 0; logicalNum < count; ++logicalNum)
				{
					GpuLogicalIndexUseMap::iterator it = 
						mIntLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(
						logicalIndex + logicalNum, 
						GpuLogicalIndexUse(currPhys, requestedSize, variability))).first;
					if (logicalNum == 0)
						insertedIterator = it;
					currPhys += 4;
				}
				indexUse = &(insertedIterator->second);

			}
			else
			{
				// no match
				return 0;
			}

		}
		else
		{
			UINT32 physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);

			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				UINT32 insertCount = requestedSize - logi->second.currentSize;
				IntConstantList::iterator insertPos = mIntConstants.begin();
				std::advance(insertPos, physicalIndex);
				mIntConstants.insert(insertPos, insertCount, 0);
				// shift all physical positions after this one
				for (GpuLogicalIndexUseMap::iterator i = mIntLogicalToPhysical->map.begin();
					i != mIntLogicalToPhysical->map.end(); ++i)
				{
					if (i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				mIntLogicalToPhysical->bufferSize += insertCount;
				if (mNamedConstants != nullptr)
				{
					for (GpuConstantDefinitionMap::iterator i = mNamedConstants->map.begin();
						i != mNamedConstants->map.end(); ++i)
					{
						if (!i->second.isFloat() && i->second.physicalIndex > physicalIndex)
							i->second.physicalIndex += insertCount;
					}
					mNamedConstants->intBufferSize += insertCount;
				}

				logi->second.currentSize += insertCount;
			}
		}

		if (indexUse)
			indexUse->variability = variability;

		return indexUse;

	}
	//-----------------------------------------------------------------------------
	UINT32 GpuProgramParameters::_getFloatConstantPhysicalIndex(
		UINT32 logicalIndex, UINT32 requestedSize, UINT16 variability) 
	{
		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	UINT32 GpuProgramParameters::_getIntConstantPhysicalIndex(
		UINT32 logicalIndex, UINT32 requestedSize, UINT16 variability)
	{
		GpuLogicalIndexUse* indexUse = _getIntConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	UINT32 GpuProgramParameters::getFloatLogicalIndexForPhysicalIndex(UINT32 physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mFloatLogicalToPhysical->map.begin();
			i != mFloatLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<UINT32>::max();

	}
	//-----------------------------------------------------------------------------
	UINT32 GpuProgramParameters::getIntLogicalIndexForPhysicalIndex(UINT32 physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mIntLogicalToPhysical->map.begin();
			i != mIntLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<UINT32>::max();

	}
	//-----------------------------------------------------------------------------
	GpuConstantDefinitionIterator GpuProgramParameters::getConstantDefinitionIterator(void) const
	{
		if (mNamedConstants == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, 
			"This params object is not based on a program with named parameters.");
		}

		return mNamedConstants->map.begin();

	}
	//-----------------------------------------------------------------------------
	const GpuNamedConstants& GpuProgramParameters::getConstantDefinitions() const
	{
		if (mNamedConstants == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, 
			"This params object is not based on a program with named parameters.");
		}

		return *mNamedConstants;
	}
	//-----------------------------------------------------------------------------
	const GpuConstantDefinition& GpuProgramParameters::getConstantDefinition(const String& name) const
	{
		if (mNamedConstants == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, 
			"This params object is not based on a program with named parameters.");
		}


		// locate, and throw exception if not found
		const GpuConstantDefinition* def = _findNamedConstantDefinition(name, true);

		return *def;
	}
	//----------------------------------------------------------------------------
	TextureHandle GpuProgramParameters::getTexture(UINT32 pos) const 
	{ 
		if(mTextures[pos] == nullptr)
		{
			return TextureHandle();
		}

		return mTextures[pos]->texture; 
	}
	//----------------------------------------------------------------------------
	const SamplerState& GpuProgramParameters::getSamplerState(UINT32 pos) const 
	{ 
		if(mTextures[pos] == nullptr)
		{
			return SamplerState::EMPTY;
		}

		return mTextures[pos]->samplerState; 
	}
	//-----------------------------------------------------------------------------
	bool GpuProgramParameters::hasNamedConstant(const String& name) const
	{
		return _findNamedConstantDefinition(name) != nullptr;
	}
	//-----------------------------------------------------------------------------
	const GpuConstantDefinition* 
		GpuProgramParameters::_findNamedConstantDefinition(const String& name, 
		bool throwExceptionIfNotFound) const
	{
		if (mNamedConstants == nullptr)
		{
			if (throwExceptionIfNotFound)
			{
				CM_EXCEPT(InvalidParametersException, 
				"Named constants have not been initialised, perhaps a compile error.");
			}

			return 0;
		}

		GpuConstantDefinitionMap::const_iterator i = mNamedConstants->map.find(name);
		if (i == mNamedConstants->map.end())
		{
			if (throwExceptionIfNotFound)
			{
				CM_EXCEPT(InvalidParametersException, 
				"Parameter called " + name + " does not exist. ");
			}

			return 0;
		}
		else
		{
			return &(i->second);
		}
	}
	//----------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, TextureHandle val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);

		if (def)
		{
			if(mTextures[def->physicalIndex] == nullptr)
				mTextures[def->physicalIndex] = GpuTextureEntryPtr(new GpuTextureEntry());

			mTextures[def->physicalIndex]->texture = val;
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const SamplerState& val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);

		if (def)
		{
			if(mTextures[def->physicalIndex] == nullptr)
				mTextures[def->physicalIndex] = GpuTextureEntryPtr(new GpuTextureEntry());

			mTextures[def->physicalIndex]->samplerState = val;
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, float val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, int val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Vector4& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Vector3& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Vector2& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Matrix4& m)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Matrix4* m, 
		UINT32 numEntries)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, numEntries);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Matrix3& m)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const float *val, UINT32 count, UINT32 multiple)
	{
		UINT32 rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const double *val, UINT32 count, UINT32 multiple)
	{
		UINT32 rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Color& colour)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, colour, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const int *val, UINT32 count, UINT32 multiple)
	{
		UINT32 rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::copyConstantsFrom(const GpuProgramParameters& source)
	{
		// Pull buffers & auto constant list over directly
		mFloatConstants = source.getFloatConstantList();
		mIntConstants = source.getIntConstantList();
		mTextures = source.getTextureList();
		mCombinedVariability = source.mCombinedVariability;
	}
}


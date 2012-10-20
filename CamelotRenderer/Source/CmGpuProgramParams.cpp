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
#include "CmVector3.h"
#include "CmVector4.h"
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
		size_t maxArrayIndex = 1;
		if (baseDef.arraySize <= 16 || msGenerateAllConstantDefinitionArrayEntries)
			maxArrayIndex = baseDef.arraySize;

		for (size_t i = 0; i < maxArrayIndex; i++)
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
	//      GpuSharedParameters Methods
	//-----------------------------------------------------------------------------
	GpuSharedParameters::GpuSharedParameters(const String& name)
		:mName(name)
		// TODO PORT - Commented out because I don't have root in port. But I dont think this will be needed in final version
		/*, mFrameLastUpdated(Root::getSingleton().getNextFrameNumber())*/
		, mFrameLastUpdated(0)
		, mVersion(0)
	{

	}
	//---------------------------------------------------------------------
	GpuSharedParameters::~GpuSharedParameters()
	{

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::addConstantDefinition(const String& name, GpuConstantType constType, size_t arraySize)
	{
		if (mNamedConstants.map.find(name) != mNamedConstants.map.end())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Constant entry with name '" + name + "' already exists. ");
		}
		GpuConstantDefinition def;
		def.arraySize = arraySize;
		def.constType = constType;
		// for compatibility we do not pad values to multiples of 4
		// when it comes to arrays, user is responsible for creating matching defs
		def.elementSize = GpuConstantDefinition::getElementSize(constType, false);

		// not used
		def.logicalIndex = 0;
		def.variability = (UINT16)GPV_GLOBAL;

		if (def.isFloat())
		{
			def.physicalIndex = mFloatConstants.size();
			mFloatConstants.resize(mFloatConstants.size() + def.arraySize * def.elementSize);
		}
		else
		{
			def.physicalIndex = mIntConstants.size();
			mIntConstants.resize(mIntConstants.size() + def.arraySize * def.elementSize);
		}

		mNamedConstants.map[name] = def;

		++mVersion;
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::removeConstantDefinition(const String& name)
	{
		GpuConstantDefinitionMap::iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			GpuConstantDefinition& def = i->second;
			bool isFloat = def.isFloat();
			size_t numElems = def.elementSize * def.arraySize;

			for (GpuConstantDefinitionMap::iterator j = mNamedConstants.map.begin();
				j != mNamedConstants.map.end(); ++j)
			{
				GpuConstantDefinition& otherDef = j->second;
				bool otherIsFloat = otherDef.isFloat();

				// same type, and comes after in the buffer
				if ( ((isFloat && otherIsFloat) || (!isFloat && !otherIsFloat)) && 
					otherDef.physicalIndex > def.physicalIndex)
				{
					// adjust index
					otherDef.physicalIndex -= numElems;
				}
			}

			// remove floats and reduce buffer
			if (isFloat)
			{
				mNamedConstants.floatBufferSize -= numElems;

				FloatConstantList::iterator beg = mFloatConstants.begin();
				std::advance(beg, def.physicalIndex);
				FloatConstantList::iterator en = beg;
				std::advance(en, numElems);
				mFloatConstants.erase(beg, en);
			}
			else
			{
				mNamedConstants.intBufferSize -= numElems;

				IntConstantList::iterator beg = mIntConstants.begin();
				std::advance(beg, def.physicalIndex);
				IntConstantList::iterator en = beg;
				std::advance(en, numElems);
				mIntConstants.erase(beg, en);

			}

			++mVersion;
			
		}

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::removeAllConstantDefinitions()
	{
		mNamedConstants.map.clear();
		mNamedConstants.floatBufferSize = 0;
		mNamedConstants.intBufferSize = 0;
		mFloatConstants.clear();
		mIntConstants.clear();
	}
	//---------------------------------------------------------------------
	GpuConstantDefinitionIterator GpuSharedParameters::getConstantDefinitionIterator(void) const
	{
		return mNamedConstants.map.begin();
	}
	//---------------------------------------------------------------------
	const GpuConstantDefinition& GpuSharedParameters::getConstantDefinition(const String& name) const
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i == mNamedConstants.map.end())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Constant entry with name '" + name + "' does not exist. ");
		}
		return i->second;
	}
	//---------------------------------------------------------------------
	const GpuNamedConstants& GpuSharedParameters::getConstantDefinitions() const
	{
		return mNamedConstants;
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, float val)
	{
		setNamedConstant(name, &val, 1);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, int val)
	{
		setNamedConstant(name, &val, 1);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Vector4& vec)
	{
		setNamedConstant(name, vec.ptr(), 4);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Vector3& vec)
	{
		setNamedConstant(name, vec.ptr(), 3);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Matrix4& m)
	{
		setNamedConstant(name, m[0], 16);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Matrix4* m, size_t numEntries)
	{
		setNamedConstant(name, m[0][0], 16 * numEntries);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const float *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;
			memcpy(&mFloatConstants[def.physicalIndex], val, 
				sizeof(float) * std::min(count, def.elementSize * def.arraySize));
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const double *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;

			count = std::min(count, def.elementSize * def.arraySize);
			const double* src = val;
			float* dst = &mFloatConstants[def.physicalIndex];
			for (size_t v = 0; v < count; ++v)
			{
				*dst++ = static_cast<float>(*src++);
			}
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Color& colour)
	{
		setNamedConstant(name, colour.ptr(), 4);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const int *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;
			memcpy(&mIntConstants[def.physicalIndex], val, 
				sizeof(int) * std::min(count, def.elementSize * def.arraySize));
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::_markDirty()
	{
		// TODO PORT - Don't have access to Root in port. Set it to 0 for now. Will probably be able to remove this completely
		mFrameLastUpdated = 0;
		/*mFrameLastUpdated = Root::getSingleton().getNextFrameNumber();*/
	}

	//-----------------------------------------------------------------------------
	//      GpuSharedParametersUsage Methods
	//-----------------------------------------------------------------------------
	GpuSharedParametersUsage::GpuSharedParametersUsage(GpuSharedParametersPtr sharedParams, 
		GpuProgramParameters* params)
		: mSharedParams(sharedParams)
		, mParams(params)
	{
		initCopyData();
	}
	//---------------------------------------------------------------------
	void GpuSharedParametersUsage::initCopyData()
	{

		mCopyDataList.clear();

		const GpuConstantDefinitionMap& sharedmap = mSharedParams->getConstantDefinitions().map;
		for (GpuConstantDefinitionMap::const_iterator i = sharedmap.begin(); i != sharedmap.end(); ++i)
		{
			const String& pName = i->first;
			const GpuConstantDefinition& shareddef = i->second;

			const GpuConstantDefinition* instdef = mParams->_findNamedConstantDefinition(pName, false);
			if (instdef)
			{
				// Check that the definitions are the same 
				if (instdef->constType == shareddef.constType && 
					instdef->arraySize == shareddef.arraySize)
				{
					CopyDataEntry e;
					e.srcDefinition = &shareddef;
					e.dstDefinition = instdef;
					mCopyDataList.push_back(e);
				}
			}

		}

		mCopyDataVersion = mSharedParams->getVersion();

	}
	//---------------------------------------------------------------------
	void GpuSharedParametersUsage::_copySharedParamsToTargetParams()
	{
		// check copy data version
		if (mCopyDataVersion != mSharedParams->getVersion())
			initCopyData();

		for (CopyDataList::iterator i = mCopyDataList.begin(); i != mCopyDataList.end(); ++i)
		{
			CopyDataEntry& e = *i;

			if (e.dstDefinition->isFloat())
			{	
				const float* pSrc = mSharedParams->getFloatPointer(e.srcDefinition->physicalIndex);
				float* pDst = mParams->getFloatPointer(e.dstDefinition->physicalIndex);

				// Deal with matrix transposition here!!!
				// transposition is specific to the dest param set, shared params don't do it
				if (mParams->getTransposeMatrices() && e.dstDefinition->constType == GCT_MATRIX_4X4)
				{
					for (int row = 0; row < 4; ++row)
						for (int col = 0; col < 4; ++col)
							pDst[row * 4 + col] = pSrc[col * 4 + row];
				}
				else
				{
					if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
					{
						// simple copy
						memcpy(pDst, pSrc, sizeof(float) * e.dstDefinition->elementSize * e.dstDefinition->arraySize);
					}
					else
					{
						// target params may be padded to 4 elements, shared params are packed
						assert(e.dstDefinition->elementSize % 4 == 0);
						size_t iterations = e.dstDefinition->elementSize / 4 
							* e.dstDefinition->arraySize;
						size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
						for (size_t l = 0; l < iterations; ++l)
						{
							memcpy(pDst, pSrc, sizeof(float) * valsPerIteration);
							pSrc += valsPerIteration;
							pDst += 4;
						}
					}
				}
			}
			else
			{
				const int* pSrc = mSharedParams->getIntPointer(e.srcDefinition->physicalIndex);
				int* pDst = mParams->getIntPointer(e.dstDefinition->physicalIndex);

				if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
				{
					// simple copy
					memcpy(pDst, pSrc, sizeof(int) * e.dstDefinition->elementSize * e.dstDefinition->arraySize);
				}
				else
				{
					// target params may be padded to 4 elements, shared params are packed
					assert(e.dstDefinition->elementSize % 4 == 0);
					size_t iterations = (e.dstDefinition->elementSize / 4)
						* e.dstDefinition->arraySize;
					size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
					for (size_t l = 0; l < iterations; ++l)
					{
						memcpy(pDst, pSrc, sizeof(int) * valsPerIteration);
						pSrc += valsPerIteration;
						pDst += 4;
					}
				}
			}
		}
	}



	//-----------------------------------------------------------------------------
	//      GpuProgramParameters Methods
	//-----------------------------------------------------------------------------
	GpuProgramParameters::GpuProgramParameters() :
		mCombinedVariability(GPV_GLOBAL)
		, mTransposeMatrices(false)
		, mIgnoreMissingParams(false)
		, mActivePassIterationIndex(std::numeric_limits<size_t>::max())	
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
		// AutoConstantEntry, RealConstantEntry, IntConstantEntry
		mFloatConstants = oth.mFloatConstants;
		mIntConstants  = oth.mIntConstants;
		mFloatLogicalToPhysical = oth.mFloatLogicalToPhysical;
		mIntLogicalToPhysical = oth.mIntLogicalToPhysical;
		mNamedConstants = oth.mNamedConstants;
		copySharedParamSetUsage(oth.mSharedParamSets);

		mCombinedVariability = oth.mCombinedVariability;
		mTransposeMatrices = oth.mTransposeMatrices;
		mIgnoreMissingParams  = oth.mIgnoreMissingParams;
		mActivePassIterationIndex = oth.mActivePassIterationIndex;

		return *this;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::copySharedParamSetUsage(const GpuSharedParamUsageList& srcList)
	{
		mSharedParamSets.clear();
		for (GpuSharedParamUsageList::const_iterator i = srcList.begin(); i != srcList.end(); ++i)
		{
			mSharedParamSets.push_back(GpuSharedParametersUsage(i->getSharedParams(), this));
		}

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
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_setLogicalIndexes(
		const GpuLogicalBufferStructPtr& floatIndexMap, 
		const GpuLogicalBufferStructPtr& intIndexMap)
	{
		mFloatLogicalToPhysical = floatIndexMap;
		mIntLogicalToPhysical = intIndexMap;

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

	}
	//---------------------------------------------------------------------()
	void GpuProgramParameters::setConstant(size_t index, const Vector4& vec)
	{
		setConstant(index, vec.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, float val)
	{
		setConstant(index, Vector4(val, 0.0f, 0.0f, 0.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const Vector3& vec)
	{
		setConstant(index, Vector4(vec.x, vec.y, vec.z, 1.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const Matrix4& m)
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
	void GpuProgramParameters::setConstant(size_t index, const Matrix4* pMatrix, 
		size_t numEntries)
	{
		if (mTransposeMatrices)
		{
			for (size_t i = 0; i < numEntries; ++i)
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
	void GpuProgramParameters::setConstant(size_t index, const Color& colour)
	{
		setConstant(index, colour.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const float *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);

		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const double *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		assert(physicalIndex + rawCount <= mFloatConstants.size());
		// Copy manually since cast required
		for (size_t i = 0; i < rawCount; ++i)
		{
			mFloatConstants[physicalIndex + i] = 
				static_cast<float>(val[i]);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const int *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mIntLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getIntConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Vector4& vec,
		size_t count)
	{
		// remember, raw content access uses raw float count rather than float4
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, vec.ptr(), std::min(count, (size_t)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, float val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, int val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Vector3& vec)
	{
		_writeRawConstants(physicalIndex, vec.ptr(), 3);		
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Matrix4& m,size_t elementCount)
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
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Matrix4* pMatrix, size_t numEntries)
	{
		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			for (size_t i = 0; i < numEntries; ++i)
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
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, 
		const Color& colour, size_t count)
	{
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, colour.ptr(), std::min(count, (size_t)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const double* val, size_t count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		for (size_t i = 0; i < count; ++i)
		{
			mFloatConstants[physicalIndex+i] = static_cast<float>(val[i]);
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const float* val, size_t count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(&mFloatConstants[physicalIndex], val, sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const int* val, size_t count)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(&mIntConstants[physicalIndex], val, sizeof(int) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(size_t physicalIndex, size_t count, float* dest)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(dest, &mFloatConstants[physicalIndex], sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(size_t physicalIndex, size_t count, int* dest)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(dest, &mIntConstants[physicalIndex], sizeof(int) * count);
	}
	//---------------------------------------------------------------------
	GpuLogicalIndexUse* GpuProgramParameters::_getFloatConstantLogicalIndexUse(
		size_t logicalIndex, size_t requestedSize, UINT16 variability)
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
				size_t physicalIndex = mFloatConstants.size();

				// Expand at buffer end
				mFloatConstants.insert(mFloatConstants.end(), requestedSize, 0.0f);

				// Record extended size for future GPU params re-using this information
				mFloatLogicalToPhysical->bufferSize = mFloatConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				size_t currPhys = physicalIndex;
				size_t count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;

				for (size_t logicalNum = 0; logicalNum < count; ++logicalNum)
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
			size_t physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);
			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				size_t insertCount = requestedSize - logi->second.currentSize;
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
	GpuLogicalIndexUse* GpuProgramParameters::_getIntConstantLogicalIndexUse(size_t logicalIndex, size_t requestedSize, UINT16 variability)
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
				size_t physicalIndex = mIntConstants.size();

				// Expand at buffer end
				mIntConstants.insert(mIntConstants.end(), requestedSize, 0);

				// Record extended size for future GPU params re-using this information
				mIntLogicalToPhysical->bufferSize = mIntConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				size_t currPhys = physicalIndex;
				size_t count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;
				for (size_t logicalNum = 0; logicalNum < count; ++logicalNum)
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
			size_t physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);

			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				size_t insertCount = requestedSize - logi->second.currentSize;
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
	size_t GpuProgramParameters::_getFloatConstantPhysicalIndex(
		size_t logicalIndex, size_t requestedSize, UINT16 variability) 
	{
		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::_getIntConstantPhysicalIndex(
		size_t logicalIndex, size_t requestedSize, UINT16 variability)
	{
		GpuLogicalIndexUse* indexUse = _getIntConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::getFloatLogicalIndexForPhysicalIndex(size_t physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mFloatLogicalToPhysical->map.begin();
			i != mFloatLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<size_t>::max();

	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::getIntLogicalIndexForPhysicalIndex(size_t physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mIntLogicalToPhysical->map.begin();
			i != mIntLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<size_t>::max();

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
		size_t numEntries)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, numEntries);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const float *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const double *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
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
		const int *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
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
		mCombinedVariability = source.mCombinedVariability;
		copySharedParamSetUsage(source.mSharedParamSets);
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::copyMatchingNamedConstantsFrom(const GpuProgramParameters& source)
	{
		if ((mNamedConstants != nullptr) && (source.mNamedConstants != nullptr))
		{
			std::map<size_t, String> srcToDestNamedMap;
			for (GpuConstantDefinitionMap::const_iterator i = source.mNamedConstants->map.begin(); 
				i != source.mNamedConstants->map.end(); ++i)
			{
				const String& paramName = i->first;
				const GpuConstantDefinition& olddef = i->second;
				const GpuConstantDefinition* newdef = _findNamedConstantDefinition(paramName, false);
				if (newdef)
				{
					// Copy data across, based on smallest common definition size
					size_t srcsz = olddef.elementSize * olddef.arraySize;
					size_t destsz = newdef->elementSize * newdef->arraySize;
					size_t sz = std::min(srcsz, destsz);
					if (newdef->isFloat())
					{

						memcpy(getFloatPointer(newdef->physicalIndex), 
							source.getFloatPointer(olddef.physicalIndex),
							sz * sizeof(float));
					}
					else
					{
						memcpy(getIntPointer(newdef->physicalIndex), 
							source.getIntPointer(olddef.physicalIndex),
							sz * sizeof(int));
					}
					// we'll use this map to resolve autos later
					// ignore the [0] aliases
					if (!StringUtil::endsWith(paramName, "[0]"))
						srcToDestNamedMap[olddef.physicalIndex] = paramName;
				}
			}

			// Copy shared param sets
			for (GpuSharedParamUsageList::const_iterator i = source.mSharedParamSets.begin();
				i != source.mSharedParamSets.end(); ++i)
			{
				const GpuSharedParametersUsage& usage = *i;
				if (!isUsingSharedParameters(usage.getName()))
				{
					addSharedParameters(usage.getSharedParams());
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void GpuProgramParameters::incPassIterationNumber(void)
	{
		if (mActivePassIterationIndex != std::numeric_limits<size_t>::max())
		{
			// This is a physical index
			++mFloatConstants[mActivePassIterationIndex];
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::addSharedParameters(GpuSharedParametersPtr sharedParams)
	{
		if (!isUsingSharedParameters(sharedParams->getName()))
		{
			mSharedParamSets.push_back(GpuSharedParametersUsage(sharedParams, this));
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::addSharedParameters(const String& sharedParamsName)
	{
		// TODO PORT - I don't think I'll be needing this. I dont plan on including GpuProgramManager in the port
		//addSharedParameters(GpuProgramManager::getSingleton().getSharedParameters(sharedParamsName));
	}
	//---------------------------------------------------------------------
	bool GpuProgramParameters::isUsingSharedParameters(const String& sharedParamsName) const
	{
		for (GpuSharedParamUsageList::const_iterator i = mSharedParamSets.begin();
			i != mSharedParamSets.end(); ++i)
		{
			if (i->getName() == sharedParamsName)
				return true;
		}
		return false;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::removeSharedParameters(const String& sharedParamsName)
	{
		for (GpuSharedParamUsageList::iterator i = mSharedParamSets.begin();
			i != mSharedParamSets.end(); ++i)
		{
			if (i->getName() == sharedParamsName)
			{
				mSharedParamSets.erase(i);
				break;
			}
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::removeAllSharedParameters()
	{
		mSharedParamSets.clear();
	}
	//---------------------------------------------------------------------
	const GpuProgramParameters::GpuSharedParamUsageList& 
	GpuProgramParameters::getSharedParameters() const
	{
		return mSharedParamSets;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_copySharedParams()
	{
		for (GpuSharedParamUsageList::iterator i = mSharedParamSets.begin(); 
			i != mSharedParamSets.end(); ++i )
		{
			i->_copySharedParamsToTargetParams();
		}

	}





}


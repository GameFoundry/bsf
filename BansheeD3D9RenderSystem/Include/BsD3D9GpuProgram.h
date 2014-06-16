#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsGpuProgram.h"
#include "BsD3D9Resource.h"
#include "BsD3D9EmulatedParamBlocks.h"

namespace BansheeEngine 
{
	enum OptimizationLevel
	{
		OPT_DEFAULT,
		OPT_NONE,
		OPT_0,
		OPT_1,
		OPT_2,
		OPT_3
	};

    /** Direct3D implementation of a few things common to low-level vertex & fragment programs. */
    class BS_D3D9_EXPORT D3D9GpuProgram : public GpuProgram, public D3D9Resource
    {   
    public:
        ~D3D9GpuProgram();

		/**
		 * @copydoc	GpuProgram::requiresMatrixTranspose
		 */
		virtual bool requiresMatrixTranspose() const { return mColumnMajorMatrices; }

		/** Sets the preprocessor defines use to compile the program. */
		void setPreprocessorDefines(const String& defines) { mPreprocessorDefines = defines; }
		/** Sets the preprocessor defines use to compile the program. */
		const String& getPreprocessorDefines() const { return mPreprocessorDefines; }

        /** Sets whether matrix packing in column-major order. */ 
        void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }
        /** Gets whether matrix packed in column-major order. */
        bool getColumnMajorMatrices() const { return mColumnMajorMatrices; }

		/** Sets the optimisation level to use.
		@param opt Optimisation level
		*/
		void setOptimizationLevel(OptimizationLevel opt) { mOptimisationLevel = opt; }

		/** Gets the optimisation level to use. */
		OptimizationLevel getOptimizationLevel() const { return mOptimisationLevel; }

		/// Overridden from GpuProgram
		GpuParamsPtr createParameters();
		/// Overridden from GpuProgram
		const String& getLanguage() const;

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuProgram(const String& source, const String& entryPoint, 
			GpuProgramType gptype, GpuProgramProfile profile, 
			const Vector<HGpuProgInclude>* includes);

		void createInternalResources(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc GpuProgram::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();
      
		/** Loads this program from microcode, must be overridden by subclasses. */
        virtual void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode) = 0;

	protected:    
		OptimizationLevel mOptimisationLevel;
		String mPreprocessorDefines;
		Vector<D3D9EmulatedParamBlock> mBlocks;
		bool mColumnMajorMatrices;
		ID3DXBuffer* mMicrocode;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D9GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };

    /** Direct3D implementation of low-level vertex programs. */
    class BS_D3D9_EXPORT D3D9GpuVertexProgram : public D3D9GpuProgram
    {  
    public:
		~D3D9GpuVertexProgram();
        
		/// Gets the vertex shader
        IDirect3DVertexShader9* getVertexShader();

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuVertexProgram(const String& source, const String& entryPoint, GpuProgramProfile profile,
			const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		typedef Map<IDirect3DDevice9*, IDirect3DVertexShader9*>   DeviceToVertexShaderMap;
		typedef DeviceToVertexShaderMap::iterator						DeviceToVertexShaderIterator;
	
		DeviceToVertexShaderMap		mMapDeviceToVertexShader;	

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D9GpuVertexProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };

    /** Direct3D implementation of low-level fragment programs. */
    class BS_D3D9_EXPORT D3D9GpuFragmentProgram : public D3D9GpuProgram
    {  
    public:
		~D3D9GpuFragmentProgram();

        /// Gets the pixel shader
        IDirect3DPixelShader9* getPixelShader();

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuFragmentProgram(const String& source, const String& entryPoint, GpuProgramProfile profile,
			const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		typedef Map<IDirect3DDevice9*, IDirect3DPixelShader9*>	DeviceToPixelShaderMap;
		typedef DeviceToPixelShaderMap::iterator						DeviceToPixelShaderIterator;

		DeviceToPixelShaderMap		mMapDeviceToPixelShader;	

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D9GpuFragmentProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };

	typedef std::shared_ptr<D3D9GpuProgram> D3D9GpuProgramPtr;
}
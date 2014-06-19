#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsGpuProgram.h"
#include "BsD3D9Resource.h"
#include "BsD3D9EmulatedParamBlocks.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Available optimization levels when compiling a GPU program.
	 */
	enum OptimizationLevel
	{
		OPT_DEFAULT,
		OPT_NONE,
		OPT_0,
		OPT_1,
		OPT_2,
		OPT_3
	};

	/**
	 * @brief	DirectX 9 implementation of a GPU program.
	 */
    class BS_D3D9_EXPORT D3D9GpuProgram : public GpuProgram, public D3D9Resource
    {   
    public:
        virtual ~D3D9GpuProgram();

		/**
		 * @copydoc	GpuProgram::requiresMatrixTranspose
		 */
		virtual bool requiresMatrixTranspose() const { return mColumnMajorMatrices; }

		/**
		 * @brief	Sets the preprocessor defines use to compile the program.
		 */
		void setPreprocessorDefines(const String& defines) { mPreprocessorDefines = defines; }

		/**
		 * @brief	Sets whether matrix packing in column-major order.
		 */
        void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }

		/**
		 * @brief	Sets optimization level to use when compiling the shader.
		 */
		void setOptimizationLevel(OptimizationLevel opt) { mOptimisationLevel = opt; }

		/**
		 * @copydoc	GpuProgram::createParameters
		 */
		GpuParamsPtr createParameters();

		/**
		 * @copydoc	GpuProgram::getLanguage
		 */
		const String& getLanguage() const;

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuProgram(const String& source, const String& entryPoint, 
			GpuProgramType gptype, GpuProgramProfile profile, 
			const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc GpuProgram::destroy_internal
		 */
		void destroy_internal();
      
		/**
		 * @brief	Loads the GPU program from compiled microcode.
		 */
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

	/**
	 * @brief	DirectX 9 implementation of a vertex GPU program.
	 */
    class BS_D3D9_EXPORT D3D9GpuVertexProgram : public D3D9GpuProgram
    {  
    public:
		~D3D9GpuVertexProgram();
        
		/**
		 * @brief	Returns internal DX9 vertex shader object.
		 */
        IDirect3DVertexShader9* getVertexShader();

		/**
		 * @copydoc D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuVertexProgram(const String& source, const String& entryPoint, GpuProgramProfile profile,
			const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D9GpuProgram::loadFromMicrocode
		 */
        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		Map<IDirect3DDevice9*, IDirect3DVertexShader9*>	mMapDeviceToVertexShader;

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

		/**
		 * @brief	Returns internal DX9 pixel shader object.
		 */
        IDirect3DPixelShader9* getPixelShader();

		/**
		 * @copydoc D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9HLSLProgramFactory;

		D3D9GpuFragmentProgram(const String& source, const String& entryPoint, GpuProgramProfile profile,
			const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D9GpuProgram::loadFromMicrocode
		 */
        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		Map<IDirect3DDevice9*, IDirect3DPixelShader9*> mMapDeviceToPixelShader;

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
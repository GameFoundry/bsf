#pragma once

#include "CmPrerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_EXPORT DeferredGpuCommand
	{
	public:
		DeferredGpuCommand();

		virtual void submitCommand(RenderSystem* rs) = 0;
	};

	class CM_EXPORT DfrdRenderGpuCommand : public DeferredGpuCommand
	{
	public:
		DfrdRenderGpuCommand(RenderOperation& _renderOp);

		RenderOperation renderOp;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdBindGpuProgramCommand : public DeferredGpuCommand
	{
	public:
		DfrdBindGpuProgramCommand(GpuProgramPtr _gpuProgram);

		GpuProgramPtr gpuProgram;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdBindGpuParamsCommand : public DeferredGpuCommand
	{
	public:
		DfrdBindGpuParamsCommand(GpuProgramType _type, GpuProgramParametersPtr& _progParams);

		GpuProgramType type;
		GpuProgramParametersPtr progParams;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdInvVertWindingCommand : public DeferredGpuCommand
	{
	public:
		DfrdInvVertWindingCommand(bool _invert);

		bool invert;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdScissorTestCommand : public DeferredGpuCommand
	{
	public:
		DfrdScissorTestCommand(bool _enabled, size_t _left, size_t _top, size_t _right, size_t _bottom);

		bool enabled;
		size_t left;
		size_t top;
		size_t right;
		size_t bottom;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdStencilBufferParamsCommand : public DeferredGpuCommand
	{
	public:
		DfrdStencilBufferParamsCommand(CompareFunction _func, UINT32 _refValue, UINT32 _mask, 
			StencilOperation _stencilFailOp, StencilOperation _depthFailOp, 
			StencilOperation _passOp, bool _twoSidedOperation);

		CompareFunction func;
		UINT32 refValue;
		UINT32 mask; 
		StencilOperation stencilFailOp;
		StencilOperation depthFailOp;
		StencilOperation passOp;
		bool twoSidedOperation;

		virtual void submitCommand(RenderSystem* rs);
	};

	// TODO !
	class CM_EXPORT DfrdVertexDeclCommand : public DeferredGpuCommand
	{
	public:
		DfrdVertexDeclCommand();

		virtual void submitCommand(RenderSystem* rs);
	};

	// TODO !
	class CM_EXPORT DfrdVertBufferBindCommand : public DeferredGpuCommand
	{
	public:
		DfrdVertBufferBindCommand();

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdPolygonModeCommand : public DeferredGpuCommand
	{
	public:
		DfrdPolygonModeCommand(PolygonMode _level);

		PolygonMode mode;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdStencilCheckCommand : public DeferredGpuCommand
	{
	public:
		DfrdStencilCheckCommand(bool _enabled);

		bool enabled;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdWaitForVerticalBlankCommand : public DeferredGpuCommand
	{
	public:
		DfrdWaitForVerticalBlankCommand(bool _enabled);

		bool enabled;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureUnitSettingsCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureUnitSettingsCommand(size_t _texUnit, const TexturePtr& _tex, 
			const SamplerState& _samplerState);

		size_t texUnit;
		TexturePtr tex;
		SamplerState samplerState;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdPointParamsCommand : public DeferredGpuCommand
	{
	public:
		DfrdPointParamsCommand(float _size, bool _attenuationEnabled, 
			float _constant, float _linear, float _quadratic, float _minSize, float _maxSize);

		float size; 
		bool attenuationEnabled; 
		float constant; 
		float linear; 
		float quadratic; 
		float minSize; 
		float maxSize;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureCommand(size_t _texUnit, bool _enabled, const TexturePtr& _tex);

		size_t texUnit;
		bool enabled;
		const TexturePtr &tex;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdVertexTextureCommand : public DeferredGpuCommand
	{
	public:
		DfrdVertexTextureCommand(size_t _texUnit, const TexturePtr& _tex);

		size_t texUnit;
		const TexturePtr& tex;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureFilteringCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureFilteringCommand(size_t _texUnit, FilterType _ftype, FilterOptions _filter);

		size_t texUnit;
		FilterType ftype;
		FilterOptions filter;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureAnisotropyCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureAnisotropyCommand(size_t _texUnit, unsigned int _maxAnisotropy);

		size_t texUnit;
		unsigned int maxAnisotropy;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureAddrModeCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureAddrModeCommand(size_t _texUnit, const SamplerState::UVWAddressingMode& _uvw);

		size_t texUnit;
		SamplerState::UVWAddressingMode uvw;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureBorderColorCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureBorderColorCommand(size_t _texUnit, const Color& _color);

		size_t texUnit;
		Color color;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdTextureMipBiasCommand : public DeferredGpuCommand
	{
	public:
		DfrdTextureMipBiasCommand(size_t _texUnit, float _bias);

		size_t texUnit;
		float bias;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdSceneBlendingCommand : public DeferredGpuCommand
	{
	public:
		DfrdSceneBlendingCommand(SceneBlendFactor _sourceFactor, 
			SceneBlendFactor _destFactor, SceneBlendOperation _op);

		SceneBlendFactor sourceFactor;
		SceneBlendFactor destFactor;
		SceneBlendOperation op;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdSeparateSceneBlendingCommand : public DeferredGpuCommand
	{
	public:
		DfrdSeparateSceneBlendingCommand(SceneBlendFactor _sourceFactor, 
			SceneBlendFactor _destFactor, SceneBlendFactor _sourceFactorAlpha, 
			SceneBlendFactor _destFactorAlpha, SceneBlendOperation _op, 
			SceneBlendOperation _alphaOp);

		SceneBlendFactor sourceFactor;
		SceneBlendFactor destFactor;
		SceneBlendFactor sourceFactorAlpha;
		SceneBlendFactor destFactorAlpha;
		SceneBlendOperation op;
		SceneBlendOperation alphaOp;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdAlphaRejectParamsCommand : public DeferredGpuCommand
	{
	public:
		DfrdAlphaRejectParamsCommand(CompareFunction _func, 
			unsigned char _value, bool _alphaToCoverage);

		CompareFunction func;
		unsigned char value;
		bool alphaToCoverage;

		virtual void submitCommand(RenderSystem* rs);
	};

	// TODO
	class CM_EXPORT DfrdViewportCommand : public DeferredGpuCommand
	{
	public:
		DfrdViewportCommand();

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdCullingCommand : public DeferredGpuCommand
	{
	public:
		DfrdCullingCommand(CullingMode _mode);

		CullingMode mode;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDepthBufferParamsCommand : public DeferredGpuCommand
	{
	public:
		DfrdDepthBufferParamsCommand(bool _depthTest, bool _depthWrite, 
			CompareFunction _depthFunction);

		bool depthTest;
		bool depthWrite;
		CompareFunction depthFunction;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDepthBufferCheckCommand : public DeferredGpuCommand
	{
	public:
		DfrdDepthBufferCheckCommand(bool _enabled);

		bool enabled;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDepthBufferWriteCommand : public DeferredGpuCommand
	{
	public:
		DfrdDepthBufferWriteCommand(bool _enabled);

		bool enabled;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDepthBufferFuncCommand : public DeferredGpuCommand
	{
	public:
		DfrdDepthBufferFuncCommand(CompareFunction _func);

		CompareFunction func;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDepthBiasCommand : public DeferredGpuCommand
	{
	public:
		DfrdDepthBiasCommand(float _constantBias, float _slopeScaleBias);

		float constantBias;
		float slopeScaleBias;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdColorBufferWriteCommand : public DeferredGpuCommand
	{
	public:
		DfrdColorBufferWriteCommand(bool _red, bool _green, bool _blue, bool _alpha);

		bool red;
		bool green;
		bool blue;
		bool alpha;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDisableTextureUnitCommand : public DeferredGpuCommand
	{
	public:
		DfrdDisableTextureUnitCommand(size_t _texUnit);

		size_t texUnit;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdDisableTextureUnitsFromCommand : public DeferredGpuCommand
	{
	public:
		DfrdDisableTextureUnitsFromCommand(size_t _texUnit);

		size_t texUnit;

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdBeginFrameCommand : public DeferredGpuCommand
	{
	public:
		DfrdBeginFrameCommand();

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdEndFrameCommand : public DeferredGpuCommand
	{
	public:
		DfrdEndFrameCommand();

		virtual void submitCommand(RenderSystem* rs);
	};

	class CM_EXPORT DfrdClearFrameBufferCommand : public DeferredGpuCommand
	{
	public:
		DfrdClearFrameBufferCommand(unsigned int _buffers, 
			const Color& _color, float _depth, unsigned short _stencil);

		unsigned int buffers;
		Color color;
		float depth;
		unsigned short stencil;

		virtual void submitCommand(RenderSystem* rs);
	};
}
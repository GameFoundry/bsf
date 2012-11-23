#include "CmDeferredGpuCommands.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	DeferredGpuCommand::DeferredGpuCommand()
	{ }

	DfrdRenderGpuCommand::DfrdRenderGpuCommand(RenderOperation& _renderOp)
		:renderOp(_renderOp)
	{ }

	void DfrdRenderGpuCommand::submitCommand(RenderSystem* rs)
	{
		rs->render(renderOp);
	}

	DfrdBindGpuProgramCommand::DfrdBindGpuProgramCommand(GpuProgramPtr _gpuProgram)
		:gpuProgram(_gpuProgram)
	{ }

	void DfrdBindGpuProgramCommand::submitCommand(RenderSystem* rs)
	{
		rs->bindGpuProgram(gpuProgram.get());
	}

	DfrdBindGpuParamsCommand::DfrdBindGpuParamsCommand(GpuProgramType _type, GpuProgramParametersPtr& _progParams)
		:type(_type), progParams(_progParams)
	{ }

	void DfrdBindGpuParamsCommand::submitCommand(RenderSystem* rs)
	{
		rs->bindGpuProgramParameters(type, progParams, GPV_ALL);
	}

	DfrdInvVertWindingCommand::DfrdInvVertWindingCommand(bool _invert)
		:invert(_invert)
	{ }

	void DfrdInvVertWindingCommand::submitCommand(RenderSystem* rs)
	{
		rs->setInvertVertexWinding(invert);
	}

	DfrdScissorTestCommand::DfrdScissorTestCommand(bool _enabled, size_t _left, size_t _top, size_t _right, size_t _bottom)
		:enabled(_enabled), left(_left), top(_top), right(_right), bottom(_bottom)
	{ }

	void DfrdScissorTestCommand::submitCommand(RenderSystem* rs)
	{
		rs->setScissorTest(enabled, left, top, right, bottom);
	}

	DfrdStencilBufferParamsCommand::DfrdStencilBufferParamsCommand(CompareFunction _func, UINT32 _refValue, UINT32 _mask, 
		StencilOperation _stencilFailOp, StencilOperation _depthFailOp, 
		StencilOperation _passOp, bool _twoSidedOperation)
		:func(_func), refValue(_refValue), mask(_mask), stencilFailOp(_stencilFailOp), depthFailOp(_depthFailOp),
		passOp(_passOp), twoSidedOperation(_twoSidedOperation)
	{ }

	void DfrdStencilBufferParamsCommand::submitCommand(RenderSystem* rs)
	{
		rs->setStencilBufferParams(func, refValue, mask, stencilFailOp, depthFailOp, passOp, twoSidedOperation);
	}

// TODO !
	DfrdVertexDeclCommand::DfrdVertexDeclCommand()
	{ }

	void DfrdVertexDeclCommand::submitCommand(RenderSystem* rs)
	{

	}

// TODO !
	DfrdVertBufferBindCommand::DfrdVertBufferBindCommand()
	{ }

	void DfrdVertBufferBindCommand::submitCommand(RenderSystem* rs)
	{

	}

	DfrdPolygonModeCommand::DfrdPolygonModeCommand(PolygonMode _mode)
		:mode(_mode)
	{ }

	void DfrdPolygonModeCommand::submitCommand(RenderSystem* rs)
	{
		rs->setPolygonMode(mode);
	}

	DfrdStencilCheckCommand::DfrdStencilCheckCommand(bool _enabled)
		:enabled(_enabled)
	{ }

	void DfrdStencilCheckCommand::submitCommand(RenderSystem* rs)
	{
		rs->setStencilCheckEnabled(enabled);
	}

	DfrdWaitForVerticalBlankCommand::DfrdWaitForVerticalBlankCommand(bool _enabled)
		:enabled(_enabled)
	{ }

	void DfrdWaitForVerticalBlankCommand::submitCommand(RenderSystem* rs)
	{
		rs->setWaitForVerticalBlank(enabled);
	}

	DfrdTextureUnitSettingsCommand::DfrdTextureUnitSettingsCommand(size_t _texUnit, const TexturePtr& _tex, 
		const SamplerState& _samplerState)
		:texUnit(_texUnit), tex(_tex), samplerState(_samplerState)
	{ }

	void DfrdTextureUnitSettingsCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureUnitSettings(texUnit, tex, samplerState);
	}

	DfrdPointParamsCommand::DfrdPointParamsCommand(float _size, bool _attenuationEnabled, 
		float _constant, float _linear, float _quadratic, float _minSize, float _maxSize)
		:size(_size), attenuationEnabled(_attenuationEnabled), constant(_constant), linear(_linear),
		quadratic(_quadratic), minSize(_minSize), maxSize(_maxSize)
	{ }

	void DfrdPointParamsCommand::submitCommand(RenderSystem* rs)
	{
		rs->setPointParameters(size, attenuationEnabled, constant, linear, quadratic, minSize, maxSize);
	}

	DfrdTextureCommand::DfrdTextureCommand(size_t _texUnit, bool _enabled, const TexturePtr& _tex)
		:texUnit(_texUnit), enabled(_enabled), tex(_tex)
	{ }

	void DfrdTextureCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTexture(texUnit, enabled, tex);
	}

	DfrdVertexTextureCommand::DfrdVertexTextureCommand(size_t _texUnit, const TexturePtr& _tex)
		:texUnit(_texUnit), tex(_tex)
	{ }

	void DfrdVertexTextureCommand::submitCommand(RenderSystem* rs)
	{
		rs->setVertexTexture(texUnit, tex);
	}

	DfrdTextureFilteringCommand::DfrdTextureFilteringCommand(size_t _texUnit, FilterType _ftype, FilterOptions _filter)
		:texUnit(_texUnit), ftype(_ftype), filter(_filter)
	{ }

	void DfrdTextureFilteringCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureFiltering(texUnit, ftype, filter);
	}

	DfrdTextureAnisotropyCommand::DfrdTextureAnisotropyCommand(size_t _texUnit, unsigned int _maxAnisotropy)
		:texUnit(_texUnit), maxAnisotropy(_maxAnisotropy)
	{ }

	void DfrdTextureAnisotropyCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureAnisotropy(texUnit, maxAnisotropy);
	}

	DfrdTextureAddrModeCommand::DfrdTextureAddrModeCommand(size_t _texUnit, const SamplerState::UVWAddressingMode& _uvw)
		:texUnit(_texUnit), uvw(_uvw)
	{ }

	void DfrdTextureAddrModeCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureAddressingMode(texUnit, uvw);
	}

	DfrdTextureBorderColorCommand::DfrdTextureBorderColorCommand(size_t _texUnit, const Color& _color)
		:texUnit(_texUnit), color(_color)
	{ }

	void DfrdTextureBorderColorCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureBorderColor(texUnit, color);
	}

	DfrdTextureMipBiasCommand::DfrdTextureMipBiasCommand(size_t _texUnit, float _bias)
		:texUnit(_texUnit), bias(_bias)
	{ }

	void DfrdTextureMipBiasCommand::submitCommand(RenderSystem* rs)
	{
		rs->setTextureMipmapBias(texUnit, bias);
	}

	DfrdSceneBlendingCommand::DfrdSceneBlendingCommand(SceneBlendFactor _sourceFactor, 
		SceneBlendFactor _destFactor, SceneBlendOperation _op)
		:sourceFactor(_sourceFactor), destFactor(_destFactor), op(_op)
	{ }

	void DfrdSceneBlendingCommand::submitCommand(RenderSystem* rs)
	{
		rs->setSceneBlending(sourceFactor, destFactor, op);
	}

	DfrdSeparateSceneBlendingCommand::DfrdSeparateSceneBlendingCommand(SceneBlendFactor _sourceFactor, 
		SceneBlendFactor _destFactor, SceneBlendFactor _sourceFactorAlpha, 
		SceneBlendFactor _destFactorAlpha, SceneBlendOperation _op, 
		SceneBlendOperation _alphaOp)
		:sourceFactor(_sourceFactor), destFactor(_destFactor), sourceFactorAlpha(_sourceFactorAlpha),
		destFactorAlpha(_destFactorAlpha), op(_op), alphaOp(_alphaOp)
	{ }

	void DfrdSeparateSceneBlendingCommand::submitCommand(RenderSystem* rs)
	{
		rs->setSeparateSceneBlending(sourceFactor, destFactor, sourceFactorAlpha, destFactorAlpha, op, alphaOp);
	}

	DfrdAlphaRejectParamsCommand::DfrdAlphaRejectParamsCommand(CompareFunction _func, 
		unsigned char _value, bool _alphaToCoverage)
		:func(_func), value(_value), alphaToCoverage(_alphaToCoverage)
	{ }

	void DfrdAlphaRejectParamsCommand::submitCommand(RenderSystem* rs)
	{
		rs->setAlphaRejectSettings(func, value, alphaToCoverage);
	}

// TODO
	DfrdViewportCommand::DfrdViewportCommand()
	{ }

	void DfrdViewportCommand::submitCommand(RenderSystem* rs)
	{

	}

	DfrdCullingCommand::DfrdCullingCommand(CullingMode _mode)
		:mode(_mode)
	{ }

	void DfrdCullingCommand::submitCommand(RenderSystem* rs)
	{
		rs->setCullingMode(mode);
	}

	DfrdDepthBufferParamsCommand::DfrdDepthBufferParamsCommand(bool _depthTest, bool _depthWrite, 
		CompareFunction _depthFunction)
		:depthTest(_depthTest)
	{ }

	void DfrdDepthBufferParamsCommand::submitCommand(RenderSystem* rs)
	{
		rs->setDepthBufferParams(depthTest, depthWrite, depthFunction);
	}

	DfrdDepthBufferCheckCommand::DfrdDepthBufferCheckCommand(bool _enabled)
		:enabled(_enabled)
	{ }

	void DfrdDepthBufferCheckCommand::submitCommand(RenderSystem* rs)
	{
		rs->setDepthBufferCheckEnabled(enabled);
	}

	DfrdDepthBufferWriteCommand::DfrdDepthBufferWriteCommand(bool _enabled)
		:enabled(_enabled)
	{ }

	void DfrdDepthBufferWriteCommand::submitCommand(RenderSystem* rs)
	{
		rs->setDepthBufferWriteEnabled(enabled);
	}

	DfrdDepthBufferFuncCommand::DfrdDepthBufferFuncCommand(CompareFunction _func)
		:func(_func)
	{ }

	void DfrdDepthBufferFuncCommand::submitCommand(RenderSystem* rs)
	{
		rs->setDepthBufferFunction(func);
	}

	DfrdDepthBiasCommand::DfrdDepthBiasCommand(float _constantBias, float _slopeScaleBias)
		:constantBias(_constantBias), slopeScaleBias(_slopeScaleBias)
	{ }

	void DfrdDepthBiasCommand::submitCommand(RenderSystem* rs)
	{
		rs->setDepthBias(constantBias, slopeScaleBias);
	}

	DfrdColorBufferWriteCommand::DfrdColorBufferWriteCommand(bool _red, bool _green, bool _blue, bool _alpha)
		:red(_red), green(_green), blue(_blue), alpha(_alpha)
	{ }

	void DfrdColorBufferWriteCommand::submitCommand(RenderSystem* rs)
	{
		rs->setColorBufferWriteEnabled(red, green, blue, alpha);
	}

	DfrdDisableTextureUnitCommand::DfrdDisableTextureUnitCommand(size_t _texUnit)
		:texUnit(_texUnit)
	{ }

	void DfrdDisableTextureUnitCommand::submitCommand(RenderSystem* rs)
	{
		rs->disableTextureUnit(texUnit);
	}

	DfrdDisableTextureUnitsFromCommand::DfrdDisableTextureUnitsFromCommand(size_t _texUnit)
		:texUnit(_texUnit)
	{ }

	void DfrdDisableTextureUnitsFromCommand::submitCommand(RenderSystem* rs)
	{
		rs->disableTextureUnitsFrom(texUnit);
	}

	DfrdBeginFrameCommand::DfrdBeginFrameCommand()
	{ }

	void DfrdBeginFrameCommand::submitCommand(RenderSystem* rs)
	{
		rs->beginFrame();
	}

	DfrdEndFrameCommand::DfrdEndFrameCommand()
	{ }

	void DfrdEndFrameCommand::submitCommand(RenderSystem* rs)
	{
		rs->endFrame();
	}

	DfrdClearFrameBufferCommand::DfrdClearFrameBufferCommand(unsigned int _buffers, 
		const Color& _color, float _depth, unsigned short _stencil)
		:buffers(_buffers), color(_color), depth(_depth), stencil(_stencil)
	{ }

	void DfrdClearFrameBufferCommand::submitCommand(RenderSystem* rs)
	{
		rs->clearFrameBuffer(buffers, color, depth, stencil);
	}
}
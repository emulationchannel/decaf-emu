#include "gpu/pm4.h"
#include "gpu/pm4_writer.h"
#include "gx2_registers.h"


void
GX2InitBlendConstantColorReg(GX2BlendConstantColorReg *reg,
                             float red,
                             float green,
                             float blue,
                             float alpha)
{
   reg->red = red;
   reg->green = green;
   reg->blue = blue;
   reg->alpha = alpha;
}


void
GX2SetBlendConstantColor(float red,
                         float green,
                         float blue,
                         float alpha)
{
   GX2BlendConstantColorReg reg;
   GX2InitBlendConstantColorReg(&reg, red, green, blue, alpha);
   GX2SetBlendConstantColorReg(&reg);
}


void
GX2SetBlendConstantColorReg(GX2BlendConstantColorReg *reg)
{
   auto values = reinterpret_cast<uint32_t*>(&reg->red);
   pm4::write(pm4::SetContextRegs { latte::Register::CB_BLEND_RED, { values, 4 } });
}


void
GX2GetBlendConstantColorReg(GX2BlendConstantColorReg *reg,
                            float *red,
                            float *green,
                            float *blue,
                            float *alpha)
{
   *red = reg->red;
   *green = reg->green;
   *blue = reg->blue;
   *alpha = reg->alpha;
}


void
GX2InitBlendControlReg(GX2BlendControlReg *reg,
                       GX2RenderTarget::Value target,
                       GX2BlendMode::Value colorSrcBlend,
                       GX2BlendMode::Value colorDstBlend,
                       GX2BlendCombineMode::Value colorCombine,
                       BOOL useAlphaBlend,
                       GX2BlendMode::Value alphaSrcBlend,
                       GX2BlendMode::Value alphaDstBlend,
                       GX2BlendCombineMode::Value alphaCombine)
{
   reg->target = target;
   reg->cb_blend_control.COLOR_SRCBLEND = static_cast<latte::BLEND_FUNC>(colorSrcBlend);
   reg->cb_blend_control.COLOR_DESTBLEND = static_cast<latte::BLEND_FUNC>(colorDstBlend);
   reg->cb_blend_control.COLOR_COMB_FCN = static_cast<latte::COMB_FUNC>(colorCombine);
   reg->cb_blend_control.SEPARATE_ALPHA_BLEND = useAlphaBlend;
   reg->cb_blend_control.ALPHA_SRCBLEND = static_cast<latte::BLEND_FUNC>(alphaSrcBlend);
   reg->cb_blend_control.ALPHA_DESTBLEND = static_cast<latte::BLEND_FUNC>(alphaDstBlend);
   reg->cb_blend_control.ALPHA_COMB_FCN = static_cast<latte::COMB_FUNC>(alphaCombine);
}

void
GX2SetBlendControl(GX2RenderTarget::Value target,
                   GX2BlendMode::Value colorSrcBlend,
                   GX2BlendMode::Value colorDstBlend,
                   GX2BlendCombineMode::Value colorCombine,
                   BOOL useAlphaBlend,
                   GX2BlendMode::Value alphaSrcBlend,
                   GX2BlendMode::Value alphaDstBlend,
                   GX2BlendCombineMode::Value alphaCombine)
{
   GX2BlendControlReg reg;
   GX2InitBlendControlReg(&reg,
                          target,
                          colorSrcBlend,
                          colorDstBlend,
                          colorCombine,
                          useAlphaBlend,
                          alphaSrcBlend,
                          alphaDstBlend,
                          alphaCombine);
   GX2SetBlendControlReg(&reg);
}

void
GX2SetBlendControlReg(GX2BlendControlReg *reg)
{
   auto id = static_cast<latte::Register::Value>(latte::Register::CB_BLEND0_CONTROL + reg->target);
   pm4::write(pm4::SetContextReg { id, reg->cb_blend_control.value });
}

void
GX2InitDepthStencilControlReg(GX2DepthStencilControlReg *reg,
                              BOOL depthTest,
                              BOOL depthWrite,
                              GX2CompareFunction::Value depthCompare,
                              BOOL stencilTest,
                              BOOL backfaceStencil,
                              GX2CompareFunction::Value frontStencilFunc,
                              GX2StencilFunction::Value frontStencilZPass,
                              GX2StencilFunction::Value frontStencilZFail,
                              GX2StencilFunction::Value frontStencilFail,
                              GX2CompareFunction::Value backStencilFunc,
                              GX2StencilFunction::Value backStencilZPass,
                              GX2StencilFunction::Value backStencilZFail,
                              GX2StencilFunction::Value backStencilFail)
{
   reg->db_depth_control.Z_ENABLE = depthTest;
   reg->db_depth_control.Z_WRITE_ENABLE = depthWrite;
   reg->db_depth_control.ZFUNC = static_cast<latte::FRAG_FUNC>(depthCompare);
   reg->db_depth_control.STENCIL_ENABLE = stencilTest;
   reg->db_depth_control.BACKFACE_ENABLE = backfaceStencil;
   reg->db_depth_control.STENCILFUNC = static_cast<latte::REF_FUNC>(frontStencilFunc);
   reg->db_depth_control.STENCILZPASS = static_cast<latte::STENCIL_FUNC>(frontStencilZPass);
   reg->db_depth_control.STENCILZFAIL = static_cast<latte::STENCIL_FUNC>(frontStencilZFail);
   reg->db_depth_control.STENCILFAIL = static_cast<latte::STENCIL_FUNC>(frontStencilFail);
   reg->db_depth_control.STENCILFUNC_BF = static_cast<latte::REF_FUNC>(backStencilFunc);
   reg->db_depth_control.STENCILZPASS_BF = static_cast<latte::STENCIL_FUNC>(backStencilZPass);
   reg->db_depth_control.STENCILZFAIL_BF = static_cast<latte::STENCIL_FUNC>(backStencilZFail);
   reg->db_depth_control.STENCILFAIL_BF = static_cast<latte::STENCIL_FUNC>(backStencilFail);
}

void
GX2SetDepthStencilControl(BOOL depthTest,
                          BOOL depthWrite,
                          GX2CompareFunction::Value depthCompare,
                          BOOL stencilTest,
                          BOOL backfaceStencil,
                          GX2CompareFunction::Value frontStencilFunc,
                          GX2StencilFunction::Value frontStencilZPass,
                          GX2StencilFunction::Value frontStencilZFail,
                          GX2StencilFunction::Value frontStencilFail,
                          GX2CompareFunction::Value backStencilFunc,
                          GX2StencilFunction::Value backStencilZPass,
                          GX2StencilFunction::Value backStencilZFail,
                          GX2StencilFunction::Value backStencilFail)
{
   GX2DepthStencilControlReg reg;
   GX2InitDepthStencilControlReg(&reg,
                                 depthTest,
                                 depthWrite,
                                 depthCompare,
                                 stencilTest,
                                 backfaceStencil,
                                 frontStencilFunc,
                                 frontStencilZPass,
                                 frontStencilZFail,
                                 frontStencilFail,
                                 backStencilFunc,
                                 backStencilZPass,
                                 backStencilZFail,
                                 backStencilFail);
   GX2SetDepthStencilControlReg(&reg);
}

void
GX2SetDepthStencilControlReg(GX2DepthStencilControlReg *reg)
{
   pm4::write(pm4::SetContextReg { latte::Register::DB_DEPTH_CONTROL, reg->db_depth_control.value });
}

void
GX2SetPrimitiveRestartIndex(uint32_t index)
{
   pm4::write(pm4::SetContextReg { latte::Register::VGT_MULTI_PRIM_IB_RESET_INDX, index });
}

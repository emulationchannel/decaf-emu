#include "gx2_surface.h"
#include "gx2_enum_string.h"
#include "gx2_format.h"
#include "gpu/latte_untile.h"
#include "gpu/pm4_writer.h"
#include "utils/log.h"
#include "utils/align.h"

void
GX2CalcSurfaceSizeAndAlignment(GX2Surface *surface)
{
   using latte::group_bytes;
   using latte::num_banks;
   using latte::tile_width;
   using latte::tile_height;

   if (surface->tileMode == GX2TileMode::Default) {
      gLog->debug("Defaulting tileMode to LinearAligned for format {}", GX2EnumAsString(surface->format));
      surface->tileMode = GX2TileMode::LinearAligned;
   } else {
      // Warn, because we have not checked if our calculations are correct...
      gLog->warn("Encountered non-default tileMode {} for format {}", GX2EnumAsString(surface->tileMode), GX2EnumAsString(surface->format));
   }

   auto blockSize = GX2GetSurfaceBlockSize(surface->format);
   auto element_bytes = GX2GetSurfaceElementBytes(surface->format);
   auto tile_thickness = GX2GetTileThickness(surface->tileMode);

   size_t num_samples = 1;
   size_t pitch_elements_align = 1;
   size_t base_align = 1;
   size_t height_align = 1;

   switch (surface->tileMode) {
   case GX2TileMode::LinearSpecial:
      break;
   case GX2TileMode::LinearAligned:
      base_align = group_bytes;
      pitch_elements_align = std::max(64ull, group_bytes / element_bytes);
      break;
   case GX2TileMode::Tiled1DThick:
      assert(num_samples == 1);
   case GX2TileMode::Tiled1DThin1:
      // TODO: Depth/slices pad to tile_thickness
      base_align = group_bytes;
      height_align = tile_height;
      pitch_elements_align = std::max(tile_width, group_bytes / (tile_height * tile_thickness * element_bytes * num_samples));
      break;
   case GX2TileMode::Tiled2DThick:
   case GX2TileMode::Tiled2BThick:
   case GX2TileMode::Tiled3DThick:
   case GX2TileMode::Tiled3BThick:
      assert(num_samples == 1);
   case GX2TileMode::Tiled2DThin1:
   case GX2TileMode::Tiled2DThin2:
   case GX2TileMode::Tiled2DThin4:
   case GX2TileMode::Tiled2BThin1:
   case GX2TileMode::Tiled2BThin2:
   case GX2TileMode::Tiled2BThin4:
   case GX2TileMode::Tiled3DThin1:
   case GX2TileMode::Tiled3BThin1:
   {
      auto macro_tile_size = GX2GetMacroTileSize(surface->tileMode);
      auto macro_tile_width = macro_tile_size.first;
      auto macro_tile_height = macro_tile_size.second;
      auto tile_bytes = tile_width * tile_height * tile_thickness * element_bytes * num_samples;
      auto macro_tile_bytes = macro_tile_width * macro_tile_height * tile_bytes;

      // TODO: Depth/slices pad to tile_thickness
      height_align = macro_tile_height;
      pitch_elements_align = std::max(macro_tile_width, ((group_bytes / tile_height) / (element_bytes * num_samples)) * num_banks);
      base_align = std::max(macro_tile_bytes, pitch_elements_align * element_bytes * macro_tile_height * num_samples);
   } break;
   }

   surface->alignment = base_align;
   surface->pitch = align_up<uint32_t>(surface->width, pitch_elements_align);
   surface->height = align_up<uint32_t>(surface->height, height_align);
   surface->imageSize = surface->width * surface->height * element_bytes;
}

void
GX2CalcDepthBufferHiZInfo(GX2DepthBuffer *depthBuffer,
                          be_val<uint32_t> *outSize,
                          be_val<uint32_t> *outAlignment)
{
   // TODO: GX2CalcDepthBufferHiZInfo
   *outSize = depthBuffer->surface.imageSize / (8 * 8);
   *outAlignment = 4;
}

void
GX2SetColorBuffer(GX2ColorBuffer *colorBuffer, GX2RenderTarget::Value target)
{
   using namespace latte::Register;
   uint32_t addr256, aaAddr256;
   auto reg = [](unsigned id) { return static_cast<latte::Register::Value>(id); };

   addr256 = colorBuffer->surface.image.getAddress() >> 8;
   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_BASE + target), addr256 });
   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_SIZE + target), colorBuffer->regs.cb_color_size.value });
   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_INFO + target), colorBuffer->regs.cb_color_info.value });

   if (colorBuffer->surface.aa != 0) {
      aaAddr256 = colorBuffer->aaBuffer.getAddress() >> 8;
   } else {
      aaAddr256 = 0;
   }

   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_TILE + target), aaAddr256 });
   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_FRAG + target), aaAddr256 });

   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_VIEW + target), colorBuffer->regs.cb_color_view.value });
   pm4::write(pm4::SetContextReg { reg(CB_COLOR0_MASK + target), colorBuffer->regs.cb_color_mask.value });
}

void
GX2SetDepthBuffer(GX2DepthBuffer *depthBuffer)
{
}

void
GX2InitColorBufferRegs(GX2ColorBuffer *colorBuffer)
{
   // FUCK
}

void
GX2InitDepthBufferRegs(GX2DepthBuffer *depthBuffer)
{
}

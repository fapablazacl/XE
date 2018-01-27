
set (target "XE.FreeImage")
set (source_base "XE/Graphics/FI")
set (sources_xe_graphics_fi
	"${source_base}/ImageFI.hpp"
	"${source_base}/ImageFI.hpp"
	"${source_base}/ImageLoaderFI.hpp"
	"${source_base}/ImageLoaderFI.hpp"
) 

source_group("\\XE\\Graphics\\FI" FILES ${sources_xe_graphics_fi})

add_library(${target} ${sources_xe_graphics_fi})

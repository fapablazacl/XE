
set (target "XE.FreeImage")
set (source_base "XE/Graphics/FI")
set (sources_xe_graphics_fi
	"${source_base}/ImageFI.h"
	"${source_base}/ImageFI.h"
	"${source_base}/ImageLoaderFI.h"
	"${source_base}/ImageLoaderFI.h"
) 

source_group("\\XE\\Graphics\\FI" FILES ${sources_xe_graphics_fi})

add_library(${target} ${sources_xe_graphics_fi})

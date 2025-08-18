
ASCII Camera Preview

ascii_camera_preview.h is a C header file for visualizing camera paths in 3D as an ASCII grid in the terminal.
It supports multiple paths, motion-aligned and blended paths, look-at targets, start/end markers, optional path indices, and color output.

Features:
- Bold start/end markers: S/E for motion-aligned paths, s/e for blended paths
- Look-at targets: O (motion-aligned) / o (blended)
- Automatic symbol assignment for each path
- Optional path index display next to start marker (0-9)
- Dynamic aspect ratio scaling to fit the ASCII grid
- Color support for better readability (toggleable)
- Legend printed at the top

Usage:

1. Include the header in your project:

#include "ascii_camera_preview.h"

2. Prepare an array of paths:

ascii_path_info paths[5];
for(int i=0; i<5; i++){
    paths[i].path = some_camera_path[i]; // pointer to camera_path_smooth
    paths[i].symbol = 0;    // auto-assign symbol
    paths[i].blended = (i % 2); // alternating blended / motion-aligned
    paths[i].beta = 0.5;    // used for blended paths
}

3. Assign symbols:

ascii_assign_symbols(paths, 5);

4. Preview paths in ASCII:

ascii_preview_paths(paths, 5, 50, 1, 1);
// 5 paths, 50 samples, color enabled (1), show indices (1)

Legend:
- S/E – motion-aligned path start/end
- s/e – blended path start/end
- * / + – path points
- O / o – look-at target points
- 0-9 – path index next to start marker

Notes:
- Designed for terminal display (ASCII)
- Works with multiple overlapping paths
- Grid size is adjustable via ASCII_WIDTH / ASCII_HEIGHT
- Colors require terminal that supports ANSI escape codes

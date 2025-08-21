# ASCII Camera Preview

This is a simple **C project** that demonstrates:
- Defining a camera path with position + quaternion orientation
- Interpolating between keyframes (LERP for position, SLERP for orientation)
- Rendering a crude ASCII preview showing the camera `C` and its forward direction `>`

## Build

```sh
gcc ascii_camera_preview.c -o ascii_preview -lm
```

## Run

```sh
./ascii_preview
```

You will see the camera moving across the ASCII grid with a little arrow showing where it’s facing.

#include <stdio.h>
#include <math.h>
#include "camera_path.h"

#define GRID_SIZE 21
#define GRID_CENTER (GRID_SIZE / 2)
#define STEPS 10  // interpolation steps between keyframes

void plot_frame(Vec3 pos, Vec3 forward, int frame) {
    char grid[GRID_SIZE][GRID_SIZE];

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = '.';
        }
    }

    int gx = (int)roundf(GRID_CENTER + pos.x);
    int gy = (int)roundf(GRID_CENTER - pos.z);
    if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
        grid[gy][gx] = 'C';
    }

    // Draw forward direction (scaled)
    int fx = gx + (int)roundf(forward.x * 2);
    int fy = gy - (int)roundf(forward.z * 2);
    if (fx >= 0 && fx < GRID_SIZE && fy >= 0 && fy < GRID_SIZE) {
        grid[fy][fx] = '>';
    }

    printf("Frame %d: Camera at (%.2f, %.2f, %.2f)\n", frame, pos.x, pos.y, pos.z);
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            putchar(grid[y][x]);
        }
        putchar('\n');
    }
    printf("\n");
}

int main() {
    int frame = 0;
    for (int i = 0; i < camera_path_length - 1; i++) {
        CameraKeyframe a = camera_path[i];
        CameraKeyframe b = camera_path[i+1];
        for (int step = 0; step < STEPS; step++) {
            float t = (float)step / (STEPS-1);
            Vec3 pos = vec3_lerp(a.position, b.position, t);
            Quat rot = quat_slerp(a.rotation, b.rotation, t);
            Vec3 forward = quat_forward(rot);
            plot_frame(pos, forward, frame++);
        }
    }
    return 0;
}
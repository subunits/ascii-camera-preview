
// ======================= ascii_camera_preview.h =======================
#ifndef ASCII_CAMERA_PREVIEW_H
#define ASCII_CAMERA_PREVIEW_H

#include "camera_path.h"
#include <stdio.h>
#include <stdlib.h>

#define ASCII_WIDTH  60
#define ASCII_HEIGHT 20

typedef struct {
    camera_path_smooth *path;
    char symbol;
    int blended;   // 0 = motion-aligned, 1 = blended
    Q_REAL beta;   // only used if blended=1
} ascii_path_info;

/* ANSI color codes */
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"

/* Assign unique symbols to paths that have symbol=0 */
void ascii_assign_symbols(ascii_path_info *paths, size_t n_paths) {
    char available_symbols[] = "*+ox#%@&$";
    size_t n_symbols = sizeof(available_symbols) - 1; // exclude null
    size_t idx = 0;

    for(size_t p=0; p<n_paths; p++) {
        if(paths[p].symbol == 0) {
            paths[p].symbol = available_symbols[idx % n_symbols];
            idx++;
        }
    }
}

/* Draw multiple paths on ASCII grid with optional color/bold, start/end markers, look-at targets, path index, and legend */
void ascii_preview_paths(ascii_path_info *paths, size_t n_paths, size_t samples, int use_color, int show_indices) {
    char grid[ASCII_HEIGHT][ASCII_WIDTH];
    int color_map[ASCII_HEIGHT][ASCII_WIDTH]; // 0=none, 1=motion, 2=blended, 3=look-at
    char mark_map[ASCII_HEIGHT][ASCII_WIDTH]; // track S/E/s/e

    for(int y=0;y<ASCII_HEIGHT;y++)
        for(int x=0;x<ASCII_WIDTH;x++){
            grid[y][x]='.';
            color_map[y][x]=0;
            mark_map[y][x]=0;
        }

    /* Determine global bounding box */
    Q_REAL xmin=1e9,xmax=-1e9,zmin=1e9,zmax=-1e9;
    for(size_t p=0;p<n_paths;p++){
        camera_path_smooth *cam = paths[p].path;
        for(size_t i=0;i<cam->n;i++){
            if(cam->eye[i].x<xmin) xmin=cam->eye[i].x;
            if(cam->eye[i].x>xmax) xmax=cam->eye[i].x;
            if(cam->eye[i].z<zmin) zmin=cam->eye[i].z;
            if(cam->eye[i].z>zmax) zmax=cam->eye[i].z;
        }
    }

    /* Dynamic aspect ratio scaling */
    Q_REAL x_range = (xmax-xmin>1e-6)?(xmax-xmin):1.0;
    Q_REAL z_range = (zmax-zmin>1e-6)?(zmax-zmin):1.0;
    Q_REAL grid_ratio = (Q_REAL)ASCII_WIDTH / ASCII_HEIGHT;
    Q_REAL path_ratio = x_range / z_range;

    Q_REAL scale_x, scale_z;
    if(path_ratio > grid_ratio) {
        scale_x = x_range;
        scale_z = x_range / grid_ratio;
    } else {
        scale_x = z_range * grid_ratio;
        scale_z = z_range;
    }

    /* Plot each path */
    for(size_t p=0;p<n_paths;p++){
        camera_path_smooth *cam = paths[p].path;
        char sym = paths[p].symbol;
        char start_mark = paths[p].blended ? 's' : 'S';
        char end_mark   = paths[p].blended ? 'e' : 'E';
        int first_marked = 0, last_gx=0, last_gz=0;
        int color = paths[p].blended ? 2 : 1;

        for(Q_REAL alpha=0; alpha<=1.0; alpha+=1.0/samples){
            q_mat4 M;
            if(paths[p].blended)
                M = camera_path_blended_eval_uniform(cam, alpha, (q_vec3){0,1,0}, paths[p].beta);
            else
                M = camera_path_motion_aligned_eval_uniform(cam, alpha, (q_vec3){0,1,0});

            int gx = (int)((M.m[12]-xmin)/scale_x*ASCII_WIDTH);
            int gz = (int)((M.m[14]-zmin)/scale_z*ASCII_HEIGHT);

            /* Look-at target */
            int target_idx = (int)(alpha * (cam->n - 1));
            if(target_idx < 0) target_idx = 0;
            if(target_idx >= cam->n) target_idx = cam->n - 1;
            int tx = (int)((cam->target[target_idx].x - xmin)/scale_x*ASCII_WIDTH);
            int tz = (int)((cam->target[target_idx].z - zmin)/scale_z*ASCII_HEIGHT);

            if(tx >=0 && tx<ASCII_WIDTH && tz>=0 && tz<ASCII_HEIGHT){
                char target_sym = paths[p].blended ? 'o' : 'O';
                grid[ASCII_HEIGHT-1-tz][tx] = target_sym;
                color_map[ASCII_HEIGHT-1-tz][tx] = 3;
            }

            if(gx>=0 && gx<ASCII_WIDTH && gz>=0 && gz<ASCII_HEIGHT){
                if(!first_marked){
                    grid[ASCII_HEIGHT-1-gz][gx]=start_mark;
                    mark_map[ASCII_HEIGHT-1-gz][gx]=1;
                    first_marked=1;

                    // Optional path index
                    if(show_indices){
                        int idx_x = gx + 1;
                        if(idx_x < ASCII_WIDTH){
                            char idx_char = '0' + (p % 10); // show last digit
                            grid[ASCII_HEIGHT-1-gz][idx_x] = idx_char;
                        }
                    }
                } else {
                    grid[ASCII_HEIGHT-1-gz][gx]=sym;
                    color_map[ASCII_HEIGHT-1-gz][gx]=color;
                    last_gx=gx; last_gz=gz;
                }
            }
        }

        // mark end position
        if(first_marked){
            grid[ASCII_HEIGHT-1-last_gz][last_gx]=end_mark;
            mark_map[ASCII_HEIGHT-1-last_gz][last_gx]=1;
        }
    }

    /* Print legend */
    if(use_color)
        printf(COLOR_BOLD COLOR_GREEN "S/E" COLOR_RESET " = motion start/end, "
               COLOR_BOLD COLOR_MAGENTA "s/e" COLOR_RESET " = blended start/end, "
               COLOR_CYAN "*" COLOR_RESET "/" COLOR_YELLOW "+" COLOR_RESET " = path points, "
               COLOR_CYAN "O" COLOR_RESET "/" COLOR_MAGENTA "o" COLOR_RESET " = look-at targets, "
               "0-9 = path index

");
    else
        printf("S/E = motion start/end, s/e = blended start/end, * / + = path points, O/o = look-at targets, 0-9 = path index

");

    /* Print grid */
    for(int y=0;y<ASCII_HEIGHT;y++){
        for(int x=0;x<ASCII_WIDTH;x++){
            char c = grid[y][x];
            if(use_color){
                if(c=='S' || c=='E') printf(COLOR_BOLD COLOR_GREEN "%c" COLOR_RESET, c);
                else if(c=='s' || c=='e') printf(COLOR_BOLD COLOR_MAGENTA "%c" COLOR_RESET, c);
                else if(color_map[y][x]==1) printf(COLOR_CYAN "%c" COLOR_RESET, c);
                else if(color_map[y][x]==2) printf(COLOR_YELLOW "%c" COLOR_RESET, c);
                else if(color_map[y][x]==3) printf((c=='O') ? COLOR_CYAN "O" COLOR_RESET : COLOR_MAGENTA "o" COLOR_RESET);
                else putchar(c);
            } else {
                putchar(c);
            }
        }
        putchar('
');
    }
}

#endif

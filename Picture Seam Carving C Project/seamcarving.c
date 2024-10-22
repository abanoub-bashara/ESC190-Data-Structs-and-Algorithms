#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "c_img.h"


void print_best_arr(double *bestarr, int width, int height){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            printf("%f      ", bestarr[i*width + j]);
        }
        printf("\n");
    }
}

void print_path(int *path, int size){
    printf("[");
    for (int i = 0; i < size-1; i++){
        printf("%d, ", path[i]);
    }
    printf("%d]\n", path[size-1]);
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    //create dual energy gradient "rgb img"
    int height = im->height;
    int width = im->width; 
    create_img(grad, height, width);
    
    //loop around the file, calculate and record dual gradient energy
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            int r_x = get_pixel(im, y, (x+1)%width, 0) - get_pixel(im, y, (x-1 + width)%width, 0);
            int g_x = get_pixel(im, y, (x+1)%width, 1) - get_pixel(im, y, (x-1 + width)%width, 1);
            int b_x = get_pixel(im, y, (x+1)%width, 2) - get_pixel(im, y, (x-1 + width)%width, 2);
            
            int r_y = get_pixel(im, (y+1)%height, x, 0) - get_pixel(im, (y-1 + height)%height, x, 0);
            int g_y = get_pixel(im, (y+1)%height, x, 1) - get_pixel(im, (y-1 + height)%height, x, 1);
            int b_y = get_pixel(im, (y+1)%height, x, 2) - get_pixel(im, (y-1 + height)%height, x, 2);

            int delta_x = (r_x * r_x) + (g_x * g_x) + (b_x * b_x);
            int delta_y = (r_y * r_y) + (g_y * g_y) + (b_y * b_y);

            double energy = sqrt (delta_x + delta_y);
            uint8_t grad_energy = (uint8_t)(energy/10);
            set_pixel(*grad, y, x, grad_energy, grad_energy, grad_energy);      
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    //(*best_arr)[i*width+j] contains the minimum cost of a seam from the top of grad to the point (i,j).
    int width = grad->width;
    int height = grad->height;
    *best_arr = (double *)malloc(sizeof(double) * width * height);

    //Base condition, copy the values of the first row 
    for (int column = 0; column < width; column ++){
        (*best_arr)[column] = get_pixel(grad, 0, column, 0); //first zero is row and second is "colour"
    }

    //Dynamic "step"
    for (int row = 1; row < height; row ++){
        for(int column = 0; column < width; column ++){
            double min = (*best_arr)[(row-1)*width + column]; //pixel directly above '|' path
            if (column != 0){ //if not on left edge, calc energy from top left pixel '\' path
                if((*best_arr)[(row-1)*width + (column-1)] < min){
                    min = (*best_arr)[(row-1)*width + (column-1)];
                }
            }
            if (column != width - 1){ //if not on right edge, calc energy from top right pixel '/' path
                if((*best_arr)[(row-1)*width + (column+1)] < min){
                    min = (*best_arr)[(row-1)*width + (column+1)];
                }
            }
            (*best_arr)[row*width + column] = get_pixel(grad, row, column, 0) + min;
            //the min_up_to(row, column) = min_at(row, column) + min_up_to(row-1, column-1)
        }
    }

}

void recover_path(double *best, int height, int width, int **path){
    //allocates and computes the path array *path.
    //(*path)[i] contains the x-coordinate of the seam at height i of the image.
    //find the minimum value at the bottom and move back up
    *path = (int *)malloc(sizeof(int) * height); 
    int min = best[(height-1)*width]; //init min as bottom leftmost value
    for (int row = height-1; row >= 0; row--){ //start at bottom
        for (int column = 0; column < width; column++){  
            if(best[row*width + column] < min){
                min = best[row*width + column];
                (*path)[row] = column;
            }
        }
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    //create the destination image and writes it to the source image with the seam removed
    int height = src->height;
    int width = src ->width - 1; //minus 1 bc we removed a 1 pixel thick seam from src img
    create_img(dest, height, width);
    for (int row = 0; row < height; row++){
        for (int column = 0; column < width; column ++){
            if (column != (path)[row]){
                set_pixel(*dest, row, column, get_pixel(src, row, column, 0), get_pixel(src, row, column, 1), get_pixel(src, row, column, 2));
            }   
        }
    }
}


// void main(){
//     struct rgb_img *im;
//     struct rgb_img *cur_im;
//     struct rgb_img *grad;
//     double *best;
//     int *path;

//     read_in_img(&im, "HJoceanSmall.bin");
// //     //Test grad energy func
// //     calc_energy(im, &grad);
// //     print_grad(grad);

// //    //Test dynamic seam func
// //     dynamic_seam(grad, &best);
// //     print_best_arr(best, grad->width, grad->height);

// //     //Test reover path func
// //     recover_path(best, grad->height, grad->width, &path);
// //     print_path(path, grad->height);

//     for(int i = 0; i < 5; i++){
//         printf("i = %d\n", i);
//         calc_energy(im,  &grad);
//         dynamic_seam(grad, &best);
//         recover_path(best, grad->height, grad->width, &path);
//         remove_seam(im, &cur_im, path);

//         char filename[200];
//         sprintf(filename, "img%d.bin", i);
//         write_img(cur_im, filename);


//         destroy_image(im);
//         destroy_image(grad);
//         free(best);
//         free(path);
//         im = cur_im;
//     }
//     destroy_image(im);

// }
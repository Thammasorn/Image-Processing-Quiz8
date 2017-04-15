#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>     /* atof */
#include <math.h>       /* sin */
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

#define IMG_SIZE 500 
#define FIRST_PLANE_SIZE 100
#define SECOND_PLANE_SIZE 20

float **allocArrayFloat(int rows,int cols){
    float *data = (float *)calloc(rows*cols,sizeof(float));
    float **array = (float **)calloc(rows,sizeof(float*));
    for (int i=0;i<rows;i++){
        array[i] = &(data[cols*i]);
    }
    return array;
}

int **allocArrayInt(int rows,int cols){
    int *data = (int *)calloc(rows*cols,sizeof(int));
    int **array = (int **)calloc(rows,sizeof(int*));
    for (int i=0;i<rows;i++){
        array[i] = &(data[cols*i]);
    }
    return array;
}

int main(int argc, char** argv )
{
    /*-----------------------------*/
    /*--Check number of arguments--*/
    /*-----------------------------*/ 
    if ( argc != 4 )
    {
        printf("Plese input Z of 2nd plane, Z of camera, and V\n");
        return -1;
    }
    
    float Z_2nd_plane = atof(argv[1]);
    float Z_camera = atof(argv[2]);
    float V = atof(argv[3]);

    int** first_plane = allocArrayInt(FIRST_PLANE_SIZE,FIRST_PLANE_SIZE);
    //random dot on first plane
    for(int i=0 ; i<FIRST_PLANE_SIZE ; i++){
    	for(int j=0 ; j<FIRST_PLANE_SIZE ; j++){
    		if(rand() % 10 < 3) first_plane[i][j] = 1;
    	}
    }
    
    Mat mat_1st_plane = Mat::zeros( FIRST_PLANE_SIZE, FIRST_PLANE_SIZE, CV_8U );
    for(int i=0 ; i<FIRST_PLANE_SIZE ; i++){
        for(int j=0 ; j<FIRST_PLANE_SIZE ; j++){
            if(first_plane[i][j] == 1)
                mat_1st_plane.at<uchar>(i,j) = 255;
        }
    }
    namedWindow("First Plane", WINDOW_NORMAL );
    imshow("First Plane", mat_1st_plane);
    waitKey(0);

    int** second_plane = allocArrayInt(SECOND_PLANE_SIZE,SECOND_PLANE_SIZE);
    //random dot on first plane
    for(int i=0 ; i<SECOND_PLANE_SIZE ; i++){
    	for(int j=0 ; j<SECOND_PLANE_SIZE ; j++){
    		if(rand() % 10 < 3) second_plane[i][j] = 1;
    	}
    }

    Mat mat_2nd_plane = Mat::zeros( SECOND_PLANE_SIZE, SECOND_PLANE_SIZE, CV_8U );
    for(int i=0 ; i<SECOND_PLANE_SIZE ; i++){
        for(int j=0 ; j<SECOND_PLANE_SIZE ; j++){
            if(second_plane[i][j] == 1)
                mat_2nd_plane.at<uchar>(i,j) = 255;
        }
    }
    namedWindow("Second Plane", WINDOW_NORMAL );
    imshow("Second Plane", mat_2nd_plane);
    waitKey(0);

    int right_camera_center_x = 50;
    int left_camera_center_x = 44;
    int camera_center_y = 50;
    //calculate pixel per unit
    float x_left_1st_plane = ((right_camera_center_x-0)*0.35)/Z_camera;
    float x_right_1st_plane = ((right_camera_center_x-100)*0.35)/Z_camera;
    float pixel_per_unit = IMG_SIZE/(x_left_1st_plane - x_right_1st_plane);
    for(int frame=0 ; frame<30 ; frame++){
    	Mat right_camera_image = Mat::zeros( IMG_SIZE, IMG_SIZE, CV_8UC3 );
        Mat left_camera_image = Mat::zeros( IMG_SIZE, IMG_SIZE, CV_8UC3 );
    	//update camera center
    	right_camera_center_x = right_camera_center_x - V;
        left_camera_center_x = left_camera_center_x - V;
    	//first plane
    	// printf ("x_left_1st_plane : %.2f x_right_1st_plane: %.2f \n",x_left_1st_plane,x_right_1st_plane);
    	for(int i=0 ; i<FIRST_PLANE_SIZE ; i++){
	    	for(int j=0 ; j<FIRST_PLANE_SIZE ; j++){
	    		if(first_plane[i][j] == 1){
	    			//cal x
	    			float X_right = right_camera_center_x - j;
	    			float x_right = (X_right*0.35)/Z_camera;
                    float X_left = left_camera_center_x - j;
                    float x_left = (X_left*0.35)/Z_camera;
	    			//cal y
	    			float Y = camera_center_y - i;
	    			float y = (Y*0.35)/Z_camera;
	    			//draw
	    			int row_in_image = (IMG_SIZE/2) + y*pixel_per_unit;
	    			int col_in_image_right = (IMG_SIZE/2) + x_right*pixel_per_unit;
                    int col_in_image_left = (IMG_SIZE/2) + x_left*pixel_per_unit;
                    if(row_in_image > 0 && row_in_image < IMG_SIZE){
                        Vec3b intensity;
                        intensity.val[0] = 255;
                        intensity.val[1] = 0;
                        intensity.val[2] = 0;
                        right_camera_image.at<Vec3b>(row_in_image, col_in_image_right) = intensity;
                        left_camera_image.at<Vec3b>(row_in_image, col_in_image_left) = intensity;
                    }
	    			//printf ("x : %.2f ",x);
	    		}
	    	}
    	}

    	//second plane
    	// float x_left_2nd_plane = ((right_camera_center_x-40)*0.35)/(Z_2nd_plane);
    	// float x_right_2nd_plane = ((right_camera_center_x-60)*0.35)/(Z_2nd_plane);
    	for(int i=0 ; i<SECOND_PLANE_SIZE ; i++){
	    	for(int j=0 ; j<SECOND_PLANE_SIZE ; j++){
	    		if(second_plane[i][j] == 1){
	    			//call x
                    float X_right = right_camera_center_x - (j + 40);
	    			float x_right = (X_right*0.35)/(Z_camera - Z_2nd_plane);
                    float X_left = left_camera_center_x - (j + 40);
                    float x_left = (X_left*0.35)/(Z_camera - Z_2nd_plane);
	    			//cal y
	    			float Y = camera_center_y - (i + 40);
	    			float y = (Y*0.35)/(Z_camera - Z_2nd_plane);
	    			//draw
                    int row_in_image = (IMG_SIZE/2) + y*pixel_per_unit;
                    int col_in_image_right = (IMG_SIZE/2) + x_right*pixel_per_unit;
                    int col_in_image_left = (IMG_SIZE/2) + x_left*pixel_per_unit;
                    if(row_in_image > 0 && row_in_image < IMG_SIZE){
                        Vec3b intensity;
                        intensity.val[0] = 0;
                        intensity.val[1] = 255;
                        intensity.val[2] = 0;
                        right_camera_image.at<Vec3b>(row_in_image, col_in_image_right) = intensity;
                        left_camera_image.at<Vec3b>(row_in_image, col_in_image_left) = intensity;
                    }
	    		}
	    	}
    	}

        //namedWindow("Full Image", WINDOW_NORMAL );
        imshow("Right Image", right_camera_image);
        imshow("Left Image", left_camera_image);
        waitKey(0);
    }
    
}
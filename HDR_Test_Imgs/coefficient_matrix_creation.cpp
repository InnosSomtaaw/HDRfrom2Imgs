// This is the main DLL file.

#include "stdafx.h"
#include "coefficient_matrix_creation.h"

void coefficient_matrix_creation::create_coff_mat(Size coff_mat_size,float* &coff_mat_group)
{	
	Mat coff_img = Mat::zeros(coff_mat_size,CV_32FC1);
	float lambda = float(-4 * Math::PI * Math::PI *1.5);
	int half_row = coff_img.rows/2,half_col = coff_img.cols/2;

	coff_mat_group = new float[coff_img.cols*coff_img.rows];
	int n = 0;
	float* r_ptr;
	r_ptr = coff_img.ptr<float>(0);
    *r_ptr = 0;
	coff_mat_group[n] = 0;
	n++;
	r_ptr++;
    for (int j = 1; j <= half_col; j++)
    {
        float tsf = (j * j) * lambda;
		tsf = 1/tsf;
		coff_mat_group[n++] = tsf;
        *r_ptr++ = tsf;
    }
	float* r_ptr_backward;
	r_ptr_backward = r_ptr;
	r_ptr_backward--;
    for (int j = half_col + 1; j < coff_img.cols; j++)
	{
		coff_mat_group[n++] = *r_ptr_backward;
		*r_ptr++ = *r_ptr_backward--;
    }
	for (int i = 1; i <= half_row; i++)
	{
		r_ptr = coff_img.ptr<float>(i);
		for (int j = 0; j <= half_col; j++)
		{
			float tsf = (i * i + j * j) * lambda;
			tsf = 1/tsf;
			coff_mat_group[n++] = tsf;
			*r_ptr++=tsf;
		}
		r_ptr_backward = r_ptr;
		r_ptr_backward--;
		for (int j = half_col + 1; j < coff_img.cols; j++)
		{
			coff_mat_group[n++] = *r_ptr_backward;
			*r_ptr++ = *r_ptr_backward--;
		}
	}
	for (int i = half_row + 1; i < coff_img.rows; i++)
	{
		r_ptr = coff_img.ptr<float>(i);
		r_ptr_backward = coff_img.ptr<float>(2*half_row-i);
		for (int j=0;j<coff_img.cols;j++)
		{
			coff_mat_group[n++] = *r_ptr_backward;
			*r_ptr++ = *r_ptr_backward++;
		}
	}

	coff_img.release();
}

extern "C" __declspec(dllexport) float* _stdcall get_coff_mat(int mat_rows,int mat_cols,int& mat_length)
{
	mat_length = mat_cols*mat_rows;
	Size cms = cv::Size(mat_cols,mat_rows);
	float* mat_data;

	coefficient_matrix_creation::create_coff_mat(cms,mat_data);

	return mat_data;
}

extern "C" __declspec(dllexport) void _stdcall release_float_group(float* float_group_name)
{
	delete[] float_group_name;
}





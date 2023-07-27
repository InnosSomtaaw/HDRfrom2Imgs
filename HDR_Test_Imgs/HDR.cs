using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Runtime.InteropServices;

using Emgu.CV;
using Emgu.CV.Structure;
using Emgu.Util;
using Emgu.CV.CvEnum;
using Emgu.CV.UI;

namespace HDR
{
    class HDR
    {
        public UMat coff_mat = null;

        [System.Runtime.InteropServices.DllImport("coefficient_matrix_creation.dll", EntryPoint = "get_coff_mat")]
        public static extern IntPtr get_coff_mat(int rows, int cols,ref int length);

        [System.Runtime.InteropServices.DllImport("coefficient_matrix_creation.dll", EntryPoint = "release_float_group")]
        public static extern void release_float_group(IntPtr name);

        public void create_coff_mat_cpp(Size coff_mat_size)
        {
            System.Diagnostics.Stopwatch msw = new System.Diagnostics.Stopwatch();
            msw.Start();

            int coff_mat_size_length = 0;
            IntPtr coff_mat_data_ptr = get_coff_mat(coff_mat_size.Height, coff_mat_size.Width, ref coff_mat_size_length);
            float[] coff_mat_data = new float[coff_mat_size_length];
            Marshal.Copy(coff_mat_data_ptr, coff_mat_data, 0, coff_mat_size_length);
            release_float_group(coff_mat_data_ptr);

            byte[] coff_mat_data_byte_groupform = new byte[4*coff_mat_size_length];
            int i = 0;
            foreach (float f in coff_mat_data)
            {
                byte[] temp_coff_mat_data_byte_groupform = BitConverter.GetBytes(f);
                foreach (byte b in temp_coff_mat_data_byte_groupform)
                {
                    coff_mat_data_byte_groupform[i] = b;
                    i++;
                }
            }

            coff_mat = new UMat(coff_mat_size, DepthType.Cv32F, 1);
            coff_mat.Bytes = coff_mat_data_byte_groupform;

            msw.Stop();
            Console.WriteLine("调用C++创建频域图像处理用系数矩阵用时：" + msw.ElapsedMilliseconds);
        }

        //////创建频域图像处理用系数矩阵
        public void create_coff_mat(Size coff_mat_size)
        {
            System.Diagnostics.Stopwatch msw = new System.Diagnostics.Stopwatch();
            msw.Start();

            Image<Gray, float> coff_img = new Image<Gray, float>(coff_mat_size);
            float lambda = -4f * (float)Math.PI * (float)Math.PI*1.5f;
            int half_row = coff_mat_size.Height / 2, half_col = coff_mat_size.Width / 2;

            unsafe
            {
                float* r_ptr;
                r_ptr = (float*)(coff_img.MIplImage.ImageData);
                r_ptr[0] = 0;
                r_ptr++;
                for (int j = 1; j <= half_col; j++)
                {
                    float tsf = (j * j) * lambda;
                    *r_ptr++ = 1 / tsf;
                }
                for (int j = half_col + 1; j < coff_img.Cols; j++)
                {
                    float tsf = ((j - coff_img.Cols) * (j - coff_img.Cols)) * lambda;
                    *r_ptr++ = 1 / tsf;
                }
                for (int i = 1; i <= half_row; i++)
                {
                    r_ptr = (float*)(coff_img.MIplImage.ImageData + i * coff_img.MIplImage.WidthStep);
                    for (int j = 0; j <= half_col; j++)
                    {
                        float tsf = (i * i + j * j) * lambda;
                        *r_ptr++ = 1 / tsf;

                    }
                    for (int j = half_col + 1; j < coff_img.Cols; j++)
                    {
                        float tsf = (i * i + (j - coff_img.Cols) * (j - coff_img.Cols)) * lambda;
                        *r_ptr++ = 1 / tsf;
                    }
                }
                for (int i = half_row + 1; i < coff_img.Rows; i++)
                {
                    r_ptr = (float*)(coff_img.MIplImage.ImageData + i * coff_img.MIplImage.WidthStep);
                    for (int j = 0; j <= half_col; j++)
                    {
                        float tsf = ((i - coff_img.Rows) * (i - coff_img.Rows) + j * j) * lambda;
                        *r_ptr++ = 1 / tsf;
                    }
                    for (int j = half_col + 1; j < coff_img.Cols; j++)
                    {
                        float tsf = ((i - coff_img.Rows) * (i - coff_img.Rows) +
                    (j - coff_img.Cols) * (j - coff_img.Cols)) * lambda;
                        *r_ptr++ = 1 / tsf;
                    }
                }
            }

            coff_mat = coff_img.ToUMat();
            coff_img.Dispose();
            coff_img = null;

            msw.Stop();
            Console.WriteLine("创建频域图像处理用系数矩阵用时：" + msw.ElapsedMilliseconds);
        }

        public Image<Gray, float> two_imgs_hdr(Image<Gray, float> img_src_bright, Image<Gray, float> img_src_dark)
        {
            //CvInvoke.UseOpenCL = false;
            if (img_src_bright.Size!=img_src_dark.Size)
                return null;

            Image<Gray, float> img_hdr = null;

            System.Diagnostics.Stopwatch msw = new System.Diagnostics.Stopwatch();
            msw.Start();

            UMat Max_X = img_src_bright.ToUMat(), Max_Y = img_src_dark.ToUMat();
            //获取最大梯度图像
            get_max_derivation(Max_X, Max_Y, out Max_X, out Max_Y);

            UMat hdr_umat = new UMat(Max_X.Size, Max_X.Depth, Max_X.NumberOfChannels);
            //从最大梯度图像重建原图像
            hdr_umat = second_method_solving_Possion(Max_X, Max_Y);

            msw.Stop();
            Console.WriteLine("重建HDR图像总用时：" + msw.ElapsedMilliseconds);

            Max_X.Dispose(); Max_X = null;
            Max_Y.Dispose(); Max_Y = null;

            img_hdr = hdr_umat.ToImage<Gray, float>();

            return img_hdr;
        }

        //获取最大梯度图像
        public void get_max_derivation(UMat bright_img, UMat dark_img, out UMat max_derivativeX, out UMat max_derivativeY)
        {
            //CvInvoke.UseOpenCL = false;
            max_derivativeX = bright_img.Clone();
            max_derivativeY = dark_img.Clone();
            //max_derivativeX = null; max_derivativeY = null;
            if (bright_img.Size != dark_img.Size)
                return;

            UMat derivativeY_bright = bright_img.Clone();
            UMat derivativeX_bright = bright_img.Clone();
            UMat derivativeY_dark = dark_img.Clone();
            UMat derivativeX_dark = dark_img.Clone();

            Single[] temp_doubles = { -1, 0, 1 };
            Matrix<Single> mat4derivate = new Matrix<Single>(temp_doubles);

            CvInvoke.Filter2D(bright_img, derivativeY_bright, mat4derivate.ToUMat(), new Point(-1, -1));
            CvInvoke.Filter2D(dark_img, derivativeY_dark, mat4derivate.ToUMat(), new Point(-1, -1));

            mat4derivate = mat4derivate.Transpose();

            CvInvoke.Filter2D(bright_img, derivativeX_bright, mat4derivate.ToUMat(), new Point(-1, -1));
            CvInvoke.Filter2D(dark_img, derivativeX_dark, mat4derivate.ToUMat(), new Point(-1, -1));

            UMat derivativeY_bright_abs = new UMat(derivativeY_bright.Size, DepthType.Cv8U, derivativeY_bright.NumberOfChannels);
            UMat derivativeX_bright_abs = new UMat(derivativeX_bright.Size, DepthType.Cv8U, derivativeX_bright.NumberOfChannels);
            UMat derivativeY_dark_abs = new UMat(derivativeY_dark.Size, DepthType.Cv8U, derivativeY_dark.NumberOfChannels);
            UMat derivativeX_dark_abs = new UMat(derivativeX_dark.Size, DepthType.Cv8U, derivativeX_dark.NumberOfChannels);

            CvInvoke.ConvertScaleAbs(derivativeX_bright, derivativeX_bright_abs, 1, 0);
            CvInvoke.ConvertScaleAbs(derivativeY_bright, derivativeY_bright_abs, 1, 0);
            CvInvoke.ConvertScaleAbs(derivativeX_dark, derivativeX_dark_abs, 1, 0);
            CvInvoke.ConvertScaleAbs(derivativeY_dark, derivativeY_dark_abs, 1, 0);

            //UMat derivativeY_bright_abs = new UMat(derivativeY_bright.Size, derivativeY_bright.Depth, derivativeY_bright.NumberOfChannels);
            //UMat derivativeX_bright_abs = new UMat(derivativeX_bright.Size, derivativeX_bright.Depth, derivativeX_bright.NumberOfChannels);
            //UMat derivativeY_dark_abs = new UMat(derivativeY_dark.Size, derivativeY_dark.Depth, derivativeY_dark.NumberOfChannels);
            //UMat derivativeX_dark_abs = new UMat(derivativeX_dark.Size, derivativeX_dark.Depth, derivativeX_dark.NumberOfChannels);

            //CvInvoke.AbsDiff(derivativeX_bright, derivativeX_bright_abs, derivativeX_bright_abs);
            //CvInvoke.AbsDiff(derivativeY_bright, derivativeY_bright_abs, derivativeY_bright_abs);
            //CvInvoke.AbsDiff(derivativeX_dark, derivativeX_dark_abs, derivativeX_dark_abs);
            //CvInvoke.AbsDiff(derivativeY_dark, derivativeY_dark_abs, derivativeY_dark_abs);

            UMat Max_X = derivativeX_bright_abs.Clone(), Max_Y = derivativeY_bright_abs.Clone();
            CvInvoke.Compare(derivativeX_bright_abs, derivativeX_dark_abs, Max_X, CmpType.GreaterEqual);
            CvInvoke.Compare(derivativeY_bright_abs, derivativeY_dark_abs, Max_Y, CmpType.GreaterEqual);

            derivativeX_bright_abs.Dispose(); derivativeX_bright_abs = null;
            derivativeX_dark_abs.Dispose(); derivativeX_dark_abs = null;
            derivativeY_bright_abs.Dispose(); derivativeY_bright_abs = null;
            derivativeY_dark_abs.Dispose(); derivativeY_dark_abs = null;

            Max_X.ConvertTo(Max_X, DepthType.Cv32F);
            CvInvoke.Multiply(derivativeX_bright, Max_X, derivativeX_bright, 1f / 255f);
            CvInvoke.Threshold(Max_X, Max_X, 100, 255, ThresholdType.BinaryInv);
            CvInvoke.Multiply(derivativeX_dark, Max_X, derivativeX_dark, 1f / 255f);
            CvInvoke.Add(derivativeX_bright, derivativeX_dark, Max_X);

            Max_Y.ConvertTo(Max_Y, DepthType.Cv32F);
            CvInvoke.Multiply(derivativeY_bright, Max_Y, derivativeY_bright, 1f / 255f);
            CvInvoke.Threshold(Max_Y, Max_Y, 100, 255, ThresholdType.BinaryInv);
            CvInvoke.Multiply(derivativeY_dark, Max_Y, derivativeY_dark, 1f / 255f);
            CvInvoke.Add(derivativeY_bright, derivativeY_dark, Max_Y);

            derivativeX_bright.Dispose(); derivativeX_bright = null;
            derivativeX_dark.Dispose(); derivativeX_dark = null;
            derivativeY_bright.Dispose(); derivativeY_bright = null;
            derivativeY_dark.Dispose(); derivativeY_dark = null;

            max_derivativeX = Max_X; max_derivativeY = Max_Y;
        }

        //傅里叶变换数值求解泊松方程
        public UMat second_method_solving_Possion(UMat derivativeX, UMat derivativeY)
        {
            //System.Diagnostics.Stopwatch msw = new System.Diagnostics.Stopwatch();
            //msw.Start();

            if (derivativeX.Size != derivativeY.Size || derivativeX.Depth != derivativeY.Depth || derivativeX.NumberOfChannels != derivativeY.NumberOfChannels)
                return null;

            UMat solution_equation = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels);

            UMat div_v = new UMat(derivativeX.Size, derivativeX.Depth, 2); 

            UMat temp_dx = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels), temp_dy = new UMat(derivativeY.Size, derivativeY.Depth, derivativeY.NumberOfChannels);
            double[] temp_doubles = {-1,0,1};
            Matrix<Double> mat4derivate = new Matrix<Double>(temp_doubles);
            CvInvoke.Filter2D(derivativeY, temp_dy, mat4derivate, new Point(-1, -1));

            mat4derivate = mat4derivate.Transpose();
            CvInvoke.Filter2D(derivativeX, temp_dx, mat4derivate, new Point(-1, -1));

            CvInvoke.Add(temp_dx, temp_dy, temp_dx);
            temp_dy.SetTo(new MCvScalar(0));

            CvInvoke.InsertChannel(temp_dx,div_v,0);
            CvInvoke.InsertChannel(temp_dy,div_v,1);

            //msw.Stop();
            //Console.WriteLine("预备用时：" + msw.ElapsedMilliseconds);
            //msw.Reset();
            //msw.Start();

            CvInvoke.UseOpenCL = false;

            CvInvoke.Dft(div_v, div_v, DxtType.Forward, 0);

            //msw.Stop();
            //Console.WriteLine("FFT用时：" + msw.ElapsedMilliseconds);
            //msw.Reset();
            //msw.Start();

            CvInvoke.UseOpenCL = true;
            CvInvoke.ExtractChannel(div_v, temp_dx, 0);
            CvInvoke.ExtractChannel(div_v, temp_dy, 1);

            CvInvoke.Multiply(temp_dx, coff_mat, temp_dx);
            CvInvoke.Multiply(temp_dy, coff_mat, temp_dy);

            CvInvoke.InsertChannel(temp_dx, div_v, 0);
            CvInvoke.InsertChannel(temp_dy, div_v, 1);

            //msw.Stop();
            //Console.WriteLine("频域图像处理用时：" + msw.ElapsedMilliseconds);
            //msw.Reset();
            //msw.Start();

            CvInvoke.UseOpenCL = false;
            CvInvoke.Dft(div_v, div_v, DxtType.Inverse, 0);
            CvInvoke.UseOpenCL = true;
            CvInvoke.ExtractChannel(div_v, solution_equation, 0);

            //msw.Stop();
            //Console.WriteLine("IFFT用时：" + msw.ElapsedMilliseconds);
            //msw.Reset();
            //msw.Start();

            //Image<Gray, float> img_emgu4show = solution_equation.ToImage<Gray, float>();
            //img_viewer.Image = img_emgu4show;
            //img_viewer.ShowDialog();
            //img_emgu4show.Dispose();
            //img_emgu4show = null;

            temp_dx.Dispose();
            temp_dx = null;
            temp_dy.Dispose();
            temp_dy = null;
            div_v.Dispose();
            div_v = null;

            return solution_equation;
        }

        ////扩散方程逼近法数值求解泊松方程（有错）
        //public UMat first_method_solving_Possion(UMat derivativeX, UMat derivativeY)
        //{
        //    if (derivativeX.Size != derivativeY.Size || derivativeX.Depth!=derivativeY.Depth || derivativeX.NumberOfChannels!=derivativeY.NumberOfChannels)
        //        return null;

        //    ImageViewer img_viewer = new ImageViewer();

        //    UMat solution_equation = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels);

        //    UMat div_v = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels);
        //    using (UMat temp_dx = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels), temp_dy = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels))
        //    {
        //        double[] temp_doubles = { 1, 0, -1 };
        //        Matrix<Double> mat4derivate = new Matrix<Double>(temp_doubles);
        //        CvInvoke.Filter2D(derivativeY, temp_dy, mat4derivate, new Point(-1, -1),0,BorderType.Reflect);

        //        mat4derivate = mat4derivate.Transpose();
        //        CvInvoke.Filter2D(derivativeX, temp_dx, mat4derivate, new Point(-1, -1), 0, BorderType.Reflect);

        //        CvInvoke.Add(temp_dx, temp_dy, div_v);
        //    }

        //    //img_viewer.Image = div_v.ToImage<Gray, float>();
        //    //img_viewer.ShowDialog();

        //    using (UMat laplace_of_u = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels), rh_equation = new UMat(derivativeX.Size, derivativeX.Depth, derivativeX.NumberOfChannels))
        //    {
        //        int n = 0; double caucy_num = 100000;
        //        do
        //        {
        //            CvInvoke.Laplacian(solution_equation, laplace_of_u, solution_equation.Depth,1,-1,0,BorderType.Reflect);
        //            CvInvoke.Add(solution_equation, laplace_of_u, rh_equation);
        //            CvInvoke.Add(rh_equation, div_v, rh_equation);

        //            img_viewer.Image = rh_equation.ToImage<Gray, float>();
        //            img_viewer.ShowDialog();

        //            caucy_num = CvInvoke.Norm(rh_equation, solution_equation, NormType.L2);
        //            rh_equation.CopyTo(solution_equation);
        //            n++;
        //        } while (caucy_num > 1000 && n < 10);
        //    }

        //    return solution_equation;
        //}

    }
}

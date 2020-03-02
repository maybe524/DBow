#include <iostream>

#include <opencv2/core.hpp>                 // Mat 核心库
#include <opencv2/imgcodecs.hpp>            // imread读图片函数
#include <opencv2/highgui.hpp>              // namedWindow imshow waitKey    界面
#include <opencv2/imgproc.hpp>              // 图像处理

#include <math.h>

using namespace cv;
using namespace std;

// 网上的矫正方法
static void testByWeiBo(void)
{
    long time = clock();
    char *filename = "1.png";   // 图像路径
    Mat img = imread(filename);
    Mat drcimg(img.rows, img.cols, CV_8UC3);
    cv::imshow("矫正前", img);
    CvPoint lenscenter(img.cols / 2, img.rows / 2); // 镜头中心在图像中的位置
    CvPoint src_a, src_b, src_c, src_d; // a、b、c、d四个顶点
    // 矫正参数
    double r;   // 矫正前像素点跟镜头中心的距离
    double s;   // 矫正后像素点跟镜头中心的距离
    CvPoint2D32f mCorrectPoint;//矫正后点坐标
    double distance_to_a_x, distance_to_a_y;    // 求得中心点和边界的距离

    for (int row = 0; row < img.rows; row++) {  // 操作数据区,要注意OpenCV的RGB的存储顺序为GBR
        for (int cols = 0; cols < img.cols; cols++) {   // 示例为亮度调节
            r = sqrt((row - lenscenter.y) * (row - lenscenter.y) + (cols - lenscenter.x) * (cols - lenscenter.x));
            s = 0.9998 - 4.2932 * pow(10, -4) * r + 3.4327 * pow(10, -6) * pow(r, 2) -
                2.8526 * pow(10, -9) * pow(r, 3) + 9.8223 * pow(10, -13) * pow(r, 4);   // 比例
            mCorrectPoint = cvPoint2D32f((cols - lenscenter.x) / s * 1.11 + lenscenter.x,
                                         (row - lenscenter.y) / s * 1.11 + lenscenter.y);
            // 越界判断
            if ((mCorrectPoint.y < 0 || mCorrectPoint.y >= img.rows - 1) || (mCorrectPoint.x < 0 || mCorrectPoint.x >= img.cols - 1))
                continue;
            src_a = cvPoint((int) mCorrectPoint.x, (int) mCorrectPoint.y);
            src_b = cvPoint(src_a.x + 1, src_a.y);
            src_c = cvPoint(src_a.x, src_a.y + 1);
            src_d = cvPoint(src_a.x + 1, src_a.y + 1);
            distance_to_a_x = mCorrectPoint.x - src_a.x;    // 在原图像中与a点的水平距离
            distance_to_a_y = mCorrectPoint.y - src_a.y;    // 在原图像中与a点的垂直距离

            drcimg.at<Vec3b>(row, cols)[0] =
                img.at<Vec3b>(src_a.y, src_a.x)[0] * (1 - distance_to_a_x) * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_b.y, src_b.x)[0] * distance_to_a_x * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_c.y, src_c.x)[0] * distance_to_a_y * (1 - distance_to_a_x) +
                img.at<Vec3b>(src_d.y, src_d.x)[0] * distance_to_a_y * distance_to_a_x;
            drcimg.at<Vec3b>(row, cols)[1] =
                img.at<Vec3b>(src_a.y, src_a.x)[1] * (1 - distance_to_a_x) * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_b.y, src_b.x)[1] * distance_to_a_x * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_c.y, src_c.x)[1] * distance_to_a_y * (1 - distance_to_a_x) +
                img.at<Vec3b>(src_d.y, src_d.x)[1] * distance_to_a_y * distance_to_a_x;
            drcimg.at<Vec3b>(row, cols)[2] =
                img.at<Vec3b>(src_a.y, src_a.x)[2] * (1 - distance_to_a_x) * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_b.y, src_b.x)[2] * distance_to_a_x * (1 - distance_to_a_y) +
                img.at<Vec3b>(src_c.y, src_c.x)[2] * distance_to_a_y * (1 - distance_to_a_x) +
                img.at<Vec3b>(src_d.y, src_d.x)[2] * distance_to_a_y * distance_to_a_x;
        }
    }
    printf("时间花费%dms\n", clock() - time);   // 修改前200左右,修改成2.0版本的变量之后在210左右
    // cv::imwrite("矫正完成.bmp", drcimg);
    cv::imshow("矫正完成", drcimg);
    cv::waitKey(0);
    img.release();
    drcimg.release();
}

static void testByMySelf(void)
{
    int i = 0, j = 0;
    double x = 0, y = 0, r = 0, c = 0;
    double x1 = 0, y1 = 0;
    Mat image, imageDst;

    image = imread("1.png");
    imageDst = Mat();

    cout << "image.type: " << image.type() << endl;
    cout << "rows: " << image.rows << endl;
    cout << "cols: " << image.cols << endl;

    if (image.type() == CV_8UC1)
        cout << "image.type: CV_8UC1" << endl;
    else if (image.type() == CV_8UC2)
        cout << "image.type: CV_8UC2" << endl;
    else if (image.type() == CV_8UC3)
        cout << "image.type: CV_8UC3" << endl;

    imageDst.create(image.rows, image.cols, CV_8UC3);
    for (i = 0; i < image.rows / 2; i++) {
        for (j = 0; j < image.cols / 2; j++) {
            x = abs(i - 376);
            y = abs(j - 240);
            r = sqrt(pow(x, 2) + pow(y, 2));
            x1 = i * (0.43344802 + 0.01937733 * r);
            y1 = j * (0.43344802 + 0.01937733 * r);

            cout << "x: " << x << ", x1: " << x1 << endl;
            cout << "y: " << y << ", y1: " << y1 << endl;
            cout << "r: " << r << endl;
            cout << "--------" << endl;
            if ((x1 <= 376 * 2) && (y1 <= 240 * 2)) {
                imageDst.at<Vec3b>(x1, y1)[0] = image.at<Vec3b>(i, j)[0];
                imageDst.at<Vec3b>(x1, y1)[1] = image.at<Vec3b>(i, j)[1];
                imageDst.at<Vec3b>(x1, y1)[2] = image.at<Vec3b>(i, j)[2];
            }
        }
    }

    namedWindow("corre_before");
    imshow("corre_before", image);
    namedWindow("corre_after");
    imshow("corre_after", imageDst);

    waitKey(0);
}

// 桶形畸变校验
int main(int argc, char *argv[])
{
    cout << "使用方法: cd test/images && ../../build/corr_barrel_distortion" << endl;
    testByWeiBo();

    return 0;
}


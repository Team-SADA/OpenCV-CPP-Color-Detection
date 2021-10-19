#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

#define var auto
#define blank ' '
#define endl '\n'

signed main(int argv, char* args[]) {
	int PM;
	if(argv >= 2)
		PM = atoi(args[1]);
	else PM = 20;
	int R = 0xFF, G = 0x00, B = 0x00;

	if(argv >= 5){
		R = atoi(args[2]);
		G = atoi(args[3]);
		B = atoi(args[4]);
	}
	Mat detectColorRGB(1, 1, CV_8UC3, Scalar(B, G, R)), hsvColor;
	cvtColor(detectColorRGB, hsvColor, COLOR_BGR2HSV);
	auto t = hsvColor.at<Vec3b>(0, 0);
	int hue = t[0], sturation = t[1], value = t[2];
	int low_hue = hue - PM, high_hue = hue + PM, low_hue1 = hue - PM, low_hue2 = 0, high_hue1 = hue + PM, high_hue2 = 0, range_count = 1;
	if(low_hue < PM) {
		range_count = 2;
		high_hue1 = 180;
		low_hue1 = low_hue + 180;
		high_hue2 = high_hue;
		low_hue2 = 0;
	}
	else if(high_hue > 180 - PM) {
		range_count = 2;
		high_hue1 = low_hue;
		low_hue1 = 180;
		high_hue2 = high_hue - 180;
		low_hue2 = 0;
	}
	
	VideoCapture cap(0);
	Mat img, img_hsv;
	while(true){
		cap >> img;
		cvtColor(img, img_hsv, COLOR_BGR2HSV);
		Mat imgMask1, imgMask2;
		inRange(img_hsv, Scalar(low_hue1, 50, 50), Scalar(high_hue1, 255, 255), imgMask1);
		if(range_count == 2) {
			inRange(img_hsv, Scalar(low_hue2, 50, 50), Scalar(high_hue2, 255, 255), imgMask2);
			imgMask1 |= imgMask2;
		}
		erode(imgMask1, imgMask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgMask1, imgMask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgMask1, imgMask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgMask1, imgMask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Mat imgLabels, stats, centroids;
		int numOfLabels = connectedComponentsWithStats(imgMask1, imgLabels, stats, centroids, 8, CV_32S);

		int max = -1, idx = 0;
		for(int j = 1; j < numOfLabels; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			if(max < area) {
				max = area;
				idx = j;
			}
		}
		int left = stats.at<int>(idx, CC_STAT_LEFT), top = stats.at<int>(idx, CC_STAT_TOP), width = stats.at<int>(idx, CC_STAT_WIDTH), height = stats.at<int>(idx, CC_STAT_HEIGHT);

		rectangle(img, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 1);

		imgMask1 = 255 - imgMask1;
		Mat imgMask;
		Mat masks[] = {imgMask1, imgMask1, imgMask1};
		merge(masks, 3, imgMask);
		imshow("3", imgMask & img);
		
		if(waitKey(25) == 27) break;
	}
	return 0;
}

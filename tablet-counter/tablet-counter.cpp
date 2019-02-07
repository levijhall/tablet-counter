// tablet-counter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

using namespace cv;
using namespace std;

//Global Variables
int shift_slider_max = 180;
int shift_slider = 0;
int shift_slider_old = shift_slider;

int hue_slider_max = 22;
int hue_slider_min = 13;

int satration_slider_max = 118;
int satration_slider_min = 68;

int value_slider_max = 182;
int value_slider_min = 119;

//file paths
#define INPUT_FILE		"tablets3.jpg"
#define OUT_FOLDER_PATH string("")

//image store
Mat src, tmp, hsv, mask;
RNG rng(12345);

void on_trackbar(int, void*)
{
	if (shift_slider != shift_slider_old)
	{
		for (int i = 0; i < mask.rows; i++)
		{
			for (int j = 0; j < mask.cols; j++)
			{
				signed short h = hsv.at<Vec3b>(i, j)[0];
				signed short h_plus_shift = h;
				h_plus_shift += shift_slider - shift_slider_old;

				if (h_plus_shift < 0)
					h = 180 + h_plus_shift;
				else if (h_plus_shift > 180)
					h = h_plus_shift - 180;
				else
					h = h_plus_shift;

				hsv.at<Vec3b>(i, j)[0] = static_cast<unsigned char>(h);
			}
		}
		shift_slider_old = shift_slider;
	}

	Scalar ORANGE_MIN = Scalar(hue_slider_min, satration_slider_min, value_slider_min);
	Scalar ORANGE_MAX = Scalar(hue_slider_max, satration_slider_max, value_slider_max);

	inRange(hsv, ORANGE_MIN, ORANGE_MAX, mask);
	imshow("Ranged HSV", mask);
}

int main(int argc, char *argv[])
{
	src = imread(INPUT_FILE);
	pyrDown(src, tmp);
	pyrDown(tmp, src);

	cvtColor(src, hsv, COLOR_BGR2HSV);
	
	namedWindow("Ranged HSV");
	namedWindow("Original");
	namedWindow("Drawing");
	resizeWindow("Ranged HSV", src.size().width / 4, src.size().height / 4);
	resizeWindow("Original", src.size().width / 4, src.size().height / 4);
	resizeWindow("Drawing", src.size().width / 4, src.size().height / 4);

	createTrackbar("Hue shift", "Ranged HSV", &shift_slider, 180, on_trackbar);
	createTrackbar("Hue Min", "Ranged HSV", &hue_slider_min, 180, on_trackbar);
	createTrackbar("Hue Max", "Ranged HSV", &hue_slider_max, 180, on_trackbar);
	createTrackbar("Satration Min", "Ranged HSV", &satration_slider_min, 255, on_trackbar);
	createTrackbar("Satration Max", "Ranged HSV", &satration_slider_max, 255, on_trackbar);
	createTrackbar("Value Min", "Ranged HSV", &value_slider_min, 255, on_trackbar);
	createTrackbar("Value Max", "Ranged HSV", &value_slider_max, 255, on_trackbar);

	imshow("Original", src);

	on_trackbar(0, nullptr);

	vector<pair<double, double>> m;
	double medianLength;
	double medianArea;

	int time = 0;
	while (1)
	{
		waitKey();
		m.clear();

		printf("----------------------\n");
		printf("Hue (%d, %d)\n", hue_slider_min, hue_slider_max);
		printf("Satration (%d, %d)\n", satration_slider_min, satration_slider_max);
		printf("Value: (%d, %d)\n", value_slider_min, value_slider_max);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE); //RETR_TREE

		int count = 0;
		Mat drawing = Mat::zeros(src.size(), CV_8UC3);
		for (size_t i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			double length = arcLength(contours[i], 0);
			double area = contourArea(contours[i], 0);
			if (area / length > 1.5)
			{
				drawContours(drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
				drawContours(src, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
				m.push_back(make_pair(length, area));
				count++;
			}
		}

		if (!m.empty())
		{
			sort(m.begin(), m.end());
			if (count % 2 == 0)
			{
				medianLength = (m[m.size() / 2 - 1].first + m[m.size() / 2].first) / 2;
				medianArea = (m[m.size() / 2 - 1].second + m[m.size() / 2].second) / 2;
			}
			else
			{
				medianLength = m[m.size() / 2].first;
				medianArea = m[m.size() / 2].second;
			}
			printf("median: (%f, %f)\n", medianLength, medianArea);
		}

		int i = 0;
		double medLengthCount = 0;
		double medAreaCount = 0;
		for (auto t = m.begin(); t != m.end(); t++)
		{
			double l = (*t).first / medianLength;
			l = (l < 1 ? 1 : round(l));
			double a = (*t).second / medianArea;
			a = (a < 1 ? 1 : round(a));

			printf("%d: (%f, %f) -> (%f, %f)\n", ++i, (*t).first, (*t).second, l, a);
			medLengthCount += l;
			medAreaCount += a;
		}

		printf("Count: %d\n", count);
		printf("Median Length Count: %f\n", medLengthCount);
		imshow("Drawing", drawing);
		imshow("Original", src);
	}

	destroyAllWindows();
	return 0;
}
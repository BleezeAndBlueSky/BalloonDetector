#include<iostream>
#include<opencv2/opencv.hpp>
//#pragma warning(disable:4996)
//#define DEBUG
//#define TRAIN

using namespace std;
using namespace cv;

class BalloonDetector {
	Mat src_;
	Mat srcHsv_;
	Mat sample_;
	//Scalar color_;
	vector<vector<Point>> contours_;
    vector<vector<Point>> contours_fine_;
	vector<Vec4i> hierarcy;
	//vector<Point> center_template;
	Point center_;
	int index = -1;

	vector<Mat> balloon_template_;

	//vector<cv::RotatedRect> roundcontours;

	Mat convertTo3Channels(const Mat& binImg)
	{
		Mat three_channel = Mat::zeros(binImg.rows, binImg.cols, CV_8UC3);
		vector<Mat> channels;
		for (int i = 0; i < 3; i++)
		{
			channels.push_back(binImg);
		}
		merge(channels, three_channel);
		return three_channel;
	}

	void matchContours() {
        if (index > -1 && contours_.size() > 0) {
			for (int i = 0; i < contours_fine_.size(); ++i) {
				cout << matchShapes(contours_fine_[i], contours_[index], CONTOURS_MATCH_I1, 1) << endl;
                if (matchShapes(contours_fine_[i], contours_[index], CONTOURS_MATCH_I1, 1) < 0.06) {

                    Rect frame;
                    frame = boundingRect(contours_[index]);
                    if (frame.width < frame.height*1.8&&frame.height < frame.width*1.8)
                        rectangle(src_, Point(frame.x, frame.y), Point(frame.x + frame.width, frame.y + frame.height), Scalar(255, 255, 255), 2);
                    center_.x = (frame.x + frame.width) / 2;
                    center_.y = (frame.y + frame.height) / 2;

                    cout << "OJBK" << endl;
					index = -1;
					return;
				}
			}
		}
		center_.x = 0;
		center_.y = 0;
		index = -1;
		cout << "GG" << endl;
	}

	void combineCAC() {
		Mat temp = convertTo3Channels(sample_);
		Mat gray = Mat::zeros(src_.size(), CV_8UC1);
		//threshold(temp, gray, 125, 255, THRESH_BINARY);
		cvtColor(temp, gray, COLOR_BGR2GRAY);
		findContours(gray, contours_, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//for (int i = 0; i < contours_.size(); ++i) {
		//	drawContours(src_, contours_, i, Scalar(0, 0, 0), 1, 8);
		//}
#ifdef DEBUG
        imshow("2", sample_);
		waitKey(3);
#endif // DEBUG
	}

	//void roundContours() {
	//	for (int i = 0; i < contours.size(); ++i) {
	//		int count = contours[i].size(); 
	//		if (count > 100)   
	//			roundcontours.push_back(fitEllipse(contours[i]));
	//	}

	//	for (int i = 0; i < roundcontours.size(); ++i) {
	//		if (MAX(roundcontours[i].size.width, roundcontours[i].size.height) > MIN(roundcontours[i].size.width, roundcontours[i].size.height) * 1.2)
	//			continue;
	//		ellipse(src_, roundcontours[i], Scalar(255, 255, 255));
	//	}

	//	imshow("4", src_);
	//	waitKey(0);
	//}

	void matchArea() {
		if (!contours_.empty()) {
			int biggest = contourArea(contours_[0]);

			for (int i = 0; i < contours_.size(); ++i) {
				if (contourArea(contours_[i]) > 500 && contourArea(contours_[i]) < 100000) {
					if (biggest <= contourArea(contours_[i])) {
						biggest = contourArea(contours_[i]);
						index = i;
					}
				}
				else {
					continue;
				}
			}
		}

#ifdef DEBUG
		imshow("3", src_);
		waitKey(0);
#endif // DEBUG
	}

	void matchColor() {
        inRange(srcHsv_, Scalar(115, 30, 46), Scalar(150, 255, 255), sample_);
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(sample_, sample_, MORPH_OPEN, element);
		morphologyEx(sample_, sample_, MORPH_CLOSE, element);

		//imshow("12", sample_);
		//imshow("4", srcHsv_);
		//waitKey(1);
	}

public:
	BalloonDetector() {
		//src_ = src;
		//cvtColor(src_, srcHsv_, COLOR_BGR2HSV);
	}
	~BalloonDetector(){}

	void loadimg(Mat src) {
		src_ = src;
		cvtColor(src_, srcHsv_, COLOR_BGR2HSV);
	}

    bool storeContours(int template_num) {
		for (int i = 0; i < template_num; ++i) {
			char address[100];
            sprintf(address, "/home/peterbaker/projects/balloons/balloons1/%d.JPG", i + 1);
            Mat img = imread(address);
            if(img.empty()){
                cout<<"load image failed! Check file path!"<<endl;
                return false;
            }
			balloon_template_.push_back(img);
		}

		//balloon_template_ = imread("C:\\Users\\lenovo\\Desktop\\6.jpg");

		for (int i = 0; i < template_num; ++i) {
			cvtColor(balloon_template_[i], balloon_template_[i], COLOR_BGR2HSV);
			Mat sample__;
			inRange(balloon_template_[i], Scalar(24, 120, 46), Scalar(35, 255, 255), sample__);//------***********************
			Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
			morphologyEx(sample__, sample__, MORPH_OPEN, element);
			morphologyEx(sample__, sample__, MORPH_CLOSE, element);

			Mat temp = convertTo3Channels(sample__);
			Mat gray = Mat::zeros(balloon_template_[i].size(), CV_8UC1);
			//threshold(temp, gray, 125, 255, THRESH_BINARY);
			cvtColor(temp, gray, COLOR_BGR2GRAY);

			//imshow("1", gray);
			//waitKey(0);

			vector<vector<Point>> contours_template_;
			findContours(gray, contours_template_, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			//cout << contours_template_.size() << endl;
			for (int j = 0; j < contours_template_.size(); ++j) {
				drawContours(balloon_template_[i], contours_template_, j, Scalar(0, 0, 0), 1, 8);
			}

			int index1 = -1;
			if (contours_template_.size() > 0) {
				int biggest = contourArea(contours_template_[0]);

				for (int i = 0; i < contours_template_.size(); ++i) {
					if (contourArea(contours_template_[i]) > 500 && contourArea(contours_template_[i]) < 100000) {
						if (biggest <= contourArea(contours_template_[i])) {
							biggest = contourArea(contours_template_[i]);
							index1 = i;
						}
					}
					else {
						continue;
					}
				}

				if (index1 >= 0) {
					Rect frame;
					frame = boundingRect(contours_template_[index1]);
					if (frame.width < frame.height*1.8 && frame.height < frame.width*1.8)
						rectangle(balloon_template_[i], Point(frame.x, frame.y), Point(frame.x + frame.width, frame.y + frame.height), Scalar(255, 255, 255), 2);
					//center_template[i].x = (frame.x + frame.width) / 2;
					//center_template[i].y = (frame.y + frame.height) / 2;
					contours_fine_.push_back(contours_template_[index1]);
				}

				drawContours(balloon_template_[i], contours_template_, index1, Scalar(255, 0, 0), 1, 8);
			}
#ifdef TRAIN
			//imshow("1", balloon_template_[i]);
			//waitKey(0);
			char name[20];
			sprintf(name, "C:\\Users\\lenovo\\Desktop\\balloonss\\%d.jpg", i + 1);
			imwrite(name, balloon_template_[i]);
			
#endif // DEBUG
            return true;
		}
	}

	Point Calculate() {
		matchColor();
		combineCAC();
		matchArea();
		matchContours();
		return Point(center_.x, center_.y);
	}

	//Point Result() {
	//	return Point(center_.x, center_.y);
	//}
};

int main() {
    VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return -1;
	}
	Mat img;
	//namedWindow("12");
	//imshow("121", img);
	
	BalloonDetector A;
    if(!A.storeContours(12)){
        return -1;
    }

	bool stop = false;
	while (!stop)
	{
		cap >> img;
		A.loadimg(img);
		A.Calculate();
		imshow("当前视频", img);
		if (waitKey(30) >= 0)
			stop = true;
	}
	//double start = static_cast<double>(getTickCount());

	//double time = ((double)getTickCount() - start) / getTickFrequency();
	//cout << "所用时间为：" << time << "秒" << endl;
	
	//waitKey(1);
	//system("pause");
	return 0;
}

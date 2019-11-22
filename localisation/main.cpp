#include <opencv2/opencv.hpp>
#include <iostream>
#include "ParticleFilter.h"

using namespace std;
using namespace cv;

void localizeRobot(Mat &workspace, Point initBelief, double initTheta)
{
	ParticleFilter filter(workspace);
	double sigma_pos[3] = { 0.03, 0.03, 0.01 };
	double dt = 0.1;
	double lin_vel = 0.0;
	double the_vel = 0.0;
	filter.initParticles(initBelief, initTheta, sigma_pos, 1);
	while (true)
	{
		filter.prediction(dt, sigma_pos, lin_vel, the_vel);
		filter.showParticles(workspace);
	}

}

int main()
{
	const int scale = 1;

	Mat workspaceTmp = cv::imread("maps/smallworld.png", cv::IMREAD_GRAYSCALE);
	Mat workspace;
	resize(workspaceTmp, workspace, Size(workspaceTmp.cols*scale*10/1.41735, workspaceTmp.rows*scale*10/1.41735), 0, 0, cv::INTER_NEAREST);

	Mat workspaceTmpBGR = cv::imread("maps/smallworld.png", cv::IMREAD_COLOR);
	Mat workspaceBGR;
	resize(workspaceTmpBGR, workspaceBGR, Size(workspaceTmpBGR.cols * scale * 10 / 1.41735, workspaceTmpBGR.rows * scale * 10 / 1.41735), 0, 0, cv::INTER_NEAREST);

	Point initPoint(workspace.cols/2, workspace.rows/2);
	double initTheta = 1.0;
	localizeRobot(workspace, initPoint, initTheta);

	imshow("image", workspace);
	cv::waitKey(0);
	return 0;
}

#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "Particle.h"
#include <numeric>
#include <random>
using namespace std;

class ParticleFilter
{
public:
	ParticleFilter();
	ParticleFilter(cv::Mat workspace);

	void initParticles(cv::Point startP, double startOri, double sigma_pos[], int N);
	void prediction(double dt, double sigma_pos[], double vlin, double vth);
	void updateWeights(double ranges[]);
	void showParticles(cv::Mat& workspace, int rx, int ry);
	void resample();

	~ParticleFilter();
private:
	std::vector<Particle> _particles;
	cv::Mat _workspace;
	double _normal_pdf(double x, double m, double s);
};

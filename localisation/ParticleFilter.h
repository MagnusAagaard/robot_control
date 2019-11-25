#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "Particle.h"
#include <numeric>
#include <random>
class ParticleFilter
{
public:
	ParticleFilter();
	ParticleFilter(cv::Mat workspace);

	void initParticles(cv::Point startP, double startOri, double sigma_pos[], int N);
	void prediction(double dt, double sigma_pos[], double vlin, double vth);
	void updateWeights(double ranges[]);
	void showParticles(cv::Mat& workspace);

	~ParticleFilter();
private:
	std::vector<Particle> _particles;
	cv::Mat _workspace;
};

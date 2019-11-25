#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
class Particle
{
public:
	Particle();
	Particle(int id);

	double x;
	double y;
	double theta;
	double weight;
	std::vector<double> _generated_distances;
	const int getId() const { return _id; }
	void generateDistances(cv::Mat& workspace);

	~Particle();
private:
	int _id;
	double _calcDistance(cv::Point a, cv::Point b);
};

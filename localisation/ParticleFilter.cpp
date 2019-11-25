#include "ParticleFilter.h"



ParticleFilter::ParticleFilter()
{
}

ParticleFilter::ParticleFilter(cv::Mat workspace)
{
	workspace.copyTo(_workspace);
	//_workspace = workspace;
}


ParticleFilter::~ParticleFilter()
{
}

void ParticleFilter::initParticles(cv::Point startP, double startOri, double sigma_pos[], int N)
{
	std::default_random_engine gen;
	std::normal_distribution<double> dist_x(startP.x, sigma_pos[0]);	//normal dist from x and its std.
	std::normal_distribution<double> dist_y(startP.y, sigma_pos[1]);
	std::normal_distribution<double> dist_theta(startOri, sigma_pos[2]);

	for (int i = 0; i < N; i++)
	{
		double sample_x = dist_x(gen);			//Get random samples along the dist. created earlier
		double sample_y = dist_y(gen);
		double sample_theta = dist_theta(gen);

		Particle par(i);
		par.x = sample_x;
		par.y = sample_y;
		par.theta = sample_theta;
		par.weight = 1.0;
		_particles.push_back(par);
	}
	for (int i = 0; i < _particles.size(); i++)
	{
		_particles[i].generateDistances(_workspace);
	}

	/*std::cout << "Init done: " << _particles.size() << std::endl;
	for (int i = 0; i < _particles.size(); i++)
	{
		std::cout << _particles[i].x << ", " << _particles[i].y << std::endl;
	}*/
}

void ParticleFilter::prediction(double dt, double sigma_pos[], double vlin, double vth)
{
	std::default_random_engine gen;
	//Create noise
	std::normal_distribution<double> noise_dist_x(0, sigma_pos[0]);
	std::normal_distribution<double> noise_dist_y(0, sigma_pos[1]);
	std::normal_distribution<double> noise_dist_theta(0, sigma_pos[2]);
	for (int i = 0; i < _particles.size(); i++)
	{
		double prev_theta = _particles[i].theta;
		if (abs(vth) > 0.0001)	//avoid dividing by 0
		{
			_particles[i].x += (vlin / vth)*(sin(prev_theta + (vth*dt)) - sin(prev_theta));
			_particles[i].y += (vlin / vth)*(cos(prev_theta) - cos(prev_theta + (vth*dt)));
		}
		else
		{
			_particles[i].x += vth * dt * cos(prev_theta);
			_particles[i].y += vth * dt*sin(prev_theta);
		}
		_particles[i].theta = prev_theta + vth * dt;
		//Add noise
		_particles[i].x += noise_dist_x(gen);
		_particles[i].y += noise_dist_y(gen);
		_particles[i].theta += noise_dist_theta(gen);
	}
}

void updateWeights(double ranges[])
{
	for(int i = 0; i < _particles.size(); i++)
	{
		for(int k = 0; k < _particles[i]._generated_distances.size(); k++)
		{
			float prob = normal_pdf(ranges[k], _particles[i]._generated_distances[k], 0.03*10)
		}
	}
	//make gaus dist for each particles gen. distance with mean true dist and sigma from laser scanner
	//use pdf to calc prob for laser scan being part of this particle. sammenlign med hver partikkel, find den der har højest prob
	//resample??

}

double normal_pdf(double x, double m, double s)
{
    static const double inv_sqrt_2pi = 0.3989422804014327;
    double a = (x - m) / s;

    return inv_sqrt_2pi / s * std::exp(-0.5f * a * a);
}

void ParticleFilter::showParticles(cv::Mat& workspace)
{
	_workspace.copyTo(workspace);
	for (int i = 0; i < _particles.size(); i++)
	{
		workspace.at<uchar>(_particles[i].y, _particles[i].x) = 0;
	}
	cv::imshow("Particles", workspace);
	cv::waitKey(1);
}

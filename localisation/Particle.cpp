#include "Particle.h"

Particle::Particle()
{
}

Particle::Particle(int id)
{
	_id = id;
}

void Particle::generateDistances(cv::Mat& workspace)
{//lav true distances d ud fra kortet. genetared_distances[0] = l�ngst til venstre
	_generated_distances.clear();
	int rayx = x;
	int rayy = y;
	double dtheta = 2 * 2.268899 / 100;
	double raytheta = theta - 2.268899;	//forskyd 5 gange bagud?
	//raytheta += dtheta;
	for (int i = 0; i < 100; i++)
	{
		bool wallFound = false;
		rayx = x + 100 * cos(raytheta);
		rayy = y - 100 * sin(raytheta);
		//std::cout << "Start: " << x << ", " << y << std::endl;
		//std::cout << "Ray: " << rayx << ", " << rayy << std::endl;
		//cv::circle(workspace, cv::Point(x, y), 5, (1, 1, 1));
		cv::LineIterator li(workspace, cv::Point(x, y), cv::Point(rayx,rayy));
		//std::cout << li.pos() << std::endl;
		for (int k = 0; k < li.count; k++, ++li)
		{
			if (workspace.at<uchar>(li.pos()) == 0)
			{
				//cv::circle(workspace, cv::Point(li.pos()), 3, (1, 1, 1));
				double dist = _calcDistance(cv::Point(x, y), cv::Point(li.pos()));
				_generated_distances.push_back(dist);
				wallFound = true;
				break;
			}
		}
		if (!wallFound)
			_generated_distances.push_back(100.0);

		raytheta += dtheta;
	}
	raytheta = theta - 2.268899;
	/*for(int i = 0; i < _generated_distances.size(); i++)
	{
		std::cout << "Angle: " << raytheta + dtheta*i << ", GD: " << _generated_distances[i] << std::endl;
	}*/
	//cv::imshow("distance", workspace);
	//cv::waitKey(0);
}

Particle::~Particle()
{
}

double Particle::_calcDistance(cv::Point a, cv::Point b)
{
	return ((double)sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))));
}

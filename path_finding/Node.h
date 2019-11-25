#pragma once
#include <opencv2/opencv.hpp>
class Node
{
public:
	Node();
	Node(cv::Point p);
	~Node();

	Node* getParent();
	void setParent(Node*);
	cv::Point getPoint();
	void setPoint(cv::Point);
	int gethCost();
	void sethCost(int);
	int getgCost();
	void setgCost(int);
	int getfCost();

	int getDistance(Node *b);
	bool traversable(cv::Mat &workspace);

private:
	Node* parent;
	cv::Point point;
	int hCost;
	int gCost;
	int fCost;
};


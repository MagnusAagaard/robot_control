#include "Node.h"

Node::Node()
{
}

Node::Node(cv::Point tmpP)
{
	point = tmpP;
	hCost = 0;
	gCost = 0;
	fCost = 0;
}

Node::~Node()
{
}

Node* Node::getParent()
{
	return parent;
}

void Node::setParent(Node* n)
{
	parent = n;
}

cv::Point Node::getPoint()
{
	return point;
}

void Node::setPoint(cv::Point p)
{
	point = p;
}

int Node::gethCost()
{
	return hCost;
}

void Node::sethCost(int cost)
{
	hCost = cost;
}

int Node::getgCost()
{
	return gCost;
}

void Node::setgCost(int cost)
{
	gCost = cost;
}

int Node::getfCost()
{
	return (gCost + hCost);
}

int Node::getDistance(Node *b)
{
	int dstX = abs(this->getPoint().x - b->getPoint().x);
	int dstY = abs(this->getPoint().y - b->getPoint().y);
	if (dstX > dstY)
		return (14 * dstY + 10 * (dstX - dstY));
	return (14 * dstX + 10 * (dstY - dstX));
}

bool Node::traversable(cv::Mat &workspace)
{
	if (workspace.at<uchar>(this->getPoint()) == 0)
		return false;
	return true;
}
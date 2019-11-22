#include <opencv2/opencv.hpp>
#include<iostream>
#include "Node.h"
#include <chrono>

using std::cout;
using std::endl;
using std::vector;
using cv::Mat;
using cv::Point;
using cv::Size;

bool inList(vector<Node*> &list, Node *node)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i]->getPoint() == node->getPoint())
			return true;
	}
	return false;
}

vector<Node*> getNeighbours(Node *currentNode, vector<Node*> &open, vector<Node*> &closed)
{
	vector<Node*> neighbours;
	bool found = false;
	Node* node;

	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			int currentX = currentNode->getPoint().x;
			int currentY = currentNode->getPoint().y;
			Point newPoint(currentX + x, currentY + y);
			for (int i = 0; i < open.size(); i++) {
				if (open[i]->getPoint() == newPoint) {
					found = true;
					neighbours.push_back(open[i]);
					break;
				}
			}
			if (!found)
			{
				for (int i = 0; i < closed.size(); i++) {
					if (closed[i]->getPoint() == newPoint)
					{
						found = true;
						break;
					}
				}
			}
			
			if (!found) {
				node = new Node(newPoint);
				neighbours.push_back(node);
			}
			found = false;
		}
	}
	return neighbours;
}

void retracePath(Node &start, Node* goal, vector<Point> &path)
{
	vector<Point> tmpPath;
	Node* currentNode = goal;
	while (currentNode->getPoint() != start.getPoint())
	{
		tmpPath.push_back(currentNode->getPoint());
		currentNode = currentNode->getParent();
	}
	tmpPath.push_back(currentNode->getPoint());
	for (int i = 0; i < tmpPath.size(); i++)
	{
		path.push_back(tmpPath[tmpPath.size()-1 - i]);
	}
}

void findPath(Mat &workspace, Mat &workspaceBGR, vector<Point> &path, Node &start, Node &goal, bool showPath)
{
	auto startTime = std::chrono::high_resolution_clock::now();
	vector<Node*> open;
	vector<Node*> closed;
	Node* currentNode;
	currentNode = new Node(start);
	open.push_back(currentNode);
	
	while (open.size())
	{
		if (showPath)
		{
			for (int i = 0; i < open.size(); i++)
			{
				workspaceBGR.at<cv::Vec3b>(open[i]->getPoint()) = cv::Vec3b(0, 255, 0);
			}
		}
		
		int currentPosIndex = 0;
		currentNode = open[0];
		for (int i = 1; i < open.size(); i++) {
			if (open[i]->getfCost() < currentNode->getfCost() || (open[i]->getfCost() == currentNode->getfCost() && open[i]->gethCost() < currentNode->gethCost())) {
				currentNode = open[i];
				currentPosIndex = i;
			}
		}
		closed.push_back(currentNode);
		open.erase(open.begin()+currentPosIndex);
		
		if (showPath)
		{
			workspaceBGR.at<cv::Vec3b>(currentNode->getPoint()) = cv::Vec3b(0, 0, 255);
			cv::imshow("A* path", workspaceBGR);
			cv::waitKey(1);
		}

		if (currentNode->getPoint() == goal.getPoint()) {
			auto finishTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finishTime - startTime;
			std::cout << "Elapsed time: " << elapsed.count() << " s\n";
			retracePath(start, currentNode, path);
			return;
		}
		vector<Node*> neighbours = getNeighbours(currentNode, open, closed);

		for (int i = 0; i < neighbours.size(); i++)
		{
			if (neighbours[i]->traversable(workspace))
			{
				int newMovementCostToNeighbour = currentNode->getgCost() + currentNode->getDistance(neighbours[i]);
				if (newMovementCostToNeighbour < neighbours[i]->getgCost() || !inList(open,neighbours[i]))
				{
					neighbours[i]->setgCost(newMovementCostToNeighbour);
					neighbours[i]->sethCost(neighbours[i]->getDistance(&goal));
					neighbours[i]->setParent(currentNode);

					if (!inList(open, neighbours[i]))
						open.push_back(neighbours[i]);
				}
			}
		}

	}

}

int main()
{
	const int scale = 2;

	Mat workspaceTmp = cv::imread("smallworld.png", cv::IMREAD_GRAYSCALE);
	Mat workspace;
	resize(workspaceTmp, workspace, Size(workspaceTmp.cols*scale*10/1.41735, workspaceTmp.rows*scale*10/1.41735), 0, 0, cv::INTER_NEAREST);

	Mat workspaceTmpBGR = cv::imread("smallworld.png", cv::IMREAD_COLOR);
	Mat workspaceBGR;
	resize(workspaceTmpBGR, workspaceBGR, Size(workspaceTmpBGR.cols * scale * 10 / 1.41735, workspaceTmpBGR.rows * scale * 10 / 1.41735), 0, 0, cv::INTER_NEAREST);

	Point startp(scale * 40, scale * 40);
	Point endp(scale * 110, scale * 20);
	Node start(startp);
	Node goal(endp);
	cv::circle(workspaceBGR, endp, 4, (0, 0, 255));
	cv::circle(workspaceBGR, startp, 4, (0,0,0));

	vector<Point> path;
	findPath(workspace, workspaceBGR, path, start, goal, true);
	cv::Vec3b color(0, 0, 255);
	cv::cvtColor(workspace, workspace, cv::COLOR_GRAY2BGR);
	cv::circle(workspace, startp, 4, (0, 0, 0));
	cv::circle(workspace, endp, 4, (0, 0, 255));
	for (int i = 0; i < path.size(); i++)
	{
		workspace.at<cv::Vec3b>(path[i]) = color;
		cout << path[i] << endl;
	}

	imshow("image", workspace);
	imwrite("smallworld_astar.png", workspace);
	cv::waitKey(0);
	return 0;
}
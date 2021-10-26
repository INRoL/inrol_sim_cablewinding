#pragma once
#include "precompiled.h"

void printmat4(glm::mat4 Matrix4)
{
	std::cout << Matrix4[0][0] << " " << Matrix4[0][1] << " " << Matrix4[0][2] << " " << Matrix4[0][3] << std::endl;
	std::cout << Matrix4[1][0] << " " << Matrix4[1][1] << " " << Matrix4[1][2] << " " << Matrix4[1][3] << std::endl;
	std::cout << Matrix4[2][0] << " " << Matrix4[2][1] << " " << Matrix4[2][2] << " " << Matrix4[2][3] << std::endl;
	std::cout << Matrix4[3][0] << " " << Matrix4[3][1] << " " << Matrix4[3][2] << " " << Matrix4[3][3] << std::endl;
}

void printvec4(glm::vec4 Vector4)
{
	std::cout << Vector4[0] << " " << Vector4[1] << " " << Vector4[2] << " " << Vector4[3] << std::endl;
}

Matrix3d rot(double th, char axis)
{
	Matrix3d R;
	if (axis == 'x')
		R << 1, 0, 0, 0, cos(th), -sin(th), 0, sin(th), cos(th);
	else if (axis == 'y')
		R << cos(th), 0, sin(th), 0, 1, 0, -sin(th), 0, cos(th);
	else
		R << cos(th), -sin(th), 0, sin(th), cos(th), 0, 0, 0, 1;

	return R;
}

void set_trajectory(const char* path, std::vector<double> &traj)
{
	std::ifstream fin(path);
	double n;
	while (!fin.eof())
	{
		fin >> n;
		traj.push_back(n);
	}
	fin.close();
}

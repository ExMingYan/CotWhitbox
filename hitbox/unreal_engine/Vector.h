#pragma once

struct FVector
{
	double X;
	double Y;
	double Z;

	FVector(double x = 0, double y = 0, double z = 0) : X(x), Y(y), Z(z) {}
	FVector(float x = 0, float y = 0, float z = 0) : X(static_cast<double>(x)), Y(static_cast<double>(y)), Z(static_cast<double>(z)) {}
};

struct FVector2D
{
	double X;
	double Y;
};

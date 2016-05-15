#pragma once

struct INT2
{
	INT2(int x, int y) : x(x), y(y) {}

	// Unused:
	//INT2 operator+(INT2 rhs) { return INT2(x + rhs.x, y + rhs.y); }
	//INT2 operator-(INT2 rhs) { return INT2(x - rhs.x, y - rhs.y); }
	//INT2 operator*(INT2 rhs) { return INT2(x * rhs.x, y * rhs.y); }
	//INT2 operator*(int scale) { return INT2(x * scale, y * scale); }
	//INT2 operator/(int factor) { return INT2(x / factor, y / factor); }
	//bool operator==(INT2 rhs) { return x == rhs.x && y == rhs.y; }
	//bool operator!=(INT2 rhs) { return !(*this == rhs); }

	int x;
	int y;
};

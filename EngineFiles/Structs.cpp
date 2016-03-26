//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"

#include "Structs.h"


//-----------------------------------------------------------------
// COLOR Constructors
//-----------------------------------------------------------------
COLOR::COLOR()
{}
COLOR::COLOR(unsigned char redVal, unsigned char greenVal, unsigned char blueVal, unsigned char alphaVal) : red(redVal), green(greenVal), blue(blueVal), alpha(alphaVal)
{}
//-----------------------------------------------------------------
// RECT2 Constructors
//-----------------------------------------------------------------
RECT2::RECT2() : left(0), top(0), right(0), bottom(0)
{}

RECT2::RECT2(double leftVal, double topVal, double rightVal, double bottomVal) : left(leftVal), top(topVal), right(rightVal), bottom(bottomVal)
{}


//-----------------------------------------------------------------
// DOUBLE2 Constructors, friend operators, operators, general methods
//-----------------------------------------------------------------
DOUBLE2::DOUBLE2() : x(0), y(0)
{}

DOUBLE2::DOUBLE2(double x, double y) : x(x), y(y)
{}

DOUBLE2::DOUBLE2(const D2D1_POINT_2F& d2dPoint) : x(d2dPoint.x), y(d2dPoint.y)
{}

DOUBLE2 operator*(double factor, const DOUBLE2& rightRef)
{
	return DOUBLE2(rightRef.x * factor, rightRef.y * factor);
}

std::ostream& operator<<(std::ostream& os, DOUBLE2 right)
{
	os << "DOUBLE2( " << right.x << ", " << right.y << " )";
	return os;
}

std::wostream& operator<<(std::wostream& wos, DOUBLE2 right)
{
	wos << L"DOUBLE2( " << right.x << L", " << right.y << L" )";
	return wos;
}

DOUBLE2 Lerp(const DOUBLE2& fromRef, const DOUBLE2& toRef, double lambda)
{
	return fromRef + (toRef - fromRef) * lambda;
}

DOUBLE2& DOUBLE2::operator=(const DOUBLE2& otherRef)
{
	if (&otherRef != this)			// check for self assignment
	{
		x = otherRef.x;
		y = otherRef.y;
	}

	return *this;
}

DOUBLE2& DOUBLE2::operator=(const D2D1_POINT_2F& otherRef)
{
	x = otherRef.x;
	y = otherRef.y;

	return *this;
}

DOUBLE2 DOUBLE2::operator+(const DOUBLE2& otherRef) const
{
	return DOUBLE2(x + otherRef.x, y + otherRef.y);
}

DOUBLE2 DOUBLE2::operator-(const DOUBLE2& otherRef) const
{
	return DOUBLE2(x - otherRef.x, y - otherRef.y);
}

DOUBLE2 DOUBLE2::operator-() const
{
	return DOUBLE2(-x, -y);
}

DOUBLE2 DOUBLE2::operator+() const
{
	return *this;
}

DOUBLE2 DOUBLE2::operator*(double factor) const
{
	return DOUBLE2(x * factor, y * factor);
}

DOUBLE2 DOUBLE2::operator/(double divisor) const
{
	return DOUBLE2(x / divisor, y / divisor);
}

DOUBLE2& DOUBLE2::operator+=(const DOUBLE2& otherRef)
{
	x += otherRef.x;
	y += otherRef.y;

	return *this;
}

DOUBLE2& DOUBLE2::operator-=(const DOUBLE2& otherRef)
{
	x -= otherRef.x;
	y -= otherRef.y;

	return *this;
}

DOUBLE2& DOUBLE2::operator*=(double factor)
{
	x *= factor;
	y *= factor;

	return *this;
}

DOUBLE2& DOUBLE2::operator/=(double divisor)
{
	x /= divisor;
	y /= divisor;

	return *this;
}

bool DOUBLE2::operator==(const DOUBLE2& otherRef) const
{
	return x == otherRef.x && y == otherRef.y;
}

bool DOUBLE2::operator!=(const DOUBLE2& otherRef) const
{
	return !(*this == otherRef);
}

bool DOUBLE2::Equals(const DOUBLE2& otherRef, double epsilon) const
{
	return abs(x - otherRef.x) < epsilon && abs(y - otherRef.y) < epsilon;
}

D2D1_POINT_2F DOUBLE2::ToPoint2F() const
{
	D2D1_POINT_2F p;
	p.x = (FLOAT)x;
	p.y = FLOAT(y);
	return p;
}

double DOUBLE2::DotProduct(const DOUBLE2& otherRef) const
{
	return x * otherRef.x + y * otherRef.y;
}

double DOUBLE2::Determinant(const DOUBLE2& otherRef) const
{
	return x * otherRef.y - y * otherRef.x;
}

String DOUBLE2::ToString() const
{
	return String("[") + String(x) + String(", ") + String(y) + String("]");
}

double DOUBLE2::Norm() const
{
	return Length();
}

double DOUBLE2::Length() const
{
	return sqrt(x * x + y * y);
}

double DOUBLE2::SquaredLength() const
{
	return x * x + y * y;
}

double DOUBLE2::AngleWith(const DOUBLE2& otherRef) const
{
	// Momenteel is er een klein probleem met de AngleWith functie. 
	// Momenteel geeft deze functie soms de grootste hoektussen de twee vectoren

	//double angleA = atan2(y, x);
	//double angleB = atan2(otherRef.y, otherRef.x);
	//return angleB - angleA;

	DOUBLE2 v1N = Normalized();
	DOUBLE2 v2N = otherRef.Normalized();
	double dp = v1N.DotProduct(v2N);
	double dAngle = acos(dp);

	// cross product (enkel z-waarde) 
	double up = x*otherRef.y - y*otherRef.x;
	if (up > 0)
		return dAngle;
	else
		return -dAngle;

}

DOUBLE2 DOUBLE2::Normalized(double epsilon) const
{
	double length = Length();
	if (length < epsilon) return DOUBLE2(0, 0);
	else return DOUBLE2(x / length, y / length);
}

DOUBLE2 DOUBLE2::Orthogonal() const
{
	return DOUBLE2(-y, x);
}

void DOUBLE2::Set(double newX, double newY)
{
	x = newX;
	y = newY;
}

//------------------------------------------------------------------------------
// MATRIX3X2 Constructors, friend operators, operators, methods, static methods
//------------------------------------------------------------------------------
MATRIX3X2::MATRIX3X2(DOUBLE2 dirX, DOUBLE2 dirY, DOUBLE2 orig) : dirX(dirX), dirY(dirY), orig(orig)
{}

MATRIX3X2::MATRIX3X2(double e1X, double e1Y, double e2X, double e2Y, double oX, double oY) : dirX(e1X, e1Y), dirY(e2X, e2Y), orig(oX, oY)
{}

MATRIX3X2::MATRIX3X2(const MATRIX3X2& sourceRef) : dirX(sourceRef.dirX), dirY(sourceRef.dirY), orig(sourceRef.orig)
{}

MATRIX3X2::MATRIX3X2(const Matrix3x2F& mat) : dirX(mat._11, mat._12), dirY(mat._21, mat._22), orig(mat._31, mat._32)
{}

MATRIX3X2 operator*(const MATRIX3X2& matrix1, const MATRIX3X2& matrix2)
{
	return MATRIX3X2(DOUBLE2(matrix1.dirX.x * matrix2.dirX.x + matrix1.dirX.y * matrix2.dirY.x, matrix1.dirX.x * matrix2.dirX.y + matrix1.dirX.y * matrix2.dirY.y),
		DOUBLE2(matrix1.dirY.x * matrix2.dirX.x + matrix1.dirY.y * matrix2.dirY.x, matrix1.dirY.x * matrix2.dirX.y + matrix1.dirY.y * matrix2.dirY.y),
		DOUBLE2(matrix1.orig.x * matrix2.dirX.x + matrix1.orig.y * matrix2.dirY.x + matrix2.orig.x, matrix1.orig.x * matrix2.dirX.y + matrix1.orig.y * matrix2.dirY.y + matrix2.orig.y));
}

std::ostream& operator<<(std::ostream& os, MATRIX3X2 matrix)
{
	os << "MATRIX3X2( " << matrix.dirX.x << ", " << matrix.dirX.y << ", " << matrix.dirY.x << ", " << matrix.dirY.y << ", " << matrix.orig.x << ", " << matrix.orig.y << " )";

	return os;
}

std::wostream& operator<<(std::wostream& wos, const MATRIX3X2& matrix)
{
	wos << L"MATRIX3X2( " << matrix.dirX.x << L", " << matrix.dirX.y << L", " << matrix.dirY.x << L", " << matrix.dirY.y << L", " << matrix.orig.x << L", " << matrix.orig.y << L" )";

	return wos;
}

DOUBLE2 MATRIX3X2::TransformVector(DOUBLE2 vector) const
{
	return vector.x * dirX + vector.y * dirY;
}

DOUBLE2 MATRIX3X2::TransformPoint(DOUBLE2 point) const
{
	return orig + TransformVector(point - DOUBLE2(0, 0));
}

double MATRIX3X2::Determinant() const
{
	return dirX.x * dirY.y - dirX.y * dirY.x;
}

MATRIX3X2 MATRIX3X2::Inverse() const
{
	//calculate derminant
	double det = Determinant();
	//1)calculate matrix of minors
	//2)Use the alternating law of signs to produce the matrix of cofactors 
	//3)Transpose
	//4)the inverse matrix is 1/Determinant * the resulting matrix
	return MATRIX3X2(DOUBLE2(+dirY.y, -dirX.y) / det,
		DOUBLE2(-dirY.x, +dirX.x) / det,
		DOUBLE2(dirY.x * orig.y - dirY.y * orig.x, -(dirX.x * orig.y - dirX.y * orig.x)) / det);
}

bool MATRIX3X2::Equals(MATRIX3X2 p, double) const
{
	return dirX.Equals(p.dirX) && dirY.Equals(p.dirY) && orig.Equals(p.orig);
}

bool MATRIX3X2::operator==(const MATRIX3X2& other) const
{
	return dirX == other.dirX && dirY == other.dirY && orig == other.orig;
}

bool MATRIX3X2::operator!=(const MATRIX3X2& other) const
{
	return !(*this == other);
}

Matrix3x2F MATRIX3X2::ToMatrix3x2F() const
{
	Matrix3x2F mat;

	mat._11 = (FLOAT)dirX.x;
	mat._12 = (FLOAT)dirX.y;
	mat._21 = (FLOAT)dirY.x;
	mat._22 = (FLOAT)dirY.y;
	mat._31 = (FLOAT)orig.x;
	mat._32 = (FLOAT)orig.y;

	return mat;
}

String MATRIX3X2::ToString() const
{
	return String("x = ") + dirX.ToString() + String("\ny = ") + dirY.ToString() + String("\no = ") + orig.ToString();
}

void MATRIX3X2::SetAsIdentity()
{
	dirX = DOUBLE2(1, 0);
	dirY = DOUBLE2(0, 1);
	orig = DOUBLE2(0, 0);
}

void MATRIX3X2::SetAsRotate(double radians)
{
	dirX = DOUBLE2(cos(radians), sin(radians));
	dirY = DOUBLE2(-sin(radians), cos(radians));
	orig = DOUBLE2(0, 0);
}
void MATRIX3X2::SetAsTranslate(double tx, double ty)
{
	dirX = DOUBLE2(1, 0);
	dirY = DOUBLE2(0, 1);
	orig = DOUBLE2(tx, ty);
}

void MATRIX3X2::SetAsTranslate(DOUBLE2 pt)
{
	dirX = DOUBLE2(1, 0);
	dirY = DOUBLE2(0, 1);
	orig = DOUBLE2(pt.x, pt.y);
}

void MATRIX3X2::SetAsScale(double scaleX, double scaleY)
{
	dirX = DOUBLE2(scaleX, 0);
	dirY = DOUBLE2(0, scaleY);
	orig = DOUBLE2(0, 0);
}

void MATRIX3X2::SetAsScale(double scale)
{
	SetAsScale(scale, scale);
}

MATRIX3X2 MATRIX3X2::CreateRotationMatrix(double angle)
{
	return MATRIX3X2(DOUBLE2(cos(angle), sin(angle)), DOUBLE2(-sin(angle), cos(angle)), DOUBLE2());
}

MATRIX3X2 MATRIX3X2::CreateIdentityMatrix()
{
	return MATRIX3X2(DOUBLE2(1, 0), DOUBLE2(0, 1), DOUBLE2());
}

MATRIX3X2 MATRIX3X2::CreateScalingMatrix(double scale)
{
	return CreateScalingMatrix(scale, scale);
}

MATRIX3X2 MATRIX3X2::CreateScalingMatrix(DOUBLE2 scaleXY)
{
	return CreateScalingMatrix(scaleXY.x, scaleXY.y);
}

MATRIX3X2 MATRIX3X2::CreateScalingMatrix(double scaleX, double scaleY)
{
	return MATRIX3X2(DOUBLE2(scaleX, 0), DOUBLE2(0, scaleY), DOUBLE2());
}

MATRIX3X2 MATRIX3X2::CreateTranslationMatrix(DOUBLE2 origin)
{
	return MATRIX3X2(DOUBLE2(1, 0), DOUBLE2(0, 1), origin);
}

MATRIX3X2 MATRIX3X2::CreateTranslationMatrix(double tx, double ty)
{
	return CreateTranslationMatrix(DOUBLE2(tx, ty));
}

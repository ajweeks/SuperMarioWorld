//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

// Store Box2D contactdata retrieved from userData pointers
//struct ContactData
//{
//	void *contactListenerPtr = nullptr;
//	void *actThisPtr = nullptr, *actOtherPtr = nullptr;
//};

struct ImpulseData
{
	void *contactListenerAPtr = nullptr, *contactListenerBPtr = nullptr;
	double impulseA = 0, impulseB = 0;
	void *actAPtr = nullptr, *actBPtr = nullptr;
};

//-----------------------------------------------------------------
// COLOR Struct
//-----------------------------------------------------------------
//! @struct Color
struct COLOR
{
	// -------------------------
	// Constructors 
	// -------------------------

	//! Default constructor fills the struct with color values 255, resulting in a white color 
	COLOR();

	//! Constructor: params are color values from 0 to 255
	//! Example: COLOR myColor(255,0,127);
	//! @param redVal red color value from 0 to 255
	//! @param greenVal red color value from 0 to 255
	//! @param blueVal red color value from 0 to 255
	//! @param alphaVal alpha color value from 0 to 255
	COLOR(unsigned char redVal, unsigned char greenVal, unsigned char blueVal, unsigned char alphaVal = 255);

	// -------------------------
	// Datamembers 
	// -------------------------	
	unsigned char red = 255, green = 255, blue = 255, alpha = 255;
};

//-----------------------------------------------------------------
// RECT2 Struct
//-----------------------------------------------------------------
struct RECT2
{
	// -------------------------
	// Constructors 
	// -------------------------	

	//! Constructor: creates a RECT2 struct with 4 double values, used to hold floating point coordinates.
	//! This default constructor sets these values to 0
	//! Example: RECT2 myRECT2();
	RECT2();

	//! Constructor: creates a RECT2 struct with 4 double values, used to hold floating point coordinates.
	//! Example: RECT2 myRECT2(1.5, 1.5, 254.2, 452.6);
	RECT2(double leftVal, double topVal, double rightVal, double bottomVal);

	// -------------------------
	// Datamembers 
	// -------------------------	
	double left, top, right, bottom;
};


//-----------------------------------------------------------------
// DOUBLE2 Struct
//-----------------------------------------------------------------
struct DOUBLE2
{
	// -------------------------
	// Friend declarations 
	// -------------------------
	friend std::ostream&	operator<<(std::ostream& os, DOUBLE2 right);					// Leftshift to a stream
	friend std::wostream&	operator<<(std::wostream& wos, DOUBLE2 right);					// Leftshift to a wide character stream
	friend DOUBLE2			operator*(double factor, const DOUBLE2& right);					// Multiplication:										DOUBLE2 w = 2 * v;
	friend DOUBLE2			Lerp(const DOUBLE2& from, const DOUBLE2& to, double lambda);	// Barycentric interpolation between two points:		DOUBLE2 r = Lerp(p,q,0.5)

	// -------------------------
	// Constructors 
	// -------------------------

	//! Default constructor gives the origin point
	DOUBLE2();

	//! Constructor. Example Usage: DOUBLE2 p(1.2,2.5);
	DOUBLE2(double x, double y);

	//! Internal use, not ment to be used by students
	//! Construct from Direct2D
	DOUBLE2(const D2D1_POINT_2F& d2dPoint);

	// -------------------------
	// Operators
	// -------------------------

	//! Assignment:	DOUBLE2 r = a;
	DOUBLE2&		operator= (const DOUBLE2& other);
	//! Assignment from D2D								
	DOUBLE2&		operator= (const D2D1_POINT_2F& other);
	//! Vector addition:									
	//! DOUBLE2 r = a + b;
	DOUBLE2			operator+ (const DOUBLE2& other) const;
	//! Vector subtraction:									
	//! DOUBLE2 r = a - b;
	DOUBLE2			operator- (const DOUBLE2& other) const;
	//! Negation:											
	//! DOUBLE2 w = -v;
	DOUBLE2			operator- () const;
	//! Identity:											
	//! DOUBLE2 w = +v;
	DOUBLE2			operator+ () const;
	//! Right multiplication with a double:					
	//! DOUBLE2 w = v * 2;
	DOUBLE2			operator* (double factor) const;
	//! Division by double:									
	//! DOUBLE2 w = v/2;
	DOUBLE2			operator/ (double divisor) const;
	//! Translate this point inplace:						
	//! p += v;
	DOUBLE2&		operator+=(const DOUBLE2& other);
	//! Translate this point inplace (opposite direction):	
	//! p -= v;
	DOUBLE2&		operator-=(const DOUBLE2& other);
	//! Scale vector inplace:								
	//! v *= s;
	DOUBLE2&		operator*=(double factor);
	//! Downscale vector inplace:							
	//! v /= s;
	DOUBLE2&		operator/=(double divisor);
	//! Are two points exactly equal?						
	//! p == q
	bool			operator==(const DOUBLE2& other) const;
	//! Are two points exactly unequal?						
	//! p != q
	bool			operator!=(const DOUBLE2& other) const;

	// -------------------------
	// Methods
	// -------------------------

	//! Convert to Direct2D	
	D2D1_POINT_2F	ToPoint2F() const;	

	//! Are two points equal within a threshold?				p.Equals(q)
	bool			Equals(const DOUBLE2& other, double epsilon = 0.001) const;	
	//! Convert to String 
	String			ToString() const;											
	//! DotProduct: double d = u.DotProduct(v);
	double			DotProduct(const DOUBLE2& other) const;						
	//! Determinant: double d = u.Determinant(v);
	double			Determinant(const DOUBLE2& other) const;					
	//! Norm of a vector: double l = v.Norm();
	double			Norm() const;												
	//! Length of a vector: double l = v.Length();
	double			Length() const;												
	//! Square Length of a vector: faster alternative for Length, sqrt is not executed. use for speed: double l = v.SquaredLength();
	double			SquaredLength() const;										
	//! AngleWith returns the smallest angle with another vector: double d = u.AngleWith(v);
	double			AngleWith(const DOUBLE2& other) const;						
	//! Normalized form of a vector: DOUBLE2 u = v.Normalized();
	DOUBLE2			Normalized(double epsilon = 0.001) const;					
	//! Orthogonal: DOUBLE2 w = v.Orthogonal();
	DOUBLE2			Orthogonal() const;											
	//! Sets the values of x and y
	void			Set(double newX, double newY);
	// -------------------------
	// Datamembers 
	// -------------------------
	double x, y;
};


//-----------------------------------------------------------------
// MATRIX3X2 Struct
//-----------------------------------------------------------------
struct MATRIX3X2
{
	// -------------------------
	// Friend declarations 
	// -------------------------
	friend MATRIX3X2 operator*(const MATRIX3X2& f1, const MATRIX3X2& f2); 			// Multiply or transform matrix f1 by matrix f2 (operator form)
	friend std::ostream& operator<<(std::ostream& os, const MATRIX3X2& right);		// Print to a stream.
	friend std::wostream& operator<<(std::wostream& wos, const MATRIX3X2& right);	// Print to a wstream.

	// -------------------------
	// Constructors 
	// -------------------------
	//! Default constructor results in a unity matrix
	MATRIX3X2(DOUBLE2 dirX = DOUBLE2(1, 0), DOUBLE2 dirY = DOUBLE2(0, 1), DOUBLE2 orig = DOUBLE2(0, 0));
	//! Constructor, using doubles, all required
	MATRIX3X2(double e1X, double e1Y, double e2X, double e2Y, double oX, double oY);
	//! Copy constructor
	MATRIX3X2(const MATRIX3X2& orig);
	//! Construct from Direct2D, not for students
	MATRIX3X2(const D2D1::Matrix3x2F& mat);

	// -------------------------
	// Operators 
	// -------------------------
	//! Are two matrices exactly equal?				p == q
	bool operator==(const MATRIX3X2& other) const;
	//! Are two matrices exactly unequal?			p != q
	bool operator!=(const MATRIX3X2& other) const;

	// -------------------------
	// General Methods 
	// -------------------------
	//! Transform a vector by this matrix, no translation			
	//! DOUBLE2 w = b.Transform(v);
	DOUBLE2				TransformVector(DOUBLE2 v) const;
	//! Transform a point by this matrix, including translation			
	//! DOUBLE2 q = f.Transform(p);
	DOUBLE2				TransformPoint(DOUBLE2 p) const;
	//! Calculate the determinant
	double				Determinant() const;
	//! Calculate the inverse matrix
	MATRIX3X2			Inverse() const;
	//! Are two matrices equal within a threshold?	p.Equals(q)
	bool				Equals(MATRIX3X2 p, double epsilon = 0.001) const;
	//! Creates a String containing a text representation of the values of the matrix
	String				ToString() const;
	//! Convert to Direct2D
	D2D1::Matrix3x2F	ToMatrix3x2F() const;

	//! Converts this matrix into a Identity matrix
	void SetAsIdentity();
	//! Converts this matrix into a Rotate matrix
	void SetAsRotate(double radians);
	//! Converts this matrix into a Translation matrix
	void SetAsTranslate(double tx, double ty);
	//! Converts this matrix into a Translation matrix
	void SetAsTranslate(DOUBLE2 pt);
	//! Converts this matrix into a Scale matrix
	void SetAsScale(double sx, double sy);
	//! Converts this matrix into a Scale matrix
	void SetAsScale(double s);

	// -------------------------
	// Static Methods 
	// -------------------------
	//! Instantiate a rotation matrix: 
	//! Matrix r = Matrix::Rotation(M_PI/2);
	static MATRIX3X2 CreateRotationMatrix(double radians);
	//! Instantiate an identity matrix: 
	//! MATRIX3X2 m = MATRIX3X2::Identity();
	static MATRIX3X2 CreateIdentityMatrix();
	//! Instantiate a scale matrix: 
	//! Matrix s = Matrix::Scaling(2);
	static MATRIX3X2 CreateScalingMatrix(double scale);
	//! Instantiate a scale matrix: 
	//! Matrix s = Matrix::Scaling(2,-3);
	static MATRIX3X2 CreateScalingMatrix(double scaleX, double scaleY);
	//! Instantiate a scale matrix: 
	//! Matrix s = Matrix::Scaling( DOUBLE2(2,-3) );
	static MATRIX3X2 CreateScalingMatrix(DOUBLE2 scaleXY);
	//! Instantiate a translation matrix: 
	//! Matrix t = Matrix::Translation( Point2(2,3) );
	static MATRIX3X2 CreateTranslationMatrix(DOUBLE2 origin);
	//! Instantiate a translation matrix: 
	//! Matrix t = Matrix::Translation(2,3);
	static MATRIX3X2 CreateTranslationMatrix(double tx, double ty);


	// -------------------------
	// Datamembers
	// -------------------------
	DOUBLE2 dirX;	//! The first matrix vector (the "x-axis")
	DOUBLE2 dirY;	//! The second matrix vector (the "y-axis")
	DOUBLE2 orig; 	//! The origin of  the coordinate matrix (the "translation")
};
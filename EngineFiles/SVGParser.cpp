//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"
#include "SVGParser.h"

using namespace std;

struct membuf : std::streambuf {
	membuf(char const* base, size_t size) {
		char* p(const_cast<char*>(base));
		this->setg(p, p, p + size);
	}
};

//-----------------------------------------------------------------
// SVGParser Class
//-----------------------------------------------------------------

// This ifdef block is here because in the 1st year students still need to learn what it means/put it in themselves. In the 2nd year engine it is defined in GameDefines.h
#ifdef _UNICODE
#define tifstream wifstream
#else
#define tifstream ifstream
#endif


using namespace std;

SVGParser::SVGParser()
{

}

SVGParser::~SVGParser()
{

}

bool SVGParser::LoadGeometryFromSvgStream(BYTE* pBlob, int blobSize, std::vector<std::vector<DOUBLE2>> &verticesArrRef)
{
	membuf sBuf((char*)pBlob, blobSize);
	istream stream(&sBuf);

	if (!stream)
	{
		GameEngine::GetSingleton()->MessageBox(String("Failed to load geometry from stream "));
		return false;
	}
	else
	{
		std::string sLine;
		string svgText;
		while (std::getline(stream, sLine))
		{
			svgText += string(sLine.begin(),sLine.end());
		}

		bool res = ExtractGeometryFromDataString(svgText, verticesArrRef);
		if (!res)
		{
			GameEngine::GetSingleton()->MessageBox(String("Malformed geometry information in stream "));
			return false;
		}
	}

	return true;
}

bool SVGParser::LoadGeometryFromSvgFile(const String& svgFilePathRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef)
{
	// open the file, close if not found
	ifstream svgFile(svgFilePathRef.C_str());

	if (!svgFile)
	{
		GameEngine::GetSingleton()->MessageBox(String("Failed to load geometry from file ") + svgFilePathRef);
		return false;
	}
	else
	{
		// read the file
		string sLine;
		string svgText;
		while (!svgFile.eof())
		{
			getline(svgFile, sLine);
			svgText += sLine;
		}

		// close the file
		svgFile.close();

		bool res = ExtractGeometryFromDataString(svgText, verticesArrRef);
		if (!res)
		{
			GameEngine::GetSingleton()->MessageBox(String("Malformed geometry information in file ") + svgFilePathRef);
			return false;
		}
	}

	return true;
}

bool SVGParser::ExtractGeometryFromDataString(string& svgTextRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef)
{
	// extract the transform information, if present
	string sTransformInfo;
	bool result = ExtractTransformInformation(svgTextRef, sTransformInfo);

	// extract the geometry information, exit if not found
	string sGeometryInfo;
	result = ExtractPathInformation(svgTextRef, sGeometryInfo);
	if (result == false)
	{
		return false;
	}

	// process the geometry information
	return ReadSvgFromData(sTransformInfo, sGeometryInfo, verticesArrRef);
}


bool SVGParser::ExtractTransformInformation(string& svgTextRef, string& sTransformInfoRef)
{
	string pattern = ("transform=");
	string::iterator posTransform = search(svgTextRef.begin(), svgTextRef.end(), pattern.begin(), pattern.end());
	if (posTransform == svgTextRef.end()) // d attribute not found, maybe with a space?
	{
		pattern = ("transform =");
		posTransform = search(svgTextRef.begin(), svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posTransform == svgTextRef.end()) // try in caps?
	{
		pattern = ("TRANSFORM=");
		posTransform = search(svgTextRef.begin(), svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posTransform == svgTextRef.end()) // last try
	{
		pattern = ("TRANSFORM =");
		posTransform = search(svgTextRef.begin(), svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posTransform == svgTextRef.end()) // give up
	{
		return false;
	}

	string::iterator posOpeningDoubleQuote = find(posTransform, svgTextRef.end(), ('\"'));
	string::iterator posClosingDoubleQuote = find(posOpeningDoubleQuote + 1, svgTextRef.end(), ('\"'));

	sTransformInfoRef = string(posOpeningDoubleQuote + 1, posClosingDoubleQuote);

	return true;
}

bool SVGParser::ExtractPathInformation(string& svgTextRef, string& sGeometryInfoRef)
{
	// extract the path information part
	string pattern(("<path"));
	string::iterator posPath = search(svgTextRef.begin(), svgTextRef.end(), pattern.begin(), pattern.end());
	pattern = (" d=");
	string::iterator posD = search(posPath, svgTextRef.end(), pattern.begin(), pattern.end());
	if (posD == svgTextRef.end()) // d attribute not found, maybe with a space?
	{
		pattern = (" d =");
		posD = search(posPath, svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posD == svgTextRef.end()) // try in caps?
	{
		pattern = (" D=");
		posD = search(posPath, svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posD == svgTextRef.end()) // last try
	{
		pattern = (" D =");
		posD = search(posPath, svgTextRef.end(), pattern.begin(), pattern.end());
	}
	if (posD == svgTextRef.end()) // give up
	{
		return false;
	}

	string::iterator posOpeningDoubleQuote = find(posD, svgTextRef.end(), ('\"'));
	string::iterator posClosingDoubleQuote = find(posOpeningDoubleQuote + 1, svgTextRef.end(), ('\"'));

	sGeometryInfoRef = string(posOpeningDoubleQuote + 1, posClosingDoubleQuote);

	return true;
}

// Points are transformed by groups into the same space.
bool SVGParser::ReadSvgFromData(string& sTransformInfoRef, string& sPathInfoRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef)
{
	// parse the transform information
	MATRIX3X2 pathTransform = ReadTransform(sTransformInfoRef);

	// parse the geometry information
	//ID2D1PathGeometry* pathGeometryPtr;
	bool result = ReadSvgPath(sPathInfoRef, verticesArrRef);
	if (!result)
	{
		GameEngine::GetSingleton()->MessageBox(String("SVG path read failed"));
		return false;
	}
	return true;
}

bool SVGParser::GetGeometryOrGroup(ID2D1Factory* factoryPtr, std::vector<ID2D1Geometry*>& childrenRef, ID2D1Geometry*& geometryPtrRef)
{
	if (!childrenRef.empty())
	{
		if (childrenRef.size() == 1)
		{
			geometryPtrRef = childrenRef[0];
		}
		else
		{
			ID2D1GeometryGroup* geometryGroupPtr;
			HRESULT hr = factoryPtr->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, &childrenRef[0], childrenRef.size(), &geometryGroupPtr);
			if (FAILED(hr))
			{
				GameEngine::GetSingleton()->MessageBox(String("CreateTransformedGeometry failed"));
				return false;
			}

			geometryPtrRef = geometryGroupPtr;
		}
	}

	return true;
}

MATRIX3X2 SVGParser::ReadTransform(string& sTransformInfoRef)
{
	MATRIX3X2 matrix;

	if (sTransformInfoRef != (""))
	{
		int argCount;
		switch (sTransformInfoRef.c_str()[0])
		{
		case ('m'):
			sscanf_s(sTransformInfoRef.c_str(), ("matrix(%lf,%lf,%lf,%lf,%lf,%lf)"), &matrix.dirX.x, &matrix.dirX.y, &matrix.dirY.x, &matrix.dirY.y, &matrix.orig.x, &matrix.orig.y);
			break;

		case ('t'):
			argCount = sscanf_s(sTransformInfoRef.c_str(), ("translate(%lf,%lf)"), &matrix.orig.x, &matrix.orig.y);
			break;

		case ('s'):
			argCount = sscanf_s(sTransformInfoRef.c_str(), ("scale(%lf,%lf)"), &matrix.dirX.x, &matrix.dirY.y);

			if (argCount == 1)
			{
				matrix.dirY.y = matrix.dirX.x;
			}
			break;

		case ('r'):
		{
			double angle;
			DOUBLE2 center;
			argCount = sscanf_s(sTransformInfoRef.c_str(), ("rotate(%lf,%lf,%lf)"), &angle, &center.x, &center.y);

			switch (argCount)
			{
			case 1:
				matrix = MATRIX3X2::CreateRotationMatrix(angle * M_PI / 180);
				break;
			case 3:
				matrix = MATRIX3X2::CreateTranslationMatrix(-center) * MATRIX3X2::CreateRotationMatrix(angle * M_PI / 180) * MATRIX3X2::CreateTranslationMatrix(center);
				break;
			}
		}
			break;

		default:
			GameEngine::GetSingleton()->MessageBox(String("unsupported transform attribute"));
			break;
		}
	}

	return matrix;
}

bool SVGParser::ReadSvgPath(string& sPathInfoRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef)
{
	// Use streamstream for parsing
	stringstream ss(sPathInfoRef);

	char cmd = 0;
	string svgCmds(("mMZzLlHhVvCcSsQqTtAa"));
	DOUBLE2 cursor;
	DOUBLE2 abscur;
	DOUBLE2 startPoint;//At the end of the z command, the new current point is set to the initial point of the current subpath.

	bool isOpen = true;

	// http://www.w3.org/TR/SVG/paths.html#Introduction

	std::vector<DOUBLE2> currentChain;
	DOUBLE2 vertex;
	while (true)
	{
		char c;
		ss >> c;

		if (ss.eof())
			break;

		if (strchr(svgCmds.c_str(), c) != 0)
		{
			cmd = c;
		}
		else
		{
			ss.putback(c);
		}

		switch (cmd)
		{
		case ('Z'):
		case ('z'):
			//geometrySinkPtr->EndFigure(D2D1_FIGURE_END_CLOSED);
			// Close the current subpath by drawing a straight line from the current point to current subpath's initial point. Since the Z and z commands take no parameters, they have an identical effect.
			verticesArrRef.push_back(currentChain);
			currentChain.clear();
			cursor = startPoint;//relative path: new point is set to the initial statpoint of the current path
			isOpen = true;
			break;

		case ('M'):
		case ('m'):
			if (isOpen)
			{
				cursor = FirstSvgPoint(ss, cursor, cmd, isOpen, true);
				startPoint = cursor;//record startpoint, cursor is set back to startPoint when path closes
				//cbs.point2 = cursor.ToPoint2F();
				//geometrySinkPtr->BeginFigure(cbs.point2, D2D1_FIGURE_BEGIN_FILLED);
				currentChain.push_back(cursor);
				isOpen = false;
				break;
			}
			// Fallthrough when isOpen
		case ('L'):
		case ('l'):
			vertex = NextSvgPoint(ss, cursor, cmd, isOpen, true).ToPoint2F();
			currentChain.push_back(vertex);
			//geometrySinkPtr->AddLine(cbs.point2);
			break;

		case ('h'):
		case ('H'):
			vertex = NextSvgCoordX(ss, cursor, cmd, isOpen).ToPoint2F();
			currentChain.push_back(vertex);
			//geometrySinkPtr->AddLine(cbs.point2);
			break;

		case ('v'):
		case ('V'):
			vertex = NextSvgCoordY(ss, cursor, cmd, isOpen).ToPoint2F();
			currentChain.push_back(vertex);
			//geometrySinkPtr->AddLine(cbs.point2);
			break;

		case ('C'):
		case ('c'):
			GameEngine::GetSingleton()->MessageBox(String(cmd) + String(" Beziers are not supported. Have another look at the guide, or select all nodes in inkscape and press shift + L"));
			break;

			//case ('S'):
			//case ('s'):
			//	abscur = cursor;
			//	cbs.point1.x = (FLOAT)(abscur.x + (abscur.x - cbs.point2.x));
			//	cbs.point1.y = (FLOAT)(abscur.y + (abscur.y - cbs.point2.y));
			//	cbs.point2 = NextSvgPoint(ss, cursor, cmd, isOpen, false).ToPoint2F();
			//	cbs.point3 = NextSvgPoint(ss, cursor, cmd, isOpen, true).ToPoint2F();
			//	geometrySinkPtr->AddBezier(cbs);
			//	break;

			//case ('Q'):
			//case ('q'):
			//	qbs.point1 = NextSvgPoint(ss, cursor, cmd, isOpen, false).ToPoint2F();
			//	qbs.point2 = NextSvgPoint(ss, cursor, cmd, isOpen, true).ToPoint2F();
			//	geometrySinkPtr->AddQuadraticBezier(qbs);
			//	break;

			//case ('T'):
			//case ('t'):
			//	abscur = cursor;
			//	qbs.point1.x = (FLOAT)((abscur.x + (abscur.x - qbs.point2.x)));
			//	qbs.point1.y = (FLOAT)((abscur.y + (abscur.y - qbs.point2.y)));
			//	qbs.point2 = NextSvgPoint(ss, cursor, cmd, isOpen, true).ToPoint2F();
			//	geometrySinkPtr->AddQuadraticBezier(qbs);
			//	break;

			//case ('A'):
			//case ('a'):
			//	// (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+	
			//	// Draws an elliptical arc from the current point to (x, y). 
			//	// The size and orientation of the ellipse are defined by two radii (rx, ry) and an x-axis-rotation, 
			//	// which indicates how the ellipse as a whole is rotated relative to the current coordinate system. 
			//	// The center (cx, cy) of the ellipse is calculated automatically to satisfy the constraints imposed by the other parameters. 
			//	// large-arc-flag and sweep-flag contribute to the automatic calculations and help determine how the arc is drawn.
			//{
			//	OutputDebugString(String("WARNING: SVG Arcs not supported yet\n"));
			//	D2D1_ARC_SEGMENT arc;
			//	arc.size.width = (float)ReadSvgValue(ss, true);
			//	arc.size.height = (float)ReadSvgValue(ss, true);
			//	arc.rotationAngle = (float)ReadSvgValue(ss, true);
			//	arc.arcSize = (D2D1_ARC_SIZE)(int)ReadSvgValue(ss, true);
			//	arc.sweepDirection = (D2D1_SWEEP_DIRECTION)(int)ReadSvgValue(ss, true);
			//	cbs.point2 = arc.point = NextSvgPoint(ss, cursor, cmd, isOpen, true).ToPoint2F();
			//	geometrySinkPtr->AddArc(arc);
			//}
			//	break;

		default:
			GameEngine::GetSingleton()->MessageBox(String(cmd) + String(" is not a supported SVG command"));
			//return false;
		}
	}

	if (!isOpen)
	{
		verticesArrRef.push_back(currentChain);
	}

	return true;
}

// Replace all whitespace by space.
char SVGParser::WhiteSpaceMapper(char c)
{
	switch (c)
	{
	case ('\n'):
	case ('\r'):
	case ('\t'):
		return (' ');

	default:
		return c;
	}
}

// Skips any optional commas in the stream
// SVG has a really funky format,
// not sure this code works for all cases.
// TODO: Test cases!
void SVGParser::SkipSvgComma(stringstream& ssRef, bool isRequired)
{
	while (true)
	{
		char c = ssRef.get();

		if (ssRef.eof())
		{
			if (isRequired) GameEngine::GetSingleton()->MessageBox(String("expected comma or whitespace"));
			break;
		}

		if (c == (','))
			return;

		if (!isspace(c))
		{
			ssRef.unget();
			return;
		}
	}
}

double SVGParser::ReadSvgValue(stringstream& ssRef, double defaultValue)
{
	double s;
	ssRef >> s;

	if (ssRef.eof())
	{
		s = defaultValue;
	}
	else
	{
		SkipSvgComma(ssRef, false);
	}

	return s;
}

double SVGParser::ReadSvgValue(stringstream& ssRef, bool separatorRequired)
{
	double s;
	ssRef >> s;
	SkipSvgComma(ssRef, separatorRequired);
	return s;
}

// Reads a single point
DOUBLE2 SVGParser::ReadSvgPoint(stringstream& ssRef)
{
	DOUBLE2 p;
	p.x = ReadSvgValue(ssRef, true);
	p.y = ReadSvgValue(ssRef, false);
	return p;
}

DOUBLE2 SVGParser::FirstSvgPoint(stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen, bool advance)
{
	if (!isOpen) GameEngine::GetSingleton()->MessageBox(String("expected 'Z' or 'z' command"));

	DOUBLE2 p = ReadSvgPoint(ssRef);

	if (islower(cmd))
	{
		// Relative point
		p = cursor + p;
	}

	if (advance)
	{
		cursor = p;
	}

	return p;
}
// Read the next point, 
// taking into account relative and absolute positioning.
// Advances the cursor if requested.
// Throws an exception if the figure is not open
DOUBLE2 SVGParser::NextSvgPoint(stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen, bool advance)
{
	if (isOpen) GameEngine::GetSingleton()->MessageBox(String("expected 'M' or 'm' command"));

	DOUBLE2 p = ReadSvgPoint(ssRef);

	if (islower(cmd))
	{
		// Relative point
		p = cursor + (p - DOUBLE2());
	}

	if (advance)
	{
		cursor = p;
	}

	return p;
}

// Reads next point, given only the new x coordinate 
DOUBLE2 SVGParser::NextSvgCoordX(stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen)
{
	if (isOpen)
		GameEngine::GetSingleton()->MessageBox(String("expected 'M' or 'm' command"));

	double c;
	ssRef >> c;

	if (islower(cmd))
	{
		// Relative point
		cursor += DOUBLE2(c, 0);
	}
	else
	{
		cursor.x = c;
	}

	return cursor;
}

// Reads next point, given only the new y coordinate 
DOUBLE2 SVGParser::NextSvgCoordY(stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen)
{
	if (isOpen)
		GameEngine::GetSingleton()->MessageBox(String("expected 'M' or 'm' command"));

	double c;
	ssRef >> c;

	if (islower(cmd))
	{
		// Relative point
		cursor += DOUBLE2(0, c);
	}
	else
	{
		cursor.y = c;
	}

	return cursor;
}

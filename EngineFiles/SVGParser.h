//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once
//-----------------------------------------------------------------
// SVGParser Class
//-----------------------------------------------------------------
class SVGParser
{
public:
	//---------------------------
	// Constructor(s) and Destructor
	//---------------------------
	SVGParser();
	virtual ~SVGParser();

	// C++11 make the class non-copyable
	SVGParser(const SVGParser&) = delete;
	SVGParser& operator=(const SVGParser&) = delete;

	//---------------------------
	// General Methods
	//---------------------------
	bool LoadGeometryFromSvgStream(BYTE* pBlob, int blobSize, std::vector<std::vector<DOUBLE2>> &verticesArrRef);
	bool LoadGeometryFromSvgFile(const String& svgFilePathRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef);

private:
	//---------------------------
	// Private methods
	//---------------------------
	bool ExtractGeometryFromDataString(std::string& svgTextRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef);
	bool ExtractTransformInformation(std::string& svgTextRef, std::string& sTransformInfoRef);
	bool ExtractPathInformation(std::string& svgTextRef, std::string& sPathInfoRef);
	bool ReadSvgFromData(std::string& sTransformInfoRef, std::string& sPathInfoRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef);
	bool GetGeometryOrGroup(ID2D1Factory* factoryPtr, std::vector<ID2D1Geometry*>& childrenRef, ID2D1Geometry*& geometryPtrRef);
	MATRIX3X2 ReadTransform(std::string& sTransformInfoRef);
	bool ReadSvgPath(std::string& sPathInfoRef, std::vector<std::vector<DOUBLE2>> &verticesArrRef);

	char WhiteSpaceMapper(char c);														// Replace all whitespace by space.

	// Skips any optional commas in the stream
	// SVG has a really funky format,
	// not sure this code works for all cases.
	// TODO: Test cases!
	void SkipSvgComma(std::stringstream& ssRef, bool isRequired);
	double ReadSvgValue(std::stringstream& ssRef, double defaultValue);
	double ReadSvgValue(std::stringstream& ssRef, bool separatorRequired);

	DOUBLE2 ReadSvgPoint(std::stringstream& ssRef);												// Reads a single point

	// Read the first point, 
	// taking into account relative and absolute positioning.
	// Stores this point, needed when path is closed
	// Advances the cursor if requested.
	DOUBLE2 FirstSvgPoint(std::stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen, bool advance);
	// Read the next point, 
	// taking into account relative and absolute positioning.
	// Advances the cursor if requested.
	// Throws an exception if the figure is not open
	DOUBLE2 NextSvgPoint(std::stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen, bool advance);

	DOUBLE2 NextSvgCoordX(std::stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen);	// Reads next point, given only the new x coordinate
	DOUBLE2 NextSvgCoordY(std::stringstream& ssRef, DOUBLE2& cursor, char cmd, bool isOpen);	// Reads next point, given only the new y coordinate 
};

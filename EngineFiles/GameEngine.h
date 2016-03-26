//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Forward Class Declarations
//-----------------------------------------------------------------
class Bitmap;
class String;
class Font;
class GUIBase;
class InputManager;
class AbstractGame;
class AudioSystem;
class PrecisionTimer;
class b2World;
class ContactListener;
class FmodSystem;

// Includes
#include <Box2D/Dynamics/b2WorldCallbacks.h>

//-----------------------------------------------------------------
// Extra global OutputDebugString function
//-----------------------------------------------------------------
void OutputDebugString(const String& textRef);

//-----------------------------------------------------------------
// Windows Procedure Declaration
//-----------------------------------------------------------------
LRESULT CALLBACK	WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class GameEngine : public b2ContactListener, public b2DestructionListener
{

	//! singleton implementation : private constructor + static pointer to game engine
private:
	GameEngine();
	static GameEngine*  m_GameEnginePtr;
	virtual ~GameEngine();

public:
	// C++11 make the class non-copyable
	GameEngine(const GameEngine&) = delete;
	GameEngine& operator=(const GameEngine&) = delete;

	// Static method
	static GameEngine* GetSingleton();

#ifdef WRAPPER_LIB
	// General Methods used by GameWinMain
	void SetGame(AbstractGame* gamePtr);
	int Run(HINSTANCE hInstance, int iCmdShow);
	static void Destroy();
#endif

// Input methods
	// Returns a DOUBLE2 containing the window coordinates of the mouse
	// Usage example:
	// DOUBLE2 mousePos = GAME_ENGINE->GetMousePosition();
	DOUBLE2	GetMousePosition() const;

	//! Returns true when button is down and was down the previous frame
	//! Example values for key are: VK_LEFT, 'A'. ONLY CAPITALS.
	bool IsKeyboardKeyDown(int key) const;

	//! Returns true when button is down and was up the previous frame
	//! Example values for key are: VK_LEFT, 'A'. ONLY CAPITALS.
	bool IsKeyboardKeyPressed(int key) const;

	//! Returns true when button is up and was down the previous frame
	//! Example values for key are: VK_LEFT, 'A'. ONLY CAPITALS.
	bool IsKeyboardKeyReleased(int key) const;

	//! Returns true when button is down and was down the previous frame
	//! Possible values for button are: VK_LBUTTON, VK_RBUTTON and VK_MBUTTON
	bool IsMouseButtonDown(int button) const;

	//! Returns true when button is down and was up the previous frame
	//! Possible values for button are: VK_LBUTTON, VK_RBUTTON and VK_MBUTTON
	bool IsMouseButtonPressed(int button) const;

	//! Returns true when button is up and was down the previous frame
	//! Possible values for button are: VK_LBUTTON, VK_RBUTTON and VK_MBUTTON
	bool IsMouseButtonReleased(int button) const;

	// Add GUI derived object to the GUI std::vector to be ticked automatically
	// NOT for students
	void RegisterGUI(GUIBase *guiPtr);

	// Remove GUI derived object from the GUI vector
	// NOT for students
	void UnRegisterGUI(GUIBase *guiPtr);

//Console Methods
	//! ConsoleSetForeColor: Set the color of the text by blending the 3 basic colors to create 8 different color values
	//! @param red:	enable(true) or disable(false) the red component
	//! @param green: enable(true) or disable(false) the green component
	//! @param blue: enable(true) or disable(false) the blue component
	//! @param intensity: enable(true) or disable(false) highlight
	void ConsoleSetForeColor(bool red, bool green, bool blue, bool intensity);

	//! ConsoleSetBackColor: Set the color of the background by blending the 3 basic colors to create 8 different color values
	//! @param red: enable(true) or disable(false) the red component
	//! @param green: enable(true) or disable(false) the green component
	//! @param blue: enable(true) or disable(false) the blue component
	//! @param intensity: enable(true) or disable(false) highlight
	void ConsoleSetBackColor(bool red, bool green, bool blue, bool intensity);

	//! ConsoleClear: Remove all characters from the console and set the cursor to the top left corner
	void ConsoleClear() const;

	//! ConsoleSetCursorPosition: Set the cursor on a specific column and row number
	void ConsoleSetCursorPosition(int column, int row);

	//! ConsolePrintString: Display a String on the current cursor position. A new line is appended automatically
	void ConsolePrintString(const String& textRef);

	// ConsolePrintString: Display a std::string on the current cursor position. A new line is appended automatically
	//void ConsolePrintString(std::string text);

	//! ConsolePrintString: Display a String on the position determined by column and row. A new line is appended automatically.
	//!	-column: the column number 
	//!	-row: the row number 
	void ConsolePrintString(const String& textRef, int column, int row);

	// ConsolePrintString: Display a std::string on the position determined by column and row. A new line is appended automatically.
	//	-column: the column number 
	//	-row: the row number 
	//void ConsolePrintString(std::string text, int column, int row);

// Draw Methods		
	//! Fills the client area of the window using the provided color, alpha is ignored 
	bool DrawSolidBackground(COLOR backgroundColor);

	//! Draws a line from the coordinate defined by x1 and y1 to the coordinate define by x2 and y2
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawLine(double x1, double y1, double x2, double y2, double strokeWidth = 1.0);

	//! Draws a line from p1 to p2 using the strokewidth
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawLine(DOUBLE2 p1, DOUBLE2 p2, double strokeWidth = 1.0);

	//! Draws a polygon defined by the coordinates in ptsArr
	//! If close is true then it will connect the start and end coordinate
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawPolygon(const std::vector<DOUBLE2>& ptsArrRef, bool close = true, double strokeWidth = 1.0);

	//! Fills the interior of a polygon defined by all the coordinates in ptsArr
	//! If close is true then it will connect the start and end coordinate
	bool FillPolygon(const std::vector<DOUBLE2>& ptsArrRef);

	//! Draws a rectangle defined by 4 numbers representing the left side, top side, the right side and the bottom side
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawRect(double left, double top, double right, double bottom, double strokeWidth = 1.0);

	//! Draws a rectangle defined by two coordinates: topleft and rightbottom
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawRect(DOUBLE2 topLeft, DOUBLE2 rightbottom, double strokeWidth = 1.0);

	//! Draws a rectangle defined by a RECT2 struct
	bool DrawRect(RECT2 rect, double strokeWidth = 1.0);

	//! Fills the interior of a rectangle defined by a RECT2 struct
	bool FillRect(RECT2 rect);

	//! Fills the interior of a rectangle defined by two coordinates: topleft and rightbottom
	bool FillRect(DOUBLE2 topLeft, DOUBLE2 rightbottom);
	//! Fills the interior of a rectangle defined by a RECT struct
	//bool			FillRect(RECT rect);
	//! Fills the interior of a rectangle defined by 4 numbers representing the left side, top side, the right side and the bottom side
	bool FillRect(double left, double top, double right, double bottom);

	//! Draws a rounded rectangle defined by 4 numbers representing the left side, top side, the right side and the bottom side
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawRoundedRect(double left, double top, double right, double bottom, double radiusX, double radiusY, double strokeWidth = 1.0);

	//! Draws a rounded rectangle defined by two coordinates: topleft and rightbottom,
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawRoundedRect(DOUBLE2 topLeft, DOUBLE2 rightbottom, double radiusX, double radiusY, double strokeWidth = 1.0);

	//! Draws a rounded rectangle defined by a RECT2 struct,
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawRoundedRect(RECT2 rect, double radiusX, double radiusY, double strokeWidth = 1.0);

	//! Fills the interior of a rounded rectangle defined by 4 numbers representing the left side, top side, the right side and the bottom side
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	bool FillRoundedRect(double left, double top, double right, double bottom, double radiusX, double radiusY);

	//! Fills the interior of a rounded rectangle defined by two coordinates: topleft and rightbottom,
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	bool FillRoundedRect(DOUBLE2 topLeft, DOUBLE2 rightbottom, double radiusX, double radiusY);

	//! Fills the interior of a rounded rectangle defined by a RECT2 struct,
	//!   the x-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	//!   the y-radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
	bool FillRoundedRect(RECT2 rect, double radiusX, double radiusY);

	//! Draws the outline of the specified ellipse using the specified bounding rect
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	//! If this parameter isn't specified, it defaults to 1.0f. The stroke is centered on the line.
	bool DrawEllipse(RECT2 boundingRect, double strokeWidth = 1.0);

	//! Draws the outline of the specified ellipse using the specified position and radius
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	//! If this parameter isn't specified, it defaults to 1.0f. The stroke is centered on the line.
	bool DrawEllipse(DOUBLE2 centerPt, double radiusX, double radiusY, double strokeWidth = 1.0);

	//! Draws the outline of the specified ellipse using the specified position and radius
	//! strokeWidth: The width of the stroke, in device-independent pixels. The value must be greater than or equal to 0.0f. 
	bool DrawEllipse(double centerX, double centerY, double radiusX, double radiusY, double strokeWidth = 1.0);

	//! Paints the outline of the specified ellipse using the specified bounding rect
	bool FillEllipse(RECT2 boundingRect, double strokeWidth = 1.0);

	//! Paints the interior of the specified ellipse using the specified position and radius
	bool FillEllipse(double centerX, double centerY, double radiusX, double radiusY);

	//! Paints the interior of the specified ellipse using the specified position and radius
	bool FillEllipse(DOUBLE2 centerPt, double radiusX, double radiusY);

	//! Draws text in the specified rectangle 
	bool DrawString(const String& textRef, RECT2 boundingRect);

	//! Draws text in the specified rectangle the topleft corner of the rectange is defined by the param topLeft
	//! The params right and bottom are optional, if left out they are set to the max value of an float type 
	bool DrawString(const String& textRef, DOUBLE2 topLeft, double right = -1, double bottom = -1);

	//! Draws text in the specified rectangle; the topleft corner of the rectange is defined by the params xPos and yPos
	//! The params right and bottom are optional, if left out they are set to the max value of an float type 
	bool DrawString(const String& textRef, double xPos, double yPos, double right = -1, double bottom = -1);

	//! Draws an image on position x:0, and y:0. Assuming that matrices are used to define the position.
	bool DrawBitmap(Bitmap* bmpPtr);

	//! Draws an image on the position defined by x and y
	bool DrawBitmap( Bitmap* bmpPtr, double x, double y );

	//! Draws an image on the position defined by position
	bool DrawBitmap( Bitmap* bmpPtr, DOUBLE2 position );

	//! Draws an image on position x:0, and y:0. Assuming that matrices are used to define the position.
	//! srcRect: defines the cliprect on the source image. Allows to draw a part of an image
	bool DrawBitmap( Bitmap* bmpPtr, RECT2 srcRect );

	//! Draws an image on the position defined by x and y
	//! srcRect: defines the cliprect on the source image. Allows to draw a part of an image
	bool DrawBitmap( Bitmap* bmpPtr, double x, double y, RECT2 srcRect );

	//! Draws an image on the position defined by position
	//! srcRect: defines the cliprect on the source image. Allows to draw a part of an image
	bool DrawBitmap( Bitmap* bmpPtr, DOUBLE2 position, RECT2 srcRect );

// Matrix
	//! Sets the matrix that defines world space
	void SetWorldMatrix(const MATRIX3X2& mat);

	//! Returns the matrix that defines world space
	MATRIX3X2 GetWorldMatrix();

	//! Sets the matrix that defines view space
	void SetViewMatrix(const MATRIX3X2& mat);

	//! Returns the matrix that defines view space
	MATRIX3X2 GetViewMatrix();

// Other
	//! Terminate the game from within a game class
	void QuitGame(void);

	//! Create a messagebox
	//! @param textRef the text to display
	void MessageBox(const String &textRef) const;

	//! Darkens the output en displays the physics debug rendering
	//! @param when true it draws the physicsdebug rendering
	//! Remark: is executed after the GamePaint method: allways set the 
	//! view matrix to the desired matrix at the end of the GamePaint
	void EnablePhysicsDebugRendering(bool enable);

	//! Bitmaps are drawn using linear interpolation: slow, results in blurred pixels
	void SetBitmapInterpolationModeLinear();

	//! Bitmaps are drawn using nearest neighbour interpolation: fast,  results in big pixels
	void SetBitmapInterpolationModeNearestNeighbor();

	//! Set the font that is used to render text
	void SetFont(Font* fntPtr);

	//! Returns the font that is used to render text
	Font* GetFont();

	//! Set the built-in font asthe one to be used to render text 
	void SetDefaultFont();

	//! Sets the color of the brush that is used to draw and fill
	//! Example: GAME_ENGINE->SetColor(COLOR(255,127,64));
	void SetColor(COLOR colorVal);

	// Accessor Methods
	//! Returns the color of the brush used to draw and fill
	//! Example: COLOR c = GAME_ENGINE->GetColor();
	COLOR GetColor();

	// Returns the width of the client area of the window
	int	GetWidth() const;

	// Returns the height of the client area of the window
	int	GetHeight() const;

// for those who wants more control ;-)
	//! returns pointer to the Audio System object
	FmodSystem* GetFmodSystem() const;

	//! returns pointer to the box2D world object
	b2World* GetBox2DWorld(){ return m_Box2DWorldPtr; }

	//! Set the Box2D world gravity vector
	void SetGravity(DOUBLE2 gravity);

private:

#ifndef WRAPPER_LIB
	// General Methods used by GameWinMain
	void SetGame(AbstractGame* gamePtr);
	int Run(HINSTANCE hInstance, int iCmdShow);
	static void Destroy();
#endif

	LRESULT HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

	// Box2D virtual overloads
	virtual void BeginContact(b2Contact* contactPtr);
	virtual void EndContact(b2Contact* contactPtr);
	virtual void PreSolve(b2Contact* contactPtr, const b2Manifold* oldManifoldPtr);
	virtual void PostSolve(b2Contact* contactPtr, const b2ContactImpulse* impulsePtr) {};
	virtual void SayGoodbye(b2Joint* jointPtr);
	virtual void SayGoodbye(b2Fixture* fixturePtr);

	// Internal use only
	void ApplyGameSettings(GameSettings &gameSettingsRef);
	// Internal use only
	void SetIcon(WORD wIcon);
	// Internal use only
	void SetSmallIcon(WORD wSmallIcon);
	// Internal use only
	HINSTANCE				GetInstance() const;
	// Internal use only
	HWND					GetWindow() const;
	// Internal use only
	String					GetTitle() const;
	// Internal use only
	WORD					GetIcon() const;
	// Internal use only
	WORD					GetSmallIcon() const;
	// Internal use only
	bool					GetSleep() const;
	// Internal use only
	ID2D1Factory*			GetD2DFactory() const;
	// Internal use only
	IWICImagingFactory*		GetWICImagingFactory() const;
	// Internal use only
	ID2D1HwndRenderTarget*	GetHwndRenderTarget() const;
	// Internal use only
	IDWriteFactory*			GetDWriteFactory() const;

	bool RegisterWindowClass();

	bool OpenWindow(int iCmdShow);

	// Reset all data members; prepare for a game
	void ResetGameData();

	// Set when the game loses focus
	void SetSleep(bool bSleep);

	// Private Mutator Methods	
	void SetInstance(HINSTANCE hInstance);
	void SetWindow(HWND hWindow);

	// SetTitle automatically sets the window class name to the same name as the title - easier for students 
	void SetTitle(const String& titleRef);

	void SetWidth(int iWidth);
	void SetHeight(int iHeight);

	// If enable == true  --> then the game framerate is locked to the framerate of the attached display (60 fps)
	// If enable == false --> then the game framerate is free and depends on the rendertime. 
	void EnableVSync(bool bEnable = true);

	// ConsoleCreate: To be called from within in the GameInitialize method to prevent main window focus loss
	void ConsoleCreate();

	// True enable smooth drawing and filling of shapes
	void EnableAntiAlias(bool isEnabled);

	// Private Draw Methods
	bool CanIPaint() const;

	// Direct2D methods
	void D2DBeginPaint();
	bool D2DEndPaint();
	void CreateDeviceIndependentResources();
	void DestroyDeviceIndependentResources();
	void CreateD2DFactory();
	void CreateWICFactory();
	void CreateWriteFactory();
	void CreateDeviceResources();
	void DiscardDeviceResources();
	void ExecuteDirect2DPaint();
	void GUITick(double deltaTime);
	void GUIPaint();
	void GUIConsumeEvents();

	// Member Variables
	HINSTANCE           m_hInstance = NULL;
	HWND                m_hWindow = NULL;
	String              m_Title;
	WORD                m_wIcon = 0, m_wSmallIcon = 0;
	int                 m_iWidth = 0, m_iHeight = 0;
	bool                m_bSleep = true;
	AbstractGame*		m_GamePtr = nullptr;
	HANDLE				m_ConsoleHandle = NULL;
	// Draw assistance variables
	bool				m_bPaintingAllowed = false;
	bool				m_bVSync = true;
	// Direct2D
	bool							m_bInitialized = false;
	ID2D1Factory*					m_D2DFactoryPtr = nullptr;
	IWICImagingFactory*				m_WICFactoryPtr = nullptr;
	ID2D1HwndRenderTarget*			m_RenderTargetPtr = nullptr;
	IDWriteFactory*					m_DWriteFactoryPtr = nullptr;
	PrecisionTimer*					m_GameTickTimerPtr = nullptr;

	// Direct2D Paint variables
	ID2D1SolidColorBrush*			m_ColorBrushPtr = nullptr;
	D2D1_ANTIALIAS_MODE				m_AntialiasMode = D2D1_ANTIALIAS_MODE_ALIASED;
	MATRIX3X2						m_MatWorld, m_MatView;
	D2D1_BITMAP_INTERPOLATION_MODE	m_BitmapInterpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;	// used when painting scaled bitmaps:
	Font*							m_DefaultFontPtr = nullptr;			// Default Font --> deleted in destructor
	Font*							m_UserFontPtr = nullptr;			// the pointer the user defines using SetFont()

	// GUI array
	std::vector<GUIBase*> m_GUIPtrArr;

	// Input manager
	InputManager* m_InputPtr = nullptr;

	// Box2D
	b2World *m_Box2DWorldPtr = nullptr;
	double m_Box2DTime = 0;
	Box2DDebugRenderer m_Box2DDebugRenderer;
	bool m_DebugRendering = false;
	double m_PhysicsTimeStep = 1;
	DOUBLE2 m_Gravity; 
	
	//Audio
	AudioSystem *m_XaudioPtr = nullptr;
	FmodSystem *m_FmodSystemPtr = nullptr;

	//Friends
	friend class GUIBase;
	friend class TextBox;
	friend class Button;
	friend class InputManager;
	friend class Font;
	friend class Bitmap;
	friend LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
	friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char * szCmdLine, int iCmdShow);
};
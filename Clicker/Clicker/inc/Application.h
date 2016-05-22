//-----------------------------------------------------------------------------
//Author: Tommas Solarino
//Description: Is called in the Application class
//-----------------------------------------------------------------------------

#ifndef STARTUP
#define STARTUP

struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	//Set up GLFW
	void SetUpGLFW();

	void Update();
	void Draw();

	void Run();
private:
	void DrawSettings();				//Draws and hand;es the other settings displayed to the user
	void DrawStartEndButton();			//Draws and handles the start/end button
	void SetOrPopStyles(bool a_set);	//Sets or Pops all the ImGui window styles
	void ManageClicks();				//Calculates and clicks each click
	void SetRunning(bool a_running);	//Sets if theclicker is running or not
	void Click();						//handles clicking the mouse

	float TimeUntilNextClick();			//Calculates the time until the next click
	float TimeUntilCompletion();		//Calculates the time until completion

	GLFWwindow* m_pWindow;		//Window pointer

	float m_currentTime = 0.0f;	//floats for calculating delta time
	float m_deltaTime = 0.0f;	//floats for calculating delta time
	float m_lastTime = 0.0f;	//floats for calculating delta time

	int m_windowWidth;			//Window dimentions
	int m_windowHeight;			//Window dimentions

	int m_clicks;				//The amount of clicks that happen between "m_startEndTimes"
	int m_clickCount;			//The amount of clicks made while running
	int m_repeatAmount;			//Amount of repeats that the user has defined 
	int m_repeatCount;			//Is how many times we have currenntly repeated
	int m_startDelay;			//Delay after pressing start before clicking begins

	float m_columnWidth;		//The width of each setting
	float m_rowHeight;			//The Height of each setting
	float m_row2Height;			//The Height of the second row each setting

	float m_countDownTimer;		//This is the timer that counts down the start delay

	float m_startEndTimes[2];	//Times between which clicks occur
	float m_lastClickTime;		//Time Since Last Click
	float m_timeSinceStart;		//Time Since Starting Clicking
	float m_estimatedTimeToEnd;	//Enstimated Time Until Completion of clicking
	float m_clickIntervals;		//How often a click occurs in seconds
	float m_timeUntilNextClick;	//Time until next click occurs

	bool m_repeat;				//Does the user want to repeat
	bool m_randomClicks;		//Does the user want random clicks
	bool m_runningClicker;		//is true if the user has pressed start
	bool m_canChangeSettings;	//Is true if the clicker ISNT running
	bool m_countingDown;		//Is true after clicking start and will begin countdown
};
#endif
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

	void SetOrPopStyles(bool a_set);

	float m_currentTime = 0.0f;
	float m_deltaTime = 0.0f;
	float m_lastTime = 0.0f;

	int m_windowWidth;
	int m_windowHeight;

	int m_clicks;				//The amount of clicks t happen between "m_startEndTimes"
	int m_repeatAmount;			//Amount of repeats that the user has defined 
	float m_startEndTimes[2];	//Times between which clicks occur
	float m_startDelay;			//Delay after pressing start before clicking begins

	float m_lastClickTime;		//Time Since Last Click
	float m_timeSinceStart;		//Time Since Starting Clicking
	float m_estimatedTimeToEnd;	//Enstimated Time Untill Completion

	bool m_repeat;				//Does the user want to repeat
	bool m_randomClicks;		//Does the user want random clicks

	GLFWwindow* m_pWindow;
};
#endif
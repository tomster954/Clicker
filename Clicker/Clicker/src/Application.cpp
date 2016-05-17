#include "Application.h"

#include <imgui.h>
#include "imgui_impl_glfw.h"

#include <Windows.h>
#include <glfw3.h>
#include <iostream>
#include <string>

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

Application::Application() :
m_currentTime(0.0f),
m_deltaTime(0.0f),
m_lastTime(0.0f),
m_startDelay(3.0f),
m_lastClickTime(0.0f),
m_timeSinceStart(0.0f),
m_estimatedTimeToEnd(0.0f),
m_rowHeight(50.f),
m_clicks(0),
m_clickCount(0),
m_repeatCount(0)
{
	//Sets up GLFW
	SetUpGLFW();

	//Initialise ImGui
	ImGui_ImplGlfw_Init(m_pWindow, true);

	//Main run loop
	Run();
}

Application::~Application()
{
	//Cleaning up 
	ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void Application::SetUpGLFW()
{
	//Setting up error call back
	glfwSetErrorCallback(error_callback);
	
	//If initialisation failed
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
		return;
	}

	//Create a new window
	m_pWindow = glfwCreateWindow(640, 200, "Auto Clicker", NULL, NULL);

	//If window initialisation failed
	if (!m_pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
		return;
	}

	//Making the new window the current context
	glfwMakeContextCurrent(m_pWindow);

	//Setting the background to white
	glClearColor(255, 255, 255, 255);
}

void Application::Run()
{
	while (!glfwWindowShouldClose(m_pWindow))
	{
		if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)	glfwSetWindowShouldClose(m_pWindow, true);

		glfwGetFramebufferSize(m_pWindow, &m_windowWidth, &m_windowHeight);
		glViewport(0, 0, m_windowWidth, m_windowHeight);
		
		Update();
		Draw();

		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}
}

void Application::Update()
{
	//Calculating deltatime
	m_currentTime = (float)glfwGetTime();
	m_deltaTime = m_currentTime - m_lastTime;
	m_lastTime = m_currentTime;

	//set whether the user can change settings
	if (m_canChangeSettings == m_runningClicker)
		m_canChangeSettings = !m_runningClicker;

	//If the clicker is running and we are no longer counting down
	if (m_runningClicker && !m_countingDown)
	{
		//manages clicking
		ManageClicks();

		//counting up from the last click
		m_lastClickTime += m_deltaTime;

		//Calculating the time since starting
		m_timeSinceStart += m_deltaTime;

		//counting down the estimated end
		m_estimatedTimeToEnd -= m_deltaTime;
	}
	else
	{
		//Sets the time since clicking to 0
		if (m_lastClickTime > 0)
			m_lastClickTime = 0;

		//Sets the time since running to 0
		if (m_timeSinceStart > 0)
			m_timeSinceStart = 0;

		//time between each click
		m_clickIntervals = (m_startEndTimes[1] - m_startEndTimes[0]) / (m_clicks + 1);

		//while changing setting the estimated end is calculated
		m_estimatedTimeToEnd = TimeUntilCompletion();

		//before we start calculate the time until the next click
		m_timeUntilNextClick = TimeUntilNextClick();
	}
	
	//Size of the individual settings blocks
	m_columnWidth = m_windowWidth * 0.33 - 4;
	m_row2Height = m_rowHeight + 5;
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render new frame
	ImGui_ImplGlfw_NewFrame();
	
		//next window attributes
		ImGui::SetNextWindowSize(ImVec2(m_windowWidth, m_windowHeight), ImGuiSetCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
		
		//Set styles
		SetOrPopStyles(true);
		//BEGIN
		if (ImGui::Begin("Auto Clicker		By Tom Solarino		version 1.0", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders))
		{
			//Draw the setting and times
			DrawSettings();

			//Start button
			DrawStartEndButton();

			ImGui::Separator();			
		}
		//End
		ImGui::End();
		//Pop Styles
		SetOrPopStyles(false);
		
	//Render IMGUI
	ImGui::Render();
}

void Application::DrawSettings()
{
	//This makes sure you can only change setting when not running the clicker
	ImGuiWindowFlags flags;
	if (!m_canChangeSettings)
		flags = ImGuiWindowFlags_NoInputs;
	else
		flags = 0;

	//Setting 1
	//--------------------------------------------------------------------------
	if (ImGui::BeginChild("Setting1", ImVec2(m_columnWidth, m_rowHeight), 1, flags))
	{
		ImGui::Text("Clicks	");
		ImGui::InputInt("", &m_clicks, 1, 1, 0);
		if (m_clicks < 1)
			m_clicks = 1;
	}ImGui::EndChild();
	//--------------------------------------------------------------------------

	//Setting2
	//--------------------------------------------------------------------------
	ImGui::SameLine();
	if (ImGui::BeginChild("Setting2", ImVec2(m_columnWidth, m_rowHeight), 1, flags))
	{
		ImGui::Text("Click Between (sec)");
		ImGui::InputFloat2("", &m_startEndTimes[0], 2, 0);
		if (m_startEndTimes[0] < 0)
			m_startEndTimes[0] = 0;
		if (m_startEndTimes[1] < 0)
			m_startEndTimes[1] = 0;

		//checks that the start time is less than the end time
		if (m_startEndTimes[1] < m_startEndTimes[0])
			m_startEndTimes[1] = m_startEndTimes[0];

	}ImGui::EndChild();
	//--------------------------------------------------------------------------

	//Setting3
	//--------------------------------------------------------------------------
	ImGui::SameLine();
	if (ImGui::BeginChild("Setting3", ImVec2(m_columnWidth, m_rowHeight), 1, flags))
	{
		ImGui::Text("Start Delay (sec)");
		ImGui::InputInt("", &m_startDelay, 1, 1, 0);

		//if we are not couting down set the timer to the current count down value
		if (!m_countingDown)
		m_countDownTimer = (float)m_startDelay;

	}ImGui::EndChild();
	//--------------------------------------------------------------------------

	//Setting4
	//--------------------------------------------------------------------------
	if (ImGui::BeginChild("Setting4", ImVec2(m_columnWidth, m_row2Height), 1, flags))
	{
		ImGui::Checkbox("Repeat", &m_repeat);
		ImGui::InputInt("", &m_repeatAmount, 1, 1, 0);

		if (!m_repeat)
			m_repeatAmount = 0;
		if (m_repeatAmount < 1)
			m_repeatAmount = 1;

	}ImGui::EndChild();
	//--------------------------------------------------------------------------

	//Setting5
	//--------------------------------------------------------------------------
	ImGui::SameLine();
	if (ImGui::BeginChild("Setting5", ImVec2(m_columnWidth, m_row2Height), 1, flags))
	{
		ImGui::Checkbox("Randomise clicks", &m_randomClicks);
	}ImGui::EndChild();
	//--------------------------------------------------------------------------

	//Setting6
	//--------------------------------------------------------------------------
	ImGui::SameLine();
	if (ImGui::BeginChild("Setting6", ImVec2(m_columnWidth, m_row2Height), 1, flags))
	{
		std::string lastTime = std::to_string(m_lastClickTime);
		std::string SinceStart = std::to_string(m_timeSinceStart);
		std::string EndTime = std::to_string(m_estimatedTimeToEnd);

		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Last Click:-------|"); ImGui::SameLine(); ImGui::Text(lastTime.c_str());
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Time since start:-|"); ImGui::SameLine(); ImGui::Text(SinceStart.c_str());
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Estimated end:----|"); ImGui::SameLine(); ImGui::Text(EndTime.c_str());
	}ImGui::EndChild();
	//--------------------------------------------------------------------------

}

void Application::DrawStartEndButton()
{
	std::string text;
	if (m_runningClicker && !m_countingDown)
		text = "End\n ";
	else
	{
		text = "Start\n  ";
		text += std::to_string(m_countDownTimer);
	}

	ImGui::Dummy(ImVec2(m_columnWidth, m_rowHeight + 5));
	ImGui::SameLine();

	if (ImGui::Button(text.c_str(), ImVec2(m_columnWidth + 1, m_rowHeight + 5)))
	{
		//if the clicker isnt running when we press the button set countdown to true
		if (!m_runningClicker)
			m_countingDown = true;
		SetRunning(!m_runningClicker);
	}

	//will countdown the start delay
	if (m_countingDown)
	{
		m_countDownTimer -= m_deltaTime;
	}
	
	if (m_countDownTimer <= 0 && m_countingDown)
	{
		m_countDownTimer = 0.0f;
		m_countingDown = false;
	}
}

void Application::SetOrPopStyles(bool a_set)
{
	if (a_set)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 7));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(3, 3));
	}
	else
	{
		//Pop styles
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}
}

void Application::ManageClicks()
{
	if (m_timeSinceStart >= m_timeUntilNextClick)
	{
		Click();

		//when a click occurs recalculate the time until end
		m_estimatedTimeToEnd = TimeUntilCompletion();

		//when a click occurs recalculate the time until next click
		m_timeUntilNextClick = TimeUntilNextClick();
	}
}

float Application::TimeUntilCompletion()
{
	//amount of time passed so far based off clicks
	float timeSoFar = m_clickIntervals * (m_clickCount);

	//one repeats worth of time
	float oneRepeatTime = m_startEndTimes[1] - m_clickIntervals;
	
	//The total amount of time
	float total = oneRepeatTime * (m_repeatAmount - m_repeatCount);

	//taking away how much time has gone by
	total -= timeSoFar;

	return total;
}

void Application::SetRunning(bool a_running)
{
	m_runningClicker = a_running;
}

void Application::Click()
{
	m_clickCount++;
	m_lastClickTime = 0;

	//Checks to see if we have finished clicking for one repeat
	if (m_clickCount >= m_clicks)
	{
		m_repeatCount++;
		m_clickCount = 0;

		if (m_repeatCount >= m_repeatAmount)
		{
			SetRunning(false);
			m_repeatCount = 0;
		}
	}

	//Left Click
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

float Application::TimeUntilNextClick()
{
	//Checks thats we have click intervals
	if (m_clickIntervals <= 0)
		return m_startEndTimes[0];

	float nextTime;

	int Num = m_clickIntervals * 1000;
	float randClick = rand() % Num;     // randClick in the range 1 to 100
	randClick /= 1000;

	//Time when the next click occurs
	if (m_randomClicks)
		nextTime = m_startEndTimes[0] + m_clickIntervals * m_clickCount + randClick;//calcs the time one click short an adds the rand on the end
	else
		nextTime = m_startEndTimes[0] + m_clickIntervals * (m_clickCount + 1);		//calcs the time until next click

	//adding the previouse repeats time to the next repeat clicks
	nextTime += (m_startEndTimes[1] - m_clickIntervals) * m_repeatCount;

	return nextTime;
}
#include "Application.h"

#include <imgui.h>
#include "imgui_impl_glfw.h"

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
m_estimatedTimeToEnd(0.0f)
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
	m_pWindow = glfwCreateWindow(640, 480, "Auto Clicker", NULL, NULL);

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
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float colwidth = m_windowWidth * 0.33 - 4;
	float row1Height = 50.f;
	float row2Height = 55.f;

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
			//Setting 1
			//--------------------------------------------------------------------------
			if (ImGui::BeginChild("Setting1", ImVec2(colwidth, row1Height), 1, 0))
			{				
				ImGui::Text("Clicks	");
				ImGui::InputInt("", &m_clicks, 1, 1, 0);
			}ImGui::EndChild();
			//--------------------------------------------------------------------------

			//Setting2
			//--------------------------------------------------------------------------
			ImGui::SameLine();
			if (ImGui::BeginChild("Setting2", ImVec2(colwidth, row1Height), 1, 0))
			{				
				ImGui::Text("Click Between (sec)");
				ImGui::InputFloat2("", &m_startEndTimes[0], 2, 0);
			}ImGui::EndChild();
			//--------------------------------------------------------------------------
			
			//Setting3
			//--------------------------------------------------------------------------
			ImGui::SameLine();
			if (ImGui::BeginChild("Setting3", ImVec2(colwidth, row1Height), 1, 0))
			{
				ImGui::Text("Start Delay (sec)");
				ImGui::InputFloat("", &m_startDelay, 0.1f, 1.0f, 2, 0);
				if (m_startDelay < 0)
					m_startDelay = 0;
			}ImGui::EndChild();
			//--------------------------------------------------------------------------

			//Setting4
			//--------------------------------------------------------------------------
			if (ImGui::BeginChild("Setting4", ImVec2(colwidth, row2Height), 1, 0))
			{ 
				if (!m_repeat)
					m_repeatAmount = 0;

				ImGui::Checkbox("Repeat", &m_repeat);
				ImGui::InputInt("", &m_repeatAmount, 1, 1, 0);
			}ImGui::EndChild();
			//--------------------------------------------------------------------------

			//Setting5
			//--------------------------------------------------------------------------
			ImGui::SameLine();
			if (ImGui::BeginChild("Setting5", ImVec2(colwidth, row2Height), 1, 0))
			{
				ImGui::Checkbox("Randomise clicks", &m_randomClicks);
			}ImGui::EndChild();
			//--------------------------------------------------------------------------

			//Setting6
			//--------------------------------------------------------------------------
			ImGui::SameLine();
			if (ImGui::BeginChild("Setting6", ImVec2(colwidth, row2Height), 1, 0))
			{
				const char* lastTime = std::to_string(m_lastClickTime).c_str();
				const char* SinceStart = std::to_string(m_timeSinceStart).c_str();
				const char* EndTime = std::to_string(m_estimatedTimeToEnd).c_str();

				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Last Click:-------|"); ImGui::SameLine(); ImGui::Text(lastTime);
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Time since start:-|"); ImGui::SameLine(); ImGui::Text(SinceStart);
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.1f, 1.0f), "Estimated end:----|"); ImGui::SameLine(); ImGui::Text(EndTime);
				
			}ImGui::EndChild();
			//--------------------------------------------------------------------------
			
			//Start button
			//--------------------------------------------------------------------------
			ImGui::Dummy(ImVec2(colwidth, row2Height));
			ImGui::SameLine();
			std::string text = "Start\n";
			text += std::to_string(m_startDelay);
			if (ImGui::Button(text.c_str(), ImVec2(colwidth + 1, row2Height)))
			{
			
			}
			//--------------------------------------------------------------------------

			ImGui::Separator();			
		}
		//End
		ImGui::End();
		//Pop Styles
		SetOrPopStyles(false);
		
	//Render IMGUI
	ImGui::Render();
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
#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	m_uOrbits = 5;
	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		std::vector<vector3> circle;
		
		vector3 leftPoint;
		vector3 rightPoint = vector3(fSize * (1 / 180.0f), 0, 0);
		vector3 centerPoint = vector3(0, 0, 0);
		double degreeInterval = 360.0f / i;

		for (int j = 0; j < i; j++)
		{
			float nextDegree = degreeInterval * j;
			leftPoint = rightPoint;
			rightPoint = vector3(fSize * (std::cos((nextDegree * 3.1415f) / 180.0f)), fSize * (std::sin((nextDegree * 3.1415f) / 180.0f)), 0);

			circle.push_back(rightPoint);
		}
		circlesList.push_back(circle);
		stepList.push_back(0);

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	for each(std::vector<vector3> circle in circlesList)
	{
		for each(vector3 location in circle)
		{
			std::cout << "X: " << location.x << " Y: " << location.y << "Z: " << location.z << "\n\n";
		}
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		static DWORD DStartingTime = GetTickCount();
		DWORD DCurrentTime = GetTickCount();
		DWORD DDelta = DCurrentTime - DStartingTime;
		float fTimer = static_cast<float>(DDelta / 1000.0f);

		vector3 v3StartPos = circlesList[i][stepList[i]];
		vector3 v3EndPos = circlesList[i][(stepList[i] + 1) % circlesList[i].size()];

		float fTimeBetweenStops = 1.0f;//in seconds
		float fPercentage = MapValue(fTimer, 0.0f, fTimeBetweenStops, 0.0f, 1.0f);

		//calculate the current position
		vector3 v3CurrentPos = glm::lerp(v3StartPos, v3EndPos, fPercentage);
		if (fPercentage >= 1.0f)
		{
			for (int j = 0; j < stepList.size(); j++)
			{
				stepList[j]++;
				stepList[j] %= circlesList[j].size();//make sure we are within boundries
			}
			
			DStartingTime = GetTickCount();
			
		}


		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}
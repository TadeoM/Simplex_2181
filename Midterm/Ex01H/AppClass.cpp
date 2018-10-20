#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This part initialize the camera position so I can see the scene better; 
		the guide cube and the tip of the pencil so all solutions on the exam 
		look the same to the grader
	*/
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 35.0f, ZERO_V3, AXIS_Y);

	m_pGuideCube = new MyMesh();
	m_pGuideCube->GenerateCube(10.0f, C_WHITE);

	m_pMesh = new MyMesh();
	m_pMesh->GenerateCone(1.0f, 2.0f, 8, vector3(0.0f, 0.0f, 0.0f));
	

#pragma endregion

	//Please change to your name and email
	m_sProgramer = "Tadeo Menichelli - txm1918@rit.edu";

	MyMesh* m_pHandle = new MyMesh();
	MyMesh* m_pTip = new MyMesh();
	m_pHandle->GenerateCylinder(1.0f, 2.0f, 8, vector3(1.0f, 1.0f, 1.0f));
	m_pTip->GenerateCone(1.0f, 1.0f, 8, vector3(0.0f, 0.0f, 0.0f));
	m_pPencil.push_back(m_pTip);
	m_pPencil.push_back(m_pHandle);
	std::vector<vector3> testPos =  m_pHandle->GetVertexList();
	
	m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(-1.0f, 0.0f, 0.0f));
	m_stopsList = m_pGuideCube->GetVertexList();
	for (int i = 0; i < m_stopsList.size(); i++)
	{
		std::cout << "X: " << m_stopsList[i].x << " Y: " << m_stopsList[i].y << " Z: " << m_stopsList[i].z <<std::endl;
	}
}
void Application::Update(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This updates the internal clock of the Simplex library and process the Arcball
	*/
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();
#pragma endregion
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer
	static int step = 0;

	// get where the tip and hanlde of pencils will start and end at a certain step
	vector3 v3_TipStartPoint = m_stopsList[step];
	vector3 v3_TipEndPoint = m_stopsList[(step + 1) % m_stopsList.size()];
	vector3 v3_HandleStartPoint = m_stopsList[step];
	vector3 v3_HandleEndPoint = m_stopsList[(step + 1) % m_stopsList.size()];
	// gets the percentage between point
	float fPercentage = MapValue(fTimer, 0.0f, m_fTimeBetweenStops, 0.0f, 1.0f);

	//calculate the current position
	vector3 v3_TipCurrentPos = glm::lerp(v3_TipStartPoint, v3_TipEndPoint, fPercentage);
	
	//calculate the current position
	matrix4 m4Translation = glm::translate(IDENTITY_M4, v3_TipCurrentPos);
	matrix4 m4Tip = m4Translation;

	static int x = 1;
	static bool rotated = false;
	if (x == 6 && rotated == false)
	{
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(0.0f, 0.0f, 1.0f));
	}
	else if (x == 12 && rotated == false)
	{
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(-1.0f, 0.0f, 0.0f));
	}
	else if (x == 19 && rotated == false)
	{
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(0.0f, 0.0f, 1.0f));
	}
	else if (x == 25 && rotated == false)
	{
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(-1.0f, 0.0f, 0.0f));
	}
	else if (x == 31 && rotated == false)
	{
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(0.0f, 0.0f, 1.0f));
	}
	else if (x == 36 && rotated == false)
	{
		x = 0;
		rotated = true;
		m_qOrientation = m_qOrientation * glm::angleAxis(glm::radians(90.0f), vector3(-1.0f, 0.0f, 0.0f));
	}

	
	m4Tip *= ToMatrix4(m_qOrientation);
	static int timesRotated = 0;

	if (fPercentage >= 1.0f)
	{
		x++;
		step++;
		rotated = false;
		fTimer = m_pSystem->GetDeltaTime(uClock);//restart the clock
		step %= m_stopsList.size();//make sure we are within boundries
	}
	//matrix4 m4Handle = glm::translate(IDENTITY_M4, v3HandlePos);
	m_pPencil[0]->Render(m4Projection, m4View, m4Tip);
	//m_pPencil[1]->Render(m4Projection, m4View, m4Handle);

#pragma region DOES NOT NEED CHANGES
	/*
		This part does not need any changes at all, it is just for rendering the guide cube, the 
		skybox as a frame of reference and the gui display
	*/
	//Render the guide cube
	//m_pGuideCube->Render(m4Projection, m4View, ToMatrix4(m_qArcBall)); 

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();
	//clear the render list
	m_pMeshMngr->ClearRenderList();
		
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
#pragma endregion
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	//I already deallocated the memory for the meshes if you allocate new memory please
	//deallocate it before ending the program
	SafeDelete(m_pGuideCube);
}
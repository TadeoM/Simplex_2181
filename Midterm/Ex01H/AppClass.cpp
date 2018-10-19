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

	MyMesh* m_pHandle = new MyMesh();
	MyMesh* m_pTip = new MyMesh();
	m_pHandle->GenerateCylinder(1.0f, 2.0f, 8, vector3(1.0f, 1.0f, 1.0f));
	m_pTip->GenerateCone(1.0f, 1.0f, 8, vector3(0.0f, 0.0f, 0.0f));
	m_pPencil.push_back(m_pTip);
	m_pPencil.push_back(m_pHandle);
#pragma endregion

	//Please change to your name and email
	m_sProgramer = "Tadeo Menichelli - txm1918@rit.edu";
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
	static uint uClock = m_pSystem->GenClock(); //generate a new clock
	float fTimer = m_pSystem->GetTimeSinceStart(uClock); //get the start time for that clock
	
	//calculate the current position
	vector3 v3TipPos = glm::lerp(vector3(0.0f, 2.0f, 0.0f), vector3(0.0f, 4.0f, 0.0f), fTimer);
	vector3 v3HandlePos = glm::lerp(vector3(0.0f, 0.5f, 0.0f), vector3(0.0f, 2.5f, 0.0f), fTimer);

	matrix4 m4Tip = glm::translate(IDENTITY_M4, v3TipPos);
	matrix4 m4Handle = glm::translate(IDENTITY_M4, v3HandlePos);
	
	m_pPencil[0]->Render(m4Projection, m4View, m4Tip);
	m_pPencil[1]->Render(m4Projection, m4View, m4Handle);
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
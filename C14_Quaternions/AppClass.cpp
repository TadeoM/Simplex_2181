#include "AppClass.h"
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//initializing the model
	m_pModel = new Simplex::Model();

	//Load a model
	m_pModel->Load("Minecraft\\Steve.obj");

	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(2.0f, C_RED);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Get a timer
	static uint uClock = m_pSystem->GenClock();
	float fTimer = m_pSystem->GetTimeSinceStart(uClock);
	float fDeltaTime = m_pSystem->GetDeltaTime(uClock);

#pragma region SLERP
	if (false)
	{
		quaternion q1;
		quaternion q2 = glm::angleAxis(glm::radians(359.9f), vector3(0.0f, 0.0f, 1.0f));
		float fPercentage = MapValue(fTimer, 0.0f, 2.0f, 0.0f, 1.0f);
		quaternion qSLERP = glm::mix(q1, q2, fPercentage);
		m_m4Steve = glm::toMat4(qSLERP);

	}
#pragma endregion
#pragma region translate vector orientation into a matrix
	if (false)
	{
		matrix4 m4OrientX = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.x), vector3(1.0f, 0.0f, 0.0f));
		matrix4 m4OrientY = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.y), vector3(0.0f, 1.0f, 0.0f));
		matrix4 m4OrientZ = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.z), vector3(0.0f, 0.0f, 1.0f));

		matrix4 m4Orientation = m4OrientX * m4OrientY * m4OrientZ;
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
#pragma region orientation using quaternions
	if (true)
	{
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
	
	//Attach the model matrix that takes me from the world coordinate system
	m_pModel->SetModelMatrix(m_m4Steve);

	//Send the model to render list
	m_pModel->AddToRenderList();

	
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Model = glm::translate(m_v3Orientation);

	//m4Projection = glm::ortho(-0.0f, 2.0f, -0.0f, 2.0f, 0.01f, 15.0f);
	float fFOV = 90.0f;
	float fAspect = static_cast<float>(m_pSystem->GetWindowWidth()) / static_cast<float>(m_pSystem->GetWindowHeight());
	float fNear = 0.01f;
	float fFar = 20.0f;
	m4Projection = glm::perspective(fFOV, fAspect, fNear, fFar);

	vector3 v3Position; vector3(0.0f, 0.0f, 10.0f);
	vector3 v3Target;
	vector3 v3Up = vector3(0.0f, 1.0f, 0.0f); 

	m4View = glm::lookAt(v3Position, v3Target, v3Up);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();	
	
	//float fovy = 45.0f; // field of view
	//float aspect = m_pSystem->GetWindowWidth() / m_pSystem->GetWindowHeight(); // aspect ratio
	//float zNear = 0.0001f;  // bear clipping plane
	//float zFar = 10000.0f; // far clipping plane

	//m4Projection = glm::perspective(fovy, aspect, zNear, zFar);

	m_pCameraMngr->SetProjectionMatrix(m4Projection);
	m_pCameraMngr->SetViewMatrix(m4View);

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();

	m_pMesh->Render(m4Projection, m4View, m4Model);
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release model
	SafeDelete(m_pModel);

	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}

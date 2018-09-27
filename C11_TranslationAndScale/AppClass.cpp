#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	//m_pMesh = new MyMesh();
	
	direction = 1;
	for (int x = 0; x < 8; x++)
	{
		std::vector<MyMesh*> cubeLine;
		allCubes.push_back(cubeLine);
		for (int y = 0; y < 11; y++)
		{
			m_pCube = new MyMesh();
			m_pCube->GenerateCube(1.0f, C_BLACK);
			if (shape[x][y] == 1)
			{
				allCubes[x].push_back(m_pCube);
			}
			else
			{
				allCubes[x].push_back(nullptr);
			}
		}
	}
	m_pCube = new MyMesh();
	//m_pCube->GenerateCube(1.0f, C_BLACK);

	
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	//m_pMesh->GenerateCube(1.0f, C_BLACK);
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
	static std::vector<std::vector<vector3>> v3Stop;
	ClearScreen();
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 11; y++)
		{
			if (shape[x][y] == 1)
			{
				matrix4 m4View = m_pCameraMngr->GetViewMatrix();
				matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
				vector3 v3StartPoint(y - 6, -x + 5,  0.0f);
				vector3 v3EndPoint(y, -x + 5, 0.0f);
				
				static float fPercentage = 0.0f;

				matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
				vector3 v3CurrentPos = glm::lerp(v3StartPoint, v3EndPoint, fPercentage);
				//matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(xValue, yValue, 0.0f));

				if (fPercentage >= 1.0f || fPercentage < 0.0f)
				{
					std::swap(v3StartPoint, v3EndPoint);
					direction = -direction;
				}
				std::cout << fPercentage << std::endl;
				fPercentage += direction * 0.0001f;

				//matrix4 m4Model = m4Translate * m4Scale;
				matrix4 m4Model = glm::translate(IDENTITY_M4, v3CurrentPos);

				//std::cout << shape[x][y];
				allCubes[x][y]->Render(m4Projection, m4View, m4Model);
			}
		}
	}

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	//value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	matrix4 m4Model = m4Scale * m4Translate;

	m_pCube->Render(m4Projection, m4View, m4Model);
	
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
}
void Application::Release(void)
{
	SafeDelete(m_pCube);

	//release GUI
	ShutdownGUI();
}
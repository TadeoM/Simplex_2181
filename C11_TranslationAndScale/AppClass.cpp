#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	//m_pMesh = new MyMesh();
	

	for (int x = 0; x < 8; x++)
	{
		std::vector<MyMesh*> cubeLine;
		allCubes.push_back(cubeLine);
		for (int y = 0; y < 11; y++)
		{
			MyMesh* m_pCube = new MyMesh();
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
	m_pCube->GenerateCube(1.0f, C_BLACK);

	
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
	ClearScreen();
	/*for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 11; y++)
		{
			if (shape[x][y] == 1)
			{
				matrix4 m4View = m_pCameraMngr->GetViewMatrix();
				matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
				float xValue = y - 6;
				float yValue = -x + 5;
				/*if (count >= 120)
				{
				count = 0;
				}
				if (count >= 50)
				{
				std::cout << "Here" << std::endl;
				count++;
				xValue += 0.01f;
				}
				else
				{
				std::cout << "NO here" << std::endl;
				count++;
				xValue -= 0.01f;
				}


				matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
				matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(xValue, yValue, 3.0f));



				//matrix4 m4Model = m4Translate * m4Scale;
				matrix4 m4Model = m4Scale * m4Translate;

				//std::cout << shape[x][y];
				allCubes[x][y]->Render(m4Projection, m4View, m4Model);
			}
		}
	}*/

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	value += 0.01f;

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
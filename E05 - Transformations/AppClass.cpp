#include "AppClass.h"
void Application::InitVariables(void)
{
	for (size_t i = 0; i < 46; i++)
	{
		cubes[i] = new MyMesh();
		cubes[i]->GenerateCube(1.0f);

	}
	
	/*
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
	*/
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();


	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = 0.0f;
	static bool right = true;

	for (size_t i = 0; i < 46; i++)
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(translations[i].x + value, translations[i].y,translations[i].z));
		matrix4 m4Model = m4Scale * m4Translate;
		cubes[i]->Render(m4Projection, m4View, m4Model);
	}

	if(right) value += 0.01f;
	else value -= 0.01f;
	
	if (value > 3) {
		right = false;
	}
	else if (value < -3) {
		right = true;
	}

	
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
	SafeDelete(m_pMesh);
	for (size_t i = 0; i < 46; i++)
	{
		delete cubes[i];
	}
	delete[] cubes;

	//release GUI
	ShutdownGUI();
}
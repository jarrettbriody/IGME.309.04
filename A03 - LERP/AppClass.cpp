#include "AppClass.h"

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	ifstream inFile;
	string line;
	inFile.open("A03s - LERP.ini"); 
	if (inFile.is_open()) {
		// Read a single line at a time and print it
		while( getline(inFile, line) ){
			//find the string "Orbits", if it exists, create a substring of the number after it and stream it to m_uOrbits
			if (line.find("Orbits") != string::npos) {
				string substr = line.substr(line.find(":") + 1);
				stringstream ss;
				ss << substr;
				ss >> m_uOrbits;
				break;
			}
		}
		inFile.close();
	}
	cout << m_uOrbits << endl;

	//init loops based on number of orbits pulled in through io
	loops = new Loop[m_uOrbits];

	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	//for each orbit
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		//allocate some number of vertices for this orbit
		loops[i - uSides].vertices = new vector3[i];
		loops[i - uSides].vertCount = i;
		//for each subdivision in this orbit
		for (size_t j = 0; j < i; j++)
		{
			//calculate an angle and add a vertex for that pos
			float angle = ((glm::two_pi<float>()) / i) * j;
			loops[i - uSides].vertices[j] = vector3(glm::cos(angle), glm::sin(angle), 0) * (fSize - 0.05f);
		}
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position for each orbit
		if (fTimer - loops[i].timeSinceLast >= loops[i].timeBetween) {
			loops[i].timeSinceLast = fTimer;
			loops[i].currentIndex++;
			loops[i].nextIndex++;
			if (loops[i].nextIndex >= loops[i].vertCount) {
				loops[i].nextIndex = 0;
			}
			if (loops[i].currentIndex >= loops[i].vertCount) {
				loops[i].currentIndex = 0;
			}
		}
		vector3 v3CurrentPos = glm::lerp(loops[i].vertices[loops[i].currentIndex], loops[i].vertices[loops[i].nextIndex], (fTimer - loops[i].timeSinceLast) / loops[i].timeBetween);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.2)), C_WHITE); //0.1
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
	delete[] loops;
	ShutdownGUI();
}
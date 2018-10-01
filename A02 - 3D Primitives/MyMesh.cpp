#include "MyMesh.h"
#include <vector>
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();
	// Replace this with your code
	vector3 topPoint = vector3(0, a_fHeight / 2.0f, 0);
	vector3 bottomCenterPoint = vector3(0, -1.0f * (a_fHeight / 2.0f), 0);
	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//create the two necessary angles to get the two points needed for each face of the cone, calculate the points based on the height and radius and draw a circle at the bottom
		float angle1 = ((glm::two_pi<float>()) / a_nSubdivisions) * i;
		float angle2 = ((glm::two_pi<float>()) / a_nSubdivisions) * (i + 1);
		vector3 point1 = vector3(glm::cos(angle1) * a_fRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle1) * a_fRadius);
		vector3 point2 = vector3(glm::cos(angle2) * a_fRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle2) * a_fRadius);
		AddTri(topPoint,point2, point1);
		AddTri(bottomCenterPoint, point1, point2);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topCenterPoint = vector3(0, a_fHeight / 2.0f, 0);
	vector3 bottomCenterPoint = vector3(0, -1.0f * (a_fHeight / 2.0f), 0);
	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//create the two angles needed to get the four points for each slice of the cylinder, then calculate the points based on height and radius
		float angle1 = ((glm::two_pi<float>()) / a_nSubdivisions) * i;
		float angle2 = ((glm::two_pi<float>()) / a_nSubdivisions) * (i + 1);
		vector3 point1 = vector3(glm::cos(angle1) * a_fRadius, (a_fHeight / 2.0f), glm::sin(angle1) * a_fRadius);
		vector3 point2 = vector3(glm::cos(angle2) * a_fRadius, (a_fHeight / 2.0f), glm::sin(angle2) * a_fRadius);
		vector3 point3 = vector3(glm::cos(angle1) * a_fRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle1) * a_fRadius);
		vector3 point4 = vector3(glm::cos(angle2) * a_fRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle2) * a_fRadius);
		AddTri(topCenterPoint, point2, point1);
		AddTri(bottomCenterPoint, point3, point4);
		AddQuad(point1, point2, point3, point4);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	for (size_t i = 0; i < a_nSubdivisions; i++)
	{
		//create two angles needed to get the eight points for each cross section of the tube, calculate the angles based on their respective radii and the height of the tube
		float angle1 = ((glm::two_pi<float>()) / a_nSubdivisions) * i;
		float angle2 = ((glm::two_pi<float>()) / a_nSubdivisions) * (i + 1);
		vector3 point1 = vector3(glm::cos(angle1) * a_fInnerRadius, (a_fHeight / 2.0f), glm::sin(angle1) * a_fInnerRadius);
		vector3 point2 = vector3(glm::cos(angle1) * a_fOuterRadius, (a_fHeight / 2.0f), glm::sin(angle1) * a_fOuterRadius);
		vector3 point3 = vector3(glm::cos(angle2) * a_fOuterRadius, (a_fHeight / 2.0f), glm::sin(angle2) * a_fOuterRadius);
		vector3 point4 = vector3(glm::cos(angle2) * a_fInnerRadius, (a_fHeight / 2.0f), glm::sin(angle2) * a_fInnerRadius);
		vector3 point5 = vector3(glm::cos(angle1) * a_fInnerRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle1) * a_fInnerRadius);
		vector3 point6 = vector3(glm::cos(angle1) * a_fOuterRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle1) * a_fOuterRadius);
		vector3 point7 = vector3(glm::cos(angle2) * a_fOuterRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle2) * a_fOuterRadius);
		vector3 point8 = vector3(glm::cos(angle2) * a_fInnerRadius, -1.0f * (a_fHeight / 2.0f), glm::sin(angle2) * a_fInnerRadius);
		
		AddQuad(point2,point1,point3,point4);
		AddQuad(point3,point7,point2,point6);
		AddQuad(point7, point8, point6, point5);
		AddQuad(point5, point8, point1, point4);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	//for each of the subdivisions that will represent sectors with respect to the origin
	for (size_t i = 0; i < a_nSubdivisionsA; i++)
	{
		//get angles from origin to the sections of the torus
		float angleA1 = ((glm::two_pi<float>()) / a_nSubdivisionsA) * i;
		float angleA2 = ((glm::two_pi<float>()) / a_nSubdivisionsA) * (i + 1);

		//get the radius in between both the inner and outer
		float middleRadius = a_fInnerRadius + ((a_fOuterRadius - a_fInnerRadius) / 2);

		//for each of the subdivisions that will represent quads around the central axis for each sector
		for (size_t j = 0; j < a_nSubdivisionsB; j++)
		{
			//calculate the current two angles within the sector to ultimately get four points
			float angleB1 = ((glm::two_pi<float>()) / a_nSubdivisionsB) * j;
			float angleB2 = ((glm::two_pi<float>()) / a_nSubdivisionsB) * (j + 1);

			//get the center of the first sector, then get the axis we will be rotating on, then calculate the first two rotated vectors on that on angle
			vector3 currentMidPoint1 = vector3(glm::cos(angleA1) * middleRadius, 0, glm::sin(angleA1) * middleRadius);
			vector3 currentMidPointNorm1 = glm::normalize(currentMidPoint1);
			vector3 axisOfRot1 = vector3(-currentMidPointNorm1.z, currentMidPointNorm1.y, currentMidPointNorm1.x);
			vector3 rotatedVector1 = currentMidPointNorm1 * glm::cos(angleB1) + glm::cross(axisOfRot1, currentMidPointNorm1) * glm::sin(angleB1) + axisOfRot1 * glm::dot(axisOfRot1, currentMidPointNorm1) * (1 - glm::cos(angleB1));
			vector3 rotatedVector2 = currentMidPointNorm1 * glm::cos(angleB2) + glm::cross(axisOfRot1, currentMidPointNorm1) * glm::sin(angleB2) + axisOfRot1 * glm::dot(axisOfRot1, currentMidPointNorm1) * (1 - glm::cos(angleB2));

			//get the center of the second sector, then get the axis we will be rotating on, then calculate the first two rotated vectors on that on angle
			vector3 currentMidPoint2 = vector3(glm::cos(angleA2) * middleRadius, 0, glm::sin(angleA2) * middleRadius);
			vector3 currentMidPointNorm2 = glm::normalize(currentMidPoint2);
			vector3 axisOfRot2 = vector3(-currentMidPointNorm2.z, currentMidPointNorm2.y, currentMidPointNorm2.x);
			vector3 rotatedVector3 = currentMidPointNorm2 * glm::cos(angleB1) + glm::cross(axisOfRot2, currentMidPointNorm2) * glm::sin(angleB1) + axisOfRot2 * glm::dot(axisOfRot2, currentMidPointNorm2) * (1 - glm::cos(angleB1));
			vector3 rotatedVector4 = currentMidPointNorm2 * glm::cos(angleB2) + glm::cross(axisOfRot2, currentMidPointNorm2) * glm::sin(angleB2) + axisOfRot2 * glm::dot(axisOfRot2, currentMidPointNorm2) * (1 - glm::cos(angleB2));

			AddQuad(currentMidPoint1 + rotatedVector1 * ((a_fOuterRadius - a_fInnerRadius) / 2), currentMidPoint1 + rotatedVector2 * ((a_fOuterRadius - a_fInnerRadius) / 2), currentMidPoint2 + rotatedVector3 * ((a_fOuterRadius - a_fInnerRadius) / 2), currentMidPoint2 + rotatedVector4 * ((a_fOuterRadius - a_fInnerRadius) / 2));
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3 topCenterPoint = vector3(0, a_fRadius, 0);
	vector3 bottomCenterPoint = vector3(0, -1.0f * a_fRadius, 0);

	//for each subdivision going from +y to -y
	for (size_t i = 1; i < a_nSubdivisions; i++)
	{
		//get the y value based on the radius of the sphere and the current y subdivision
		float y1 = glm::sin(glm::pi<float>() / 2 - glm::pi<float>() / a_nSubdivisions * i) * a_fRadius;
		float y2 = glm::sin(glm::pi<float>() / 2 - glm::pi<float>() / a_nSubdivisions * (i + 1)) * a_fRadius;

		//get the radius at that subdivision based on the radius of the sphere and the current y subdivision
		float r1 = glm::cos(glm::pi<float>() / 2 - glm::pi<float>() / a_nSubdivisions * i) * a_fRadius;
		float r2 = glm::cos(glm::pi<float>() / 2 - glm::pi<float>() / a_nSubdivisions * (i + 1)) * a_fRadius;

		//for each face intersecting the x-z plane for whatever y subdivision
		for (size_t j = 0; j < a_nSubdivisions; j++)
		{
			//calculate the two necessary angles to ultimately get four points for a face
			float angle1 = ((glm::two_pi<float>()) / a_nSubdivisions) * j;
			float angle2 = ((glm::two_pi<float>()) / a_nSubdivisions) * (j + 1);

			//if this is the first y subdivision, tris need to be made with the top center point rather than quads
			if (i == 1) {
				vector3 point1 = vector3(glm::cos(angle1) * r1, y1, glm::sin(angle1) * r1);
				vector3 point2 = vector3(glm::cos(angle2) * r1, y1, glm::sin(angle2) * r1);
				AddTri(topCenterPoint, point1, point2);
			}

			//same if it is the last
			if (i == a_nSubdivisions - 1) {
				vector3 point1 = vector3(glm::cos(angle1) * r1, y1, glm::sin(angle1) * r1);
				vector3 point2 = vector3(glm::cos(angle2) * r1, y1, glm::sin(angle2) * r1);
				AddTri(bottomCenterPoint, point2, point1);
			}

			//otherwise just make four points and make a quad
			else {
				vector3 point1 = vector3(glm::cos(angle1) * r1, y1, glm::sin(angle1) * r1);
				vector3 point2 = vector3(glm::cos(angle1) * r2, y2, glm::sin(angle1) * r2);
				vector3 point3 = vector3(glm::cos(angle2) * r2, y2, glm::sin(angle2) * r2);
				vector3 point4 = vector3(glm::cos(angle2) * r1, y1, glm::sin(angle2) * r1);
				AddQuad(point3, point4, point2, point1);
			}
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
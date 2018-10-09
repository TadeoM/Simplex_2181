#include "MyMesh.h"
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
	//std::cout << "Next Tri" << std::endl;
	//std::cout << " X: " << a_vBottomLeft.x << " Y: " << a_vBottomLeft.y << " Z: " << a_vBottomLeft.z << std::endl;
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
	vector3 leftPoint;
	vector3 rightPoint = vector3(a_fRadius * (1 / 180.0f), 0, 0);
	vector3 baseCenterPoint = vector3(0, -0.5f * a_fHeight, 0);
	vector3 topCenterPoint = vector3(0, 0.5f * a_fHeight, 0);
	// set the degree at which each triangle will be made
	double degreeInterval = 360.0f / a_nSubdivisions;

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		// find the next place that you will draw the following triangle
		float nextDegree = degreeInterval * i;
		// x = cos(degree) y = sin(degree)
		// to get radians divide by 180 multiply by pi
		// we already know 2 points on the triangle, it is calculated by the previous triangle
		// therefore, we simply set the leftPoint to the triangle we calculated, rightPoint, and then calculate a new rightPoint
		leftPoint = rightPoint;
		rightPoint = vector3(a_fRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), -0.5f * a_fHeight, a_fRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)));

		AddTri(baseCenterPoint, leftPoint, rightPoint);
		AddTri(topCenterPoint, rightPoint, leftPoint);
	};
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

	vector3 baseLeftPoint;
	vector3 baseRightPoint = vector3(a_fRadius * (1 / 180.0f), -0.5f * a_fHeight, a_fRadius);
	vector3 topLeftPoint;
	vector3 topRightPoint = vector3(a_fRadius * (1 / 180.0f), 0.5f * a_fHeight, a_fRadius);
	vector3 baseCenterPoint = vector3(0, -0.5f * a_fHeight, 0);
	vector3 topCenterPoint = vector3(0, 0.5f * a_fHeight, 0);
	// set the degree at which each triangle will be made
	double degreeInterval = 360.0f / a_nSubdivisions;
	std::cout << "DRAWING CIRCLE" << std::endl;

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		// find the next place that you will draw the following triangle
		float nextDegree = degreeInterval * i;
		// x = cos(degree) y = sin(degree)
		// to get radians divide by 180 multiply by pi
		// we already know 2 points on the triangle, it is calculated by the previous triangle
		// therefore, we simply set the leftPoint to the triangle we calculated, rightPoint, and then calculate a new rightPoint
		baseLeftPoint = baseRightPoint;
		topLeftPoint = topRightPoint;
		baseRightPoint = vector3(a_fRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)), -0.5f * a_fHeight, a_fRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)));
		topRightPoint = vector3(a_fRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)), 0.5f * a_fHeight, a_fRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)));

		if(i != 0)
			AddQuad(baseLeftPoint, baseRightPoint, topLeftPoint, topRightPoint);
		AddTri(baseCenterPoint, baseRightPoint, baseLeftPoint);
		AddTri(topCenterPoint, topLeftPoint, topRightPoint);
	};

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

	// Replace this with your code
	vector3 iBaseLeftPoint;
	vector3 iBaseRightPoint = vector3(a_fInnerRadius * (1 / 180.0f), 0.5f * a_fHeight, a_fInnerRadius);
	vector3 iTopLeftPoint;
	vector3 iTopRightPoint = vector3(a_fInnerRadius * (1 / 180.0f), 0.5f * a_fHeight, a_fInnerRadius);

	vector3 oBaseLeftPoint;
	vector3 oBaseRightPoint = vector3(a_fOuterRadius * (1 / 180.0f), -0.5f * a_fHeight, a_fOuterRadius);
	vector3 oTopLeftPoint;
	vector3 oTopRightPoint = vector3(a_fOuterRadius * (1 / 180.0f), -0.5f * a_fHeight, a_fOuterRadius);

	// set the degree at which each triangle will be made
	double degreeInterval = 360.0f / a_nSubdivisions;

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		// find the next place that you will draw the following triangle
		float nextDegree = degreeInterval * i;
		// x = cos(degree) y = sin(degree)
		// to get radians divide by 180 multiply by pi
		// we already know 2 points on the triangle, it is calculated by the previous triangle
		// therefore, we simply set the leftPoint to the triangle we calculated, rightPoint, and then calculate a new rightPoint
		iBaseLeftPoint = iBaseRightPoint;
		iTopLeftPoint = iTopRightPoint;
		iBaseRightPoint = vector3(a_fInnerRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), -0.5f * a_fHeight, a_fInnerRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)));
		iTopRightPoint = vector3(a_fInnerRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), 0.5f * a_fHeight, a_fInnerRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)));

		oBaseLeftPoint = oBaseRightPoint;
		oTopLeftPoint = oTopRightPoint;
		oBaseRightPoint = vector3(a_fOuterRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), -0.5f * a_fHeight, a_fOuterRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)));
		oTopRightPoint = vector3(a_fOuterRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), 0.5f * a_fHeight, a_fOuterRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)));

		if (i != 0)
		{
			AddQuad(iBaseLeftPoint, iBaseRightPoint, iTopLeftPoint, iTopRightPoint);
			AddQuad(oTopLeftPoint, oTopRightPoint, oBaseLeftPoint, oBaseRightPoint);
			AddQuad(oBaseLeftPoint, oBaseRightPoint, iBaseLeftPoint, iBaseRightPoint);
			AddQuad( iTopLeftPoint, iTopRightPoint, oTopLeftPoint, oTopRightPoint);
		}

		
	};
	// -------------------------------

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


	// declare a center point and a 2d array of vec3 to hold positions
	vector4 tubeCenterPoint;
	std::vector<std::vector<vector3>> locations;
	
	//// set the degree at which each triangle will be made
	float overallDegreeInterval = 360.0f / a_nSubdivisionsA;
	float tubeDegreeInterval = 360.0f / a_nSubdivisionsB;

	// the radius of the tubes
	// the radius of the torus overall
	float tubeRadius = 0.5f * (a_fOuterRadius - a_fInnerRadius);
	float overallRadius = a_fOuterRadius - tubeRadius;

	// declare two rations
	matrix4 rotation = matrix4();
	matrix4 rotation2 = matrix4();

	// set first rotation
	rotation = glm::rotate(matrix4(), 3.14159f / 2, vector3(1.0f, 0.0f, 0.0f));
	
	std::cout << "\n";
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		// find the next place that you will draw the following circle
		float nextDegree = overallDegreeInterval * i;
		rotation2 = glm::rotate((-nextDegree *3.14159f) / 180.0f, vector3(0.0f, 1.0f, 0.0f));			// next rotation that will make the tube circle to be directly perpendicular to the center
		tubeCenterPoint = vector4(overallRadius * (std::cos((nextDegree * 3.1415f) / 180.0f)), 0, overallRadius * (std::sin((nextDegree * 3.1415f) / 180.0f)), 0.0f);

		// set the tube points to their starting location
		vector4 tubeLeftPoint;
		vector4 tubeRightPoint = vector4(tubeRadius * (1 / 180.0f), 0, 0, 0);

		// make a new circle in the vector
		std::vector<vector3> circle;
		locations.push_back(circle);
		printf("Circle: %i\n", i);
		for (int j = 0; j <= a_nSubdivisionsB; j++)
		{
			// find next place where you will draw the following triangle
			float tubeNextDegree = tubeDegreeInterval * j;
			tubeLeftPoint = tubeRightPoint;
			tubeRightPoint = tubeCenterPoint +  vector4(tubeRadius * (std::cos((tubeNextDegree * 3.1415f) / 180.0f)), 0, tubeRadius * (std::sin((tubeNextDegree * 3.1415f) / 180.0f)), 0.0f);

			// rotate the vectors first along the x to get them to face at the camera, and then rotate along the Y to make theme perpendicular to the center of torus
			tubeRightPoint = (rotation * (tubeRightPoint - tubeCenterPoint));
			tubeRightPoint = (rotation2 * tubeRightPoint) + tubeCenterPoint;
			//std::cout << " X: " <<  rotation[0][0] << " Y: " << rotation[1][1] << " Z: " << rotation[2][2] << std::endl;
			printf("X: %.2f  Y: %.2f  Z: %.2f\n", tubeRightPoint.x, tubeRightPoint.y, tubeRightPoint.z);

			locations[i].push_back(tubeRightPoint);
		}
		//std::cout << i << std::endl;
	};

	// take a point and get 3 adjacent points to draw a square 
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			int nextI = (i + 1) % a_nSubdivisionsA;
			int nextJ = (j + 1) % a_nSubdivisionsB;

			AddQuad(locations[i][j], locations[nextI][j], locations[i][nextJ], locations[nextI][nextJ]);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	//a_nSubdivisions = 4;
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	std::vector<std::vector<vector4>> topPoints;
	std::vector<std::vector<vector4>> basePoints;

	vector3 baseCircleCenter = vector4(0, -1 * a_fRadius, 0, 0);
	vector3 baseCircleLeft;
	vector3 baseCircleRight = vector4(a_fRadius * std::cos(0), -1 * a_fRadius + (2 * a_fRadius / a_nSubdivisions), 0.5f * a_fRadius * std::sin(0), 0);

	vector3 topCircleCenter;
	vector3 topCircleLeft;
	vector3 topCircleRight;

	float degreeInterval = 360.0f / a_nSubdivisions;
	
	for (int i = 0; i <  a_nSubdivisions; i++)
	{
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			// get next degree interval
			float nextDegree = degreeInterval * j;

			baseCircleCenter = vector4(0, -1 * i * (a_fRadius / a_nSubdivisions), 0, 0);
			baseCircleLeft = baseCircleRight;
			baseCircleRight = vector4(i*(a_fRadius / a_nSubdivisions) * std::cos(nextDegree * PI / 180), -a_fRadius + i * (a_fRadius / a_nSubdivisions), i * (a_fRadius / a_nSubdivisions) * std::sin(nextDegree * PI / 180), 0);

			topCircleCenter = vector4(0, i * (a_fRadius / a_nSubdivisions), 0, 0);
			topCircleLeft = topCircleRight;
			topCircleRight = vector4(i * (a_fRadius / a_nSubdivisions) * std::cos(nextDegree * PI / 180), a_fRadius - i * (a_fRadius / a_nSubdivisions), i * (a_fRadius / a_nSubdivisions) * std::sin(nextDegree * PI / 180), 0);
		
			if (j != 0 && i == 2)
			{
				AddTri(baseCircleLeft, baseCircleRight, baseCircleLeft);
				AddTri(topCircleRight, topCircleLeft, topCircleCenter);
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
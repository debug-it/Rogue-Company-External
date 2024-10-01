#pragma once
#include "includes.h"
#include "helpers.h"
#include "menu.h"
#include "offsets.h"
#include <assert.h>
void Print(std::string message, bool fail = false)
{
	std::cout << "[+] " << message << std::endl;
	if (fail)
		std::cin.get();
}

struct Matrix4x4
{
	Matrix4x4() {}
	Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m24, float m25, float m26, float m27)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
		m_flMatVal[3][0] = m24;	m_flMatVal[3][1] = m25; m_flMatVal[2][2] = m26; m_flMatVal[3][3] = m27;

	}

	float* operator[](int i) { assert((i >= 0) && (i < 4)); return m_flMatVal[i]; }
	const float* operator[](int i) const { assert((i >= 0) && (i < 4)); return m_flMatVal[i]; }
	float* Base() { return &m_flMatVal[0][0]; }
	const float* Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[4][4];
};

struct Matrix3x4
{
	Matrix3x4() {}
	Matrix3x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float* operator[](int i) { assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	const float* operator[](int i) const { assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	float* Base() { return &m_flMatVal[0][0]; }
	const float* Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

Matrix3x4 Matrix(Vector3 rot, Vector3 origin)
{
	origin = Vector3(0, 0, 0);
	float radPitch = (rot.x * float(PI) / 180.f);
	float radYaw = (rot.y * float(PI) / 180.f);
	float radRoll = (rot.z * float(PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	Matrix3x4 matrix;
	matrix[0][0] = CP * CY;
	matrix[0][1] = CP * SY;
	matrix[0][2] = SP;
	matrix[0][3] = 0.f;

	matrix[1][0] = SR * SP * CY - CR * SY;
	matrix[1][1] = SR * SP * SY + CR * CY;
	matrix[1][2] = -SR * CP;
	matrix[1][3] = 0.f;

	matrix[2][0] = -(CR * SP * CY + SR * SY);
	matrix[2][1] = CY * SR - CR * SP * SY;
	matrix[2][2] = CR * CP;
	matrix[2][3] = 0.f;

	return matrix;
}

bool WorldToScreen(Vector3 origin, Vector2* out, Vector3 Rotation, Vector3 cameraLocation) {

	Vector3 Screenlocation = Vector3(0, 0, 0);

	Matrix3x4 tempMatrix = Matrix(Rotation, Vector3(0, 0, 0)); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix[0][0], tempMatrix[0][1], tempMatrix[0][2]);
	vAxisY = Vector3(tempMatrix[1][0], tempMatrix[1][1], tempMatrix[1][2]);
	vAxisZ = Vector3(tempMatrix[2][0], tempMatrix[2][1], tempMatrix[2][2]);

	Vector3 vDelta = origin - cameraLocation;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = Cheat::fov; //CameraFov; 
	float ScreenCenterX = Cheat::screenWidth / 2.0f;
	float ScreenCenterY = Cheat::screenHeight / 2.0f;

	out->x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * static_cast<float>(PI) / 360.f)) / vTransformed.z;
	out->y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * static_cast<float>(PI) / 360.f)) / vTransformed.z;


	return true;
}

const float RADS_DIVIDED_BY_2 = 0.0194533f;

float Fmod(float x, float y) {
	return x - y * std::floor(x / y);
}

void SinCos(float* outSin, float* outCos, float angle) {
	*outSin = std::sin(angle);
	*outCos = std::cos(angle);
}
struct Quat
{
public:

	float X, Y, Z, W;

	Quat() : X(0.f), Y(0.f), Z(0.f), W(0.f) {}

	Quat(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}

	Quat(const Vector3 R) : X(R.x), Y(R.y), Z(R.z), W(0.f) {}

	Vector3 RotateVector(const Vector3& V) const // Mark as const
	{
		const Vector3 Q(X, Y, Z);
		const Vector3 T = (Q ^ V) * 2.f;
		const Vector3 Result = V + (T * W) + (Q ^ T);
		return V;
	}
	Quat Quaternion() const
	{
		float SP, SY, SR;
		float CP, CY, CR;
		const float PitchNoWinding = Fmod(X, 360.0f);
		const float YawNoWinding = Fmod(Y, 360.0f);
		const float RollNoWinding = Fmod(Z, 360.0f);
		SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
		SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
		SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);
		Quat RotationQuat;
		RotationQuat.X = CR * SP * SY - SR * CP * CY;
		RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
		RotationQuat.Z = CR * CP * SY - SR * SP * CY;
		RotationQuat.W = CR * CP * CY + SR * SP * SY;
		return RotationQuat;
	}

};


struct FTransform
{
public:
	Quat Rotation;
	Vector3 Translation;
	char UnknownData00[0x4];
	Vector3 Scale3D;
	char UnknownData01[0x4];

	// Default constructor
	FTransform() : Rotation(0.f, 0.f, 0.f, 1.f), Translation(), Scale3D(Vector3(1, 1, 1)) {}

	// Constructor with rotation
	FTransform(Quat& InRotation) : Rotation(InRotation), Translation(Vector3(0, 0, 0)), Scale3D(Vector3(1, 1, 1)) {}

	// TransformPosition function
	Vector3 TransformPosition(const Vector3& V) const // Mark as const
	{
		return Rotation.RotateVector(V) + Translation;;// 
	}
	Matrix4x4 ToMatrixWithScale()
	{
		Matrix4x4 m;

		m[3][0] = Translation.x;
		m[3][1] = Translation.y;
		m[3][2] = Translation.z;

		float x2 = Rotation.X + Rotation.X;
		float y2 = Rotation.Y + Rotation.Y;
		float z2 = Rotation.Z + Rotation.Z;

		float xx2 = Rotation.X * x2;
		float yy2 = Rotation.Y * y2;
		float zz2 = Rotation.Z * z2;

		m[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.x;
		m[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.y;
		m[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.z;

		float yz2 = Rotation.Y * z2;
		float wx2 = Rotation.W * x2;
		m[2][1] = (yz2 - wx2) * Scale3D.z;
		m[1][2] = (yz2 + wx2) * Scale3D.y;

		float xy2 = Rotation.X * y2;
		float wz2 = Rotation.W * z2;
		m[1][0] = (xy2 - wz2) * Scale3D.y;
		m[0][1] = (xy2 + wz2) * Scale3D.x;

		float xz2 = Rotation.X * z2;
		float wy2 = Rotation.W * y2;
		m[2][0] = (xz2 + wy2) * Scale3D.z;
		m[0][2] = (xz2 - wy2) * Scale3D.x;

		m[0][3] = 0.0f;
		m[1][3] = 0.0f;
		m[2][3] = 0.0f;
		m[3][3] = 1.0f;

		return m;
	}
	
};

Matrix4x4 MatrixMultiplication(Matrix4x4 pM1, Matrix4x4 pM2)
{
	Matrix4x4 pOut;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			pOut[i][j] = pM1[i][0] * pM2[0][j] +
				pM1[i][1] * pM2[1][j] +
				pM1[i][2] * pM2[2][j] +
				pM1[i][3] * pM2[3][j];
		}
	}

	return pOut;
}

std::vector<Vector2> Render3DBox(const Vector3 origin, const Vector3 extent, const Vector3 rotation, const Vector3 cameraLocation)
{
	Vector3 vertex[2][4];
	vertex[0][0] = { -extent.x, -extent.y,  -extent.z };
	vertex[0][1] = { extent.x, -extent.y,  -extent.z };
	vertex[0][2] = { extent.x, extent.y,  -extent.z };
	vertex[0][3] = { -extent.x, extent.y, -extent.z };

	vertex[1][0] = { -extent.x, -extent.y, extent.z };
	vertex[1][1] = { extent.x, -extent.y, extent.z };
	vertex[1][2] = { extent.x, extent.y, extent.z };
	vertex[1][3] = { -extent.x, extent.y, extent.z };

	Vector2 screen[2][4];
	auto quatRot = Quat(rotation);
	FTransform  Transform(quatRot);

	for (auto k = 0; k < 2; k++)
	{
		for (auto i = 0; i < 4; i++)
		{
			auto& vec = vertex[k][i];
			vec = Transform.TransformPosition(vec) + origin;//  
			if (!WorldToScreen(vec, &screen[k][i], rotation, cameraLocation))
			{
				std::cout << "FAILED" << std::endl;
				return std::vector<Vector2>();
			}
		}

	}

	auto ImScreen = reinterpret_cast<Vector2(&)[2][4]>(screen); // Add all the values we want to draw to a global array to be accessed by the draw thread

	//auto window = ImGui::GetCurrentWindow();
	std::vector<Vector2> vectorArray;
	for (auto i = 0; i < 4; i++)
	{
		vectorArray.push_back(ImScreen[0][i]);
		vectorArray.push_back(ImScreen[1][(i + 1) % 4]);


	}

	return vectorArray;

}



Vector3 FindLookAtRotation(const Vector3& Start, const Vector3& Target) {
	Vector3 Result;
	Vector3 direction = Target - Start ;

	float yaw = atan2(direction.y, direction.x) * 180 / PI;
	float pitch = atan2(direction.z, sqrt(direction.x * direction.x + direction.y * direction.y)) * 180 / PI;

	Result.y = yaw;
	Result.x = pitch;
	Result.z = 0;

	return Result;
}

FTransform GetBoneIndex(uintptr_t mesh, int index, Driver driver)
{
	uintptr_t bonearray;
	driver.ReadMemory<uintptr_t>(mesh + Offsets::BoneArray, sizeof(uintptr_t), bonearray);
	if (!bonearray)
		driver.ReadMemory<uintptr_t>(mesh + Offsets::BoneArray + 0x10, sizeof(uintptr_t), bonearray);

	FTransform out;
	driver.ReadMemory<FTransform>(bonearray + (index * 0x30), sizeof(FTransform), out);
	return out;
}

Vector3 GetBoneWithRotation(uintptr_t mesh, int id, Driver driver)
{
	FTransform bone = GetBoneIndex(mesh, id, driver);
	FTransform componentToWorld;
	driver.ReadMemory<FTransform>(mesh + Offsets::ComponentToWorld, sizeof(FTransform), componentToWorld);
	auto Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), componentToWorld.ToMatrixWithScale());

	return Vector3(Matrix[3][0], Matrix[3][1], Matrix[3][2]);
}


int GetClosestLOSPlayer(bool aimlock, Vector3 CameraLocation, Vector3 CameraRotation)
{
	auto players = NoStam::players;
	int closest_dist = 99999999;
	int actorId = 0;
	for (int i = 0; i < players.size(); i++)
	{
		auto player = players[i];
		Vector2 Out;
		auto w2s = WorldToScreen(player.Location, &Out, CameraRotation, CameraLocation);
		if (!w2s)
			continue;

		Vector2 Screen;
		Screen.x = Cheat::screenWidth / 2;
		Screen.y = Cheat::screenHeight / 2;

		auto heading = Out - Screen;
		auto distance = sqrtf(heading.x * heading.x + heading.y * heading.y);
		//std::cout << nose_pos_w2s.X << " " << nose_pos_w2s.Y << " | " << centerScreen.X << " " << centerScreen.Y << " | " << distance << std::endl;
		if (distance < closest_dist)
		{
			actorId = player.ActorID;
			closest_dist = distance;
		}
	}
	return actorId;
}

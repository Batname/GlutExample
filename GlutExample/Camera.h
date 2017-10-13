#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>
#include <stdio.h>
#include <cmath>

// http://www.binarytides.com/udp-socket-programming-in-winsock/
#include <stdio.h>
#include <winsock2.h>
#include <chrono>
#include <thread>
#include <atomic>


#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 6768   //The port on which to listen for incoming data

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 CameraOffset;

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	int SoketID;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char UDPbuf[BUFLEN];

	std::atomic_bool bIsUDPThreadRunning;
	std::thread UDPThread;
	int ThreadDelayMS = 0;

	glm::vec3 LeftEye = glm::vec3(3.f, 0.f, 160.f);
	glm::vec3 RightEye = glm::vec3(-3.f, 0.f, 160.f);

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		CameraOffset = glm::normalize(glm::vec3(0.f, 0.f, 160.f));
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();

		SoketID = 0;
		bIsUDPThreadRunning = false;
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		// CameraOffset - it is data from camera

		return glm::lookAt(Position + CameraOffset, Position + CameraOffset + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

	void RunCamerasUDPThread()
	{
		//start communication
		while (bIsUDPThreadRunning == true)
		{
			//clear the buffer by filling null, it might have previously received data
			memset(UDPbuf, '\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if (recvfrom(SoketID, UDPbuf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			ParseUDPString(UDPbuf);

			std::this_thread::sleep_for(std::chrono::milliseconds(ThreadDelayMS));
		}
	}

	//  Listen Cameras UDP packages
	void ListenCamerasUDP()
	{
		WSADATA wsa;

		//Initialise winsock
		printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("Initialised.\n");

		//create socket
		if ((SoketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			printf("socket() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//setup address structure
		memset((char *)&si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(PORT);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

		if (bind(SoketID, (SOCKADDR *)&si_other, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			printf("socket() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		if (bIsUDPThreadRunning == false)
		{
			bIsUDPThreadRunning = true;
			UDPThread = std::thread([this]() { RunCamerasUDPThread(); });
		}
	}

	void CloseCamerasUDP()
	{

		if (bIsUDPThreadRunning)
		{
			bIsUDPThreadRunning = false;
			UDPThread.join();

			if (SoketID)
			{
				closesocket(SoketID);
				WSACleanup();
			}
		}
	}

	glm::vec3 ConvertCoordToVector(const std::string& EyeString)
	{
		std::string TempString = "";
		glm::vec3 PositionVector;

		int i;
		bool IsXCoord = true;

		for (i = 0; i <= EyeString.length(); i++)
		{
			if (*(EyeString.c_str() + i) == ',')
			{
				if (IsXCoord)
				{
					PositionVector.x = std::stof(TempString);
				}
				else
				{
					PositionVector.y = std::stof(TempString);
				}

				// Reset string
				TempString = "";

				IsXCoord = false;

				// skip this iteration
				continue;
			}

			// last coordinate
			if (EyeString.length() == i)
			{
				PositionVector.z = std::stof(TempString);
				break;
			}
			TempString += *(EyeString.c_str() + i);
		}

		return PositionVector /= 10.f;
	}

	void ParseUDPString(const char* Buffer)
	{
		std::string BufferString(Buffer);

		std::string PositionLeftStartString = "<PositionLeft>";
		std::string PositionLeftStartEnd = "</PositionLeft>";
		int PositionLeftStart = BufferString.find(PositionLeftStartString) + PositionLeftStartString.length();
		int PositionLeftEnd = BufferString.find(PositionLeftStartEnd);
		int PositionLeftCount = PositionLeftEnd - PositionLeftStart;

		std::string PositionRightStartEnd = "</PositionRight>";
		std::string PositionRightStartString = "<PositionRight>";
		int PositionRightStart = BufferString.find(PositionRightStartString) + PositionRightStartString.length();
		int PositionRightEnd = BufferString.find(PositionRightStartEnd);
		int PositionRightCount = PositionRightEnd - PositionRightStart;

		std::string PositionLeftString = BufferString.substr(PositionLeftStart, PositionLeftCount);
		std::string PositionRightString = BufferString.substr(PositionRightStart, PositionRightCount);


		LeftEye = ConvertCoordToVector(PositionLeftString);
		RightEye = ConvertCoordToVector(PositionRightString);
	}

/*************************MATRIX CALC/*************************/
	glm::mat4 PerspectiveOffCenter(float left, float right, float bottom, float top, float NearPlane, float FarPlane)
	{
		glm::mat4 Result;

		Result[0][0] = 2.0f * NearPlane / (right - left);
		Result[0][1] = 0.0f;
		Result[0][2] = 0.0f;
		Result[0][3] = 0.0f;
		Result[1][0] = 0.0f;
		Result[1][1] = 2.0f * NearPlane / (top - bottom);
		Result[1][2] = 0.0f;
		Result[1][3] = 0.0f;
		Result[2][0] = (right + left) / (right - left);
		Result[2][1] = (top + bottom) / (top - bottom);
		Result[2][2] = 0.0f;
		Result[2][3] = -1.0f;
		Result[3][0] = 0.0f;
		Result[3][1] = 0.0f;
		Result[3][2] = -NearPlane;
		Result[3][3] = 0.0f;

		return Result;
	}


	glm::mat4 GeneralizedPerspectiveProjection(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pe, float Near, float Far)
	{
		glm::vec3 va, vb, vc;
		glm::vec3 vr, vu, vn;

		float left, right, bottom, top, eyedistance;

		vr = pb - pa;
		vr = glm::normalize(vr);
		vu = pc - pa;
		vu = glm::normalize(vu);
		vn = glm::cross(vr, vu);
		vn = glm::normalize(vn);


		//Calculate the vector from eye (pe) to screen corners (pa, pb, pc)
		va = pa - pe;
		vb = pb - pe;
		vc = pc - pe;

		//Get the distance;; from the eye to the screen plane
		eyedistance = -(glm::dot(va, vn));

		//Get the varaibles for the off center projection
		left = (glm::dot(vr, va) * Near) / eyedistance;
		right = (glm::dot(vr, vb) * Near) / eyedistance;
		bottom = (glm::dot(vu, va) * Near) / eyedistance;
		top = (glm::dot(vu, vc) * Near) / eyedistance;


		//Get this projection
		//glm::mat4 frustum_m = glm::frustum(left, right, bottom, top, Near, Far);
		//return frustum_m;
		return PerspectiveOffCenter(left, right, bottom, top, Near, Far);
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
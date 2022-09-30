#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

#include <iostream>

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			Vector3 right{ Vector3::Cross(up, forward).Normalized() };
			Vector3 up{ Vector3::Cross(forward, right).Normalized() };
			return Matrix
			{
				{right, 0},
				{up, 0},
				{forward, 0},
				{origin, 1}
			};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const float cameraMovementSpeed{ 20.f };
			const float cameraRoatationSpeed{ 30.f*TO_RADIANS };

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin.z += cameraMovementSpeed * pTimer->GetElapsed();
			}

			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin.z -= cameraMovementSpeed * pTimer->GetElapsed();
			}

			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin.x += cameraMovementSpeed * pTimer->GetElapsed();
			}

			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin.x -= cameraMovementSpeed * pTimer->GetElapsed();
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (mouseState & SDL_BUTTON_RMASK)
			{				
				if (mouseY > 0)
				{
					origin.z -= cameraMovementSpeed * pTimer->GetElapsed();
				}

				if (mouseY < 0)
				{
					origin.z += cameraMovementSpeed * pTimer->GetElapsed();
				}

				if (mouseX > 0)
				{
					Matrix rotationMatrix{ Matrix::CreateRotationY( cameraRoatationSpeed * pTimer->GetElapsed()) };
					forward = rotationMatrix.TransformVector(forward);
				}
				
				if (mouseX < 0)
				{
					Matrix rotationMatrix{ Matrix::CreateRotationY( -cameraRoatationSpeed * pTimer->GetElapsed()) };
					forward = rotationMatrix.TransformVector(forward);
				}

			}

			if ((mouseState & SDL_BUTTON_RMASK) && (mouseState & SDL_BUTTON_LMASK))
			{
				if (mouseY > 0)
				{
					origin.y -= cameraMovementSpeed * pTimer->GetElapsed();
				}

				if (mouseY < 0)
				{
					origin.y += cameraMovementSpeed * pTimer->GetElapsed();
				}
			}
		}
	};
}

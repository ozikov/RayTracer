#pragma once

#include <cstdint>
#include <vector>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	struct HitRecord;
	struct Vector3;
	struct Light;
	struct ColorRGB;
	struct Camera;
	class Material;

	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;
		
		void CycleLightingMode();
		void ToggleShadows() { m_ShadowsEnabled = !m_ShadowsEnabled; };

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		enum class LightingMode
		{
			ObservedArea, //Lambert Cosine Law
			Radiance, //Incident Radiance
			BRFD, //Scattering of the light
			Combined //ObservedArea * Radiance * BRFD
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShadowsEnabled{ true };

		void RenderPixel
		(
			Scene* pScene,
			uint32_t pixelIndex,
			float fov,
			float aspectRatio,
			const Camera& camera,
			const std::vector<Light>& lights,
			const std::vector<Material*>& materials
		) const;

		void CalculateFinalColor
		(
			const HitRecord& closestHit,
			const Vector3& toLight,
			const std::vector<Material*>& materials,
			const Light& light,
			const Vector3& rayDirection,
			ColorRGB& finalColor
		) const;
	};
}

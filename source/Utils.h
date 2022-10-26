#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			const Vector3 sphereToRayOriginVector{ ray.origin - sphere.origin };
			const float A{ Vector3::Dot(ray.direction, ray.direction)};
			const float B{ 2 * Vector3::Dot(ray.direction, sphereToRayOriginVector) };
			const float C{ Vector3::Dot(sphereToRayOriginVector, sphereToRayOriginVector) - sphere.radius * sphere.radius };

			const float discriminant{ B * B - 4 * A * C };

			if (discriminant > 0)
			{
				const float sqrtDiscriminant{ sqrtf(discriminant) };

				const float t0{ (-B - sqrtDiscriminant) / (2.f * A) };

				if (t0 > ray.min && t0 < ray.max)
				{
					if (ignoreHitRecord) return true;

					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.origin = ray.origin + t0 * ray.direction;
					hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
					hitRecord.t = t0;

					return true;
				}
				
				const float t1{ (-B + sqrtDiscriminant) / (2.f * A) };

				if (t1 > ray.min && t1 < ray.max)
				{
					if (ignoreHitRecord) return true;

					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.origin = ray.origin + t1 * ray.direction;
					hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
					hitRecord.t = t1;

					return true;
				}
			}
			
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			const float t = Vector3::Dot(plane.origin - ray.origin, plane.normal) / Vector3::Dot(ray.direction, plane.normal);

			if (t >= ray.min && t <= ray.max)
			{
				if (ignoreHitRecord) return true;

				hitRecord.didHit = true;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.t = t;

				return true;
			}
			
			return  false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			
			const Vector3 v{ ray.direction };
			const float normalDotViewRay{ Vector3::Dot(triangle.normal, v) };

			if(ignoreHitRecord)
			{
				if (triangle.cullMode == TriangleCullMode::BackFaceCulling && normalDotViewRay < 0) return false;

				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && normalDotViewRay > 0) return false;
			}
			else
			{
				if (triangle.cullMode == TriangleCullMode::BackFaceCulling && normalDotViewRay > 0) return false;

				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && normalDotViewRay < 0) return false;
			}
			
			if (normalDotViewRay == 0.f) return false;
			
			const Vector3 center{ (triangle.v0 + triangle.v1 + triangle.v2) / 3.f };
			const Vector3 L{ center - ray.origin };
			const float t = Vector3::Dot(L, triangle.normal) / normalDotViewRay;
			
			if (t > ray.min && t < ray.max)
			{
				const Vector3 hitPoint{ ray.origin + t * v };

				const Vector3 edge0{ triangle.v1 - triangle.v0 };
				const Vector3 point0ToHitPoint{ hitPoint - triangle.v0 };
				if (Vector3::Dot(Vector3::Cross(edge0, point0ToHitPoint), triangle.normal) < 0) return false;

				const Vector3 edge1{ triangle.v2 - triangle.v1 };
				const Vector3 point1ToHitPoint{ hitPoint - triangle.v1 };
				if (Vector3::Dot(Vector3::Cross(edge1, point1ToHitPoint), triangle.normal) < 0) return false;

				const Vector3 edge2{ triangle.v0 - triangle.v2 };
				const Vector3 point2ToHitPoint{ hitPoint - triangle.v2 };
				if (Vector3::Dot(Vector3::Cross(edge2, point2ToHitPoint), triangle.normal) < 0) return false;

				if (ignoreHitRecord) return true;

				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				hitRecord.origin = ray.origin + t * v;
				hitRecord.t = t;

				return true;
			}
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion

#pragma region TriangleMesh SlabTest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tX1{ (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x };
			float tX2{ (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x };

			float tMin{ std::min(tX1, tX2) };
			float tMax{ std::max(tX1, tX2) };

			float tY1{ (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y };
			float tY2{ (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y };

			tMin = std::max(tMin, std::min(tY1, tY2));
			tMax = std::max(tMax, std::min(tY1, tY2));

			float tZ1{ (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z };
			float tZ2{ (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z };

			tMin = std::max(tMin, std::min(tZ1, tZ2));
			tMax = std::min(tMax, std::min(tZ1, tZ2));

			return tMax > 0 && tMax >= tMin;
		}
#pragma endregion

#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//slabTest
			if (!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}

			HitRecord tempClosestHit{};

			const size_t amountOfIndices{ mesh.indices.size() };
			for(size_t index{}; index < amountOfIndices - 1; ++index)
			{
				Triangle triangle{};
				triangle.normal = mesh.transformedNormals[index / 3];
				triangle.materialIndex = mesh.materialIndex;
				triangle.cullMode = mesh.cullMode;

				triangle.v0 = mesh.transformedPositions[mesh.indices[index]];
				++index;
				triangle.v1 = mesh.transformedPositions[mesh.indices[index]];
				++index;
				triangle.v2 = mesh.transformedPositions[mesh.indices[index]];

				if (ignoreHitRecord)
				{
					if (HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord)) return true;
				}

				HitTest_Triangle(triangle, ray, tempClosestHit, ignoreHitRecord);

				if (tempClosestHit.t < hitRecord.t)
				{
					hitRecord = tempClosestHit;
				}
			}

			if (hitRecord.didHit)
			{
				return true;
			}

			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			return { light.origin - origin };
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			if(light.type == LightType::Point)
			{
				const Vector3 targetToLight{ GetDirectionToLight(light, target) };
				const float irradiance{ light.intensity / (targetToLight.SqrMagnitude()) };
				return{ light.color * irradiance };
			}

			if(light.type == LightType::Directional)
			{
				return{ light.color * light.intensity };
			}

			return ColorRGB{ 0.f, 0.f, 0.f };
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}
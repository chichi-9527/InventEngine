#ifndef _ICOLLISIONDETECTION_
#define _ICOLLISIONDETECTION_

#include "ICollider.h"

namespace INVENT
{
	class ICollisionDetection
	{
	public:
		struct LineSegment 
		{
			glm::vec3 point_O;
			glm::vec3 vector_N;
			float range_Min;
			float range_Max;

			LineSegment(const glm::vec3& point, const glm::vec3& vector, float range_min, float range_max)
				: point_O(point)
				, vector_N(vector)
				, range_Min(range_min)
				, range_Max(range_max)
			{
				if (range_Min > range_Max)
				{
					INVENT_ASSERT(false, "YOU SETTED MIN > MAX ,WILL SET THEM 0!");
					range_Min = 0.0f;
					range_Max = 0.0f;
				}
			}
		};

		// collision direction always first point to second
		static bool IsCollision(const IColliderBase* first, const IColliderBase* second, glm::vec3& collision_direction, float& overlap_distance);

		static bool IsCollisionTwoCapsule(const IColliderCapsule* first, const IColliderCapsule* second, glm::vec3& collision_direction, float& overlap_distance);
		static bool IsCollisionCapsuleBall(const IColliderCapsule* capsule, const IColliderBall* ball, glm::vec3& collision_direction, float& overlap_distance);
		static bool IsCollisionCapsuleBox(const IColliderCapsule* capsule, const IColliderBox* box, glm::vec3& collision_direction, float& overlap_distance);
		static bool IsCollisionBallBox(const IColliderBall* ball, const IColliderBox* box, glm::vec3& collision_direction, float& overlap_distance);
		static bool IsCollisionTwoBall(const IColliderBall* first, const IColliderBall* second, glm::vec3& collision_direction, float& overlap_distance);
		static bool IsCollisionTwoBox(const IColliderBox* first, const IColliderBox* second, glm::vec3& collision_direction, float& overlap_distance);

		static float TwoLineSegmentMinDistance(const LineSegment& first, const LineSegment& second, glm::vec3& point_in_first, glm::vec3& point_in_second);
		static float PointLineSegmentMinDistance(const LineSegment& line, const glm::vec3& point, glm::vec3& point_in_line);
	};
}

#endif // !_ICOLLISIONDETECTION_

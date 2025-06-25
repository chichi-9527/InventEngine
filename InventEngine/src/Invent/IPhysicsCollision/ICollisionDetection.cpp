#include "ICollisionDetection.h"

#include <glm/gtc/matrix_transform.hpp>

namespace INVENT
{
    bool ICollisionDetection::IsCollision(const IColliderBase* first, const IColliderBase* second)
    {
        return false;
    }

    float ICollisionDetection::TwoLineSegmentMinDistance(const LineSegment& first, const LineSegment& second, glm::vec3& point_in_first, glm::vec3& point_in_second)
    {
        glm::vec3 vector_W = first.point_O - second.point_O;

        float a = glm::dot(first.vector_N, first.vector_N);
        float b = glm::dot(first.vector_N, second.vector_N);
        float c = glm::dot(second.vector_N, second.vector_N);
        float d = glm::dot(first.vector_N, vector_W);
        float e = glm::dot(second.vector_N, vector_W);

        float mid = a * c - b * b;
        float s = 0.0f, t = 0.0f;
        if (mid)
        {
            s = (b * e - c * d) / mid;
            t = (a * e - b * d) / mid;

            if (s > first.range_Min && s<first.range_Max && t>second.range_Min && t < second.range_Max)
            {
                point_in_first = first.point_O + s * first.vector_N;
                point_in_second = second.point_O + t * second.vector_N;
                return glm::distance(point_in_first, point_in_second);
            }
            else if (s < first.range_Min)
            {
                s = first.range_Min;
                point_in_first = first.point_O + s * first.vector_N;
                return PointLineSegmentMinDistance(second, point_in_first, point_in_second);
            }
            else if (s > first.range_Max)
            {
                s = first.range_Max;
                point_in_first = first.point_O + s * first.vector_N;
                return PointLineSegmentMinDistance(second, point_in_first, point_in_second);
            }
            else if (t < second.range_Min)
            {
                t = second.range_Min;
                point_in_second = second.point_O + t * second.vector_N;
                return PointLineSegmentMinDistance(first, point_in_second, point_in_first);
            }
            else if (t > second.range_Max)
            {
                t = second.range_Max;
                point_in_second = second.point_O + t * second.vector_N;
                return PointLineSegmentMinDistance(first, point_in_second, point_in_first);
            }
        }
        
        glm::vec3 point1{}, point2{};
        auto distance1 = PointLineSegmentMinDistance(second, first.point_O + first.range_Min * first.vector_N, point1);
        auto distance2 = PointLineSegmentMinDistance(second, first.point_O + first.range_Max * first.vector_N, point2);
        if (distance1 < distance2)
        {
            point_in_first = first.point_O + first.range_Min * first.vector_N;
            point_in_second = point1;
            return distance1;
        }

        point_in_first = first.point_O + first.range_Max * first.vector_N;
        point_in_second = point2;
        return distance2;
    }

    float ICollisionDetection::PointLineSegmentMinDistance(const LineSegment& line, const glm::vec3& point, glm::vec3& point_in_line)
    {
        glm::vec3 vector_W = point - line.point_O;
        float t = glm::dot(vector_W, line.vector_N) / glm::dot(line.vector_N, line.vector_N);
        t = std::min({ std::max({t,line.range_Min}),line.range_Max });
        point_in_line = line.point_O + t * line.vector_N;
        return glm::distance(point, point_in_line);
    }
}
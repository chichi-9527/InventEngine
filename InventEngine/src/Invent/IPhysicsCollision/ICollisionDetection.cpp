#include "ICollisionDetection.h"

#include "ILog.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace INVENT
{
    bool ICollisionDetection::IsCollision(const IColliderBase* first, const IColliderBase* second, glm::vec3& collision_direction, float& overlap_distance)
    {
        switch (first->GetColliderType())
        {
        case IColliderBase::ColliderType::CAPSULE:
            if (second->GetColliderType() == IColliderBase::ColliderType::CAPSULE)
            {
                return IsCollisionTwoCapsule((IColliderCapsule*)first, (IColliderCapsule*)second, collision_direction, overlap_distance);
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BALL)
            {
                return IsCollisionCapsuleBall((IColliderCapsule*)first, (IColliderBall*)second, collision_direction, overlap_distance);
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BOX)
            {
                return IsCollisionCapsuleBox((IColliderCapsule*)first, (IColliderBox*)second, collision_direction, overlap_distance);
            }
            break;
        case IColliderBase::ColliderType::BALL:
            if (second->GetColliderType() == IColliderBase::ColliderType::CAPSULE)
            {
                auto res = IsCollisionCapsuleBall((IColliderCapsule*)second, (IColliderBall*)first, collision_direction, overlap_distance);
                collision_direction = -collision_direction;
                return res;
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BALL)
            {
                return IsCollisionTwoBall((IColliderBall*)first, (IColliderBall*)second, collision_direction, overlap_distance);
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BOX)
            {
                return IsCollisionBallBox((IColliderBall*)first, (IColliderBox*)second, collision_direction, overlap_distance);
            }
            break;
        case IColliderBase::ColliderType::BOX:
            if (second->GetColliderType() == IColliderBase::ColliderType::CAPSULE)
            {
                auto res = IsCollisionCapsuleBox((IColliderCapsule*)second, (IColliderBox*)first, collision_direction, overlap_distance);
                collision_direction = -collision_direction;
                return res;
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BALL)
            {
                auto res = IsCollisionBallBox((IColliderBall*)second, (IColliderBox*)first, collision_direction, overlap_distance);
                collision_direction = -collision_direction;
                return res;
            }
            else if (second->GetColliderType() == IColliderBase::ColliderType::BOX)
            {
                return IsCollisionTwoBox((IColliderBox*)first, (IColliderBox*)second, collision_direction, overlap_distance);
            }
            break;
        }
        INVENT_ASSERT(false, "have not default collider type, you can change this function to achieve : ICollisionDetection::IsCollision");
        return false;
    }

    bool ICollisionDetection::IsCollisionTwoCapsule(const IColliderCapsule* first, const IColliderCapsule* second, glm::vec3& collision_direction, float& overlap_distance)
    {
        glm::vec3 point_in_first_capsule_line{}, point_in_second_capsule_line{};
        auto dist = TwoLineSegmentMinDistance(ICollisionDetection::LineSegment(first->GetWorldPosition(), first->GetDirection(), -first->GetHeight() / 2.0f, first->GetHeight() / 2.0f)
            , ICollisionDetection::LineSegment(second->GetWorldPosition(), second->GetDirection(), -second->GetHeight() / 2.0f, second->GetHeight() / 2.0f)
            , point_in_first_capsule_line, point_in_second_capsule_line);
        collision_direction = point_in_second_capsule_line - point_in_first_capsule_line;
        overlap_distance = (first->GetRadius() + second->GetRadius()) - dist;
        return overlap_distance >= 0.0f ;
    }

    bool ICollisionDetection::IsCollisionCapsuleBall(const IColliderCapsule* capsule, const IColliderBall* ball, glm::vec3& collision_direction, float& overlap_distance)
    {
        glm::vec3 point_in_capsule_line{};
        PointLineSegmentMinDistance(ICollisionDetection::LineSegment(capsule->GetWorldPosition(), capsule->GetDirection(), -capsule->GetHeight() / 2.0f, capsule->GetHeight() / 2.0f)
            , ball->GetWorldPosition(), point_in_capsule_line);
        collision_direction = ball->GetWorldPosition() - point_in_capsule_line;
        overlap_distance = (capsule->GetRadius() + ball->GetRadius()) -  glm::distance(point_in_capsule_line, ball->GetWorldPosition());
        return overlap_distance >= 0.0f;
    }

    bool ICollisionDetection::IsCollisionCapsuleBox(const IColliderCapsule* capsule, const IColliderBox* box, glm::vec3& collision_direction, float& overlap_distance)
    {
        glm::vec3 point_in_capsule_line{};
        PointLineSegmentMinDistance(ICollisionDetection::LineSegment(capsule->GetWorldPosition(), capsule->GetDirection(), -capsule->GetHeight() / 2.0f, capsule->GetHeight() / 2.0f)
            , box->GetWorldPosition(), point_in_capsule_line);
        IColliderBall ball(nullptr, point_in_capsule_line, capsule->GetRadius());
        return IsCollisionBallBox(&ball, box, collision_direction, overlap_distance);
    }

    bool ICollisionDetection::IsCollisionBallBox(const IColliderBall* ball, const IColliderBox* box, glm::vec3& collision_direction, float& overlap_distance)
    {
        glm::vec3 vector_W = ball->GetWorldPosition() - box->GetWorldPosition();
        glm::vec3 point = box->GetWorldPosition();

        auto _x = glm::dot(vector_W, box->GetAxisDirextionX());
        if (_x < -(box->GetSize().x / 2.0f))
            _x = -(box->GetSize().x / 2.0f);
        else if (_x > (box->GetSize().x / 2.0f))
            _x = box->GetSize().x / 2.0f;

        auto _y = glm::dot(vector_W, box->GetAxisDirextionY());
        if (_y < -(box->GetSize().y / 2.0f))
            _y = -(box->GetSize().y / 2.0f);
        else if (_y > (box->GetSize().y / 2.0f))
            _y = box->GetSize().y / 2.0f;

        auto _z = glm::dot(vector_W, box->GetAxisDirextionZ());
        if (_z < -(box->GetSize().z / 2.0f))
            _z = -(box->GetSize().z / 2.0f);
        else if (_z > (box->GetSize().z / 2.0f))
            _z = box->GetSize().z / 2.0f;

        point += box->GetAxisDirextionX() * _x
            + box->GetAxisDirextionY() * _y
            + box->GetAxisDirextionZ() * _z;

        collision_direction = point - ball->GetWorldPosition();
        overlap_distance = ball->GetRadius() - glm::distance(point, ball->GetWorldPosition());
        return overlap_distance >= 0.0f;
    }

    bool ICollisionDetection::IsCollisionTwoBall(const IColliderBall* first, const IColliderBall* second, glm::vec3& collision_direction, float& overlap_distance)
    {
        collision_direction = second->GetWorldPosition() - first->GetWorldPosition();
        overlap_distance = (first->GetRadius() + second->GetRadius()) - glm::distance(first->GetWorldPosition(), second->GetWorldPosition());
        return overlap_distance >= 0.0f;
    }

    bool ICollisionDetection::IsCollisionTwoBox(const IColliderBox* first, const IColliderBox* second, glm::vec3& collision_direction, float& overlap_distance)
    {
        std::array<glm::vec3, 15> axes{};
        unsigned int index = 0;
        axes[index++] = first->GetAxisDirextionX();
        axes[index++] = first->GetAxisDirextionY();
        axes[index++] = first->GetAxisDirextionZ();
        axes[index++] = second->GetAxisDirextionX();
        axes[index++] = second->GetAxisDirextionY();
        axes[index++] = second->GetAxisDirextionZ();

        axes[index++] = glm::cross(first->GetAxisDirextionX(), second->GetAxisDirextionX());
        axes[index++] = glm::cross(first->GetAxisDirextionX(), second->GetAxisDirextionY());
        axes[index++] = glm::cross(first->GetAxisDirextionX(), second->GetAxisDirextionZ());

        axes[index++] = glm::cross(first->GetAxisDirextionY(), second->GetAxisDirextionX());
        axes[index++] = glm::cross(first->GetAxisDirextionY(), second->GetAxisDirextionY());
        axes[index++] = glm::cross(first->GetAxisDirextionY(), second->GetAxisDirextionZ());

        axes[index++] = glm::cross(first->GetAxisDirextionZ(), second->GetAxisDirextionX());
        axes[index++] = glm::cross(first->GetAxisDirextionZ(), second->GetAxisDirextionY());
        axes[index++] = glm::cross(first->GetAxisDirextionZ(), second->GetAxisDirextionZ());

        auto GetProjectionRadius = [](const IColliderBox* obb, const glm::vec3& axis) -> float {
            return std::abs(obb->GetSize().x / 2.0f * glm::dot(axis, obb->GetAxisDirextionX()))
                + std::abs(obb->GetSize().y / 2.0f * glm::dot(axis, obb->GetAxisDirextionY()))
                + std::abs(obb->GetSize().z / 2.0f * glm::dot(axis, obb->GetAxisDirextionZ()));
            };

        auto OverlapOnAxis = [first, second, GetProjectionRadius](glm::vec3& axis, float& distance) -> bool {
            if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f) return true;
            auto norm = glm::normalize(axis);
            float center_dist = glm::dot(second->GetWorldPosition() - first->GetWorldPosition(), norm);
            if (center_dist < 0.0f)
            {
                axis = -axis;
            }
            auto r1 = GetProjectionRadius(first, norm);
            auto r2 = GetProjectionRadius(second, norm);

            distance = (r1 + r2) - std::abs(center_dist);
            return distance >= 0;
            };
        
        float distance = 0.0f;
        overlap_distance = FLT_MAX;
        for (auto& axis : axes)
        {
            if (!OverlapOnAxis(axis, distance))
            {
                return false;
            }
            if (overlap_distance > distance)
            {
                overlap_distance = distance;
                collision_direction = axis;
            }
            
        }

        return true;
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
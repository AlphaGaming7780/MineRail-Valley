#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include <array>
#include <cmath>

#include <PallasEngine/ECS/Components/Transform.hpp>

namespace pallas
{
    class TransformUtils
    {
    public:

        // --- 1) Local bounds : rectangle brut sans rotation ni scale mais avec origine---
        static sf::FloatRect GetLocalBounds(const Transform& t)
        {
            sf::Vector2f origin = t.m_Origin;
            sf::Vector2f size = t.m_Size;

            return sf::FloatRect(
                sf::Vector2f(-origin.x, -origin.y),
                size
            );
        }

        // --- 2) Global bounds : AABB après scale + rotation + position ---
        static sf::FloatRect GetGlobalBounds(const Transform& t)
        {
            auto corners = GetTransformedCorners(t);
            return ComputeAABB(corners);
        }

        // --- 3) Matrice 3x3 (comme SFML mais maison) ---
        static std::array<float, 9> GetTransformMatrix(const Transform& t)
        {
            float rad = t.m_Rotation.asRadians();
            float c = std::cos(rad);
            float s = std::sin(rad);

            float sx = t.m_Scale.x;
            float sy = t.m_Scale.y;

            float ox = t.m_Origin.x;
            float oy = t.m_Origin.y;

            float px = t.m_Position.x;
            float py = t.m_Position.y;

            return {
                c * sx, -s * sy, px - (c * sx * ox - s * sy * oy),
                s * sx,  c * sy, py - (s * sx * ox + c * sy * oy),
                0.f,     0.f,    1.f
            };
        }

        // --- 4) Applique la matrice à un point ---
        static sf::Vector2f TransformPoint(const Transform& t, const sf::Vector2f& p)
        {
            auto m = GetTransformMatrix(t);

            return {
                m[0] * p.x + m[1] * p.y + m[2],
                m[3] * p.x + m[4] * p.y + m[5]
            };
        }

        // --- 5) Renvoie les 4 coins transformés ---
        static std::array<sf::Vector2f, 4> GetTransformedCorners(const Transform& t)
        {
            sf::Rect<float> local = GetLocalBounds(t);

            std::array<sf::Vector2f, 4> pts = {
                sf::Vector2f(local.position.x,                     local.position.y),
                sf::Vector2f(local.position.x + local.size.x,      local.position.y),
                sf::Vector2f(local.position.x + local.size.x,      local.position.y + local.size.y),
                sf::Vector2f(local.position.x,                     local.position.y + local.size.y)
            };

            for (auto& p : pts)
                p = TransformPoint(t, p);

            return pts;
        }

        // --- 6) AABB d’un ensemble de points ---
        static sf::FloatRect ComputeAABB(const std::array<sf::Vector2f, 4>& pts)
        {
            float minX = pts[0].x, maxX = pts[0].x;
            float minY = pts[0].y, maxY = pts[0].y;

            for (int i = 1; i < 4; ++i)
            {
                minX = std::min(minX, pts[i].x);
                maxX = std::max(maxX, pts[i].x);
                minY = std::min(minY, pts[i].y);
                maxY = std::max(maxY, pts[i].y);
            }

            return sf::Rect<float>(
                sf::Vector2f{ minX, minY },
                sf::Vector2f{ maxX - minX, maxY - minY }
            );

        }
    };
}
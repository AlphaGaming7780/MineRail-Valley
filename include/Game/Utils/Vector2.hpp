#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <ostream>

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
constexpr sf::Vector2<T> operator/(sf::Vector2<T> left, sf::Vector2<T> right)
{
    assert(right.x != 0 && "Vector2::operator/ cannot divide by 0" || right.y != 0 && "Vector2::operator/ cannot divide by 0");
    return sf::Vector2<T>(left.x / right.x, left.y / right.y);
}

template <typename T>
constexpr sf::Vector2<T> operator*(sf::Vector2<T> left, sf::Vector2<T> right)
{
    return sf::Vector2<T>(left.x * right.x, left.y * right.y);
}

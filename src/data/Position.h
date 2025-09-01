#pragma once

#include <cmath>
#include <string>

namespace carpet_mod_for_ll {

struct Position {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    
    Position() = default;
    Position(double x, double y, double z) : x(x), y(y), z(z) {}
    
    // 距离计算
    double distanceTo(const Position& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    // 2D距离（忽略Y轴）
    double distance2DTo(const Position& other) const {
        double dx = x - other.x;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dz * dz);
    }
    
    // 操作符重载
    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y, z + other.z);
    }
    
    Position operator-(const Position& other) const {
        return Position(x - other.x, y - other.y, z - other.z);
    }
    
    bool operator==(const Position& other) const {
        return std::abs(x - other.x) < 0.001 && 
               std::abs(y - other.y) < 0.001 && 
               std::abs(z - other.z) < 0.001;
    }
    
    // 转换为字符串
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
    
    // 转换为方块坐标
    Position toBlockPos() const {
        return Position(std::floor(x), std::floor(y), std::floor(z));
    }
};

// 区域结构
struct Region {
    Position min;
    Position max;
    
    Region() = default;
    Region(const Position& min, const Position& max) : min(min), max(max) {}
    
    bool contains(const Position& pos) const {
        return pos.x >= min.x && pos.x <= max.x &&
               pos.y >= min.y && pos.y <= max.y &&
               pos.z >= min.z && pos.z <= max.z;
    }
    
    Position getCenter() const {
        return Position(
            (min.x + max.x) / 2.0,
            (min.y + max.y) / 2.0,
            (min.z + max.z) / 2.0
        );
    }
    
    double getVolume() const {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
};

} // namespace carpet_mod_for_ll
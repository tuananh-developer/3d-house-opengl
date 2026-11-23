#pragma once
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <cmath>

namespace modern {

/**
 * @brief Thuật toán Perlin Noise 2D nguyên bản (Ken Perlin's Improved Noise).
 * 
 * Nguyên lý toán học:
 * 1. Chia không gian thành lưới ô vuông (grid cells).
 * 2. Mỗi đỉnh ô vuông gán một vector gradient ngẫu nhiên từ bảng hoán vị (Permutation Table).
 * 3. Với điểm (x, z), tính vector khoảng cách tới 4 đỉnh ô vuông: d = p - p_corner.
 * 4. Tính tích vô hướng giữa vector khoảng cách và vector gradient tại mỗi đỉnh: s = dot(g, d).
 * 5. Khử gián đoạn bậc 2 bằng hàm Fade Hermite bậc 5: f(t) = 6t^5 - 15t^4 + 10t^3.
 * 6. Nội suy song tuyến tính (Bilinear Interpolation) để thu được giá trị nhiễu liên tục mượt mà.
 */
class PerlinNoise {
private:
    std::vector<int> p; // Bảng hoán vị kích thước 512

    // Hàm làm mịn Fade Curve bậc 5 của Ken Perlin
    static float fade(float t) {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // Hàm nội suy tuyến tính (Linear Interpolation)
    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    // Tính tích vô hướng gradient dựa trên hash code
    static float grad(int hash, float x, float z) {
        // Chuyển 4 bit thấp của hash thành 8 hướng gradient 2D chuẩn
        int h = hash & 7;
        float u = h < 4 ? x : z;
        float v = h < 4 ? z : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }

public:
    explicit PerlinNoise(unsigned int seed = 1337) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        // Nhân đôi bảng hoán vị để tránh xử lý tràn biên (modulo 256)
        p.insert(p.end(), p.begin(), p.end());
    }

    /**
     * @brief Tính giá trị Perlin Noise tại tọa độ (x, z), đầu ra trong khoảng [-1, 1]
     */
    float noise(float x, float z) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;

        x -= std::floor(x);
        z -= std::floor(z);

        float u = fade(x);
        float v = fade(z);

        int A  = p[X] + Z;
        int AA = p[A];
        int AB = p[A + 1];
        int B  = p[X + 1] + Z;
        int BA = p[B];
        int BB = p[B + 1];

        float x1 = lerp(grad(p[AA], x, z), grad(p[BA], x - 1.0f, z), u);
        float x2 = lerp(grad(p[AB], x, z - 1.0f), grad(p[BB], x - 1.0f, z - 1.0f), u);

        return lerp(x1, x2, v);
    }

    /**
     * @brief Fractal Brownian Motion (fBm) đa tầng tần số (Multi-octave noise)
     * Giúp địa hình đồi cỏ tự nhiên có cả những con sóng lớn nhấp nhô và các chi tiết gồ ghề nhỏ.
     */
    float fBm(float x, float z, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f) const {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / maxValue;
    }
};

} // namespace modern


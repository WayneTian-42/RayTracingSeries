#ifndef PERLIN_H
#define PERLIN_H

#include "NextWeek/global.h"
#include "NextWeek/vec3.h"
#include <cmath>
#include <utility>
class perlin
{
  public:
    perlin()
    {
        for (int i = 0; i < point_count; ++i)
        {
            randvec[i] = random_unit_vector();
        }

        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    double noise(const point3 &p) const
    {
        double u = p.x() - std::floor(p.x());
        double v = p.y() - std::floor(p.y());
        double w = p.z() - std::floor(p.z());

        // Hermite cubic

        // u = u * u * (3 - 2 * u);
        // v = v * v * (3 - 2 * v);
        // w = w * w * (3 - 2 * w);

        int i = int(std::floor(p.x()));
        int j = int(std::floor(p.y()));
        int k = int(std::floor(p.z()));

        vec3 c[2][2][2];

        for (int di = 0; di < 2; ++di)
        {
            for (int dj = 0; dj < 2; ++dj)
            {
                for (int dk = 0; dk < 2; ++dk)
                {
                    c[di][dj][dk] = randvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
                }
            }
        }

        return perlin_interp(c, u, v, w);
    }

    double turb(const point3 &p, int depth) const
    {
        double accum = 0.0;
        point3 tmp_p = p;
        double weight = 1.0;

        for (int i = 0; i < depth; ++i)
        {
            accum += weight * noise(tmp_p);
            tmp_p *= 2;
            weight *= 0.5;
        }

        return std::fabs(accum);
    }

  private:
    static const int point_count = 256;
    // double randfloat[point_count];
    vec3 randvec[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_perm(int *p)
    {
        for (int i = 0; i < point_count; ++i)
            p[i] = i;

        permute(p, point_count);
    }

    static void permute(int *p, int n)
    {
        for (int i = n - 1; i > 0; --i)
        {
            int target = random_int(0, i);
            std::swap(p[i], p[target]);
        }
    }

    /**
     * @brief 三线性插值
     *
     * @param c 立方体
     * @param u x轴权重
     * @param v y轴权重
     * @param w z轴权重
     * @return 插值结果
     */
    static double trilinear_interp(double c[2][2][2], double u, double v, double w)
    {
        double accum = 0.0;
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) *
                             c[i][j][k];
                }
            }
        }
        return accum;
    }

    static double fade(double x)
    {
        // 初始的缓和曲线计算方法，一阶导连续
        // return x * x * (3 - 2 * x);
        // 二阶导连续，可以用于位移贴图等场景
        return x * x * x * (10 - 15 * x + 6 * x * x);
    }

    static double perlin_interp(vec3 c[2][2][2], double u, double v, double w)
    {
        double accum = 0.0;

        double uu = fade(u);
        double vv = fade(v);
        double ww = fade(w);

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    vec3 p(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], p);
                }
            }
        }

        // 返回值可能是负数
        return accum;
    }
};

#endif // !PERLIN_H

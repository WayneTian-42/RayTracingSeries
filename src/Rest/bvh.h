#ifndef BVH_H
#define BVH_H

#include "NextWeek/aabb.h"
#include "NextWeek/interval.h"
#include "global.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ctime>
#include <iterator>
#include <vector>

class bvh_node : public hittable
{
  public:
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size())
    {
        // ?
        // 生成list的拷贝，然后执行构造函数
    }

    bvh_node(std::vector<shared_ptr<hittable>> &objects, size_t start, size_t end)
    {
        // 排序，构造二叉树

        // int axis = random_int(0, 2);
        //
        // auto comparator = (axis == 0) ? box_x_compare : ((axis == 1) ? box_y_compare : box_z_compare);
        bbox = aabb::empty;

        for (size_t index = start; index < end; ++index)
        {
            bbox = aabb(bbox, objects[index]->bounding_box());
        }

        // 选取最长的轴进行切分
        int axis = bbox.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare : ((axis == 1) ? box_y_compare : box_z_compare);

        size_t object_span = end - start;

        if (object_span == 1)
        {
            left = right = objects[start];
        }
        else if (object_span == 2)
        {
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            // int mid = object_span / 2;
            int mid = SAHSplit(objects, start, end);
            left = make_shared<bvh_node>(objects, start, start + mid);
            right = make_shared<bvh_node>(objects, start + mid, end);
        }

        // bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        if (!bbox.hit(r, ray_t))
            return false;
        bool hit_left = left->hit(r, ray_t, rec);
        // 在多个相交物体中，寻找更近的交点
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

  private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index)
    {
        auto a_axis_interval = a->bounding_box().axis_interal(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interal(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 2);
    }

    int SAHSplit(const std::vector<shared_ptr<hittable>> &objects, int begin, int end)
    {
        double cost_traversal = 0.125, cost_intersect = 1;
        size_t n = end - begin;
        // 预先设定好划分的区域数，区域数等于n时说明每种划分都考虑
        // int area_num = n > 20 ? 20 : n;
        int area_num = n;
        if (n == 0)
            return -1; // 边界条件检查

        // 计算object的前缀和以及后缀和，后续选择分割点时只需查询即可
        std::vector<aabb> pre(n + 1, aabb::empty), suff(n + 1, aabb::empty);
        for (int i = begin; i < end; ++i)
        {
            // 当前物体在列表中的相对位置
            int index = i - begin;
            pre[index + 1] = aabb(pre[index], objects[i]->bounding_box());
            // 注意如何合并bbox
            suff[n - 1 - index] = aabb(suff[n - index], objects[n - 1 - index + begin]->bounding_box()); // 修正后缀计算
        }

        assert(pre[n].surface_area() == suff[0].surface_area());

        int split_num = -1;
        double min_cost = std::numeric_limits<double>::infinity();
        double total_area = pre[n].surface_area();

        for (int i = 0; i < area_num; ++i)
        {
            int index = n * (i + 1) / area_num; // 避免在第0个位置或n位置分割
            double cost = cost_traversal + pre[index].surface_area() / total_area * cost_intersect * (index) +
                          suff[index].surface_area() / total_area * cost_intersect * (n - index);
            if (cost < min_cost)
            {
                min_cost = cost;
                split_num = index;
            }
        }

        return split_num;
    }
};

#endif // !BVH_H

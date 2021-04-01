#include "pch.h"

#include "SchwartzChurchill.h"

#include <vector>
#include <algorithm>
#include <cmath>

static const float OUTLIER = 1e5;

IntVector2D::IntVector2D()
{
	this->x = 0;
	this->y = 0;
}
IntVector2D::IntVector2D(int x, int y)
{
	this->x = x;
	this->y = y;
}
Vector2D::Vector2D()
{
	this->x = 0;
	this->y = 0;
}
Vector2D::Vector2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

SearchContext::SearchContext(const std::vector<std::vector<std::vector<Point>>>* points, const Vector2D center, const Vector2D avg_dist) :
	points(*points),
	center(center),
	minimum(((center.x - (avg_dist.x * 2))), ((center.y - (avg_dist.y * 2)))),
	maximum(((center.x + (avg_dist.x * 2))), ((center.y + (avg_dist.y * 2))))
{
	this->num_chunks = points->size() * points[0].size();
	this->chunk_length.x = (maximum.x - minimum.x + 1) / points->size();
	this->chunk_length.y = (maximum.y - minimum.y + 1) / points[0].size();
}

bool comp(Point first, Point second)
{
	return first.rank < second.rank;
}

IntVector2D getChunkIndex(Vector2D point, Vector2D min, Vector2D chunk_length, int num_chunks)
{
	IntVector2D index;

	index.x = (int)std::floor((point.x - min.x) / chunk_length.x);
	index.x = std::clamp(index.x, 0, (int)(std::sqrt(num_chunks) - 1));

	index.y = (int)std::floor((point.y - min.y) / chunk_length.y);
	index.y = std::clamp(index.y, 0, (int)(std::sqrt(num_chunks) - 1));

	return index;
}

void mergePoints(const std::vector<Point>* vector1, const std::vector<Point>* vector2, std::vector<Point>* out, const int exit_size = INT_MAX, const Rect* rect = nullptr)
{
	std::vector<Point>::const_iterator it1 = vector1->cbegin();
	std::vector<Point>::const_iterator it2 = vector2->cbegin();
	out->clear();

	while (
		((it1 != vector1->cend()) || (it2 != vector2->cend())) &&
		(out->size() < exit_size)
		)
	{
		if ((it1 != vector1->cend()) && ((it2 == vector2->cend()) || (it1->rank < it2->rank)))
		{
			if (
				(rect == nullptr) ||
				((it1->x >= rect->lx) && (it1->x <= rect->hx) && (it1->y >= rect->ly) && (it1->y <= rect->hy))
				)
			{
				out->push_back(*it1);
			}
			it1++;
		}
		else if (it2 != vector2->cend())
		{
			if (
				(rect == nullptr) ||
				((it2->x >= rect->lx) && (it2->x <= rect->hx) && (it2->y >= rect->ly) && (it2->y <= rect->hy))
				)
			{
				out->push_back(*it2);
			}
			it2++;
		}
	}
}

SearchContext* create(const Point* points_begin, const Point* points_end)
{
	if (points_begin == points_end)
	{
		return nullptr;
	}

	int num_points = 0;
	long double sum_x = 0;
	long double sum_y = 0;
	for (const Point* it = points_begin; it != points_end; it++)
	{
		num_points++;
		if ((std::abs(it->x) < OUTLIER) && (std::abs(it->y) < OUTLIER))
		{
			sum_x += it->x;
			sum_y += it->y;
		}
	}
	Vector2D center((sum_x / num_points), (sum_y / num_points));

	long double sum_dist_x = 0;
	long double sum_dist_y = 0;
	for (const Point* it = points_begin; it != points_end; it++)
	{
		if ((std::abs(it->x) < OUTLIER) && (std::abs(it->y) < OUTLIER))
		{
			sum_dist_x += std::abs(it->x - center.x);
			sum_dist_y += std::abs(it->y - center.y);
		}
	}
	Vector2D avg_dist((sum_dist_x / num_points), (sum_dist_y / num_points));

	Vector2D minimum((center.x - (avg_dist.x * 2)), (center.y - (avg_dist.y * 2)));
	Vector2D maximum((center.x + (avg_dist.x * 2)), (center.y + (avg_dist.y * 2)));
	int num_chunks = std::pow(std::ceil(std::log2(num_points)), 2);
	Vector2D chunk_length;
	chunk_length.x = (maximum.x - minimum.x + 1) / std::sqrt(num_chunks);
	chunk_length.y = (maximum.y - minimum.y + 1) / std::sqrt(num_chunks);

	std::vector<std::vector<std::vector<Point>>> points(std::sqrt(num_chunks), std::vector<std::vector<Point>>(std::sqrt(num_chunks), std::vector<Point>(0)));
	for (const Point* it = points_begin; it != points_end; it++)
	{
		Vector2D point(it->x, it->y);
		IntVector2D index = getChunkIndex(point, minimum, chunk_length, num_chunks);

		std::vector<Point>* chunk = &points[index.x][index.y];
		std::vector<Point>::const_iterator pos = std::upper_bound(chunk->cbegin(), chunk->cend(), *it, comp);
		chunk->insert(pos, *it);
	}

	return new SearchContext(&points, center, avg_dist);
}

int32_t search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
{
	if (sc == nullptr)
	{
		return 0;
	}

	std::vector<Point> out;

	Vector2D point_lo(rect.lx, rect.ly);
	Vector2D point_hi(rect.hx, rect.hy);
	IntVector2D index_lo = getChunkIndex(point_lo, sc->minimum, sc->chunk_length, sc->num_chunks);
	IntVector2D index_hi = getChunkIndex(point_hi, sc->minimum, sc->chunk_length, sc->num_chunks);

	for (int i = index_lo.x; i <= index_hi.x; i++)
	{
		for (int j = index_lo.y; j <= index_hi.y; j++)
		{
			std::vector<Point> temp(out);
			mergePoints(&temp, &sc->points[i][j], &out, count, &rect);
		}
	}

	Point* out_buffer = out_points;
	for (int i = 0; i < out.size(); i++)
	{
		*out_buffer = *(new Point(out[i]));
		out_buffer++;
	}

	return (int32_t)out.size();
}

SearchContext* destroy(SearchContext* sc)
{
	delete sc;
	return nullptr;
}
#include "pch.h"

#include "SchwartzChurchill.h"

#include <vector>
#include <algorithm>

struct SearchContext
{
	std::vector<Point> points;
};

bool comp(Point first, Point second)
{
	return first.rank < second.rank;
}

/* Load the provided points into an internal data structure. The pointers follow the STL iterator convention, where
"points_begin" points to the first element, and "points_end" points to one past the last element. The input points are
only guaranteed to be valid for the duration of the call. Return a pointer to the context that can be used for
consecutive searches on the data. */
SearchContext* create(const Point* points_begin, const Point* points_end)
{
	SearchContext* sc = new SearchContext;
	for (const Point* it = points_begin; it < points_end; it++)
	{
		sc->points.push_back(*it);
	}

	std::sort(sc->points.begin(), sc->points.end(), comp);

	return sc;
}

/* Search for "count" points with the smallest ranks inside "rect" and copy them ordered by smallest rank first in
"out_points". Return the number of points copied. "out_points" points to a buffer owned by the caller that
can hold "count" number of Points. */
int32_t search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
{
	std::vector<Point> out;
	for (int i = 0; (i < sc->points.size()) && (out.size() < count); i++)
	{
		if (
			(sc->points[i].x >= rect.lx) &&
			(sc->points[i].x <= rect.hx) &&
			(sc->points[i].y >= rect.ly) &&
			(sc->points[i].y <= rect.hy)
			)
		{
			out.push_back(sc->points[i]);
		}
	}

	Point* out_buffer = out_points;
	for (int i = 0; i < out.size(); i++)
	{
		*out_buffer = *(new Point(out[i]));
		out_buffer++;
	}

	return out.size();
}

/* Release the resources associated with the context. Return nullptr if successful, "sc" otherwise. */
SearchContext* destroy(SearchContext* sc)
{
	delete sc;
	return nullptr;
}
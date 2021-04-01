#include "pch.h"

#include "SchwartzChurchill.h"

#include <vector>
#include <algorithm>


struct SearchContext
{
	std::vector<Point*> x;
	std::vector<Point*> y;
};

bool compX(Point* first, Point* second)
{
	return first->x < second->x;
}

bool compY(Point* first, Point* second)
{
	return first->y < second->y;
}

bool compRank(Point* first, Point* second)
{
	return first->rank < second->rank;
}

/* Load the provided points into an internal data structure. The pointers follow the STL iterator convention, where
"points_begin" points to the first element, and "points_end" points to one past the last element. The input points are
only guaranteed to be valid for the duration of the call. Return a pointer to the context that can be used for
consecutive searches on the data. */
SearchContext* create(const Point* points_begin, const Point* points_end)
{
	SearchContext* sc = new SearchContext;
	for (const Point* it = points_begin; it != points_end; ++it)
	{
		Point* point = new Point(*it);
		sc->x.push_back(point);
		sc->y.push_back(point);
	}

	std::sort(sc->x.begin(), sc->x.end(), compX);
	std::sort(sc->y.begin(), sc->y.end(), compY);

	return sc;
}

/* Search for "count" points with the smallest ranks inside "rect" and copy them ordered by smallest rank first in
"out_points". Return the number of points copied. "out_points" points to a buffer owned by the caller that
can hold "count" number of Points. */
int32_t search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
{
	if (sc->x.empty() || sc->y.empty())
	{
		return 0;
	}

	Point* low_point = new Point;
	Point* high_point = new Point;
	low_point->x = rect.lx;
	low_point->y = rect.ly;
	high_point->x = rect.hx;
	high_point->y = rect.hy;

	size_t lx_index = (size_t)(std::lower_bound(sc->x.cbegin(), sc->x.cend(), low_point, compX) - sc->x.begin());
	size_t ly_index = (size_t)(std::lower_bound(sc->y.cbegin(), sc->y.cend(), low_point, compY) - sc->y.begin());
	size_t hx_index = (size_t)(std::upper_bound(sc->x.cbegin(), sc->x.cend(), high_point, compX) - sc->x.begin());
	size_t hy_index = (size_t)(std::upper_bound(sc->y.cbegin(), sc->y.cend(), high_point, compY) - sc->y.begin());

	delete low_point;
	delete high_point;

	std::vector<Point*> out;

	if ((hx_index - lx_index) < (hy_index - ly_index))
	{
		for (size_t i = lx_index; i < hx_index; i++)
		{
			if ((sc->x[i]->y >= rect.ly) && (sc->x[i]->y <= rect.hy))
			{
				std::vector<Point*>::const_iterator pos = std::upper_bound(out.cbegin(), out.cend(), sc->x[i], compRank);
				out.insert(pos, sc->x[i]);
				if (out.size() > count)
				{
					out.pop_back();
				}
			}
		}
	}
	else
	{
		for (size_t i = ly_index; i < hy_index; i++)
		{
			if ((sc->y[i]->x >= rect.lx) && (sc->y[i]->x <= rect.hx))
			{
				std::vector<Point*>::const_iterator pos = std::upper_bound(out.cbegin(), out.cend(), sc->y[i], compRank);
				out.insert(pos, sc->y[i]);
				if (out.size() > count)
				{
					out.pop_back();
				}
			}
		}
	}

	Point* out_buffer = out_points;
	for (int i=0; i < out.size(); i++)
	{
		*out_buffer = *(new Point(*out[i]));
		out_buffer++;
	}

	return (int32_t)out.size();
}

/* Release the resources associated with the context. Return nullptr if successful, "sc" otherwise. */
SearchContext* destroy(SearchContext* sc)
{
	for (int i = 0; i < sc->x.size(); i++)
	{
		delete sc->x[i];
	}
	delete sc;
	return nullptr;
}
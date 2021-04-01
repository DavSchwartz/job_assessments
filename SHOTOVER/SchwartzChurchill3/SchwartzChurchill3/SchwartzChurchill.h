#pragma once

#ifdef	SCHWARTZCHURCHILL_EXPORTS
#define SCHWARTZCHURCHILL_API __declspec(dllexport)
#else
#define SCHWARTZCHURCHILL_API __declspec(dllimport)
#endif

/* Given 10 million uniquely ranked points on a 2D plane, design a datastructure and an algorithm that can find the 20
most important points inside any given rectangle. The solution has to be reasonably fast even in the worst case, while
also not using an unreasonably large amount of memory.
Create an x64 Windows DLL, that can be loaded by the test application. The DLL and the functions will be loaded
dynamically. The DLL should export the following functions: "create", "search" and "destroy". The signatures and
descriptions of these functions are given below. You can use any language or compiler, as long as the resulting DLL
implements this interface. */

/* This standard header defines the sized types used. */
#include <stdint.h>

#include <vector>

/* The following structs are packed with no padding. */
#pragma pack(push, 1)

/* Defines a point in 2D space with some additional attributes like id and rank. */
struct Point
{
	int8_t id;
	int32_t rank;
	float x;
	float y;
};

/* Defines a rectangle, where a point (x,y) is inside, if x is in [lx, hx] and y is in [ly, hy]. */
struct Rect
{
	float lx;
	float ly;
	float hx;
	float hy;
};
#pragma pack(pop)

struct IntVector2D
{
	IntVector2D();
	IntVector2D(int x, int y);

	int x;
	int y;
};

struct Vector2D
{
	Vector2D();
	Vector2D(float x, float y);

	float x;
	float y;
};

/* Declaration of the struct that is used as the context for the calls. */
struct SearchContext
{
	SearchContext(const std::vector<std::vector<std::vector<Point>>>* points, const Vector2D center, const Vector2D avg_dist);

	std::vector<std::vector<std::vector<Point>>> points;
	Vector2D center;
	Vector2D minimum;
	Vector2D maximum;
	int num_chunks;
	Vector2D chunk_length;
};

bool comp(Point first, Point second);
IntVector2D getChunkIndex(Vector2D point, Vector2D min, Vector2D chunk_length, int num_chunks);
void mergePoints(const std::vector<Point>* vector1, const std::vector<Point>* vector2, std::vector<Point>* out, const int exit_size, const Rect* rect);

/* Load the provided points into an internal data structure. The pointers follow the STL iterator convention, where
"points_begin" points to the first element, and "points_end" points to one past the last element. The input points are
only guaranteed to be valid for the duration of the call. Return a pointer to the context that can be used for
consecutive searches on the data. */
extern "C" SCHWARTZCHURCHILL_API SearchContext * create(const Point * points_begin, const Point * points_end);

/* Search for "count" points with the smallest ranks inside "rect" and copy them ordered by smallest rank first in
"out_points". Return the number of points copied. "out_points" points to a buffer owned by the caller that
can hold "count" number of Points. */
extern "C" SCHWARTZCHURCHILL_API int32_t search(SearchContext * sc, const Rect rect, const int32_t count, Point * out_points);

/* Release the resources associated with the context. Return nullptr if successful, "sc" otherwise. */
extern "C" SCHWARTZCHURCHILL_API SearchContext * destroy(SearchContext * sc);

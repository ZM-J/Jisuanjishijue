#include "sjpf.h"

sjpf::sjpf(const vector<Point>& pts)
{
	BowyerWatson(pts);
}

void sjpf::BowyerWatson(const vector<Point>& dianji)
{
	// https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm
	int l = dianji.size();
	// triangulation := empty triangle mesh data structure
	tris.clear();
	// add super-triangle to triangulation
	// must be large enough to completely contain all the points in pointList
	tris.push_back(triangle(l - 4, l - 3, l - 2));
	tris.push_back(triangle(l - 3, l - 2, l - 1));
	// add all the points one at a time to the triangulation
	for (int i = 0; i < l; i++) {
		vector<triangle> badTris;
		// first find all the triangles that are no longer valid due to the insertion
		for (auto t : tris) {
			if (t.circleContain(dianji, i)) {
				// add triangle to badTriangles
				badTris.push_back(t);
			}
		}
		vector<pair<int, int>> polygon;
		// find the boundary of the polygonal hole
		for (auto badt : badTris) {
			// for each edge in badTris do
			for (int j = 0; j < 3; j++) {
				pair<int, int> edge = badt.getEdge(j);
				// if edge is not shared by any other triangles in badTriangles
				bool ouligay = true;
				for (auto othert : badTris) {
					if (othert == badt) {
						continue;
					}
					if (othert.hasEdge(edge)) {
						ouligay = false;
						break;
					}
				}
				if (ouligay) {
					// add edge to polygon
					polygon.push_back(edge);
				}
			}
		}
		// remove bad triangles from the data structure
		vector<triangle> temptris;
		for (auto t : tris) {
			bool ouligay = true;
			for (auto badt : badTris) {
				if (t == badt) {
					ouligay = false;
					break;
				}
			}
			if (ouligay) {
				temptris.push_back(t);
			}
		}
		tris = temptris;
		// re-triangulate the polygonal hole
		for (auto e : polygon) {
			// form a triangle from edge to point
			triangle newTri(e.first, e.second, i);
			// add newTri to triangulation
			tris.push_back(newTri);
		}
	}
	// done inserting points, now clean up
	// 不需要做
}

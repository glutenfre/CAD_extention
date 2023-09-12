#pragma once

#include <vector>

class MyPoint {
public:
	double x, y, z;
	MyPoint();
	MyPoint(double def_val);
	void Update(MyPoint point, bool f);
};

class Parallelepiped {
public:
	MyPoint first_point;
	MyPoint second_point;
	Parallelepiped();
	Parallelepiped(double def_val);
	void Update(Parallelepiped parallelepiped);
};

class PointsToDraw {
private:
	bool draw_mode = false;
	IPart7Ptr rootPart;
	IDocumentFramesPtr doc_frames;
public:
	std::vector<double> x_points_vector;
	std::vector<double> y_points_vector;
	std::vector<double> z_points_vector;
	std::vector<int> points_inside_model;
	void RandomizePoints(int correction, double x, double y,
		double z, int x_size,
		int y_size, int z_size);
	void StartDrawing();
	bool DrawMode();
	void EndDrawing();
	void SetPart(IPart7Ptr rootPart);
	void SetDocFrames(IDocumentFramesPtr doc_frames);
	void UnConvertCoordinates(int i, long* x, long* y);
	void Reset();
};

template<typename Lambda>
void MySafeArrayForEach(_variant_t& safeArrayVariant, Lambda l)
{
	switch (safeArrayVariant.vt) {

		// no item returned
	case VT_EMPTY:
		return;

		// 1 item returned
	case VT_DISPATCH:
	{
		IDispatch* disp = safeArrayVariant.pdispVal;
		l(disp);
	}
	break;

	// many items returned
	case VT_DISPATCH | VT_ARRAY:
	{
		SAFEARRAY* safeArray = safeArrayVariant.parray;
		IDispatch** dispBegin(NULL);
		if (SUCCEEDED(SafeArrayAccessData(safeArray, (void**)&dispBegin))) {
			for (UINT i = 0; i < safeArray->rgsabound->cElements; ++i) {
				l(dispBegin[i]);
			}
		}
	}
	break;
	}
}

void TraversePart(IPart7Ptr part, 
	bool to_update_paral, 
	Parallelepiped* res_parallepiped,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model);
void TraverseBodiesWithUpdate(IPart7Ptr part, Parallelepiped* res_parallepiped);
void UpdateParal(IBody7Ptr body, Parallelepiped* res_parallepiped);
void TraverseBodies(IPart7Ptr part,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model);
void TraverseBody(IBody7Ptr body,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<bool>* points_inside_model);
void TraverseFace(IFacePtr face,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model);
void CheckPointsOld(IPart7Ptr part, 
	int correction, 
	std::vector<bool>* 
	volume_points, 
	Parallelepiped* res_parallepiped);

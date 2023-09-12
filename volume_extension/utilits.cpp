#include "stdafx.h"
#include "utilits.h"

#include <iostream>
#include <fstream>
#include <string>

double GetCoord(double coord) {
	return coord;
}

Parallelepiped::Parallelepiped(double def_val) {
	first_point = MyPoint(def_val);
	second_point = MyPoint();
}

Parallelepiped::Parallelepiped() {
	first_point = MyPoint();
	second_point = MyPoint();
}

void Parallelepiped::Update(Parallelepiped parallelepiped) {
	bool first = true;
	bool second = false;
	first_point.Update(parallelepiped.first_point, first);
	second_point.Update(parallelepiped.second_point, second);
}

MyPoint::MyPoint()
	: x(0), y(0), z(0) {};

MyPoint::MyPoint(double def_val) {
	x = def_val;
	y = def_val;
	z = def_val;
}

void MyPoint::Update(MyPoint point, bool first) {
	if (first) {
		if (x > point.x) x = point.x;
		if (y > point.y) y = point.y;
		if (z > point.z) z = point.z;
	}
	else {
		if (x < point.x) x = point.x;
		if (y < point.y) y = point.y;
		if (z < point.z) z = point.z;
	}
}

void TraversePart(IPart7Ptr part,
	bool to_update_paral,
	Parallelepiped* res_parallepiped,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model) {

	// recursively traverse this component
	IParts7Ptr subParts = part->GetParts();
	if (subParts) {
		int num_parts = subParts->GetCount();
		for (int i = 0; i < subParts->GetCount(); ++i) {
			TraversePart(subParts->GetItem(i), 
				to_update_paral, 
				res_parallepiped,
				x_points_vector,
				y_points_vector,
				z_points_vector,
				points_inside_model);
		}
	}
	if (to_update_paral) {
		TraverseBodiesWithUpdate(part, res_parallepiped);
	}
	else {
		TraverseBodies(part,
			x_points_vector,
			y_points_vector,
			z_points_vector,
			points_inside_model);
	}

	return;
}

void TraverseBodiesWithUpdate(IPart7Ptr part, Parallelepiped* res_parallepiped) {

	if (!part) return;
	if (!part->GetDetail()) return;

	IFeature7Ptr feature(part);
	if (feature) {
		MySafeArrayForEach(feature->GetResultBodies(), 
			[res_parallepiped](IDispatch* disp) {
			if (disp) {
				IBody7Ptr body(disp);
				if (body) {
					UpdateParal(body, res_parallepiped);
				}
			}
			}
		);
	}
}

void UpdateParal(IBody7Ptr body, Parallelepiped* res_parallepiped){
	Parallelepiped parallepiped;
	body->GetGabarit(&parallepiped.first_point.x,
		&parallepiped.first_point.y,
		&parallepiped.first_point.z,
		&parallepiped.second_point.x,
		&parallepiped.second_point.y,
		&parallepiped.second_point.z);
	res_parallepiped->Update(parallepiped);
}

int CheckIfPointInside(IMathSurface3DPtr math_sur,
	double x_point,
	double y_point,
	double z_point,
	std::ofstream* log_point) {
	
	bool Ext = false;
	int res = 0;
	_variant_t projection = math_sur->PointProjection(
		x_point, y_point, z_point, 0, 1, 0, Ext);
	if (projection.vt == (VT_ARRAY | VT_R8)) {
		double* doubleBegin(NULL);
		SAFEARRAY* safeArray = projection.parray;
		int array_size = safeArray->rgsabound->cElements;
		*log_point << " projections";
		if (SUCCEEDED(SafeArrayAccessData(safeArray, (void**)&doubleBegin))) {
			for (UINT j = 0; j < array_size; j += 2) {
				double x1, y1, z1;
				if (math_sur->GetPoint(doubleBegin[j], doubleBegin[j + 1], &x1, &y1, &z1)) {
					if (z1 > z_point) *log_point << " ( " << x1 << ", " << y1 << ", " << z1 << " )";
					double U, V;
					if (math_sur->NearPointProjection( x1, y1, z1, &U, &V, Ext)) {
						double x2, y2, z2;
						if (math_sur->GetPoint(U, V, &x2, &y2, &z2)) {
							if (z1 > z_point) *log_point << "= near ( " << x2 << ", " << y2 << ", " << z2 << " )";
							if ((std::abs(x1 - x2) < 0.0001) && (std::abs(y1 - y2) < 0.0001) && (std::abs(z1 - z2) < 0.0001) && (z1 > z_point)) res++;
						}
					}
				}
			}
		}
		if (array_size % 2 != 0)
		{
			MessageBox(NULL, L"Points array size is not even...", L"GG", MB_OK);
		}
	}
	return res;
}

void TraverseFace(IFacePtr face,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model) {
	if (!face) return;
	
	std::ofstream clear_file("C:\\Users\\Ivan\\source\\repos\\volume_extension\\log.txt");
	clear_file.close();
	std::ofstream log_point("C:\\Users\\Ivan\\source\\repos\\volume_extension\\log.txt", std::ios::app);
	log_point << "----------------------------------------surface----------------------------------------\n";

	IMathSurface3DPtr math_sur = face->MathSurface;
	if (math_sur) {
		double surf_x1, surf_y1, surf_z1, surf_x2, surf_y2, surf_z2;
		bool res_gabarit = math_sur->GetGabarit(&surf_x1, &surf_y1, &surf_z1, &surf_x2, &surf_y2, &surf_z2);
		for (int i = 0; i < x_points_vector->size(); i++) {
			log_point << "point( " << (*x_points_vector)[i]
				<< ", " << (*y_points_vector)[i]
				<< ", " << (*z_points_vector)[i]
				<< " )";
			if ((res_gabarit) && ((*z_points_vector)[i] - surf_z2 < -0.001)) {
				int res = CheckIfPointInside(
					math_sur,
					(*x_points_vector)[i],
					(*y_points_vector)[i],
					(*z_points_vector)[i],
					&log_point);
				if (res > 0) {
					(*points_inside_model)[i] += res;
					log_point << " have " << res <<" projections\n";
				}
				else
					log_point << "\n";
			}
			else
				log_point << "\n";
		}
		log_point << " ";
	}
}

void TraverseBody(IBody7Ptr body,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model)
{
	if (!body) return;

	IFeature7Ptr feature(body);
	if (feature)
	{
		MySafeArrayForEach(feature->GetModelObjects(o3d_face),
			[x_points_vector,
			y_points_vector,
			z_points_vector,
			points_inside_model](IDispatch* disp) {
				if (disp) {
					IModelObjectPtr faceModelObject(disp);
					if (faceModelObject) {
						IFacePtr face(faceModelObject);
						if (face) {
							TraverseFace(face,
								x_points_vector,
								y_points_vector,
								z_points_vector,
								points_inside_model);
						}
					}
				}
			}
		);
	}
}

void TraverseBodies(IPart7Ptr part,
	std::vector<double>* x_points_vector,
	std::vector<double>* y_points_vector,
	std::vector<double>* z_points_vector,
	std::vector<int>* points_inside_model)
{
	if (!part) return;
	if (!part->GetDetail()) return;

	IFeature7Ptr feature(part);
	if (feature)
	{
		MySafeArrayForEach(feature->GetResultBodies(),
			[x_points_vector,
			y_points_vector,
			z_points_vector,
			points_inside_model](IDispatch* disp) {
				if (disp) {
					IBody7Ptr body(disp);
					if (body) {
						TraverseBody(body,
							x_points_vector,
							y_points_vector,
							z_points_vector,
							points_inside_model);
					}
				}
			}
		);
	}
}

void CheckPointsOld(IPart7Ptr part, int correction, std::vector<bool>* volume_points, Parallelepiped* res_parallepiped) {
	int x_size = res_parallepiped->second_point.x - res_parallepiped->first_point.x;
	int y_size = res_parallepiped->second_point.y - res_parallepiped->first_point.y;
	int z_size = res_parallepiped->second_point.z - res_parallepiped->first_point.z;
	std::vector<double> x_points_vector;
	std::vector<double> y_points_vector;
	std::vector<double> z_points_vector;
	for (int i = 0; i < correction; i++) {
		x_points_vector.push_back(res_parallepiped->first_point.x + std::rand() % x_size);
		y_points_vector.push_back(res_parallepiped->first_point.y + std::rand() % y_size);
		z_points_vector.push_back(res_parallepiped->first_point.z + std::rand() % z_size);
	}
	for (int j = 0; j < correction; j++) {
		auto is_there_a_part =
			part->FindObjectsByPoint(x_points_vector[j], y_points_vector[j], z_points_vector[j], false);
		if (is_there_a_part.vt != VT_EMPTY) 
			(*volume_points)[j] = true;
		if (j == correction - 1) {
			volatile int dummy;
		}
	}
}

void PointsToDraw::RandomizePoints(int correction, double x, double y, double z, int x_size, int y_size, int z_size) {
	std::vector<int> dummy_v(correction, 0);
	points_inside_model = std::move(dummy_v);

	for (int i = 0; i < correction; i++) {
		x_points_vector.push_back(x + std::rand() % x_size);
		y_points_vector.push_back(y + std::rand() % y_size);
		z_points_vector.push_back(z + std::rand() % z_size);
	}
}

void PointsToDraw::StartDrawing() {
	draw_mode = true;
}

bool PointsToDraw::DrawMode() {
	return draw_mode;
}

void PointsToDraw::EndDrawing() {
	draw_mode = false;
}

void PointsToDraw::Reset() {
	draw_mode = false;
	x_points_vector.clear();
	y_points_vector.clear();
	z_points_vector.clear();
	points_inside_model.clear();
}

void PointsToDraw::SetPart(IPart7Ptr part) {
	rootPart = part;
}

void PointsToDraw::SetDocFrames(IDocumentFramesPtr frames) {
	doc_frames = frames;
}

void PointsToDraw::UnConvertCoordinates(int j, long* x, long* y) {
	for (int i = 0; i < doc_frames->GetCount(); i++) {
		IDocumentFramePtr doc_frame = doc_frames->GetItem(i);
		if (doc_frame->GetActive()) {
			bool result = doc_frame->UnConvertCoordinates(
				kcDocument,
				x_points_vector[j],
				y_points_vector[j],
				z_points_vector[j],
				x, y);
			return;
		}
	}
}
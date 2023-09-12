#include "stdafx.h"
#include "traversing.h"
#include <afxstr.h>

#include <vector>

extern IApplicationPtr newKompasAPI;

template<typename Lambda>
void MySafeArrayForEach(_variant_t& safeArrayVariant, Lambda l);


void TraverseFeatures(IPart7Ptr part, std::ofstream& log, int depth)
{
	// all features are IModelObjects? these are
	if (!part) return;

	IModelContainerPtr modelContainer (part);
	if (modelContainer) {
		ISketchsPtr sketchs = modelContainer->GetSketchs();
		if (sketchs) {
			for (long i = 0; i < sketchs->Count; ++i) {
				IModelObjectPtr sketchModel (sketchs->Item[i]);
				if ( sketchModel && sketchModel->GetValid() ) {
					ISketchPtr sketch(sketchModel);
					if (sketch) {
						log << std::string(depth + 1, '\t') << "Sketch: " << sketchModel->GetName() << " type = " << sketch->Plane->ModelObjectType << '\n';
					}
				}
			}
		}
	}

	IAuxiliaryGeomContainerPtr auxContainer (part);
	if (auxContainer) {
	
		ILocalCoordinateSystemsPtr LCSs = auxContainer->GetLocalCoordinateSystems();
		if (LCSs) {
			for (long i = 0; i < LCSs->Count; ++i) {
				IModelObjectPtr lcsModel (LCSs->Item[i]);
				if (lcsModel && lcsModel->GetValid()) {
					ILocalCoordinateSystemPtr lcs (lcsModel);
					if (lcs) {
						double lx, ly, lz;
						double v[3];
						lcs->GetVector(o3d_axisOX, &v[0], &v[1], &v[2]);
						lx = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
						lcs->GetVector(o3d_axisOY, &v[0], &v[1], &v[2]);
						ly = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
						lcs->GetVector(o3d_axisOZ, &v[0], &v[1], &v[2]);
						lz = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
						log << std::string(depth + 1, '\t') << "LCS: " << lcsModel->GetName() << ";  Lengths: " << lx << ", " << ly << ", " << lz << '\n';
					}
				}
			}
		}
	
		IPlanes3DPtr planes = auxContainer->GetPlanes3D();
		if (planes) {
			for (long i = 0; i < planes->Count; ++i) {
				IModelObjectPtr planeModel (planes->Item[i]);
				if ( planeModel && planeModel->GetValid() ) {
					log << std::string(depth + 1, '\t') << "Plane: " << planeModel->GetName() << '\n';
				}
			}
		}
	}
}

bool StartsWith(_bstr_t starts, _bstr_t with)
{
	if (starts.length() < with.length()) return false;
	if (memcmp(starts.GetBSTR(), with.GetBSTR(), sizeof(OLECHAR)) == 0) {
		return true;
	}
	return false;
}

void TraverseVariables(IFeature7Ptr feat)
{
	if (feat)
	{
		int count = feat->GetVariablesCount(VARIANT_FALSE, VARIANT_TRUE);
		if (count > 0)
		{
			_variant_t vVars = feat->GetVariables(VARIANT_FALSE, VARIANT_TRUE);
			std::vector<IVariable7Ptr> kvars;
			MySafeArrayForEach(vVars, [&kvars](IDispatch* disp) {
				if (disp)
				{
					IVariable7Ptr var(disp);
					if (var)
					{
						kvars.push_back(var);
					}
				}
				});

			for (const auto& kvar : kvars)
			{
				if (kvar)
				{
					CString name = (wchar_t*) kvar->ParameterNote;
					name;
				}
			}

		}
	}
}


void TraverseEdge(IEdgePtr edge, std::ofstream& log, int depth)
{
	log << std::string(depth, '\t') << "Edge (" << (edge->GetIsStraight() ? "" : "not ") << "straight): Length = " << edge->GetLength(ksLUnDocument) << "\n";

	IVertexPtr startV = edge->GetVertex(true);
	IVertexPtr endV = edge->GetVertex(false);

	if (startV && endV) {
		std::streamsize p = log.precision(2);
		double xyz[3];
		startV->GetPoint(&xyz[0], &xyz[1], &xyz[2]);
		log << std::string(depth + 1, '\t') << "Start Vertex: (" << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << ")\n";
		endV->GetPoint(&xyz[0], &xyz[1], &xyz[2]);
		log << std::string(depth + 1, '\t') << "  End Vertex: (" << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << ")\n";
		log.precision(p);
	}
}

void TraverseFace(IFacePtr face, std::ofstream& log, int depth)
{
	if (!face) return;

	log << std::string(depth, '\t') << "Face: Area =  " << face->GetArea(ksLUnDocument) << "\n";

	log << std::string(depth + 1, '\t') << "Edges and vertexes:" << "\n";

	MySafeArrayForEach(face->GetLimitingEdges(),
		[&log, depth](IDispatch* disp) {
			if (disp) {
				IEdgePtr edge (disp);
				if (edge) {
					TraverseEdge(edge, log, depth + 2);
				}
			}
		}
	);

	log << std::string(depth + 1, '\t') << "Triangulation:" << "\n";
	ITessellation7Ptr tessellation = face->GetTessellation();
	if (tessellation) {
		_variant_t pointsVariant, indexesVariant, normalsVariant;
		if (tessellation->GetFacetPoints(pointsVariant.GetAddress(), indexesVariant.GetAddress(), normalsVariant.GetAddress())) {
			LONG lBound, uBound;
			if (SUCCEEDED(SafeArrayGetUBound(indexesVariant.GetVARIANT().parray, 1, &uBound))
				&&
				SUCCEEDED(SafeArrayGetLBound(indexesVariant.GetVARIANT().parray, 1, &lBound))) {

				log << std::string(depth + 2, '\t') << "Vertexes count: " << uBound - lBound << "\n";
			}

			auto get3Doubles = [&log](_variant_t& safeArrayVariant) {
				if (safeArrayVariant.GetVARIANT().vt != (VT_ARRAY | VT_R8)) return;

				SAFEARRAY* safeArray = safeArrayVariant.GetVARIANT().parray;
				double* begin (NULL);
				if (SUCCEEDED(SafeArrayAccessData(safeArray, (void**)&begin))) {
					for (UINT i = 0; i < safeArray->rgsabound->cElements; ++i) {
						if (i % 3 == 0) log << "(";
						log << begin[i] << (i % 3 == 2 ? ") " : ", ");
					}
				}
			};

			log << std::string(depth + 2, '\t') << "Vertexes: ";
			get3Doubles(pointsVariant);
			log << "\n" << std::string(depth + 2, '\t') << " Normals: ";
			get3Doubles(normalsVariant);
			log << "\n";
		}
	}
}

void TraverseBody(IBody7Ptr body, std::ofstream& log, int depth)
{
	if (!body) return;

	log << std::string(depth, '\t') << "Body: " << body->GetName() << "\n";

	IFeature7Ptr feature (body);
	if (feature)
	{
		MySafeArrayForEach(feature->GetModelObjects(o3d_face),
			[&log, depth](IDispatch* disp) {
				if (disp) {
					IModelObjectPtr faceModelObject (disp);
					if (faceModelObject) {
						IFacePtr face(faceModelObject);
						if (face) {
							TraverseFace(face, log, depth + 1);
						}
					}
				}
			}
		);
	}
}

void TraverseBodies(IPart7Ptr part, std::ofstream& log, int depth)
{
	if (!part) return;
	if (!part->GetDetail()) return;

	IFeature7Ptr feature (part);
	if (feature)
	{
		MySafeArrayForEach(feature->GetResultBodies(),
			[&log, depth](IDispatch* disp) {
				if (disp) {
					IBody7Ptr body (disp);
					if (body) {
						TraverseBody(body, log, depth + 1);

					}
				}
			}
		);
	}
}

void TraversePart(IPart7Ptr part, PartTraverseFunction what, std::ofstream& log, int depth)
{
	if (!part) return;

	log << std::string(depth, '\t') << "Part: " << part->GetName() << " (" << ( part->GetDetail() ? "detail" : "assembly" ) << ")\n";

	// traverse what we need of this component
	what(part, log, depth);

	// recursively traverse this component
	IParts7Ptr subParts = part->GetParts();
	if (subParts) {
		for (int i = 0; i < subParts->GetCount(); ++i) {
			TraversePart(subParts->GetItem(i), what, log, depth + 1);
		}
	}//

	return;
}

void TraverseThisEmbodiment(PartTraverseFunction what)
{
	std::ofstream log(std::string(THIS_DIR) + "modelLog.txt");

	IKompasDocument3DPtr doc3d = newKompasAPI->GetActiveDocument();
	if (doc3d) {

		DocumentTypeEnum type = doc3d->GetDocumentType();
		switch (type) {
		case ksDocumentPart:
			break;
		case ksDocumentAssembly:
			break;
		default:
			log << "Unsupported document type (neither part nor assembly)\n";
			return;
		}

		IEmbodimentsManagerPtr embManager(doc3d);
		if (embManager) {
			IEmbodimentPtr curEmb = embManager->GetCurrentEmbodiment();
			if (curEmb) {
				log << "Embodiment (configuration): " << curEmb->GetName() << "\n";
				IPart7Ptr roorPart = curEmb->GetPart();
				TraversePart(roorPart, what, log, 1);
			}
		}
	}
}

template<typename Lambda>
void MySafeArrayForEach(_variant_t& safeArrayVariant, Lambda l)
{
	switch (safeArrayVariant.vt) {

	// no one item returned
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

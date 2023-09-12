#pragma once
#include "stdafx.h"

#include <fstream>
#include <string>


const char THIS_DIR[] = "C:\\Users\\assemnika\\source\\repos\\KOMPAS_SANDBOX\\";

typedef void(*PartTraverseFunction)(IPart7Ptr, std::ofstream&, int);


void TraverseFeatures(IPart7Ptr part, std::ofstream& log, int depth);

void TraverseVariables(IFeature7Ptr part);


void TraverseEdge(IEdgePtr edge, std::ofstream& log, int depth); void TraverseTriangulation(IEdgePtr edge, std::ofstream& log, int depth);
//		^                                                                ^
//		|________________________________________________________________|
//		|
void TraverseFace(IFacePtr face, std::ofstream& log, int depth);
//		^
//		|
//		|
void TraverseBody(IBody7Ptr body, std::ofstream& log, int depth);
//		^
//		|
//		|
void TraverseBodies(IPart7Ptr part, std::ofstream& log, int depth);
//		^
//		|
//		|
void TraversePart(IPart7Ptr part, PartTraverseFunction what, std::ofstream& log, int depth);
//		^
//		|
//		|
void TraverseThisEmbodiment(PartTraverseFunction what);

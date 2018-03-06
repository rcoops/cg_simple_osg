// raaOSGSimpleDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Geometry> 
#include <osg/ShapeDrawable>
#include <osg/Material>

#include "raaRotationCallback.h"
#include "raaOSGSimpleEventHandler.h"

const static float csg_AmbCoef=0.1f;
const static float csg_DiffCoef=0.8f;
const static float csg_SpecCoef=1.0f;
const static float csg_XSpacing=20.0f; // Spacing between 3 models


osg::Group *g_pRoot=0;

// applies the fixer to loaded models. Uncomment for this to be applied
#define RAA_FIX_MODELS

// since I'm using defines these now control the exercises - uncomment to use, and make sure this is done in numerical order
#define RAA_EX2
//#define RAA_EX3
//#define RAA_EX4
#define RAA_EX5

#ifdef RAA_FIX_MODELS
class raaFixVisitor : osg::NodeVisitor
{
public:
	virtual void apply(osg::Geode& node)
	{
		for (unsigned int i = 0; i < node.getNumDrawables();node.getDrawable(i++)->setUseDisplayList(false));
		traverse((osg::Node&)node);
	}

	osg::Node* operator()(osg::Node* pNode)
	{
		if(pNode) traverse(*pNode);
		return pNode;
	}

	raaFixVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	virtual ~raaFixVisitor() {}
};
#endif

#ifdef RAA_EX2
osg::Node* makeLitSphere(float fDim, float *afCol)
{
	osg::Node *pNode=0;
	if(afCol)
	{
		osg::Geode *pGeode=new osg::Geode();
		osg::ShapeDrawable *pGeom=new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0.0f, 0.0f, 0.0f), fDim));
		pGeode->addDrawable(pGeom);

		osg::Material *pMat=new osg::Material();

		pMat->setAmbient(osg::Material::FRONT, osg::Vec4(afCol[0]*csg_AmbCoef, afCol[1]*csg_AmbCoef, afCol[2]*csg_AmbCoef, afCol[0]));
		pMat->setDiffuse(osg::Material::FRONT, osg::Vec4(afCol[0]*csg_DiffCoef, afCol[1]*csg_DiffCoef, afCol[2]*csg_DiffCoef, afCol[0]));
		pMat->setSpecular(osg::Material::FRONT, osg::Vec4(afCol[0]*csg_SpecCoef, afCol[1]*csg_SpecCoef, afCol[2]*csg_SpecCoef, afCol[0]));
		pMat->setShininess(osg::Material::FRONT, 2.0f);

		pGeode->getOrCreateStateSet()->setAttributeAndModes(pMat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		pGeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		pNode=pGeode;
	}

	return pNode;
}

osg::Node* makeBox(float fDim)
{
	// make a vertex based shape
	osg::Geode *pGeode=new osg::Geode();
	osg::Geometry *pGeom=new osg::Geometry();

	pGeode->addDrawable(pGeom);

	pGeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	float fHalfDim=fDim*0.5f;

	osg::Vec3Array *pVerts=new osg::Vec3Array();
	pVerts->push_back(osg::Vec3(-fHalfDim, -fHalfDim, -fHalfDim));
	pVerts->push_back(osg::Vec3(-fHalfDim, fHalfDim, -fHalfDim));
	pVerts->push_back(osg::Vec3(fHalfDim, fHalfDim, -fHalfDim));
	pVerts->push_back(osg::Vec3(fHalfDim, -fHalfDim, -fHalfDim));

	pVerts->push_back(osg::Vec3(-fHalfDim, -fHalfDim, fHalfDim));
	pVerts->push_back(osg::Vec3(fHalfDim, -fHalfDim, fHalfDim));
	pVerts->push_back(osg::Vec3(fHalfDim, fHalfDim, fHalfDim));
	pVerts->push_back(osg::Vec3(-fHalfDim, fHalfDim, fHalfDim));

	pGeom->setVertexArray(pVerts);

	osg::DrawElementsUByte *pIndex=new osg::DrawElementsUByte(osg::PrimitiveSet::QUADS, 4);

	pIndex->push_back(0);
	pIndex->push_back(1);
	pIndex->push_back(2);
	pIndex->push_back(3);

	pIndex->push_back(0);
	pIndex->push_back(3);
	pIndex->push_back(5);
	pIndex->push_back(4);

	pIndex->push_back(3);
	pIndex->push_back(2);
	pIndex->push_back(6);
	pIndex->push_back(5);

	pIndex->push_back(2);
	pIndex->push_back(1);
	pIndex->push_back(7);
	pIndex->push_back(6);

	pIndex->push_back(1);
	pIndex->push_back(0);
	pIndex->push_back(4);
	pIndex->push_back(7);

	pIndex->push_back(4);
	pIndex->push_back(5);
	pIndex->push_back(6);
	pIndex->push_back(7);

	pGeom->addPrimitiveSet(pIndex);

	osg::Vec4Array *pCols=new osg::Vec4Array();
	pCols->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	pCols->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	pCols->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	pCols->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	pCols->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));
	pCols->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	pCols->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pCols->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	pGeom->setColorArray(pCols);
	pGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	return pGeode;
}
#endif

// THE COW!!
#ifdef RAA_FIX_MODELS
void makeCustomModel(osg::MatrixTransform *pMT, raaFixVisitor &fixer, osg::ArgumentParser &arguments) {
	pMT->addChild(fixer(osgDB::readNodeFiles(arguments)));
}
#else
void makeCustomModel(osg::MatrixTransform *pMT, osg::ArgumentParser &arguments) {
	pMT->addChild(osgDB::readNodeFiles(arguments));
}
#endif

int main(int argc,char* argv[])
{
	osg::ArgumentParser arguments(&argc,argv);

	g_pRoot=new osg::Group();
	g_pRoot->ref();

// load model
#ifdef RAA_FIX_MODELS 
	raaFixVisitor fixer;
	osg::MatrixTransform *pMT = new osg::MatrixTransform();
	g_pRoot->addChild(pMT);
	makeCustomModel(pMT, fixer, arguments);
	//pMT->addChild(fixer(osgDB::readNodeFiles(arguments)));
#else
	osg::MatrixTransform *pMT = new osg::MatrixTransform();
	g_pRoot->addChild(pMT);
	makeCustomModel(pMT, arguments);
	//pMT->addChild(osgDB::readNodeFiles(arguments));
#endif

#ifdef RAA_EX4
	raaRotationCallback *pRotator0 = new raaRotationCallback(osg::Vec3f(0.0f, 1.0f, 0.0f), 2.0f, true);
	pMT->setUpdateCallback(pRotator0);
#endif

	osg::Matrixf m;
	m.makeTranslate(csg_XSpacing, 0.0f, 0.0f);

#ifdef RAA_EX2
	osg::MatrixTransform *pMT0=new osg::MatrixTransform();
	pMT0->setName("unlit_cube");
	pMT0->setMatrix(m);
	osg::MatrixTransform *pMT01 = new osg::MatrixTransform();
	pMT01->addChild(makeBox(4.0f));
	pMT0->addChild(pMT01);
	g_pRoot->addChild(pMT0);

	// add lit sphere as shape
	float afCol[]={1.0f, 0.0f, 0.0f, 1.0f};
	m.makeTranslate(-csg_XSpacing, 0.0f, 0.0f);
	osg::MatrixTransform *pMT1=new osg::MatrixTransform();
	pMT1->setName("lit_sphere");
	pMT1->setMatrix(m);
	osg::MatrixTransform *pMT11 = new osg::MatrixTransform();
	pMT11->addChild(makeLitSphere(4.0f, afCol));
	pMT1->addChild(pMT11);
	g_pRoot->addChild(pMT1);
#endif

	raaRotationCallback *pRotator1 = 0;

#ifdef RAA_EX5
	pRotator1= new raaRotationCallback(osg::Vec3f(0.0f, 0.0f, 1.0f), 1.0f);
	pMT01->setUpdateCallback(pRotator1);
	pMT11->setUpdateCallback(pRotator1);
#endif

	// setup viewer
	osgViewer::Viewer viewer;

	// define graphics context
	osg::GraphicsContext::Traits *pTraits = new osg::GraphicsContext::Traits();
	pTraits->x = 20;
	pTraits->y = 20;
	pTraits->width = 600;
	pTraits->height = 480;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = 0;
	osg::GraphicsContext *pGC = osg::GraphicsContext::createGraphicsContext(pTraits);

	osgGA::KeySwitchMatrixManipulator* pKeyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
	pKeyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
	pKeyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
	pKeyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
	viewer.setCameraManipulator(pKeyswitchManipulator);

	osg::Camera *pCamera = viewer.getCamera();

	pCamera->setGraphicsContext(pGC);
	pCamera->setViewport(new osg::Viewport(0,0, pTraits->width, pTraits->height));

	// add custom handler -> press 'i' for info, 'p' for rendering modes, ('o' for rotation control, exercise 5)
	// see implmentation for how this works
#ifdef RAA_EX2
	viewer.addEventHandler(new raaOSGSimpleEventHandler(pRotator1));
#endif
	// add the thread model handler -> press 'm'
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler -> press 'f'
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler -> press 's'
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the record camera path handler
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler -> press 'c'. look for image file in working dir($(osg)\bin)
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	// add the help handler -> press 'h'
	viewer.addEventHandler(new osgViewer::HelpHandler);

	// set the scene to render
	viewer.setSceneData(g_pRoot);

	viewer.realize();

	return viewer.run();
}


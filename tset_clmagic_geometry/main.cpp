//#include "../src/clmagic/geometry.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <osgDB/ReadFile>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>
#include <osg/Timer>
#include <osg/CoordinateSystemNode>
#include <osg/Notify>
#include <osg/io_utils>
#include <osg/Geometry>
#include <osg/TriangleIndexFunctor>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/UpdateVisitor>

#include <osgSim/LineOfSight>
#include <osgSim/HeightAboveTerrain>
#include <osgSim/ElevationSlice>

#include <osgViewer/Viewer>
#include <osg/KdTree>
#include <iostream>

#pragma comment(lib, "OpenThreadsd.lib")
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "osgUtild.lib")
#pragma comment(lib, "osgGAd.lib")
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgTextd.lib")




int main(int argc, char** argv) {

    osgViewer::Viewer viewer;
    viewer.setSceneData(osgDB::readNodeFile("skydome.osgt"));
    viewer.realize();
    auto* _Wnd = dynamic_cast<osgViewer::GraphicsWindow*>(viewer.getCamera()->getGraphicsContext());
    _Wnd->setWindowRectangle(0, 0, 1600, 900);
    _Wnd->setWindowDecoration(true);

    return viewer.run();
}
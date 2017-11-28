
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

using namespace std;

//The workspace for all spatial objects (universe) in case this is not deduced from input
#define X_MIN 4233432
#define Y_MIN 15525828
#define X_MAX 4282357
#define Y_MAX 15574999


//A rectangular box defined by its lower left and upper right corner.
typedef struct boxInfo
{
	double x_min;
	double y_min;
	double x_max;
	double y_max;
        //Constructors for boxes
	boxInfo(const double x1, const double y1, const double x2, const double y2)
	{
            x_min = x1;
            y_min = y1;
            x_max = x2;
            y_max = y2;
	}
        boxInfo() {};
} box;


//Positional data for a given object
typedef struct Obj_tuple {
	unsigned int ts;	//Location timestamp
	int id,time;
	int cellid;	//Object id
	string VenueID,VenueCatID,VenueName,region;

	string date;
	double x,y;               //May be used for storing LONgitude coordinates and LATitude coordinates
	string tmzoffset;
} TObject;

typedef struct clustercenter{
	double x,y;
	int belongsto;
	bool assigned;
}clstcent;





typedef unordered_multimap<unsigned int, TObject * > TObjChain;       //CAUTION: pointer reference to object location
typedef unordered_map<string,vector<TObject> > clusters;
typedef unordered_map<string,clstcent  > clcenter;

typedef unordered_map<string,vector<unsigned int> > regions2;
typedef unordered_map<string,vector<TObject> > regions3;


//---------------------------------------------------------------------------

//Check whether an object's location is contained within the given rectangle.
bool pointInRect(double x, double y, box rect)
{
	return ( ((x >= rect.x_min) && (x <= rect.x_max) && (y >= rect.y_min) && (y <= rect.y_max)) ? true : false);
}

//Check if two segments overlap along the same axis, i.e. they have a common interval.
bool segmentOverlap(double a_min, double a_max, double b_min, double b_max)
{
	return ( ((a_max < b_min) || (b_max < a_min)) ? false : true);
}

//Intersection exists if only these rectangles have overlapping extents over both axes.
bool rectIntersect(box rect1, box rect2)
{
	return (segmentOverlap(rect1.x_min, rect1.x_max, rect2.x_min, rect2.x_max) && segmentOverlap(rect1.y_min, rect1.y_max, rect2.y_min, rect2.y_max));
}

//Check whether the first rectangle is fully contained within the second one.
bool rectContain(box rect1, box rect2)
{
	return ((rect1.x_min >= rect2.x_min) && ((rect1.y_min >= rect2.y_min) && (rect1.x_max <= rect2.x_max) && (rect1.y_max <= rect2.y_max)) ? true : false);
}

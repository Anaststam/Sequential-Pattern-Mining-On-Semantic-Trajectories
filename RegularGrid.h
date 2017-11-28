#include "structures.h"


class RegularGrid {
public:
  RegularGrid(double &, double &, unsigned int &, unsigned int &);	//1st Constructor
  RegularGrid(double &, double &, double &);				//2nd Constructor
  RegularGrid(unsigned int &, unsigned int &);                          //3rd Constructor
  RegularGrid(unsigned int &, unsigned int &, box);                     //4th Constructor
  ~RegularGrid();							//Destructor

  bool Allocate(unsigned int &);
  inline void UpdateObject(TObject &);
  void clustercent();
  void FindClusterCenter(unsigned int &);
  void regions();
  void FindRegions(unsigned int &);
  void CountTheNewRegions(unsigned int , unsigned int , string );
  void printCenter();
  void PrintClusterCenter(unsigned int &);
  void printGridState();
  void getCellObjects(unsigned int &);
  void Translate(TObject &);
  void RegionsTable();
  void PrintRegionsTable();
  void rangeQuery(box &, vector<long> &);
  unsigned int ts;
  unsigned int cell_cnt;			//Total number of cells


private:
  //Cell contents. Note that pointer references to object locations are used in cell listings (data type TObjChain)
  struct GridCell
  {
	TObjChain ObjInfo;    //Object locations assigned into this cell
	clusters cellclust;
	clcenter cccenter;
        box cellBox;          //Cell rectangle specified by its coordinates
	//bool processed;
  };



  regions2 RegionsTbl;
  regions3 RegionsPoints;
  vector<unsigned int> ObjAssignments;	//For each object, remember the cell it has been allocated to.

  inline unsigned int HashLocation(double &, double &);
  inline map<unsigned int, bool> HashBox(box &);
  inline box getCellBox(unsigned int &);
  void refineCandidates(const unsigned int &, box &, vector<long> &);

  unsigned int obj_cnt;             //Number of distinct objects indexed in the grid

  double XMIN, YMIN, XMAX, YMAX;    //Space bounds (universe of discourse)
  unsigned int GranX, GranY;        //granularity of each dimension for hashing
  double width, height;             //width, height of the 2D space.
  double dx, dy, diag, ndx, ndy;                    //the dimensions of each cell
  GridCell *cell;                   //Cell table

};

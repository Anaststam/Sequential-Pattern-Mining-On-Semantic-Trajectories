#include "RegularGrid.h"
#include <map>

//Constructor #1, getting explicitly the granularity of x,y axes
RegularGrid::RegularGrid(double &pWidth, double &pHeight, unsigned int &pGranX, unsigned int &pGranY)
{
	//In older implementations, dimensions were artificially expanded by 0.01% ...
	//...to avoid assignments at the boundaries. This practice is NOT used here.
	width = pWidth;
	height = pHeight;
	GranX = pGranX;
	GranY = pGranY;
	//Compute cell dimensions
	dx = width/(double)GranX;
	dy = height/(double)GranY;
}

//Constructor #2, implements square cells, by getting their size
RegularGrid::RegularGrid(double &pWidth, double &pHeight, double &pSize)
{
	width = pWidth;
	height = pHeight;
	dx = dy = pSize;
	//Compute granularity for axes
	GranX = (unsigned int)ceil(width/dx);
	GranY = (unsigned int)ceil(height/dy);
}

//Constructor #3, assuming a fixed 2-dimensional Universe, but allowing user to specify the granularity of x,y axes
RegularGrid::RegularGrid(unsigned int &pGranX, unsigned int &pGranY)
{
        XMIN = X_MIN; YMIN = Y_MIN; XMAX = X_MAX; YMAX = Y_MAX;     //Universe defined in structures.h
	width = XMAX - XMIN;
	height = YMAX - YMIN;
	GranX = pGranX;
	GranY = pGranY;
	//Compute cell dimensions
	dx = width/(double)GranX;
	dy = height/(double)GranY;
	ndx = pow(dx,2);
	ndy = pow(dy,2);
	diag= sqrt (ndx+ndy);
	//diag = 4000;
	cout<< "diag is"<<diag<<endl;

}

//Constructor #4, allowing user to specify the granularity of x,y axes and a 2-dimensional Universe
RegularGrid::RegularGrid(unsigned int &pGranX, unsigned int &pGranY, box pBox)
{
        XMIN = pBox.x_min; YMIN = pBox.y_min; XMAX = pBox.x_max; YMAX = pBox.y_max;   //Universe given as argument
	width = XMAX - XMIN;
	height = YMAX - YMIN;
	GranX = pGranX;
	GranY = pGranY;
	//Compute cell dimensions
	dx = width/(double)GranX;
	dy = height/(double)GranY;
}


//Destructor
RegularGrid::~RegularGrid()
{
	//No need to maintain information about object allocations anymore
	ObjAssignments.clear();

	//Destroy contents for all cells
	for(unsigned int cid=0; cid<cell_cnt; cid++)
		cell[cid].ObjInfo.clear();

	delete[] cell;	 //deallocate Cell table
}


//Allocate memory and enable the use of this structure.
//It must be called explicitly after the constructor.
bool RegularGrid::Allocate(unsigned int &pobj_cnt)
{
	obj_cnt = pobj_cnt;
	cell_cnt = GranX*GranY;
	try
	{
		cell = new GridCell[cell_cnt];		//Allocate Cell table

		//Initialize Cell table
		for (unsigned int i=0; i<cell_cnt; ++i)
                {
                    cell[i].cellBox = getCellBox(i);     //Store cell coordinates
                    //cell[i].processed = false;

                }


	}
	catch(...) { return false; }

	return true;
}


//Evaluates hash function (resulting into cell ranges between 0 and GranX*GranY-1 ).
//Returns the cell id, i.e. a pointer to the Cell table.
inline unsigned int RegularGrid::HashLocation(double &x, double &y)
{
    return GranX*(unsigned int)((y-YMIN)/dy)+(unsigned int)((x-XMIN)/dx);
}


//Updates the new position (x,y) of the specified moving object id (oid)
inline void RegularGrid::UpdateObject(TObject &cur_obj)
{
	unsigned int i;
	string venname;


	//Assume ascending timestamp ordering: if update ts < current ts, reject this object update 1h allagh
	//if (cur_obj.ts < ts)
		//return;

	//Remove this object from the cell it was previously allocated to 2h allagh
	//i = ObjAssignments[cur_obj.id];					//Cell id
	//cell[i].ObjInfo.erase(cur_obj.id);				//Delete object from that cell

	//Hashing object's positional update to find the proper cell
	i = HashLocation(cur_obj.x, cur_obj.y);


  cur_obj.cellid=i;
	venname=cur_obj.VenueName;



	//Assign new object location into the cell just found
	//cell[i].ObjInfo.insert(pair< unsigned int, TObject * >(cur_obj.id, &cur_obj));


	unordered_map<string,vector<TObject> >::iterator clustIter;
	clustIter = cell[i].cellclust.find(venname);
	vector<TObject> test;
	test.push_back(cur_obj);
	//insert into cellclust the clusters of each cell
	 if (clustIter != cell[i].cellclust.end())
	{
		cell[i].cellclust[venname].push_back(cur_obj);
	}
	else
	{
		cell[i].cellclust.insert(make_pair(venname, test));
	}

	//cell[i].processed = false;			//Mark that this cell must be probed by query reevaluation
	//ObjAssignments[cur_obj.id] = i;			//Remember this object assignment for subsequent execution cycles
}


//Returns a list of cells overlapping with the given MBB
inline map<unsigned int, bool> RegularGrid::HashBox(box &pBox)
{
    map<unsigned int, bool> cellList;               //List of cells returned with indicators for partial overlap

    unsigned int lc, uc, c, i, gdx;
    bool partialCover = false;
    box cellBox;

    //First, hashing box corners
    lc = HashLocation(pBox.x_min, pBox.y_min);
    uc = HashLocation(pBox.x_max, pBox.y_max);

    //Range of cells affected in each row of the grid
    gdx = uc%GranX - lc%GranX;

    //Find all cells covered by this box after examining the cells of its corners
    for(c=lc; c<=uc-gdx; c+=GranX)
        for(i=c; i<=c+gdx; ++i)
        {
            if (i >= cell_cnt)		//i is the cell currently being examined for overlap
                continue;

            //Check if cell box is fully contained within the query range
            partialCover = !rectContain(cell[i].cellBox, pBox);

            //Insert this cell into the chain according to its total/partial overlap
            cellList.insert(pair<unsigned int, bool>(i, partialCover));
        }

    return cellList;
}


//Calculate cell box coordinates (may be also used for printing them into KML)
inline box RegularGrid::getCellBox(unsigned int &cellID)
{
    //Cell matrix indices along x and y axes
    unsigned int i = cellID % GranX;
    unsigned int j = cellID / GranX;

    double x_min = XMIN + i * dx;
    double y_min = YMIN + j * dy;
    double x_max = XMIN + (i+1) * dx;
    double y_max = YMIN + (j+1) * dy;

    return box(x_min, y_min, x_max, y_max);
}



void RegularGrid::clustercent()
{
	for (unsigned int i=0; i<cell_cnt; ++i)
	{
		FindClusterCenter(i);
	}


}


void RegularGrid::FindClusterCenter(unsigned int &cellID)
{	clstcent   v;
	double x=0.0,x2=0.0;
	double y=0.0,y2=0.0;
	unordered_map<string,vector<TObject> > objSequence3= cell[cellID].cellclust;
	unordered_map<string,vector<TObject> >::iterator objIter3;
	for(objIter3=objSequence3.begin(); objIter3 !=objSequence3.end(); objIter3++)
	{
		string name=objIter3->first;
		//cout<<"CELL "<< cellID<<"   "<< objIter3->first<<endl;
		vector<TObject> ww = objIter3->second;
		vector<TObject>::iterator it2;
		for(it2 = ww.begin(); it2 != ww.end(); it2++)
		{
			//cout<< it2->x << it2->y<<endl;

			x=it2->x;
			y=it2->y;

			x2=(x2+x)/2;


			y2=(y2+y)/2;

		}
		v.x=x2;


		v.y=y2;

		v.belongsto=cellID;
		v.assigned=false;
		cell[cellID].cccenter.insert(make_pair(name,v));

	}

}

void RegularGrid::regions()
{
	for (unsigned int i=0; i<cell_cnt; ++i)
	{
		FindRegions(i);

	}

}

void RegularGrid::FindRegions(unsigned int &cellID)
{	double x=0,y=0,x1=0,y1=0,x3=0,y3=0,d=0;
	int nid;

	unordered_map<string,clstcent > objSequence5 = cell[cellID].cccenter;
	unordered_map<string,clstcent >::iterator objIter5;
	for(objIter5=objSequence5.begin(); objIter5 != objSequence5.end(); objIter5++)
	{
		//cout<<"CELLID:: "<<cellID<<endl;
		string e = objIter5->first;

		x=cell[cellID].cccenter[e].x;
		y=cell[cellID].cccenter[e].y;


		//cout<<" x is"<<x<<" y is"<<y<<endl;
		if(cellID==9 or cellID==19 or cellID==29 or cellID==39 or cellID==49 or cellID==59 or cellID==69 or cellID==79 or cellID==89 or cellID==99)
		{}
		else
		{
			if((cellID+1)>=0 and (cellID+1)<100)
			{


				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID+1].cccenter.find(e);
				if (in != cell[cellID+1].cccenter.end())
				{	//cout<<"cellID+1 is  "<<cellID+1<<endl;
					x1=cell[cellID+1].cccenter[e].x;
					y1=cell[cellID+1].cccenter[e].y;
					//cout<<"x1 is  "<<x1<<"  y1 is"<<y1<<endl;
					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);
					if(d<=diag)
					{
					  nid=cellID+1;
						//cout<<"mindis "<<mindist<<"nid    "<<nid<<endl;
						CountTheNewRegions(nid,cellID,e);
					}
				}

			}
		}
		if(cellID==0 or cellID==10 or cellID==20 or cellID==30 or cellID==40 or cellID==50 or cellID==60 or cellID==70 or cellID==80 or cellID==90)
		{}
		else
		{
			if((cellID-1)>=0 and (cellID-1)<100)
			{
				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID-1].cccenter.find(e);
				if (in != cell[cellID-1].cccenter.end())
				{	//cout<<"cellID-1 is  "<<cellID-1<<endl;
					x1=cell[cellID-1].cccenter[e].x;
					//cout<<"x1 is  "<<x1<<"  y1 is"<<y1<<endl;
					y1=cell[cellID-1].cccenter[e].y;

					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);
					if(d<=diag)
					{

						nid=cellID-1;
						CountTheNewRegions(nid,cellID,e);
					}
				}
			}
		}
		if((cellID+GranX)>=0 and (cellID+GranX)<100)
		{
			unordered_map<string,clstcent  >::iterator in;
			in = cell[cellID+GranX].cccenter.find(e);
			if (in != cell[cellID+GranX].cccenter.end())
			{
				x1=cell[cellID+GranX].cccenter[e].x;
				y1=cell[cellID+GranX].cccenter[e].y;
				//Euclidean Distance between the center of the clusters
				x3=x1-x;
				x3=pow(x3,2);
				y3=y1-y;
				y3=pow(y3,2);
				d= sqrt(x3+y3);

				if(d<diag)
				{

					nid=cellID+GranX;

					CountTheNewRegions(nid,cellID,e);
				}
			}
		}
		if((cellID-GranX)>=0 and (cellID-GranX)<100)
		{
			unordered_map<string,clstcent  >::iterator in;
			in = cell[cellID-GranX].cccenter.find(e);
			if (in != cell[cellID-GranX].cccenter.end())
			{
				x1=cell[cellID-GranX].cccenter[e].x;
				y1=cell[cellID-GranX].cccenter[e].y;
				//Euclidean Distance between the center of the clusters
				x3=x1-x;
				x3=pow(x3,2);
				y3=y1-y;
				y3=pow(y3,2);
				d= sqrt(x3+y3);
				if(d<=diag)
				{
					nid=cellID-GranX;

					CountTheNewRegions(nid,cellID,e);
				}
			}
		}
		if(cellID==9 or cellID==19 or cellID==29 or cellID==39 or cellID==49 or cellID==59 or cellID==69 or cellID==79 or cellID==89 or cellID==99)
		{}
		else
		{
			if((cellID+GranX+1)>=0 and (cellID+GranX+1)<100)
			{
				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID+GranX+1].cccenter.find(e);
				if (in != cell[cellID+GranX+1].cccenter.end())
				{
					x1=cell[cellID+GranX+1].cccenter[e].x;
					y1=cell[cellID+GranX+1].cccenter[e].y;
					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);
					//
					if(d<=diag)
					{

						nid=cellID+GranX+1;
						CountTheNewRegions(nid,cellID,e);
					}
				}
			}
		}
		if(cellID==0 or cellID==10 or cellID==20 or cellID==30 or cellID==40 or cellID==50 or cellID==60 or cellID==70 or cellID==80 or cellID==90)
		{}
		else
		{
			if((cellID+GranX-1)>=0 and (cellID+GranX-1)<100)
			{
				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID+GranX-1].cccenter.find(e);
				if (in != cell[cellID+GranX-1].cccenter.end())
				{
					x1=cell[cellID+GranX-1].cccenter[e].x;
					y1=cell[cellID+GranX-1].cccenter[e].y;
					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);
					if(d<=diag)
					{
						nid=cellID+GranX-1;
						CountTheNewRegions(nid,cellID,e);
					}
				}
			}
		}
		if(cellID==0 or cellID==10 or cellID==20 or cellID==30 or cellID==40 or cellID==50 or cellID==60 or cellID==70 or cellID==80 or cellID==90)
		{}
		else
		{
			if((cellID-GranX-1)>=0 and (cellID-GranX-1)<100)
			{
				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID-GranX-1].cccenter.find(e);
				if (in != cell[cellID-GranX-1].cccenter.end())
				{
					x1=cell[cellID-GranX-1].cccenter[e].x;
					y1=cell[cellID-GranX-1].cccenter[e].y;
					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);
					if(d<=diag)
					{

						nid=cellID-GranX-1;

						CountTheNewRegions(nid,cellID,e);

					}
				}
			}
		}
		if(cellID==9 or cellID==19 or cellID==29 or cellID==39 or cellID==49 or cellID==59 or cellID==69 or cellID==79 or cellID==89 or cellID==99)
		{}
		else
		{
			if((cellID-GranX+1)>=0 and (cellID-GranX+1)<100)
			{
				unordered_map<string,clstcent  >::iterator in;
				in = cell[cellID-GranX+1].cccenter.find(e);
				if (in != cell[cellID-GranX+1].cccenter.end())
				{
					x1=cell[cellID-GranX+1].cccenter[e].x;
					y1=cell[cellID-GranX+1].cccenter[e].y;
					//Euclidean Distance between the center of the clusters
					x3=x1-x;
					x3=pow(x3,2);
					y3=y1-y;
					y3=pow(y3,2);
					d= sqrt(x3+y3);

					if(d<=diag)
					{
						//mindist=d;
						nid=cellID-GranX+1;
						//cout<<"mindis "<<mindist<<"nid    "<<nid<<endl;
						CountTheNewRegions(nid,cellID,e);

					}
				}
			}
		}


	}



}

void RegularGrid::CountTheNewRegions(unsigned int ncellID, unsigned int cellID, string e2)
{
			int k,k2;

					if(cell[ncellID].cccenter[e2].assigned == false)
					{

						if(cell[cellID].cccenter[e2].assigned == false)
						{
							cell[ncellID].cccenter[e2].assigned=true;
							cell[cellID].cccenter[e2].assigned=true;
							cell[ncellID].cccenter[e2].belongsto=cellID;//kanoume to belongsto tou geitonikou iso me cellid
							int a=cell[ncellID].cccenter[e2].belongsto;
							//cout <<"ncellID.belongs to(prepei na einai iso me cellid)::  "<<a<<endl;
							cell[cellID].cccenter[e2].belongsto=cellID;//kai autou pou eksetazoume iso me to cellid
							int b=cell[cellID].cccenter[e2].belongsto;
							//cout <<"cellID.belongsto prepei na einai iso me cellid:: "<<b<<endl;
						}
						else
						{
							k=cell[cellID].cccenter[e2].belongsto;

							cell[ncellID].cccenter[e2].belongsto=k;
							cell[ncellID].cccenter[e2].assigned=true;
						}
					}
					else
					{	//cout<<"**true"<<endl;
						if(cell[cellID].cccenter[e2].assigned == false)
						{	//cout<<"true false"<<endl;
							k=cell[ncellID].cccenter[e2].belongsto;
							cell[cellID].cccenter[e2].belongsto=k;
							cell[cellID].cccenter[e2].assigned=true;
						}
						else
						{	//cout<<"true true"<<endl;
							k=cell[cellID].cccenter[e2].belongsto;
							k2=cell[ncellID].cccenter[e2].belongsto;
							//cout<<"K:::::where the cell belongs"<<k<<endl;
							//cout<<"K2:::::where the ncell belongs"<<k2<<endl;
							if(k==k2)
							{
							}
							else
							{
								for(unsigned int i=0; i<cell_cnt; i++)
								{
									//unordered_map<string,clstcent > objSequence6= cell[i].cccenter;
									unordered_map<string,clstcent >::iterator objIter6;
									objIter6 = cell[i].cccenter.find(e2);
									if (objIter6 != cell[i].cccenter.end())
									{


											if(cell[i].cccenter[e2].belongsto == k2)
											{	//cout<<"to keli pou ekseta twra einai:: "<<i<<endl;
												//cout<<"to cluster einai:: "<<objIter6->first<<endl;
												//cout<<"pou anhke  "<<cell[i].cccenter[e2].belongsto<<endl;
												cell[i].cccenter[e2].belongsto=k;
												//cout<<"pleon exei anatethei sto keli:: "<<cell[i].cccenter[e2].belongsto<<endl;
											}
									//}

									}
								}
							}

						}


					}










}

void RegularGrid::printCenter()
{
	for (unsigned int h=0; h<cell_cnt; ++h)
	{
		PrintClusterCenter(h);				//Currently retained object locations
	}


}

void RegularGrid::PrintClusterCenter(unsigned int &cellID)
{
	unordered_map<string,clstcent > objSequence4= cell[cellID].cccenter;
	unordered_map<string,clstcent >::iterator objIter4;
	for(objIter4=objSequence4.begin(); objIter4 != objSequence4.end(); objIter4++)
	{
		clstcent c = objIter4->second;
		cout<<cellID<<"  "<< objIter4->first<< "   "<<c.x<< "   "<<c.y<<"  belongs to"<<c.belongsto<<endl;
	}

}


//Type all contents of each cell
void RegularGrid::printGridState()
{
	for (unsigned int i=0; i<cell_cnt; ++i)
		getCellObjects(i);



}




//Type object locations currently contained in the given cell
void RegularGrid::getCellObjects(unsigned int &cellID)
{
	int fid;
	//Object contents
	//unordered_multimap < unsigned int, TObject * > objSequence = cell[cellID].ObjInfo;
	//unordered_multimap < unsigned int, TObject * >::iterator objIter;
	unordered_map<string,vector<TObject> > objSequence2= cell[cellID].cellclust;
	unordered_map<string,vector<TObject> >::iterator objIter2;


	//Iterate through the sequence of objects in that cell
	//for( objIter = objSequence.begin(); objIter != objSequence.end(); objIter++ )
	//{
	//	TObject *o = objIter->second;
	//	cout << "CELL " << cellID << " OBJ <" << o->ts << " " << o->id << " " << o->x << " " << o->y << ">" << endl;
	//}

	for(objIter2=objSequence2.begin(); objIter2 !=objSequence2.end(); objIter2++)
	{	cout<<"CELL "<< cellID<<"   "<< objIter2->first<<endl;
		vector<TObject> w = objIter2->second;
		vector<TObject>::iterator it;
		for(it = w.begin(); it != w.end(); it++)
		{
			cout<<it->id<<"   "<< it->x << "   "<< it->y<<endl;
		}

	}
}
void RegularGrid::Translate(TObject &cur_obj2)
{


	string b=to_string(cell[cur_obj2.cellid].cccenter[cur_obj2.VenueName].belongsto);
	string Reg2=cur_obj2.VenueName+b;
	cur_obj2.region=Reg2;





}

void RegularGrid::RegionsTable()
{
	cout<<"here************************"<<endl;
	for (unsigned int i=0; i<cell_cnt; ++i)
	{
		unordered_map<string,clstcent > objSequence7= cell[i].cccenter;
		unordered_map<string,clstcent >::iterator objIter7;
		for(objIter7=objSequence7.begin(); objIter7 != objSequence7.end(); objIter7++)
		{	cout<<"here2************************"<<endl;
			string RegName=objIter7->first;
			clstcent c2 = objIter7->second;
			string RegCode=to_string(c2.belongsto);
			string Reg=RegName+RegCode;
			//cout<<Reg<<endl;
			//cout<<i<<endl;
			unordered_map<string,vector<unsigned int> >::iterator objIter8;
			objIter8=RegionsTbl.find(Reg);
			if(objIter8 != RegionsTbl.end())
			{	cout<<"here3************************"<<endl;
				RegionsTbl[Reg].push_back(i);

			}
			else
			{
				cout<<"here4************************"<<endl;
				vector<unsigned int> test2;
				test2.push_back(i);
				RegionsTbl.insert(make_pair(Reg,test2));

			}
			}
		}
}




void RegularGrid::PrintRegionsTable()
{

	unordered_map<string,vector<unsigned int> > objSequence9= RegionsTbl;
	unordered_map<string,vector<unsigned int> >::iterator objIter9;
	for(objIter9=objSequence9.begin(); objIter9 != objSequence9.end(); objIter9++)
	{
		cout<<objIter9->first<<endl;
		vector<unsigned int> c;
		c=objIter9->second;
		vector<unsigned int>::iterator it6;
		for(it6 = c.begin(); it6 != c.end(); it6++)
		{

		}

}


}

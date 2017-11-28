#include "RegularGrid.cpp"
#include "Prefixspan.cpp"
#include <string>
#include <fstream>
#include <vector>
#include<iostream>
//---------------------------------------------------------------------------
//Global variables
using namespace std;
fstream f1;
double obj_x, obj_y;
string obj_tmzoffset;
int obj_id,obj_time;
int p;
int cur_ts, obj_ts;
int N;
string obj_venueid,obj_venuecatid,obj_venuename;
unsigned int obj_cnt =100000000;
int            min_sup              = 5;    // minimum support for LCM
int            max_pat              = 5;    // maximum_itemset_size : default infinity;
int i3=1;


string obj_date;
vector<TObject> ObjData;
TObject l;

Transaction trans;
TransactionTime transTime;
Pairdata prdata;







//Read the file obj.txt and save each raw at the vector ObjData
void readObjects(string fileName1)
{
    ifstream in;
	in.open(fileName1);
    p=0;
    while(true){
            if (in.eof()) break;
			in>> obj_id>>obj_venueid>> obj_venuecatid >> obj_venuename >> obj_x >> obj_y >> obj_tmzoffset >> obj_date >> obj_time;
            			l.id = obj_id;
            			l.VenueID = obj_venueid;
				l.VenueCatID = obj_venuecatid;
				l.VenueName = obj_venuename;
				l.x = (double)obj_x;
				l.y = (double)obj_y;
				l.tmzoffset = obj_tmzoffset;
				//ObjData[p].utc = obj_utc;
				l.date = obj_date;
				//ObjData[p].month = obj_month;
				//ObjData[p].year = obj_year;
				l.time = (int)obj_time;
				ObjData.push_back(l);


   }

}


int main(int argc, char* argv[])
{
	unsigned int gx, gy;
	unsigned int i, j, k;
	char *fileName1;


	if (argc<4)
	{
		//EXAMPLE: ./grid 50 50 obj100k.txt 100000
		cerr << "\nUsage: " << argv[0] << " [Granularity_X] [Granularity_Y] [Object_file] [Object_count]";
		return 1;
	}

	//Interpretation of given parameters
	gx = atoi(argv[1]);             //Number of grid cells along x-dimension
	gy = atoi(argv[2]);		          //Number of grid cells along y-dimension
	fileName1 = argv[3];            //Filename

	cout << "Initializing..." << endl;

	//Create grid partitioning according to the given parameters
        RegularGrid *grid = new RegularGrid(gx, gy);                 //CAUTION: Bounds for grid must have been declared in structures.h
	if (!grid->Allocate(obj_cnt))
	{
		cout << "Memory Allocation failed for grid partitioning!" << endl;
		return 1;
	}




	cout << "Processing data..." << endl;
	readObjects(fileName1);


		//Assign current object locations into grid cells

		for (std::vector<TObject>::iterator it = ObjData.begin() ; it != ObjData.end(); ++it)

		{
			//cout<<it->id<<"   "<<it->x<<"    "<<it->y<<"    "<<it->VenueName<<endl;

			grid->UpdateObject(*it);


		}

		grid->clustercent();

		grid->regions();
		//grid->RegionsTable();
		//grid->PrintRegionsTable();
		//grid->printCenter();
		//grid->printGridState();


		//Translate x and y coordinates of the trajectory into neighborhoods
		for (std::vector<TObject>::iterator it5 = ObjData.begin() ; it5 != ObjData.end(); ++it5)
    {
			grid->Translate(*it5);
			//cout<<"ID:  "<<it5->id<<"VenueName  "<<it5->VenueName<<"  Region   "<<it5->region<<endl;

		}

		//PrefixSpan
		Prefixspan prefixspan(min_sup, max_pat);
		int pid;
		for (std::vector<TObject>::iterator it6 = ObjData.begin() ; it6 != ObjData.end(); ++it6)
             	{
				int curid=it6->id;
				if(i3==1)
				{
					pid=it6->id;
					trans.first=curid;
					trans.second.push_back(it6->region); //CAUTION: trajectories with only one point won't be taken into account

					transTime.first=curid;
					transTime.second.push_back(it6->time);
					i3++;
				}
				else
				{
					if(curid==pid)
					{
						trans.first=curid;
						trans.second.push_back(it6->region);

						transTime.first=curid;
						transTime.second.push_back(it6->time);
						pid=curid;
					}
					else
					{
						prdata.database.push_back(trans);

						prdata.time.push_back(transTime); //Time

						transTime.second.clear(); //Time
						trans.second.clear();

						trans.first=curid;
						trans.second.push_back(it6->region);

						transTime.first=curid; //xronoi
						transTime.second.push_back(it6->time);//Time

						i3=1;
					}
				}
				//cout<<trans.first<<endl;
		}
		prdata.database.push_back(trans);
		prdata.time.push_back(transTime);

		for(int j=0; j<prdata.database.size(); j++)
		{
			prdata.indeces.push_back(0);
		}
		cout<<"database.size"<<prdata.database.size()<<endl;
		cout<<"indeces.size"<<prdata.indeces.size()<<endl;
		//print the elements of the database
		/*for (std::vector<Transaction>::iterator it7 = prdata.database.begin() ; it7 != prdata.database.end(); ++it7)
             	{
			cout<<"************************ID:   "<<(*it7).first<<endl;
			vector<string> v;
			v=(*it7).second;
			for (std::vector<string>::iterator it8 = v.begin() ; it8 != v.end(); ++it8)
             		{
					cout<<*it8<<endl;

			}

		}*/

		//print the elements of the time
		/*for (std::vector<TransactionTime>::iterator it7 = prdata.time.begin() ; it7 != prdata.time.end(); ++it7)
             	{
			cout<<"************************ID:   "<<(*it7).first<<endl;
			vector<int> v;
			v=(*it7).second;
			for (std::vector<int>::iterator it8 = v.begin() ; it8 != v.end(); ++it8)
             		{
					cout<<*it8<<endl;

			}

		}*/




		prefixspan.project(prdata);

		//end of PrefixSpan


		cout << "end"<< endl;

  f1.close();

	cout << "*****************************************************" << endl;
	cout << "Execution completed successfully!" << endl;
	return 0;
}

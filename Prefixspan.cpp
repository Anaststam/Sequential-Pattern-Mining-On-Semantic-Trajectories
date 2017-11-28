#include "Prefixspan.h"



/********************************************************************
 * Print frequent sequential patterns
 ********************************************************************/
void Prefixspan::print_pattern(Pairdata &projected) {

  for (vector<string>::iterator it = pattern.begin(); it != pattern.end(); it++) {
    cout << *it << " ";
  }

  cout << endl <<  "( ";
  for (vector<Transaction>::iterator it = projected.database.begin(); it != projected.database.end(); it++) {
    cout << it->first << " ";
  }
  cout << ") : " << projected.database.size() << endl;



}

/********************************************************************
 * Project database
 ********************************************************************/
void Prefixspan::project(Pairdata &projected) {

//cout<<"database.size"<<projected.database.size()<<endl;
//cout<<"indeces.size"<<projected.indeces.size()<<endl;



  if (projected.database.size() < min_sup)
    return;


  print_pattern(projected);

  if (max_pat != 0 && pattern.size() == max_pat)
    return;



  map<string, unsigned int> map_item;
  const vector<Transaction> &database = projected.database;
  const vector<TransactionTime> &time = projected.time;

  //print the const vector database
  /*for (vector<Transaction>::const_iterator it6 = database.begin(); it6 != database.end(); it6++)
  {
    cout<<"Project*************************"<<endl;
    vector<unsigned int> v2;
    v2=it6->second;
    for ( vector<unsigned int>::iterator it7 = v2.begin(); it7 !=  v2.end(); it7++)
    {
    	cout<< (*it7)<<endl;
    }

  }*/
  //end of print



  for (unsigned int i = 0; i < database.size(); i++)
  {

    const vector<string> &itemset = database[i].second;
    for (unsigned int iter = projected.indeces[i]; iter < itemset.size(); iter++) //iter=0 ...iter<itemset.size
    {

      ++map_item[itemset[iter]];
    }
  }

  //print map_item
  // cout<<"database size:::"<<database.size()<<endl;
 // for (map<unsigned int, unsigned int>::iterator it_2 = map_item.begin(); it_2 != map_item.end(); it_2++)
 // {
//	cout<<it_2->first<<"     "<<it_2->second<<endl;


 // }
  //end of print


  Pairdata pairdata;


  //print pairdata database
  /*cout<<"here****************"<<endl;
  for (vector<Transaction>::iterator it4 = pairdata.database.begin(); it4 != pairdata.database.end(); it4++)
  {
    cout << it4->first<<"*"<<endl;

    vector<unsigned int> v;
    v=it4->second;
    for ( vector<unsigned int>::iterator it5 = v.begin(); it5 !=  v.end(); it5++)
    {
    cout<< (*it5)<<endl;
    }
  }*/
  //for (vector<unsigned int>::iterator it3 = pairdata.indeces.begin(); it3 != pairdata.indeces.end(); it3++)
  //{
  //  cout << *it3<<"*"<<endl;
  //}

  //end of print


  vector<Transaction> &new_database = pairdata.database;
  vector<unsigned int> &new_indeces = pairdata.indeces;
  vector <TransactionTime> &new_time   = pairdata.time;//xronos


//print new_database
/*cout<<"new_database"<<endl;
for (vector<Transaction>::iterator it4 = new_database.begin(); it4 != new_database.end(); it4++)
  {
    cout<<"in***********"<<endl;
    cout << it4->first<<"*"<<endl;

    vector<unsigned int> v;
    v=it4->second;
    for ( vector<unsigned int>::iterator it5 = v.begin(); it5 !=  v.end(); it5++)
    {
    cout<< (*it5)<<endl;
    }
  }*/
//end of print





  for (map<string, unsigned int>::iterator it_1 = map_item.begin(); it_1 != map_item.end(); it_1++)
  {

   // cout<<"it_1->first:::"<<it_1->first<<endl;
    //cout<<"database.size  "<<database.size()<<endl;//print
    for (unsigned int i = 0; i < database.size(); i++)
    {

      const Transaction &transaction = database[i];

      const TransactionTime &transactiontime = time[i]; //xronos

      //cout<< "transaction.first::::" << transaction.first <<endl; //print
      //cout<< "transaction.second::::" << transaction.second <<endl; //print

      const vector<string> &itemset = transaction.second;
      const vector<int>    &timeset = transactiontime.second; //xronos

      for (unsigned int iter = projected.indeces[i]; iter < itemset.size(); ++iter)
      {
	//cout<<iter<<endl;
        //cout<<"for 3******"<<endl;
        //cout<<"iter::::"<<iter<<endl;
	//cout<<"projected.indeces   "<<projected.indeces[i]<<endl;//print
        //cout<<"itemset[iter]   "<<itemset[iter]<<endl;//print
	if (itemset[iter] == it_1->first)
	{
	     if (iter==0)
	     {

	  	new_database.push_back(transaction);
		new_time.push_back(transactiontime);//xronos
	  	new_indeces.push_back(iter + 1);

	  	break;
	     }
	     else
	     {
			if(abs(timeset[iter]-timeset[iter-1])<=2000)
			{
				//cout<<timeset[iter-1]<<" "<<timeset[iter]<<endl;
				new_database.push_back(transaction);
				new_time.push_back(transactiontime);//xronos
	  			new_indeces.push_back(iter + 1);

	  			break;

			}

	     }

	}
      }
    }
    //print new_database
//cout<<"new_database"<<endl;
for (vector<Transaction>::iterator it4 = new_database.begin(); it4 != new_database.end(); it4++)
  {

    //cout << it4->first<<"*"<<endl;

    vector<string> v;
    v=it4->second;
    for ( vector<string>::iterator it5 = v.begin(); it5 !=  v.end(); it5++)
    {
    //cout<< (*it5)<<endl;
    }
  }
//end of print

    pattern.push_back(it_1->first);

//print pattern push_back
for (vector<string>::iterator it6 = pattern.begin(); it6 != pattern.end(); it6++)
  {
	//cout<<"Pattern:::::"<<(*it6)<<endl;
	//cout<<(*it6)<<endl;
  }
//end of print


//print pairdata databasa
for (vector<Transaction>::iterator it8 = pairdata.database.begin(); it8 != pairdata.database.end(); it8++)
  {
     //cout<<"project2******************"<<endl;//print
    //cout << it8->first<<"*"<<endl;

    vector<string> v;
    v=it8->second;
    for ( vector<string>::iterator it9 = v.begin(); it9 !=  v.end(); it9++)
    {
    //cout<< (*it9)<<endl;
    }
  }
//end of print

//print pairdata indeces
for (vector<unsigned int>::iterator it10 = pairdata.indeces.begin(); it10 != pairdata.indeces.end(); it10++)
  {
    //cout <<"pairdata indeces::::"<< *it10<<"*"<<endl;
  }

 // cout<<"pairdata.database.size::::"<<pairdata.database.size()<<endl;
    project(pairdata);

    pattern.pop_back();

//print of pattern pop_back
for (vector<string>::iterator it7 = pattern.begin(); it7 != pattern.end(); it7++)
  {
	//cout<<"Pattern pop_back:::::"<<endl;
	//cout<<(*it7)<<endl;
  }
//end of print


    pairdata.clear();
    //cout<<"end*************"<<endl;

  }
}

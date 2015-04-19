#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <ctime>
typedef unsigned long long uint64;
using namespace std;

int num;
int total_nodes;
int root_num;
int insert(double,string); //returns -1 if fails, else 1
int insert_in_leaf(string file_name,double key, string value ); // same as above
int insert_in_parent(string file_name, string value, string split_file_name);//same as above

uint64 get_time()
{
	 struct timeval tv;
	 gettimeofday(&tv, NULL);
	 uint64 ret = tv.tv_usec;
	 ret /= 1000;
	 ret += (tv.tv_sec * 1000);
	 return ret;
}

string find1(double,int); //if found returns the corresponding string else, returns the app. leaf node

string find_next_pointer(double, ifstream*);

string find_parent(string child);
std::vector<std::string> linkedList;
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void reindex()
{
	ifstream data;
	data.open("assgn2_bplus_data.txt");
	string line;
	
	total_nodes = 0;
	root_num = 0;
	if(!data.is_open())
	{
		cout<<"ERROR: Not able to open bplus data";
		exit(0);
	}
	std::vector<std::string> x;
	while(getline(data,line))
	{
		x = split(line,'\t');
		if(insert(stod(x[0]),x[1])==-1)
		{
			cout<<"ERROR : Unable to insert "<<x[0]<<" :: "<<x[1]<<endl;
			exit(0);
			
		}	
	}

	data.close();
}
string range_query(double center,double range)
{

	// cout<<"REcieved"<<center<<range<<endl;
	double lower_limit = center-range;
	double upper_limt = center+range;

	string first_node = find1(lower_limit,0);
	if(first_node=="")
	{
		cout<<"RANGE QUERY NOT FOUND"<<endl;
		// exit(0)
	}
	else
	{
		ifstream leaf_file;
		leaf_file.open(first_node);
		// cout<<"Opening "<<first_node<<endl;
		if(!leaf_file.is_open())
		{
			cout<<"ERROR : Unable to open "<<first_node<<endl;
		}
		else
		{
			string line;int flag=0;

			string output="";
			std::vector<string> x;
			string temp;
			getline(leaf_file,temp);
			getline(leaf_file,temp);
			while(getline(leaf_file,line))
			{
				x = split(line,';');
				if(x[1]!="-1" && stod(x[1])>=lower_limit && stod(x[1])<=upper_limt)
				{
					// if(falg==0)
						flag=1;

					output+=x[1]+" :: "+x[0]+"\n";
				}
				else if(x[1]=="-1" && flag ==1)
				{
					// cout<<"ENETRING NEXT FIEL : "<<x[0]<<endl;
					leaf_file.close();
					leaf_file.open(x[0]);
					if(!leaf_file.is_open())
						cout<<"ERROR: Unable to open "<<x[0];
					getline(leaf_file,line);
					getline(leaf_file,line);
			
				}
			}
			return output;
		}
	}

}
void run_query_file()
{
	ifstream query_file;
	query_file.open("querysample.txt");
	if(!query_file.is_open())
	{
		cout<<"ERROR : Unable to open querysample.txt"<<endl;
		exit(0);
	}
	uint64 min_insert=9999999,max_insert=0,min_point=9999999,max_point=0,min_range=9999999,max_range=0;
	uint64 cur_insert=0,cur_point=0,cur_range=0;
	uint64 tot_insert=0,tot_point=0,tot_range=0;
	uint64 tot_insert_square=0,tot_point_square=0,tot_range_square=0;
	int num_insert=0;
	int num_point=0;
	int num_range=0;
	string line;
	std::vector<string> x;
	while(getline(query_file,line))
	{
		x = split(line,'\t');
		if(stoi(x[0])==0)
		{	
			cur_insert = get_time();
			if(insert(stod(x[1]),x[2])==-1)
			{
				cout<<"ERROR : Unable to insert "<<x[1]<<" :: "<<x[2]<<endl;
				exit(0);
			}
			cur_insert = get_time() - cur_insert;
			if(cur_insert<min_insert)
				min_insert = cur_insert;
			if(cur_insert > max_insert)
				max_insert = cur_insert;
			tot_insert+=cur_insert;
			tot_insert_square+=(cur_insert*cur_insert);

			num_insert++;
			cout<<"INSERT SUCCESS!"<<endl;
		}
		else if(stoi(x[0])==1)
		{
			cur_point = get_time();
			string res = find1(stod(x[1]),1);
			cur_point = get_time() - cur_point;
			if(cur_point<min_point)
				min_point = cur_point;
			if(cur_point > max_point)
				max_point = cur_point;
			tot_point+=cur_point;
			tot_point_square+=(cur_point*cur_point);
			num_point++;
			if(res=="")
				cout<<x[1]<<" NOT FOUND"<<endl;
			else
				cout<<"FOUND ["<<x[1]<<"] = "<<res<<endl;

		}
		else if(stoi(x[0])==2)
		{

			cur_range = get_time();
			string out = range_query(stod(x[1]),stod(x[2]));
			cur_range = get_time() - cur_range;
			if(cur_range<min_range)
				min_range = cur_range;
			if(cur_range > max_range)
				max_range = cur_range;
			tot_range+=cur_range;
			tot_range_square+=(cur_range*cur_range);
			num_range++;
			
			cout<<"RANGE QUERY RESULT "<</*lower_limit<<" upper:: "<<upper_limt<<*/"\n****\n"<<out<<"\n****"<<endl;

		}
	}
	uint64 avg=0;
	cout<<"**********STATS**********"<<endl;
	cout<<"***INSERT OPERATION***\n";
	avg = tot_insert/num_point;
	cout<<"MIN TIME: "<<min_insert<<"ms\nMAX TIME : "<<max_insert<<"ms\n";	
	cout<<"TOTAL TIME : "<<tot_insert<<"ms\n";
	cout<<"AVERAGE TIME : "<<avg<<"ms\n";
	cout<<"STANDARD DEVIATION : "<<sqrt(tot_insert_square/num_insert - avg*avg)<<"ms\n";
	
	cout<<"***POINT OPERATION***\n";
	avg = tot_point/num_point;
	cout<<"MIN TIME: "<<min_point<<"ms\nMAX TIME : "<<max_point<<"ms\n";	
	cout<<"TOTAL TIME : "<<tot_point<<"ms\n";
	cout<<"AVERAGE TIME : "<<avg<<"ms\n";
	cout<<"STANDARD DEVIATION c: "<<sqrt(tot_point_square/num_point - avg*avg)<<"ms\n";
	
	cout<<"***RANGE OPERATION***\n";
	avg = tot_range/num_range;
	cout<<"MIN TIME: "<<min_range<<"ms\nMAX TIME : "<<max_range<<"ms\n";	
	cout<<"TOTAL TIME : "<<tot_range<<"ms\n";
	cout<<"AVERAGE TIME : "<<avg<<"ms\n";
	cout<<"STANDARD DEVIATION : "<<sqrt(tot_range_square/num_range - avg*avg)<<"ms\n";
	
	// cout<<"MIN_point : "<<min_point<<"\nMAX_point : "<<max_point<<endl;	
	// cout<<"MIN_range : "<<min_range<<"\nMAX_range : "<<max_range<<endl;	
	// <<"\nTOTAL_POINT : "<<tot_point<<endl<<"\nTOTAL_RANGE : "<<tot_range<<endl;	

}
int main(int argc, char const *argv[])
{
	fstream config_file;
	num=-1;
	total_nodes=-1;
	root_num=-1;
	string line;
	config_file.open("bplustree.config") ;
	if (config_file.is_open())
	{
		 getline (config_file,line);
		  num = atoi(line.c_str());
	}
	// cout<<num<<endl;
	config_file.close();

	config_file.open("index.metadata") ;
	if (config_file.is_open())
	{
		 getline (config_file,line);
		 total_nodes = atoi(line.c_str());
		 getline (config_file,line);
		 root_num = atoi(line.c_str());

	}
	config_file.close();

	
	if(argc>1)
	{
		if(atoi(argv[1])==0)
			reindex();
		else if(atoi(argv[1])==1 )
			run_query_file();


	}
	else
		cout<<"PLEASE ENTER SUITABLE FLAG"<<endl;


	//TODO update the metadoata back befiore exiting
	config_file.open("index.metadata");
	if(!config_file.is_open())
	{
		cout<<"ERROR : UNABEL TO OPEN INDEX_METADATA"<<endl;
		exit(0);
	}
	
	config_file<<to_string(total_nodes)+"\n"+to_string(root_num);

	config_file.close();
	return 0;
}

int insert(double key,string record)
{
	if(total_nodes == 0)
	{
		total_nodes++;
		ofstream node_file;
		
		// std::ostringstream strs;
		// strs << total_nodes;
		
		node_file.open(("node"+to_string(total_nodes)).c_str());
		// cout<<"OPENING FILE 2: "<<("node"+to_string(total_nodes)).c_str()<<endl;
		if(!node_file.is_open())
		{
			cout<<"Error Creating the first node";
			exit(0);
		}
		string line;

		// strs << key;
		line = "leaf_node\n1\n"+record+";"+to_string(key);
		node_file<<line;
		node_file.close();
		root_num=1;
		return 1;
	}
	else
	{
		string leaf = find1(key,0);
		fstream node_file;
		//cout<<"RETURNED FROM find1 \nOPENING FILE 3: "<<leaf<<endl;
		node_file.open(leaf.c_str());
		if(!node_file.is_open())
		{
			printf("\nError: Open leaf node 1 %s\n",leaf.c_str());
			exit(0);
		}
		string num_nodes;
		getline(node_file,num_nodes);
		getline(node_file,num_nodes);
		int num_in_leaf = atoi(num_nodes.c_str());

		if(num_in_leaf<num)
		{
			node_file.close();
			if(insert_in_leaf(leaf,key,record)==1)
			{
				//cout<<"Insert success!"<<endl;
				return 1;
			}
			else
				cout<<"ERROR: insert_in_leaf returned -1"<<endl;
		}
		else
		{

			node_file.close();
			if(insert_in_leaf(leaf,key,record))
			{
				total_nodes++;

				string contents;
				string contents_to_send_away;
				string temp;
				
				//cout<<"OPENING FILE 5:  "<<leaf<<endl;
				node_file.open(leaf.c_str());
				
				if(!node_file.is_open())
				{
					printf("\nError: Open leaf node 99 %s\n",leaf.c_str());
					exit(0);
				}

				getline(node_file,temp);//"leaf_node"
				getline(node_file,temp);//num of nodes;

				int copy_length = ceil((double)((num+1)/2));

				contents+="leaf_node\n"+to_string(copy_length)+"\n"; 
				contents_to_send_away+="leaf_node\n"+to_string(num+1 - copy_length)+"\n";
				int flag=0;
				string least_k;
				int i =0;
				while(getline(node_file,temp))
				{
					if(i<copy_length)
					{
						contents+=temp+"\n";
					}
					else
					{
						if(flag==0)
						{
							std::vector<std::string> x;//for ease of processing
							x=split(temp,';');
							least_k = x[1];
							flag=1;
						}
						contents_to_send_away+=temp+"\n";
					}
					i++;

				}
				// cout<<" I : "<<i<<endl;
				contents+="node"+to_string(total_nodes)+";-1\n";//adding next pointer
				/*for (int i = 0; i < copy_length; i++)
				{
					getline(node_file,temp);
					contents+=temp+"\n";
				}
				

				//starting next file's contents


				while(getline(node_file,temp))
				{
					if(flag==0)
					{
						std::vector<std::string> x;//for ease of processing
						x=split(temp,';');
						least_k = x[1];
						flag=1;
					}
					contents_to_send_away+=temp+"\n";
				}*/

				// node_file.clear();
				node_file.close();
				
				ofstream node_file1;
				node_file1.open(leaf.c_str());
				if(!node_file1.is_open())
				{
					cout<<"ERROR : Unable to  open 97 "<<leaf<<endl;
					exit(0);
				}
				
				// cout<<"CONTENT WRITING TO OLD FILE 6 \n *****\n"<<contents<<"*****"<<endl;
				node_file1<<contents;
				node_file1.close();

				ofstream new_leaf;
				// cout<<"OPENING FILE 4: "<<("node"+to_string(total_nodes)).c_str()<<endl;
				new_leaf.open(("node"+to_string(total_nodes)).c_str());
				if(!new_leaf.is_open())
				{
					printf("\nError: Open leaf node 98 %s\n",("node"+to_string(total_nodes)).c_str());
					exit(0);
				}
				// cout<<"CONTENT WRITING TO NEW FILE 7\n *****\n"<<contents_to_send_away<<"*****"<<endl;
				new_leaf<<contents_to_send_away;
				new_leaf.close();


				// exit(0);
				// strs << total_nodes;
			
				if(insert_in_parent(leaf,least_k,("node"+to_string(total_nodes)).c_str())==-1)
				{
					cout<<"Uh oh. Some error"<<endl;
				}
				else
				{
					//cout<<"Insert Success! 1"<<endl;
					return 1;
				}

  				// node_file<<("node"+strs.str()+"\n").c_str();

				//ASSUMING THE READ AND WRITE POINTERS MOVE together


			}
			else
			{
				cout<<"ERROR : Unable to insert into node: node"<<total_nodes<<" in the 2nd insert"<<endl;

			}

		}

	}
	return -1;
}

string find1(double key,int flag)//if success then return the rec value else return the app node file name
{
	// linkedList new list<string>();
	linkedList.clear();
	string c = "node"+ to_string(root_num);

	linkedList.push_back(c);

	ifstream node_file;
	string is_leaf;
	// int total_entries;
	string total_records;

	//cout<<"OPENING FILE 7: "<<c<<endl;
	node_file.open(c.c_str()) ;
	if (node_file.is_open())
	{
		 getline (node_file,is_leaf);
		 getline (node_file,total_records);
		 // total_entries = atoi(total_records.c_str());
	}
	else
	{
		printf("Unable to open 1%s\n",c.c_str() );
		node_file.close();
		// cout<<"Unable to open"+c.c_str();
		exit(0);
	}
	//cout<<"is_leaf is :: "<<is_leaf.c_str()<<endl<<"total_entries is :: "<<total_records<<endl; 

	while( is_leaf!="leaf_node" )
	{
		c = find_next_pointer(key,&node_file);
		if(c=="")
		{
			cout<<"Uh oh"<<endl;
			exit(0);
		}
		linkedList.push_back(c);
		node_file.close();
		//cout<<"OPENING FILE 8: "<<c<<endl;
		node_file.open(c.c_str()) ;
		if (node_file.is_open())
		{
			 getline (node_file,is_leaf);
			 getline (node_file,total_records); //THese 2 are kinda vital for functioning of find_next_pointer due to file structure
			 // total_entries = atoi(total_records.c_str());
		}
		else
		{
			printf("ERROR: Unable to open %s\n",c.c_str() );
			// cout<<"Unable to open"+c.c_str();
			node_file.close();
			exit(0);
		}
		// node_file.close();
	}
	string line;
	std::vector<std::string> x;
	//reopening file to search for things

	/*node_file.open(c.c_str());
	if (node_file.is_open())
	{
		 getline (node_file,is_leaf);
		 getline (node_file,total_records);
		 // total_entries = atoi(total_records.c_str());
	}
	else
	{
		cout<<"ERROR: Unable to open file in find : "<<c.c_str()<<endl;
		exit(0);
	}	*/
	if(flag ==0)
	{
		node_file.close();
		return c;
	}
	while(getline(node_file,line))
	{
		x = split(line,';');
		if(atof(x[1].c_str())==key)
		{
			node_file.close();
			return x[0];
		}
	}
	node_file.close();
	return "";
	// node_file.close();
	// return c;
}
string find_next_pointer(double key, ifstream* node_file)
{
	//cout<<"*** ENTERING find_next_pointer ***"<<endl;
	double temp_val;
	string temp_line; 
	//assuming that each node_file objecct will have file pointer at the 3rd line
	bool flag=false;
	std::vector<std::string> x;
	std::vector<std::string> prev_x;//for ease of processing
	if(!(node_file)->is_open())
	{
		cout<<"ERROR : NODE_FILE NOT OPEN ANYMORE! "<<endl;
	}
	while(getline(*node_file, temp_line))
	{
		//cout<<"here<"<<temp_line<<endl;
		x =split(temp_line,';');
		if(x[1]=="-1")//reached last ppointer
			break;
		temp_val =  stod(x[1]);
		if(key<=temp_val)
		{
			flag=true;
			prev_x = x;
			break;
		}	
		prev_x = x;
	}
	if(flag==true)
	{
		if(stod(x[1])==key)
		{
			getline(*node_file, temp_line);
			x=split(temp_line,';');
			return x[0];
		}
		else
		{
			//cout<<"TEMPVAL : "<<endl;

			return prev_x[0];

		}	
	}
	else
	{
		//no suitable i found
		return x[0];
	}

	return "";
}

int insert_in_leaf(string leaf_name, double key, string value)
{
	//cout<<"ENTERING insert_in_leaf;\n*****\nLEAF : "<<leaf_name<<"\nkey: "<<key<<"\nvalue: "<<value<<"\n*****"<<endl;
	
	fstream leaf_file;
	
	leaf_file.open(leaf_name);
	//cout<<"OPENING FILE 9: "<<leaf_name<<endl;//("node"+to_string(total_nodes)).c_str()<<endl;

	if(!leaf_file.is_open())
	{
		printf("ERROR : Open leaf node %s",leaf_name.c_str());
		exit(0);
	}
	string contents_to_send_away="";
	
	string last_line;
	
	
	getline(leaf_file,last_line);//for leaf_node
	contents_to_send_away+=last_line+"\n";
	getline(leaf_file,last_line);//for number
	
	contents_to_send_away+=to_string(stoi(last_line)+1)+"\n";
	

	string contents_to_write = value+";"+to_string(key)+"\n";
	// cout<<"contents_to_write : "<<contents_to_write<<endl;
	std::vector<std::string> x;
	int flag=0;
	int special_case = 0;//special case is when the first line is less but next is greater, but to handle the equality case we need special_case var

	getline(leaf_file,last_line);
	//cout<<"last_line:"<<last_line<<endl;
	x = split(last_line,';');

	if(x[1]!="-1" && key < stod(x[1]) )
	{
		contents_to_send_away+=contents_to_write;
		flag=1;
	}
	contents_to_send_away+=last_line+"\n";
	//cout<<"co : \n***\n"<<contents_to_send_away<<"***"<<endl;
	if(key>= stod(x[1]))
		special_case=1;
	// double first_val = stod(x[1]);
	while(getline(leaf_file,last_line))
	{
		x = split(last_line,';');
		//cout<<"SPLIT RESULT : "<<x[0]<<" :: "<<x[1]<<endl;
		if(x[1]=="-1")
		{
			if(flag==0)
			{
				contents_to_send_away+=contents_to_write;
				contents_to_send_away+=last_line+"\n";
				flag=1;
				break;
			}
			else
				contents_to_send_away+=last_line+"\n";
		}

		if(x[1]!="-1" && stod(x[1]) < key)
			contents_to_send_away+=last_line+"\n";
		else if(x[1]!="-1" && flag==0)
		{
			contents_to_send_away+=contents_to_write;
			contents_to_send_away+=last_line+"\n";
			flag=1;
		}
		else if(x[1]!="-1" && flag==1)
		{
			contents_to_send_away+=last_line+"\n";
		}
		
	}
	if(flag==0)
	{
		if(!(key>=atof(x[1].c_str()))) //scenario when adding is need to be done at last line of the file
			return -1;
		else
			contents_to_send_away+=contents_to_write;
	}
	//cout<<"CONTENT TO WRITE IN insert_in_leaf :\n"<<contents_to_send_away<<endl;
	leaf_file.close();
	// leaf_file.clear();
	ofstream li;
	li.open(leaf_name); 
	// leaf_file.seekp(0,ios::beg);
	li<< contents_to_send_away;
	li.close();
	return 1;
}

int insert_in_parent(string original_node,string key,string new_node )
{
	//cout<<"ENTERING insert_in_parent\n ORIGINAL NODE SENT "<<original_node<<" NEW NODE "<<new_node<<" key "<<key<<endl;
	if(original_node == linkedList.front())
	{
		total_nodes++;
		root_num = total_nodes;

		string contents_to_write;
		contents_to_write="node\n1\n";
		contents_to_write+=original_node+";"+key+"\n"+new_node+";-1\n";
		
		ofstream new_root;
		//cout<<"OPENING NEW ROOT 10: "<<("node"+to_string(total_nodes)).c_str()<<endl;//("node"+to_string(total_nodes)).c_str()<<endl;

		new_root.open(("node"+to_string(total_nodes)).c_str());

		if(!new_root.is_open())
		{
			printf("\nError: Open leaf node 98 %s\n",("node"+to_string(total_nodes)).c_str());
			exit(0);
		}
		
		//cout<<"CONTENTS WRRITING TO NEW NODE\n******\n"<<contents_to_write<<"****"<<endl;
		new_root<<contents_to_write;
		new_root.close();
		//cout<<"RETURN From insert_in_parent"<<endl;
		// exit(0);
		return 1;
	}
	else
	{
		string parent = find_parent(original_node);
		
		//cout<<"CHILD : "<<original_node<<" PARENT NODE : "<<parent<<endl;
		fstream parent_file;

		//cout<<"OPENING FILE 11: "<<parent<<endl;
		parent_file.open(parent.c_str());
		if(!parent_file.is_open())
		{
			cout<<"Unable to open"+parent+" here"<<endl;
			exit(0);
		}
		/*Problem can be here*/	
		string contents_to_write = new_node+";"+key+"\n";	//new file contents
		string contents_to_send_away="";//orignal file contents

		
		string line;
		getline(parent_file,line);
		contents_to_send_away+=line+"\n";

		getline(parent_file,line);
		//TODO update the number accordingly
		if(stoi(line)<num)
		{
			std::vector<std::string> x;
			int flag=0;
			contents_to_send_away+=to_string(stoi(line.c_str())+1)+"\n";

			while(getline(parent_file,line))
			{
				x =split(line,';');
				if(x[0]==original_node)
				{
					// if(x[1]=="-1")
						// contents_to_send_away+=x[0]+";"+key+"\n"+new_node+";-1\n";
					// else
					// {
						//cout<<"here"<<endl;
						contents_to_send_away+=x[0]+";"+key+"\n"+new_node+";"+x[1]+"\n";
						// contents_to_send_away+=line+"\n";
						// contents_to_send_away+=contents_to_write;
					// }
				}
				else
					contents_to_send_away+=line+"\n";
			}
			parent_file.close();

			ofstream new_parent_file;
			new_parent_file.open(parent);
			if(!new_parent_file.is_open())
			{
				cout<<"ERROR: UNABLE TO OPEN 94"<<parent<<endl;
				exit(0);
			}
			//cout<<"CONETENTS HERE\n******\n"<<contents_to_send_away<<"******"<<endl;
			new_parent_file<<contents_to_send_away;
			new_parent_file.close();
			// exit(0);
			return 1;
		}
		else
		{
			// exit(0);
			std::vector<std::string> x;
			// string last_line;
			
			int copy_length = ceil((num+1)/2); //allowed k:= n-1, but here k=n so adding one
			
			// strs<<copy_length;
			contents_to_send_away+=to_string(copy_length)+"\n";

			string new_contents="";
			new_contents = "node\n";

			// strs<< (num+1) - copy_length;
			new_contents+=to_string((num) - copy_length)+"\n";

			std::vector<string> temp_strs;// = new std::vector<string>();
			//cout<<" COPY LENGTH "<<copy_length<<endl;
			while(getline(parent_file,line))
			{
				// contents_to_send_away.append(line);
				// cout<<"LINE READ : "<<line<<endl;
				// if(line=="")
					// break;
				x = split(line,';');
				//cout<<"SPLIT RESULT IN PARENT x[0]: "+ x[0] +":: x[1] : "<<x[1]<<endl;
				if(x[0]==original_node)
				{ 
					// if(x[1]=="-1")
						temp_strs.push_back(x[0]+";"+key+"\n");	
						temp_strs.push_back(new_node+";"+x[1]+"\n");
					// else
					// {
						// cout<<"here"<<endl;
						// temp_strs.push_back(contents_to_write);
					// }	
				}
				else
					temp_strs.push_back(line+"\n");
			}
			std::string least_k="";
			//cout<<"LABEL"<<endl;
			// vector<string>::iterator it;
			//cout<<"NUM TILL WE GO "<<num+1<<endl;
			//cout<<"NUM TILL WE COPY "<<copy_length<<endl;
			for(int i=0; i<=num+1; i++ ) 
			{
				if(i<copy_length)
					contents_to_send_away+=temp_strs[i];
				else if(i==copy_length)
				{
					std::vector<std::string> v;
					v = split(temp_strs[i],';');
					contents_to_send_away+=v[0]+";-1\n";
					least_k = v[1];
				}
				else
					new_contents+=temp_strs[i];
            }
			// string new_file_val = temp_strs[copy_length-1];

			total_nodes++;
			// strs<<total_nodes;

			string new_file_name = "node"+to_string(total_nodes);
			
			// parent_file.seekg(0,ios::beg);
			parent_file.close();

			ofstream new_parent;
			new_parent.open(parent.c_str());
			if(!new_parent.is_open())
			{
				cout<<"ERROR :  UNABLE TO OPEN 96 "<<parent<<endl;
				exit(0);
			}
			//cout<<"CONETENTS WRITING TO PARENT 46\n*****"<<endl<<contents_to_send_away<<endl<<"*****"<<endl;
			new_parent<<contents_to_send_away;
			new_parent.close();
			
			ofstream new_node;
			new_node.open(new_file_name.c_str(),ios::trunc);
			if(!new_node.is_open())
			{
				cout<<"ERROR :  UNABLE TO OPEN 95 "<<parent<<endl;
				exit(0);
			}
			//cout<<"CONETENTS WRITING TO NEW FILE 45 \n*****"<<endl<<new_contents<<endl<<"*****"<<endl;
			//cout<<"OPENING FILE 1:"<<new_file_name<<endl;
			new_node<<new_contents;
			new_node.close();

			least_k.erase(std::remove(least_k.begin(), least_k.end(), '\n'), least_k.end());
			// x= split(new_file_val,';');

			if(insert_in_parent(parent,least_k,new_file_name))
				return 1;
			else
				return -1;
		}
		// if()
	}

	return -1;
}
string find_parent(string child)
{
	std::vector<string>::iterator pos = find(linkedList.begin(), linkedList.end(),child);
	if(pos>=linkedList.end())
		cout<<"ERROR : Unable to find parent of a child";
	else
		return *(pos-1);
	return "";
}
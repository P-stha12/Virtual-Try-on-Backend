#include<iostream>
#include<fstream>
#include<set>
#include<sstream>
#include<vector>
#include<algorithm>
#include<iterator>

using namespace std;

//provide source file as first argument and new destination file as second argument in command line

int main(int argc,char* argv[]){
	if(argc<3){
		cout<<"Not enough arguments. Either source or destination file not provided"<<'\n';
		exit(0);
	}
	ifstream myfile(argv[1]);
	ofstream outfile(argv[2]);
	istringstream iss;
	string line;
	string token,v_ind;
	vector<set<int>> con_comps;
	vector<int> deleted_sets;
	vector<int> con_vts;
	int count=0;
	cout<<"Processing... "<<'\n';
	if (myfile.is_open() && outfile.is_open()){
		while(getline(myfile,line)){
			istringstream iss(line);
			iss>>token;
			if (token=="f"){
			count+=1;	
			/*if (count==10)
				break;*/
			//cout<<count<<"number of connected components:"<<con_comps.size()<<'\n';
				if (con_comps.empty()){
					con_comps.push_back(set<int>());
					while(iss>>token){
						//cout<<"inseted into empty:"<<token<<"\n";	
						istringstream token_stream(token);
						getline(token_stream,v_ind,'/');
						con_comps[0].insert(stoi(v_ind));
					}			
					}	
				else{
				int l_set_ind = -1;
				while(iss>>token){
					
					istringstream token_stream(token);
					getline(token_stream,v_ind,'/');
					con_vts.push_back(stoi(v_ind));
					//cout<<v_ind<<' ';
				}
				bool added=false;
				vector<int> con_inds_sets;
				for (auto ind=con_vts.begin();ind!=con_vts.end();++ind){
					int j=0;
					for (int i = 0; i != con_comps.size(); ++i, ++j){
						if (con_comps[i].find((*ind)) != con_comps[i].end()){
							if(!added){
							copy(con_vts.begin(),con_vts.end(),inserter(con_comps[i],con_comps[i].end()));
							added = true;
							}
							con_inds_sets.push_back(i);
							break;
							}
						
					}
					
					//deleted_sets.clear();
				}
				if(!added){
					//cout<<"inside !added0"<<con_comps.size();
					con_comps.push_back(set<int> (con_vts.begin(),con_vts.end()));
					//cout<<"inside !added"<<con_comps.size();
				}
				else{
					sort(con_inds_sets.begin(),con_inds_sets.end(),greater<int>());
					con_inds_sets.erase(std::unique(con_inds_sets.begin(), con_inds_sets.end()), con_inds_sets.end());

					//cout<<"deleting"<<con_inds_sets.size()-1<<"element"<<'\n';
					for(auto it=con_inds_sets.begin(); it!=con_inds_sets.end()-1;++it){
						con_comps[con_inds_sets.back()].merge(con_comps[*it]);
						con_comps.erase(con_comps.begin()+(*it));
					}
				}
					con_inds_sets.clear();
					con_vts.clear();
					
				
				}
				/*
				for(int k=0;k<con_comps.size();++k){
					cout<<"{";
					for(auto it=con_comps[k].begin();it!=con_comps[k].end();++it){
						cout<<*it<<',';
					}
					cout<<"}\n";
				}
				cout<<'\n';*/
			}

		}
	sort(con_comps.begin(),con_comps.end(),[](const std::set<int>& set1, const std::set<int>& set2) { return set1.size() > set2.size(); });

	cout<<con_comps[0].size()<<'\n';
	cout<<con_comps[1].size()<<'\n';
	set<int> largest_set = con_comps[0];
	myfile.clear();
	myfile.seekg(0,ios::beg);
	

	vector<string> lines;
	while(getline(myfile,line)){
		lines.push_back(line);
	istringstream iss(line);
	iss>>token;
	con_vts.clear();
	if(token=="f"){
					
				while(iss>>token){
					istringstream token_stream(token);
					getline(token_stream,v_ind,'/');
					con_vts.push_back(stoi(v_ind));

				}
				for(auto & v_ind: con_vts){
					if (largest_set.find(v_ind)== largest_set.end()){
						lines.pop_back();
						break;	
					}
				}

	}

	}	
	for(auto & l : lines){
		outfile<<l<<'\r'<<'\n';
	}	
	cout<<"Largest connected component extracted successfully"<<'\n';
		myfile.close();
		outfile.close();
	}

}

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<set>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"

using namespace std;

string input_file_path = "pandey/sugarfine_3Dgs7000_densityestim02_sdfnorm02_level03_decim200000_normalconsistency01_gaussperface6.obj";
string output_file_path = "pandey/new_shoes.obj";
const char png_file_path[] = "pandey/sugarfine_3Dgs7000_densityestim02_sdfnorm02_level03_decim200000_normalconsistency01_gaussperface6.png";
int main(){
	std::ifstream myfile;
	std::ofstream outfile(output_file_path);
	string line;
	string token;
	float u,v;
	int x,y;
	int height,width,channels;
	int c0,c1,c2;
	int offset = 0;
	vector<string> lines;
	set<int> dark_vt;

	unsigned char *img = stbi_load(png_file_path,&width,&height, &channels, 0);
	if (img==NULL){
		cout<<"Error in loading the image\n";
		exit(1);
	}
	int count = 1;
       myfile.open(input_file_path);
       if(myfile.is_open()&& outfile.is_open()){
	while (getline(myfile,line)){

		istringstream iss(line);
		iss>>token;
		if(token=="vt"){
			iss>>u>>v;
			
			x = static_cast<int>(u*width-0.5);
		        y = static_cast<int>((1.0f-v)*height-0.5);	
			
			x = max(0,min(width-1,x));
			y = max(0,min(height-1,y));

			int index = (x+y*width)*channels;
			c0 = static_cast<int>(img[index]);
			c1 = static_cast<int>(img[index+1]);
			c2 = static_cast<int>(img[index+2]);
			//cout<<c0<<','<<c1<<','<<c2<<'\n';
			if(c0==0 && c1==0 && c2==0){
				dark_vt.insert(count);
			}
count+=1;
		}
	}
	myfile.clear();
	myfile.seekg(0,ios::beg);
       	while (getline(myfile,line)){
		lines.push_back(line);
		istringstream iss(line);
		iss>>token;
		 if( token=="f"){
			
			string vts;
			string index_str;
			int index;
			string out;
			ostringstream oss("");
			oss<<"f ";
			bool run = true;
			while(iss>>vts && run){
				istringstream vts_stream(vts);
				getline(vts_stream,index_str,'/');
				if(getline(vts_stream,index_str,'/')){
					index = stoi(index_str);
					if (dark_vt.find(index)!=dark_vt.end())
					{run=true;
						//cout<<index<<lines.back()<<'\n';
						lines.pop_back();
						run = false;}

					else
						run=false;
							
					}
					
			}
			if (run){
				lines.pop_back();
			}
	}
	}
	for(const auto &l:lines){
		outfile<<l<<'\r'<<'\n';
	}
	  cout<<"File processing completed successfully.\n";
       	   myfile.close();
       	   outfile.close();	
	}

cout<<"Total dark faces removed: "<<dark_vt.size()<<'\n';
		stbi_image_free(img);
}	

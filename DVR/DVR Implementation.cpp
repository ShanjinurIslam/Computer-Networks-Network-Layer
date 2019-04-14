#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <bitset>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

int id = 0 ;

class routeDetails{
public:
	int node1 ;
	int node2 ;
	int distance ;


	routeDetails(int node1,int node2,int distance){
		this->node1 = node1 ;
		this->node2 = node2 ;
		this->distance = distance ;
	}

	void showDetails(){
		cout<<node1<<" "<<node2<<" "<<distance<<endl ;
	}
};

class routerAddress{
public:
	int id ;
	string ip ;
	routerAddress(){
	}
	routerAddress(string ip,int id){
		this->id = id ;
		this->ip = ip ;
	}


};

class DVRProtocol{
private:
	int n ;
	routerAddress ra ;
	std::vector<int> adjacent ;
	std::vector<routerAddress> neigh;
	std::vector<routeDetails> rd ;
	std::vector<routeDetails> downroutes ;
	std::vector<int> down;
	int *dvecmap ;
	string routerIP ;
	int **distance ;
	int bytes_received;
	int sockfd;
    int bind_flag;
    int **nexthop ;
    struct sockaddr_in neigh_address;
	struct sockaddr_in router_address;
public:
	DVRProtocol(routerAddress ra,std::vector<routerAddress> v,std::vector<routeDetails> rd){
		//router table fix
		this->n = v.size() ;
		this->ra = ra ;
		this->rd = rd ;
		this->neigh = v ;
		routerIP = ra.ip ;
		///router table setup

		distance = new int*[n];
		nexthop = new int*[n] ;
		for(int i = 0; i < n; ++i){
    		distance[i] = new int[n];
    		nexthop[i] = new int[n] ;
    		dvecmap = new int[n] ;
		}


    	for(int i=0;i<n;i++){
    		dvecmap[i] = 0 ;
    		for(int j=0;j<n;j++){
    			if(i==ra.id && j==ra.id){

    				 distance[i][j] = 0 ;
    				 nexthop[i][j] = ra.id ;
    			}
    			else{
    				distance[i][j] = std::numeric_limits<int>::max();
    				nexthop[i][j] = -1 ;
    			}
    		}
    	}

    	for(int i=0;i<rd.size();i++){
    		if(rd.at(i).node1==ra.id)
    		{
    			if (std::find(adjacent.begin(), adjacent.end(), rd.at(i).node2) == adjacent.end()) {
					  adjacent.push_back(rd.at(i).node2);
				}
    			distance[ra.id][rd.at(i).node2] = rd.at(i).distance ;
    			nexthop[ra.id][rd.at(i).node2] = rd.at(i).node2 ;
    		}
    		if(rd.at(i).node2==ra.id)
    		{
    			if (std::find(adjacent.begin(), adjacent.end(), rd.at(i).node1) == adjacent.end()) {
					  adjacent.push_back(rd.at(i).node1);
				}
    			distance[ra.id][rd.at(i).node1] = rd.at(i).distance ;
    			nexthop[ra.id][rd.at(i).node1] = rd.at(i).node1 ;
    		}
    	}

		router_address.sin_family = AF_INET;
		router_address.sin_port = htons(4747);
		router_address.sin_addr.s_addr = inet_addr(routerIP.c_str());	
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		bind_flag = bind(sockfd, (struct sockaddr*) &router_address, sizeof(sockaddr_in));

		if(!bind_flag){
			cout<<"Connection Successful\n------------------------------"<<endl ;
		}
		else{
			cout<<"Address already in bind"<<endl ;
		}
	}

	void showRouteTable(){
		cout<<endl ;
		cout<<"IP\tCost\tNextHop"<<endl ;
		for(int i=0;i<n;i++){
			if(nexthop[ra.id][i]==-1){
				cout<<getIP(i)<<" \t-- --\t "<<endl ;
			}
			else 
				cout<<getIP(i)<<" "<<distance[ra.id][i]<<" "<<getIP(nexthop[ra.id][i])<<endl ;
		}

		cout<<endl ;
	}

	void sendData(string ip,const char data[],int len){
		neigh_address.sin_family = AF_INET;
		neigh_address.sin_port = htons(4747);
		neigh_address.sin_addr.s_addr = inet_addr(ip.c_str());
		sendto(sockfd, data,len, 0, (struct sockaddr*) &neigh_address, sizeof(sockaddr_in));
	}

	int getID(string ip){
		for(int i=0;i<neigh.size();i++){
			if(neigh.at(i).ip==ip){
				return i ;
			}
		}
		return 0 ;
	}

	string getIP(int id){
		for(int i=0;i<neigh.size();i++){
			if(neigh.at(i).id==id){
				return neigh.at(i).ip ;
			}
			
		}
		return "0.0.0.0" ;
	}

	void BF(int id){
		for(int i=0;i<n;i++){
				if(distance[ra.id][i] > distance[ra.id][id] + distance[id][i]){
					distance[ra.id][i] = distance[ra.id][id] + distance[id][i] ;
					if(nexthop[ra.id][i]!=nexthop[ra.id][id])
						nexthop[ra.id][i] = id ;

					cout<<"Recalculated Values :"<<endl ;
					cout<<"Source : "<<getIP(ra.id)<<" Midpoint: "<<getIP(id)<<" Destination :"<<getIP(i)<<endl ;
					cout<<endl ;
				}
		}
	}

	void clear(){
					for(int i=0;i<n;i++){
			    		for(int j=0;j<n;j++){
			    			if(i==ra.id && j==ra.id){

			    				 distance[i][j] = 0 ;
			    				 nexthop[i][j] = ra.id ;
			    			}
			    			else{
			    				distance[i][j] = std::numeric_limits<int>::max();
			    				nexthop[i][j] = -1 ;
			    			}
			    		}
		    	    }

			    	for(int i=0;i<rd.size();i++){
			    		if(rd.at(i).node1==ra.id)
			    		{
			    			if (std::find(adjacent.begin(), adjacent.end(), rd.at(i).node2) == adjacent.end()) {
								  adjacent.push_back(rd.at(i).node2);
							}
							if (std::find(down.begin(), down.end(), rd.at(i).node2) == down.end()) {  
				    			distance[ra.id][rd.at(i).node2] = rd.at(i).distance ;
				    			nexthop[ra.id][rd.at(i).node2] = rd.at(i).node2 ;
							}
			    		}
			    		if(rd.at(i).node2==ra.id)
			    		{
			    			if (std::find(adjacent.begin(), adjacent.end(), rd.at(i).node1) == adjacent.end()) {
								  adjacent.push_back(rd.at(i).node1);
							}
							if (std::find(down.begin(), down.end(), rd.at(i).node1) == down.end()) {  
				    			distance[ra.id][rd.at(i).node1] = rd.at(i).distance ;
				    			nexthop[ra.id][rd.at(i).node1] = rd.at(i).node1 ;
							}
			    		}
			    	}
	}


	void start(){
		socklen_t addrlen;
		int clock_count = 0 ;
		showRouteTable() ;
		while(true)
		{
			unsigned char buffer[1024];
	    	if ((bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &router_address, &addrlen))!=-1)
	        {
	        	clock_t begin = clock();
	        	string recvip(inet_ntoa(router_address.sin_addr)) ;
	        	char subbuff[5];
				memcpy( subbuff, &buffer, 4 );
				subbuff[4] = '\0';
				if(!strcmp(subbuff,"clk ")){
						for(int i=0;i<adjacent.size();i++){
							string ip = getIP(adjacent.at(i)) ;
						
							string d = "dvec" ;
							for (int i = 0; i < n; i++)
							{
								unsigned char a = distance[ra.id][i] ;
								d += a ;
							}
							for (int i = 0; i < n; i++)
							{
								unsigned char a = nexthop[ra.id][i] ;
								d += a ;
							}
							sendData(ip,d.c_str(),(n)+8) ;
						}
						clock_count++ ;
						if(clock_count%3==0){
							for(int i=0;i<adjacent.size();i++){
								if(dvecmap[adjacent[i]]==0){
									if(std::find(down.begin(), down.end(), adjacent[i]) != down.end()){
									}
									else{
										cout<<getIP(adjacent[i])<<" :: Not sending distance vector"<<endl ;
										down.push_back(adjacent[i]) ;
										/*for(int i=0;i<rd.size();i++){
											if(rd[i].node1==adjacent[i] && rd[i].node2==ra.id){
												downroutes.push_back(rd[i]) ;
												rd.erase(rd.begin()+i) ;
											}
											if(rd[i].node2==adjacent[i] && rd[i].node1==ra.id){
												downroutes.push_back(rd[i]) ;
												rd.erase(rd.begin()+i) ;
											}
										}*/
										clear() ;
										string clr = "clr " ;
										for(int i=0;i<adjacent.size();i++){
											sendData(getIP(adjacent[i]),clr.c_str(),4) ;
										}
									}
								}
								else{
									//cout<<getIP(adjacent[i])<<"\t"<<dvecmap[adjacent[i]]<<endl ;
									dvecmap[adjacent[i]]=0 ;
								}									
							}
						}
				}

				if(!strcmp(subbuff,"clr ")){
					clear() ;
 				}
				if(!strcmp(subbuff,"dvec")){
					//cout<<recvip<<endl ;
					//eikhane link deactive hoise kina check korte hobe
					int id = getID(recvip) ;
					if(std::find(down.begin(), down.end(), id) != down.end()){
						down.erase(std::remove(down.begin(), down.end(), id), down.end());
						cout<<"Link Updated :"<<recvip<<endl ;
						string clr = "clr " ;
						showRouteTable() ;
						for(int i=0;i<adjacent.size();i++){
							sendData(getIP(adjacent[i]),clr.c_str(),4) ;
						}
					}
					else{
						dvecmap[id]++ ;
						int a[2*n] ;
						for(int i=4;i<bytes_received;i++){
							a[i-4] = (int)((unsigned char)buffer[i]) ;
						}
						
						for(int i=0;i<n;i++){
							distance[id][i] = a[i] ;
						}
						for(int i=n;i<2*n;i++){
							nexthop[id][i-n] = a[i] ;
						}
						BF(id) ;
					}
				}
				if(!strcmp(subbuff,"show")){
					showRouteTable() ;
				}
				if(!strcmp(subbuff,"cost")){
					std::vector<int> v;
					for(int i=4;i<bytes_received;i++){
						v.push_back((int)((unsigned)buffer[i])) ;
					}
					cout<<endl ;
					string ip1,ip2 ;
					ip1 = to_string(v.at(0))+"."+to_string(v.at(1))+"."+to_string(v.at(2))+"."+to_string(v.at(3)) ;
					ip2 = to_string(v.at(4))+"."+to_string(v.at(5))+"."+to_string(v.at(6))+"."+to_string(v.at(7)) ;

					int cost = v.at(8) + v.at(9)*256 ;

					cout<<"Updated Cost : "<<ip1<<"-->"<<ip2<<"	Cost: "<<cost<<endl ;
	
					int node1 = getID(ip1) ;
					int node2 = getID(ip2) ;
					//clearing part
				
					showRouteTable() ;
					for(int i=0;i<rd.size();i++){
						if(rd.at(i).node1==node1 && rd.at(i).node2==node2)
			    		{
			    			rd.at(i).distance = cost ;

			    			cout<<rd.at(i).node1<<" "<<rd.at(i).node2<<" "<<rd.at(i).distance<<endl ;
			    		}
			    		if(rd.at(i).node1==node2 && rd.at(i).node2==node1)
			    		{
			    			rd.at(i).distance = cost ;

			    			cout<<rd.at(i).node1<<" "<<rd.at(i).node2<<" "<<rd.at(i).distance<<endl ;
			    		}
					}
					
					clear() ;
					cout<<"After\n-----------\n" ;
					showRouteTable() ;
					string clr = "clr " ;

					for(int i=0;i<adjacent.size();i++){
						sendData(getIP(adjacent[i]),clr.c_str(),4) ;
					}
  				}
				if(!strcmp(subbuff,"send")){
					std::vector<int> v;
					for(int i=4;i<12;i++){
						v.push_back((int)((unsigned)buffer[i])) ;
					}
					string ip1,ip2 ;
					ip1 = to_string(v.at(0))+"."+to_string(v.at(1))+"."+to_string(v.at(2))+"."+to_string(v.at(3)) ;
					ip2 = to_string(v.at(4))+"."+to_string(v.at(5))+"."+to_string(v.at(6))+"."+to_string(v.at(7)) ;
					int size = (int)((unsigned char)buffer[12]) + 256*(int)((unsigned char)buffer[13]) ;
					char a[size];
					for(int i=14;i<14+size;i++){
						a[i-14] = (char) buffer[i] ;
					}

					int did = getID(ip2) ;
					int next = nexthop[ra.id][did] ;
					string nexthop = getIP(next) ;
					string packet(a) ;
					string msg = "frwd "+to_string(did)+" "+to_string(packet.length())+" "+packet ;

					cout<<a<<" "<<"packet forwared to "<<nexthop<<endl ;
					sendData(nexthop,msg.c_str(),msg.length()+0)  ;
				}

				if(!strcmp(subbuff,"frwd")){
					int spacecount = 0 ;
					std::vector<string> v;
					string tem = "" ;
					int j=0 ;
					for(int i=5;i<bytes_received;i++){
						if(buffer[i]==' ' && j<2){
							v.push_back(tem) ;
							tem = "" ;
							j++ ;
						}
						else if(i==bytes_received-1){
							tem+=buffer[i];
							v.push_back(tem) ;
							tem = "" ;
						}
						else{
							tem += buffer[i] ;
						}
					}

					if(stoi(v[0])==ra.id){
						cout<<v[2]<<" packet received by "<<ra.ip<<endl ;
					}
					else{
						int did = stoi(v[0]) ;
						int r = stoi(v[1])+1 ;
						int next = nexthop[ra.id][did] ;
						string nexthop = getIP(next) ;
						string msg = "frwd "+to_string(did)+" "+to_string(v[2].length())+" "+v[2] ;
						cout<<v[2]<<" "<<"packet forwared to "<<nexthop<<endl ;
						sendData(nexthop,msg.c_str(),msg.length())  ;
					}
				}
				
		    }
		    sleep(1) ;
		}
	}

};

int main(int argc, char *argv[]){
	std::vector<routerAddress> v;
	std::vector<routeDetails> rd;
	string ip(argv[1]) ;
	routerAddress ra;
	std::ifstream infile(argv[2]);
	int flag = false ;
	string ip1,ip2 ;
	int distance ;

	while (infile >> ip1 >> ip2 >> distance)
	{
		for(int i=0;i<v.size();i++)    {
		    routerAddress tem = v.at(i) ;
		    if(tem.ip==ip1){
		    	flag = true ;
		    	break ;	
		    }
		}
		if(!flag){
			routerAddress a(ip1,id++) ;
			v.push_back(a) ;
		}

		flag = false ;

		for(int i=0;i<v.size();i++)    {
		    routerAddress tem = v.at(i) ;
		    if(tem.ip==ip2){
		    	flag = true ;
		    	break ;	
		    }
		}
		if(!flag){
			routerAddress a(ip2,id++) ;
			v.push_back(a) ;
		}

		int node1 ;
		int node2 ;
		for(int i=0;i<v.size();i++)    {
		    routerAddress tem = v.at(i) ;
		    if(tem.ip==ip1){
		    	node1 = tem.id ;
		    }
		    if(tem.ip==ip2){
		    	node2 = tem.id ;
		    }
		}
		routeDetails temrd(node1,node2,distance) ;
		rd.push_back(temrd) ;
	}
	
	for(int i=0;i<v.size();i++){
		if(v.at(i).ip==ip){
			ra.ip = v.at(i).ip ;
			ra.id = v.at(i).id ;
			break ; 
		}
	}

	DVRProtocol d(ra,v,rd) ;
	d.start() ;
	return 0 ;
}

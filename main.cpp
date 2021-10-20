#include "tile.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <unordered_map>
#include <set>
#include <algorithm>

userlog *Log;


void pointRecord(int x,int y,std::list<std::pair<int,int>>&pointRec);
int parser(std::string filename,std::list<tile*>&blocks,std::list<std::pair<int,int>>&pointRec);
void output(std::string filename,std::vector<tile*>&blocks,std::list<std::pair<int,int>>&pointRec);

int main(int argc,char *argv[])
{

    if(argc!=2){std::cerr<<"please enter {./Lab1}.exe <input> \n";exit(1);}

    std::list<tile*>blocks;
    std::list<std::pair<int,int>>FindingRec;
    int BlocksNum = parser(argv[1],blocks,FindingRec);

    // sort by id 
    std::vector<tile*>blocksVec;blocksVec.resize(BlocksNum);
    std::copy(blocks.begin(),blocks.end(),blocksVec.begin());
    std::sort(blocksVec.begin(),blocksVec.end(),[](tile*t1,tile*t2){return t1->id() < t2->id();});

    // output 
    output("output.txt",blocksVec,FindingRec);

    return 0;
}

void pointRecord(int x,int y,std::list<std::pair<int,int>>&pointRec){
    // if t is a spacetile, it may no longer exist after some insertblocks.
    tile* t = pointFinding(x,y);
    pointRec.push_back({t->x(),t->y()});
}
int parser(std::string filename,std::list<tile*>&blocks,std::list<std::pair<int,int>>&pointRec){
    
    std::ifstream input{filename};
    if(!input){std::cerr<<"open file"<<filename<<"failed!\n";exit(1);}

    int window_Width,window_Height;
    input>>window_Width>>window_Height;

    Log = new userlog(window_Width,window_Height);

    int num = 0;
    std::string id;
    while(input >> id){
        if(id!="P"){
            int x, y, w, h;
            input >> x >> y >> w >> h;
            blocks.push_front(InsertBlock(std::stoi(id), x, y, w, h));
            num += 1;
        }else{
            int x, y;
            input >> x >> y;
            pointRecord(x,y,pointRec);
        }
    }
    input.close();
    return num;
}
void output(std::string filename,std::vector<tile*>&blocks,std::list<std::pair<int,int>>&pointRec){

    std::ofstream out{filename};

    
    std::unordered_map<int,std::pair<int,int>>Blocktiles; // id , <adjacent block num,adjacent space num>
    std::set<tile*>Alltiles;

    // count neighbors 
    for(auto block : blocks){
        Alltiles.insert(block);
        int adjBlock = 0,adjSpace = 0;
        auto neighbors = getNeighbor(block);
        for(auto nb:neighbors){
            if(nb->isSpace())
                adjSpace ++;
            else
                adjBlock ++;
            Alltiles.insert(nb);
        }
        Blocktiles.insert({block->id(),{adjBlock,adjSpace}});
    }


    out << Alltiles.size() <<"\n";

    for(auto block:blocks){
        auto &adj = Blocktiles[block->id()];
        out << block -> id() << " " << adj.first << " " << adj.second << "\n"; 
    }
    
    for(auto searchRec : pointRec){
        out << searchRec.first << " " << searchRec.second << "\n";
    }

    std::cout<<"already save to "<<filename<<"\n";
    out.close();

}

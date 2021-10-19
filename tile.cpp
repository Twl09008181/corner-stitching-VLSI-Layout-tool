#include "tile.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
userlog *Log;

// show information of tile t , including id,x,y,w,h.    
std::ostream& operator<<(std::ostream&os,const tile &t){
    return os<<"tile id:"<<t.id()<<" left-corner : ("<<t.x()<<","<<t.y()<<")  width : "<<t.w()<<" height : "<<t.h();
}

// Horizontal/Vertical move   
tile* Hfind(int x,tile*t){
    if(t && x < t->x()){
        while(t && x < t->x())
            t = t -> bl();
    }else if(t && x >= t->x() + t->w()){   // point can't lie on right edge , so when x = t->x() + t->w() , it must move.
        while(t && x >= t->x() + t->w())
            t = t -> tr();
    }
    return t;
}
tile* Vfind(int y,tile*t){
    if(t && y > t->y() + t->h()){
        while(t && y > t->y() + t->h())
            t = t -> rt();
    }else if(t && y <= t->y()){   // point can't lie on bottom edge , so when y = t->y()  , it must move.
        while(t && y <= t->y())
            t = t -> lb();
    }
    return t;
}


// (x,y) Point is allowed to lie on left/top edge of tile, but is not allowed to lie on right/bottom of tile.       
// This property make one point only be contained by one tile.   
//hint   x can equal to t->x()  (left edge )  , y can equal to t->y() + t->h()  (top edge )
// but x can't equal to t->x() + t->w() (right edge) , y can't eqautl to t->y() (bottom edge)
tile* pointFinding(int x,int y){  
    tile *t = Log->lastLog();
    if(t==nullptr){std::cerr<<"erro arg in point Finding\n";exit(1);}
    auto found = [](int x,int y,tile*t){return ( (x >= t->x()) && (x < t->x() + t->w()) && (y > t->y()) && (y <= t->y() + t->h()));};
    while(t&&!found(x,y,t)){
        t = Hfind(x,t);//Horizontal 
        if(t && found(x,y,t))break;
        t = Vfind(y,t);//Vertical
    }
    if(!t){std::cerr<<"erro in point Finding, can't find target ("<<x<<","<<y<<")\n";exit(1);}
    Log->addLog(t);
    return t;
}

// left - bottom (x1,y1)   right top (x2,y2)
// check if this area has any block by checkin maximal horizontal stripes.
// If has no blocks , return the space tiles in this region    
// otherwise return empty.     
std::vector<tile*> AreaSearch(int x1,int y1,int x2,int y2){
    std::vector<tile*>Tiles;
    
    bool block = false;
    while(!block && y2 > y1){  // can't equal
        tile*t1 = nullptr,*t2 = nullptr;
        
        t1 = pointFinding(x1,y2);
        if(x2 != Log->getw())
            t2 = pointFinding(x2,y2);
        if(t2 && t1 != t2){   // if t1 and t2 are not the same  and t2->x() is smaller than x2 , it must has block tiles in this region!(t2 or t1)      
            if(t2->x() < x2)
                block = true;
        }
        if(!t1->isSpace())
            block = true;
        Tiles.push_back(t1);
        y2 = t1->y();//move down if no block.
    }
    if(block)return {};
    return Tiles;  //if Tiles = empty : this area has block tile.
}




void setTr_if(tile* left,tile* right){
    if(gety2(right) >= gety2(left) && gety2(left) > gety(right))
        left->settr(right);
}

void setRt_if(tile* bottom,tile* top){
    if(getx2(top) >= getx2(bottom) && getx2(bottom) > getx(top))
        bottom->setrt(top);
}

void setBl_if(tile* left,tile*right){
    if(gety2(left) > gety(right) && gety(right) >= gety(left))
        right->setbl(left);
}
void setLb_if(tile* bottom,tile* top){
    if(getx2(bottom) > getx(top) && getx(top) >= getx(bottom))
        top->setlb(bottom);
}

using set_if = decltype(setRt_if);


void OneSideUpdate(set_if* s1,set_if*s2,tile* t,const std::vector<tile*>&neighbors,bool tIsLB){
    for(auto nb:neighbors){
        if(tIsLB){
            s1(t,nb);        
            s2(t,nb);
        }else{
            s1(nb,t);        
            s2(nb,t);
        }
    }
}
void updateRight(tile*t,const std::vector<tile*>&neighbors){
    OneSideUpdate(setTr_if,setBl_if,t,neighbors,true);
}

void updateTop(tile*t,const std::vector<tile*>&neighbors){
    OneSideUpdate(setRt_if,setLb_if,t,neighbors,true);
}

void updateLeft(tile*t,const std::vector<tile*>&neighbors){
    OneSideUpdate(setTr_if,setBl_if,t,neighbors,false);
}

void updateBottom(tile*t,const std::vector<tile*>&neighbors){
    OneSideUpdate(setRt_if,setLb_if,t,neighbors,false);
}



tile* Hsplit(tile*t,int y,bool bottom){
    tile *top = new tile(t->id(),t->x(),y, t->w(), t->y() + t->h() - y,nullptr,nullptr,t->tr(),t->rt());
    tile *bot = new tile(t->id(),t->x(),t->y(),t->w(), y - t->y(),t->bl(),t->lb());
    bot->setrt(top);
    top->setlb(bot);
    auto LFT = getLeft(t);
    auto RGT = getRight(t);
    auto BOT= getBottom(t);
    auto TOP = getTop(t);
    updateRight(top,RGT);
    updateRight(bot,RGT);
    updateLeft(top,LFT);
    updateLeft(bot,LFT);
    updateTop(top,TOP);
    updateBottom(bot,BOT);
    Log->addLog(bot);
    return bottom ? bot:top;
}

tile* Vsplit(tile*t,int x,bool left){
    tile *rgt = new tile(t->id(),  x    ,t->y(), t->w() + t->x() - x, t->h(), nullptr,nullptr,t->tr(),t->rt());
    tile *lft  = new tile(t->id(),t->x() ,t->y(),   x - t->x()       , t->h(), t->bl(),t->lb());
    rgt->setbl(lft);
    lft->settr(rgt);
    auto LFT = getLeft(t);
    auto RGT = getRight(t);
    auto BOT= getBottom(t);
    auto TOP = getTop(t);
    updateTop(rgt,TOP);
    updateTop(lft,TOP);
    updateBottom(rgt,BOT);
    updateBottom(lft,BOT);
    updateRight(rgt,RGT);
    updateLeft(lft,LFT);
    Log->addLog(lft);
    return left? lft:rgt;
}











// cut "t" to the shape w * h
tile* tailor(tile*t ,int x,int y,int w,int h)
{
    std::set<tile*>rec;
    if(gety2(t) > y + h){
        rec.insert(t);
        t = Hsplit(t,y+h);
    }
    if(gety(t) < y){
        rec.insert(t);
        t = Hsplit(t,y,false);
    }
    if(getx(t) < x){
        rec.insert(t);
        t = Vsplit(t,x,false);
    }
    if(getx2(t) > x + w){
        rec.insert(t);
        t = Vsplit(t,x+w);
    }
    for(auto r:rec)delete r;
    return t;
}


bool checkspan(const std::vector<tile*>&tiles){
    for(int i = 1;i < tiles.size();i++)
        if(tiles.at(i-1)->x()!=tiles.at(i)->x()||tiles.at(i-1)->w()!=tiles.at(i)->w())
            return false;
    return true;
}

bool continuous(const std::vector<tile*>&tiles){
    if(!checkspan(tiles))return false;
    for(int i = 1;i < tiles.size();i++)
        if(gety(tiles.at(i-1)) != gety2(tiles.at(i)))
            return false;
    return true;
}

// merge continuous tiles from high to low sorted. 
tile* merge(const std::vector<tile*>&tiles)
{
    if(continuous(tiles)&&tiles.size()>1){
        tile*upper = tiles.at(0);
        tile*lower = tiles.at(tiles.size()-1);
        
        // ptr setting for updating
        upper ->seth(gety2(upper) - lower->y());
        upper ->sety(lower->y());
        
        upper ->setlb(lower->lb());
        upper ->setbl(lower->bl());

        updateRight(upper,getRight(upper));
        updateLeft(upper,getLeft(upper));
        updateTop(upper,getTop(upper));
        updateBottom(upper,getBottom(upper));

    }else{
        return tiles.at(0);
    }
    for(int i = 1;i<tiles.size();i++)delete tiles.at(i);
    return tiles.at(0);
}



void doMerge(const std::vector<tile*>&tiles,t_move* next,t_move* precheck,t_get* v)
{
    if(tiles.empty()){return ;}
    tile* t = tiles.at(0);
    if(v(precheck(t)) == v(t)){
        t = precheck(t);
    }
    bool needSort = false;
    auto decendingY = [](tile*t1,tile*t2){return t1->y() > t2->y();};
    if(next==rt){
        needSort = true;
    }
    else if(next!=lb){
        std::cerr<<"error arg pass in do Merge, next only accept rt/lb !\n";
        return ;
    }
    std::vector<tile*> mergeTiles{t};
    while( (t = next(t)) && ( v(t) == v(tiles.at(0)) ) )
    {
        if(t->w() == mergeTiles.at(0)->w())
            mergeTiles.push_back(t);
        else{
            if(needSort)std::sort(mergeTiles.begin(),mergeTiles.end(),decendingY);
            tile* m = merge(mergeTiles);
            mergeTiles.clear();
            mergeTiles.push_back(t);
        }
    }
    if(needSort)std::sort(mergeTiles.begin(),mergeTiles.end(),decendingY);
    t = merge(mergeTiles);
}


tile* InsertBlock(int id,int x,int y,int w,int h){
    auto SpaceTiles = AreaSearch(x,y,x+w,y+h);
    if(SpaceTiles.empty()){return nullptr;} // blocks exist.
    std::vector<tile*>blocks;
    for(int i = 0;i < SpaceTiles.size() ; i++)
        blocks.push_back(tailor(SpaceTiles.at(i),x,y,w,h));
    tile* blockTile = merge(blocks); 
    blockTile->setid(id);
    MergLeft(getLeft(blockTile));
    MergeRight(getRight(blockTile));
    Log->addLog(blockTile);
    return blockTile;
}



std::vector<tile*> NeighborTraversal(tile*t,tile*nb,t_get*get,int v,cmp *comp,t_move*next){   //return last neighbor sufficeint to value.
    std::vector<tile*>nbs;
    nbs.push_back(nb);
    nb = next(nb);
    while(nb && comp(get(nb),v)){
        nbs.push_back(nb);
        nb = next(nb);
    }
    return nbs;
}
std::vector<tile*>getNeighbor(tile*t){
    auto left = getLeft(t);     // from down to top
    auto bottom = getBottom(t); // from left to right
    auto right = getRight(t);   // from top to down
    auto top = getTop(t);       // from right to left
    std::vector<tile*>nbs;
    nbs.resize(left.size() + bottom.size() + right.size() + top.size());
    auto it = std::copy(left.begin(),left.end(),nbs.begin());
    it = std::copy(bottom.begin(),bottom.end(),it);
    it = std::copy(right.begin(),right.end(),it);
    std::copy(top.begin(),top.end(),it);
    return nbs;
}



int main()
{
    Log = new userlog(500,500);
    

    tile* b1 = InsertBlock(1,50,40,250,60);
    tile* b2 = InsertBlock(2,55,250,50,150);
    tile* b3 = InsertBlock(3,250,355,170,45);
    tile* b4 = InsertBlock(4,250,310,170,45);
    tile* b5 = InsertBlock(5,320,265,75,45);

    std::cout<<"blocks\n";
    std::cout<<*b1<<"\n";
    std::cout<<*b2<<"\n";
    std::cout<<*b3<<"\n";
    std::cout<<*b4<<"\n";
    std::cout<<*b5<<"\n";

    auto nbs1 = getNeighbor(b1);
    auto nbs2 = getNeighbor(b2);
    auto nbs3 = getNeighbor(b3);
    auto nbs4 = getNeighbor(b4);
    auto nbs5 = getNeighbor(b5);
   
    std::cout<<"nb1:\n";
    std::for_each(nbs1.begin(),nbs1.end(),[](tile* t){std::cout<<*t<<"\n";});
    std::cout<<"nb2:\n";
    std::for_each(nbs2.begin(),nbs2.end(),[](tile* t){std::cout<<*t<<"\n";});
    std::cout<<"nb3:\n";
    std::for_each(nbs3.begin(),nbs3.end(),[](tile* t){std::cout<<*t<<"\n";});
    std::cout<<"nb4:\n";
    std::for_each(nbs4.begin(),nbs4.end(),[](tile* t){std::cout<<*t<<"\n";});
    std::cout<<"nb5:\n";
    std::for_each(nbs5.begin(),nbs5.end(),[](tile* t){std::cout<<*t<<"\n";});
}

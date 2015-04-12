#include <cstdio>
#include <set>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <cassert>
using namespace std;

struct Tree {
    string attribute;
    vector<map<string,string> > data;
    vector<Tree> child;

    Tree(){}
    Tree(string _attribute,vector<map<string,string> > _data) {
        attribute = _attribute;
        data = _data;
        child.clear();
    }
};
double I(int le,int ri) {
    int total = (le + ri);
    double ple = ((double)le/total);
    double pri = ((double)ri/total);
    return ((ple!=0)?(-ple*log2(ple)):(0))+((pri!=0)?(-pri*log2(pri)):(0));
}
map<string,double> calculateIG(vector<map<string,string> >& data,vector<string>& nameOfAttribute) {
    map<string,double> ret;

    pair<int,int> cc; // ID를 위한 class의 yes no 카운터
    string lastClassName = nameOfAttribute[(int)nameOfAttribute.size()-1];
    for ( int i = 0 ; i < (int)data.size() ; i++ ) {
        cc.first += data[i][lastClassName] == "yes";
        cc.second += data[i][lastClassName] == "no";
    }
    double curInformation = I(cc.first,cc.second);
    for ( int i = 0 ; i < (int)nameOfAttribute.size() ; i++ ) {
        if ( nameOfAttribute[i] == lastClassName ) continue;
        map<string,int> mp;
        map<string,pair<int,int> > classCounter;
        for ( int j = 0 ; j < (int)data.size() ; j++ ) {
            mp[data[j][nameOfAttribute[i]]]++;
            classCounter[data[j][nameOfAttribute[i]]].first +=
                data[j][lastClassName] == "yes";
            classCounter[data[j][nameOfAttribute[i]]].second +=
                data[j][lastClassName] == "no";
        }
        double ans = 0;
        for ( map<string,int>::iterator it=mp.begin();it!=mp.end();it++ ) {
            pair<int,int> yes_no_count = classCounter[it->first];
            double t = (it->second/(double)data.size())*
                I(yes_no_count.first,yes_no_count.second);
            ans += t;
        }
        ret[nameOfAttribute[i]] = curInformation - ans;
    }
    return ret;
}
pair<string,double> getMaxIG(map<string,double> now) {
    pair<string,double> ret("",0.0);
    for ( map<string,double>::iterator it=now.begin();it!=now.end();it++ ) 
        if ( it->second > ret.second ) 
            ret = pair<string,double>(it->first,it->second);
    return ret;
}
// vector<map<string,string> > data;
void createTree(Tree& root,vector<string>& nameOfAttribute) {
    map<string,int> childCounter;
    for ( int j = 0 ; j < (int)root.data.size() ; j++ ) 
        childCounter[root.data[j][root.attribute]]++;
    if ( (int)childCounter.size() == 1 ) return;
    for ( map<string,int>::iterator it=childCounter.begin();it!=childCounter.end();it++ ) {
        vector<map<string,string> > nextData;
        for ( int i = 0 ; i < (int)root.data.size() ; i++ ) 
            if ( root.data[i][root.attribute] == it->first ) 
                nextData.push_back(root.data[i]);
        map<string,double> curIG = calculateIG(nextData,nameOfAttribute);
        string nextAttribute = getMaxIG(curIG).first;
        if ( (int)nextAttribute.length() == 0 )
            nextAttribute = it->first;
        Tree next(nextAttribute,nextData);
        root.child.push_back(next);
    }
    printf("%s %d\n",root.attribute.c_str(),(int)root.child.size());
    if ( (int)root.child.size() > 1 ) {
        for ( int i = 0 ; i < (int)root.child.size() ; i++ ) 
            createTree(root.child[i],nameOfAttribute);
    }
}
void printTreeUsingPreorder(Tree &root,vector<string>& nameOfAttribute) {
    printf("%s\n",root.attribute.c_str());
    for ( int i = 0 ; i < (int)root.data.size() ; i++ ) {
        printf("id[%s] : ",root.data[i]["id"].c_str());
        for ( int j = 0 ; j < (int)nameOfAttribute.size() ; j++ ) 
            printf("%s ",root.data[i][nameOfAttribute[j]].c_str());
        puts("");
    }
    if ( root.child.empty() ) {
        string lastClassName = nameOfAttribute[(int)nameOfAttribute.size()-1];
        printf("decision : %s\n",root.data[0][lastClassName].c_str());
    }
    for ( int i = 0 ; i < (int)root.child.size() ; i++ ) 
        printTreeUsingPreorder(root.child[i],nameOfAttribute);
}
string intToString(int n) {
    char s[1024];
    sprintf(s,"%d",n);
    return string(s);
}

int main(int argc,char *argv[]) {
    assert(argc==3);
    string traningDataFileName;
    string testingDataFileName;

    traningDataFileName = string(argv[1]);
    testingDataFileName = string(argv[2]);

    vector<string> nameOfAttribute;
    map<string,set<string> > valueOfAttribute;

    FILE *fp = fopen(traningDataFileName.c_str(),"r");
    char ts[2014];
    fscanf(fp,"%[^\n]\n",ts);
    for ( char* p=strtok(ts,"\t");p;p=strtok(NULL,"\t") ) {
        nameOfAttribute.push_back(string(p));
    }

    vector<map<string,string> > data;
    int id = 0;
    while ( fgets(ts,1024,fp) != NULL ) {
        while ( ts[strlen(ts)-1] == '\n' || ts[strlen(ts)-1] == '\r' ) 
            ts[strlen(ts)-1] = 0;
        vector<string> dataOfAttribute;
        for ( char *p=strtok(ts,"\t");p;p=strtok(NULL,"\t") ) 
            dataOfAttribute.push_back(string(p));
        assert(dataOfAttribute.size()==nameOfAttribute.size());
        map<string,string> tmap;
        tmap["id"] = intToString(id);
        for ( int i = 0 ; i < (int)dataOfAttribute.size() ; i++ ) {
            tmap[nameOfAttribute[i]] = dataOfAttribute[i];
            valueOfAttribute[nameOfAttribute[i]].insert(dataOfAttribute[i]);
        }
        data.push_back(tmap);
        id++;
    }
    map<string,double> now = calculateIG(data,nameOfAttribute);
    pair<string,double> maxIG = getMaxIG(now);

    Tree root(maxIG.first,data);
    createTree(root,nameOfAttribute);
    printTreeUsingPreorder(root,nameOfAttribute);

    return 0;
}

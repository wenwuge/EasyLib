#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <set>

using namespace std;
template<typename T> void output(const T& t)
{
    cout << t << " ";
}
class Solution {
    public:
        vector<int> findMinHeightTrees(int n, vector<pair<int, int> >& edges) 
        {
               vector<set<int> > graph(n, set<int>());
               vector<pair<int, int> >::iterator  begin(edges.begin()), end(edges.end());

               if(n==1)
                   return vector(1,0);
               for(;begin != end; begin ++){
                     graph[begin->first].insert(begin->second);
                     graph[begin->second].insert(begin->first);
               }

               vector<int> leaves;
               
               for(int i = 0; i < n; i++){
                    if(graph[i].size() == 1){
                        leaves.push_back(i);
                    }
               }
               
               while(n > 2){
                   vector<int> new_leaves;
                   for(int i = 0; i < leaves.size(); i++){
                        set<int>::iterator  set_begin(graph[leaves[i]].begin());     
                        set<int>::iterator  set_end(graph[leaves[i]].end());     

                        for(;set_begin != set_end; set_begin++){
                            graph[*set_begin].erase(leaves[i]);
                            if(graph[*set_begin].size() == 1){
                                new_leaves.push_back(*set_begin);
                            }
                        }
                   }
                   //for_each(new_leaves.begin(), new_leaves.end(), output<int>);
                   //cout << endl;
                   n -= leaves.size();
                   leaves.swap(new_leaves);
               }

               return leaves;
        }
};


int main(int argc, char** argv)
{
    vector<pair<int,int> > vec;
    //vec.push_back(make_pair(0,3));
    //vec.push_back(make_pair(1,3));
    //vec.push_back(make_pair(2,3));
    //vec.push_back(make_pair(4,3));
    //vec.push_back(make_pair(5,4));
    
    vec.push_back(make_pair(0,1));
    vec.push_back(make_pair(0,2));
    vec.push_back(make_pair(0,3));
    vec.push_back(make_pair(3,4));
    vec.push_back(make_pair(4,5));
   
    
    //vec.push_back(make_pair(0,1));
    Solution s;
    vector<int> result;
    result = s.findMinHeightTrees(6, vec);

    for_each(result.begin(), result.end(), output<int>);
    return 0;
}

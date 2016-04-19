#include<iostream>
#include<vector>
struct Interval {  
    int start;  
    int end;  
    Interval() : start(0), end(0) {}  
    Interval(int s, int e) : start(s), end(e) {}  
};  

bool compare(const Interval& val1, const Interval& val2)
{
    return (val1.start < val2.start);
}

vector<Interval> merge(vector<Interval> &intervals) 
{
    sort(intervals.begin() ,intervals.end(), compare);
    vector<Interval>::iterator curr(intervals.begin());

    while(curr != intervals.end() && (curr + 1) != intervals.end()){
        if(curr->end >= (curr + 1)->start){
            curr->end = (curr + 1)->end;
            intervals.erase(curr + 1);
        }else
            curr++;
    }
    

    return intervals;

}


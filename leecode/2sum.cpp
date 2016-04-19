#include <iostream>
#include <vector>
#include <map>
using namespace std;
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) 
    {
        map<int,int> hmap;
        vector<int>::iterator begin(nums.begin()), end(nums.end());
        vector<int> result;

        for(int i = 0; i < nums.size(); i++){
                hmap[nums[i]] = i;
        }

        for(int i = 0; i < nums.size(); i++){
            map<int, int>::iterator it;
            if((it = hmap.find(target - nums[i])) != hmap.end()){
                if(it->second == i)
                    continue;
                result.push_back(i);
                result.push_back(it->second);
                return result;
            }
        }

        return result;
    }

};

int main(int argc ,char** argv)
{
    Solution s;
    vector<int> test;
    test.push_back(3);
    test.push_back(2);
    test.push_back(4);
    test = s.twoSum(test, 6);
    return 0;
}

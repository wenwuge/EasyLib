#include <iostream>
#include <vector>
using namespace std;

class Solution {
    public:
            void helper(vector<vector<int> >&= vecto(<int>& vec, int lower, int total, int cnt)
            {
                if(cnt == 0&& total == 0){
                    solutions.push_back(vec);
                    return;
                }
                
                if(cnt == 0 || total <= 0)
                {
                    return;
                }

                int low = total /cnt;
                if(low > 9){
                    low = 9;
                }

                for(int i = lower ; i <= low; i++){
                    vec.push_back(i);
                    helper(solutions, vec, i + 1, total - i, cnt - 1);
                    vec.pop_back();
                }

                

            }

            vector<vector<int> > combinationSum3(int k, int n) {
                if(k <= 0 || k > n)
                    return vector<vector<int> >();
                vector<vector<int> > solutions;
                vector<int> vec;
                helper(solutions, vec, 1, n, k); 

                return solutions;
            }
};

int main(int argc, char** argv)
{
    Solution s;
    vector<vector<int> > solutions;
    solutions = s.combinationSum3(3, 7);
    cout << "solutions size: "<< solutions.size() <<endl;
    return 0;

}

#include <iostream>
#include <boost/ptr_container/ptr_container.hpp>
#include <algorithm>

using namespace std;
using namespace boost;
class base{
public:
    ~base(){
        cout << "base destruct!" << endl;
    }

    base(){
        cout << "base construct called" << endl;
    }

    base(const base& copy){
        cout << "base copy construct called" << endl;
    }

    void print(){
        cout << "hello" << endl;
    }
};
int main(int argc, char** argv)
{
    {
        ptr_vector<base> base_ptr_vec;

        base_ptr_vec.push_back(new base);
        base_ptr_vec.push_back(new base);
        base_ptr_vec.push_back(new base);
        
        ptr_vector<base>::iterator it =base_ptr_vec.begin();
        base_ptr_vec.pop_back();//will free the pop item;

        for_each(base_ptr_vec.begin(), base_ptr_vec.end(), mem_fun_ref(&base::print));
        {
            //only delete the item in vector, don't free, the free time is item no effect
            ptr_vector<base>::auto_type item = base_ptr_vec.release(it);
            item->print();
        }
        ptr_vector<base> second;
        second = base_ptr_vec;

    }
}

#include <iostream>
#include <boost/ptr_container/ptr_container.hpp>

using namespace std;
using namespace boost;
class base{
public:
    ~base(){
        cout << "base destruct!" << endl;
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
        ptr_vector<base>::auto_type item = base_ptr_vec.release(it);
        item->print();
        cout << "release one item" << endl;
    }
}

#include <iostream>
#include <map>
using namespace std;

class Command{
public:
    virtual void Excute() = 0;
};

class Command1:public Command{
public:
    virtual void Excute()
    {
        command1();
    }
private:
    void command1()
    {
        cout << "command1" << endl;
    }
};

class Command2:public Command{
public:
    virtual void Excute()
    {
        command2();
    }
private:
    void command2()
    {
        cout << "command2" << endl;
    }
};

class CommandMenu{
public:
    void RegisterCommand(string name, Command* command)
    {
        menu_[name] = command;
    }

    void UnRegisterCommand(string name)
    {
        map<string, Command*>::iterator it;
        it = menu_.find(name);
        if(it != menu_.end()){
            menu_.erase(it);
        }
    }
    
    void ExecuteCommand(string command)
    {
        map<string, Command*>::iterator it;
        it = menu_.find(command);
        if(it != menu_.end()){
            it->second->Excute();
        }else{
            cout << "command " << command << " can't be found" << endl;
        }
    }
private:
    map<string, Command*> menu_;
};

int main(int argc, char** argv)
{
    Command1 cmd1;
    Command2 cmd2;

    CommandMenu menu;
    menu.RegisterCommand("command1", &cmd1);
    menu.RegisterCommand("command2", &cmd2);

    menu.ExecuteCommand("command2");

}

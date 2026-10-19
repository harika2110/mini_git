#include<iostream>
#include<string> 
#include<filesystem> 
#include<fstream> 
using namespace std;
namespace fs =  std::filesystem ;
void printUsage()
{
    cout << "Usage: mygit <command> [arguments]" << endl 
         << "Commands: " << endl
         << " init                 Initialize repository" << endl
         << " hash-object <file>   Hash and store a file " << endl
         << " add <file>           Stage a file" << endl 
         << " commit -m <message>  Create a commit" << endl
         << " log                  Show commit history" << endl 
         << " status               Show repository status" << endl 
         << " diff                 Show file differences" << endl
         << " branch [name]        List or create branches" << endl 
         << " checkout <branch>    Switch branches" << endl
         << " merge <branch>       Merge a branch" << endl ;     
}

void initRepository()
{
    try
    {
        if( fs::exists(".mygit"))
        {
            std::cout <<"Repository already initialized" << endl;
            return;
        }
        
        // Creating Directories 
        fs::create_directories(".mygit/objects");
        fs::create_directories(".mygit/refs/heads");
     
        // Creating the Head file 
        ofstream head(".mygit/HEAD");
        head << "ref: refs/heads/main" << endl ;
        head.close() ; 

        //Create the index file 
        ofstream index(".mygit/index");
        index.close();

        cout << "Initialized empty MyGit repository." << endl;
    }
    catch( const fs::filesystem_error& e ) 
    {
        cerr << " Error initialising repository: "
             << e.what() << endl;
    }
}
int main( int argc , char* argv[])
{
    if( argc < 2 ) 
    {
        printUsage() ; 
        return 1 ; 
    }
    string command = argv[1];
    if( command == "init")
    {
        cout << "Initialising repository..." << endl ;
        initRepository();
    }
    else if ( command == "hash-object")
    {
        cout << "Hashing object..." << endl ;
    }
    else if( command == "add")
    {
        cout << "Staging file.." << endl;
    }
    else if( command == "commit")
    {
        cout << "Creating commit..." << endl;
    }
    else if( command == "log")
    {
        cout << "Displaying history..." << endl;
    }
    else if( command == "status")
    {
        cout << "Checking repository status..." << endl;
    }
    else if( command == "diff")
    {
        cout << "Calculating differences..." << endl;
    }
    else if( command == "branch")
    {
        cout << "Managing branches..." << endl ; 
    }
    else if( command == "checkout")
    {
        cout << "Switching branches..." << endl;
    }
    else if( command == "merge")
    {
        cout << "Merging branches..." << endl ;
    }
    else 
    {
        cerr << "Unknown command: " << command << endl;
        printUsage();
        return 1;
    }
    return 0;
}
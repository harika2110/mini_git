#include<iostream>
#include<string> 
#include<filesystem> 
#include<openssl/sha.h> 
#include<cstdio> 
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
string calculateHash( string contents )
{
    unsigned char hash[SHA_DIGEST_LENGTH] ; 
    SHA1((unsigned char*)contents.c_str() , contents.size() , hash );
    string result = "" ; 
    for( int i=0; i < SHA_DIGEST_LENGTH ; i++ )
    {
        char hex[3] ; 
        sprintf( hex , "%02x" , hash[i]);
        result += hex ; 
    }
    return result ; 
}
void hashObject( string filename ) 
{
    ifstream file( filename ) ; 
    if( !file ) 
    {
        cout << "File not found" << endl; 
        return ; 
    }

    string contents = "";
    string line ; 

    while( getline(file , line))
    {
        contents +=  line + "\n";
    }
    string objectData = "blob " + to_string(contents.size()) + '\0' + contents ;
    string hash = calculateHash(objectData); 
    
    string folder = ".mygit/objects/" + hash.substr(0,2) ; 
    filename = folder + "/" + hash.substr(2) ; 

    fs::create_directories( folder ) ; 
    ofstream objectFile(filename , ios::binary );

    objectFile.write(objectData.c_str() , objectData.size());
    objectFile.close() ; 

    cout << hash << endl;
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
        if( argc < 3 ) 
        {
            cout << "Usage: mygit hash-object <file>\n" ; 
            return 1;
        }

        hashObject(argv[2]);
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
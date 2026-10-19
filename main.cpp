#include<iostream>
#include<string> 
#include<filesystem>
#include<iterator> 
#include<openssl/sha.h> 
#include<cstdio> 
#include<fstream> 
#include<vector> 
#include<sstream> 
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
string hashObject( string filename ) 
{
    ifstream file( filename ) ; 
    if( !file ) 
    {
        cout << "File not found" << endl; 
        return ""; 
    }

    // string contents = "";
    // string line ; 

    // while( getline(file , line))
    // {
    //     contents +=  line + "\n";
    // }
    string contents((istreambuf_iterator<char>(file)) , istreambuf_iterator<char>()); 
    string objectData = "blob " + to_string(contents.size()) + '\0' + contents ;
    string hash = calculateHash(objectData); 
    
    string folder = ".mygit/objects/" + hash.substr(0,2) ; 
    filename = folder + "/" + hash.substr(2) ; 

    fs::create_directories( folder ) ; 
    ofstream objectFile(filename , ios::binary );

    objectFile.write(objectData.c_str() , objectData.size());
    objectFile.close() ; 

    return hash;
}

void addFile( string filename )
{
    string hash = hashObject(filename ) ; 
    if( hash == "")
    {
        return ; 
    }

    ifstream indexFile(".mygit/index");

    vector<string> entries ; 
    string line ; 
    bool found = false;

    while( getline(indexFile , line ))
    {
        istringstream input(line) ; 
        string oldFilename, oldHash ; 
        input >> oldFilename >> oldHash ; 
        if( oldFilename == filename ) 
        {
            entries.push_back(filename + " " + hash ); 
            found = true ;
        }
        else 
        {
            entries.push_back(line);
        }
    }
    indexFile.close();
    if( !found ) 
    {
        entries.push_back(filename + " " + hash ) ; 
    }

    ofstream output(".mygit/index");
    for( string entry : entries )
    {
        output << entry << endl;
    }
    output.close() ; 
    cout << "Added " << filename << endl;
}

void status()
{
    ifstream indexFile(".mygit/index");
    
    if( !indexFile)
    {
        cout << "Run init first" << endl;
        return ; 
    }

    string line ; 
    
    while( getline( indexFile, line ))
    {
        istringstream input(line ) ; 
        string filename , oldHash ; 
        input >> filename >> oldHash ; 

        if( !fs::exists(filename ))
        {
            cout << "Deleted: " << filename << endl ; 
            continue ; 
        }

        ifstream file( filename , ios::binary ); 
        
        string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        string objectData = "blob " + to_string( contents.size()) + '\0' + contents ; 
        string currentHash = calculateHash( objectData);

        if( currentHash != oldHash )
        {
            cout << "Modified: " << filename << endl;
        }
    }

    indexFile.close();
}

void makeCommit(string message)
{
    ifstream indexFile(".mygit/index") ; 
    if( !indexFile ) 
    {
        cout << "Run init first" << endl ; 
        return ; 
    }

    string line ; 
    string commitContents = "";
    bool empty = true ; 

    while( getline(indexFile , line ))
    {
        if( line.empty())
        {
            continue ;
        }

        commitContents += line + "\n" ; 
        empty = false;
    }
    indexFile.close();
    if( empty ) 
    {
        cout << "Nothing to commit" << endl ;
        return ; 
    }

    ifstream headFile(".mygit/HEAD");

    string head ;
    getline( headFile , head ) ; 
    headFile.close();

    if( head.substr(0,5) != "ref: ")
    {
        cout << "Invalid HEAD" << endl;
        return ; 
    }
    
    string branchPath = ".mygit/" + head.substr(5) ; 
    string parentHash = "" ;
    ifstream branchFile(branchPath);
    if( branchFile)
    {
        getline( branchFile, parentHash ); 
        branchFile.close();
    }

    string finalContents = "message " + message + "\n" ; 

    if( parentHash != "")
    {
        finalContents += "parent " + parentHash + "\n" ;
    }

    finalContents += commitContents ; 

    string objectData = "commit " + to_string(finalContents.size()) + '\0' + finalContents ;
    string hash = calculateHash( objectData) ; 

    string folder = ".mygit/objects/" + hash.substr(0,2); 
    string filename = folder + "/" + hash.substr(2) ; 

    fs::create_directories(folder) ; 

    ofstream objectFile(filename, ios::binary );

    objectFile.write( objectData.c_str(),objectData.size());
    objectFile.close();

    ofstream branchOutput(branchPath);
    branchOutput << hash << endl ; 
    branchOutput.close();

    cout << "[" << head.substr(5) << " " << hash.substr(0,7) 
         << "] " << message << endl;

}

void showLog()
{
    ifstream headFile(".mygit/HEAD") ; 

    if( !headFile)
    {
        cout << "Run init first" << endl;
        return ; 
    }

    string head ; 
    getline( headFile , head) ; 
    headFile.close();

    if( head.substr(0,5) != "ref: ")
    {
        cout << "Invalid HEAD"<< endl; 
        return ; 
    }

    string branchPath = ".mygit/" + head.substr(5);
    ifstream branchFile( branchPath);
    
    if( !branchFile)
    {
        cout << "No commits yet" << endl; 
        return ; 
    }

    string hash ; 
    getline ( branchFile , hash ) ;
    branchFile.close();

    while( hash != "")
    {
        string folder = ".mygit/objects/" + hash.substr(0,2);
        string filename = folder + "/" + hash.substr(2) ;
        
        ifstream objectFile( filename , ios::binary);

        if( !objectFile )
        {
            cout << "Commit object not found" << endl;
            return ;
        }

        string objectData((istreambuf_iterator<char>(objectFile)) , istreambuf_iterator<char>());
        objectFile.close();

        size_t separator = objectData.find('\0');

        if( separator == string::npos )
        {
            cout << "Invalid commit object" << endl;
            return ;   
        }

        string contents = objectData.substr(separator + 1 );

        istringstream input(contents);
        string line , message , parentHash = "";
        while( getline(input , line))
        {
            if( line.substr(0,8) == "message ")
            {
                message = line.substr(8) ;
            }
            else if( line.substr(0,7) == "parent")
            {
                parentHash = line.substr(7);
            }
        }

        cout << "Commit: " << hash << endl;
        cout << "Message: " << message << endl;
        cout << "-------------------------" << endl;

        hash = parentHash ;
    }
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

        cout << hashObject(argv[2]) << endl;
    }
    else if( command == "add")
    {
        cout << "Staging file.." << endl;
        if( argc < 3 ) 
        {
            cout << "Usage: mygit add <file>" << endl;
            return 1 ;
        }
        addFile( argv[2]);
    }
    else if( command == "commit")
    {
        cout << "Creating commit..." << endl;
        if( argc < 3 ) 
        {
            cout << "Usage: mygit commit \"message\"" << endl;
            return 1 ; 
        }
        makeCommit( argv[2]);
    }
    else if( command == "log")
    {
        cout << "Displaying history..." << endl;
        showLog();
    }
    else if( command == "status")
    {
        cout << "Checking repository status..." << endl;
        status();
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
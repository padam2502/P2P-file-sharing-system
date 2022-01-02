#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <openssl/sha.h>
// Function to send data to
// server socket.
using namespace std;
// Driver Code
string client_ip, client_port;
vector<string >downinfo;
vector<string >cominfo;
int newSocket;

long long KB(int con)
{
    long long retval=con;
    return retval;
}
long long filesize(string a)
{
    int fd = open(a.c_str(), O_RDONLY);
    if (fd == -1)
    {
        perror("Error openning file ");
        cout<<"\nPATH "<<a;
        return -1;
    }
    struct stat fileStats;
    if (fstat(fd, &fileStats) == -1)
    {
        perror("Error getting file stats ");
        cout<<"\nPATH "<<a;
        return -1;
    }
    long long filecap = fileStats.st_size;
    return filecap;
}
string comphash(string s)
{
        unsigned char temp[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH * 2 ];
    vector<string >sha1;
    int fd = open(s.c_str(),O_RDONLY);
    if (fd == -1)
    {
        perror("Error openning file ");
    }
    long long blocknumber, remsize = 0;
            long long blocksize = KB(512);
            long long file_size=filesize(s);
            if (file_size % blocksize == 0)
            {
                blocknumber = file_size / blocksize;
            }
            else
            {
                remsize = file_size % blocksize;
                blocknumber = file_size / blocksize;
                ++blocknumber;
            }
            long long k = 0;
            //int fd = open("/home/padam/Music/shiv.txt", O_RDONLY);
            while (k < blocknumber)
            {
                if (k != blocknumber - 1)
                {
                    // char chunks[SHA_DIGEST_LENGTH * 2];
                    // memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    // unsigned char arr[SHA_DIGEST_LENGTH];
                    // memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    // void *buf = malloc(blocksize);
                    // read(fd, buf, blocksize);
                    // SHA1((unsigned char *)buf, blocksize, arr);

                    // for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    // {
                    //     sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    // }
                    // string resultsha = (chunks);
                    // //sha1.push_back(resultsha.substr(0,20));
                    // free(buf);
                }
                else
                {

                    char chunks[SHA_DIGEST_LENGTH * 2];
                    memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    unsigned char arr[SHA_DIGEST_LENGTH];
                    memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    void *buf = malloc(remsize);
                    read(fd, buf, remsize);
                    SHA1((unsigned char *)buf, remsize, arr);

                    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    {
                        sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    }
                    string resultsha = chunks;
                    sha1.push_back(resultsha.substr(0,20));
                    free(buf);
                }
                ++k;
            }
    // while((actually_read = read(fd,data,SIZE)) > 0) {
    //     SHA1_Update(&ctx, data, actually_read);
    //     for(int i=0;i<SIZE;i++)
    //     {
    //         data[i]='\0';
    //     }
    string sharet="";
    for(int i=0;i<sha1.size();i++)
    sharet=sharet+sha1[i];
    return sharet;
}


unordered_map<string, string> filen;
void sendtotracker(int sock, string s)
{
    send(sock, &s[0], strlen(&s[0]), 0);
}
void rcevfromtracker(int sock)
{
    char buffer[100005];
    int bytes = recv(sock, buffer, 100005, 0);
    if (bytes == 0)
    {
        cout << sock << " shutted down!\n";
    }
    string msg = (buffer);
    string tp = msg.substr(0, bytes);
    cout << tp << endl;
    for (int i = 0; i < 100005; i++)
    {
        buffer[i] = '\0';
    }
}
string rcevdownloadinfo(int sock)
{
    char buffer[100005];
    for (int i = 0; i < 100005; i++)
    {
        buffer[i] = '\0';
    }
    int bytes = recv(sock, buffer, 100005, 0);
    if (bytes == 0)
    {
        cout << sock << " shutted down!\n";
    }
    string msg = (buffer);
    string tp = msg.substr(0, bytes);
    // cout << tp << endl;
    for (int i = 0; i < 100005; i++)
    {
        buffer[i] = '\0';
    }
    return tp;
}
void process_cmnds(int sock, vector<string> commnds, string a, string b)
{
    if (commnds[0] == "create_user")
    {
        if(commnds.size()!=3)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1] + " " + commnds[2];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "login")
    {
        if(commnds.size()!=3)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1] + " " + commnds[2] + " " + a + " " + b;
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "create_group")
    {
        if(commnds.size()!=2)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "join_group")
    {
        if(commnds.size()!=2)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "leave_group")
    {
        if(commnds.size()!=2)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "list_requests")
    {
        if(commnds.size()!=2)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "accept_request")
    {
        if(commnds.size()!=3)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1] + " " + commnds[2];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "list_groups")
    {
        if(commnds.size()!=1)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "list_files")
    {
        if(commnds.size()!=2)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    // else if (commnds[0] == "upload_file")
    // {
    //     string temp = commnds[0] + " " + commnds[1] + " " + commnds[2];
    //     sendtotracker(sock, temp);
    //     rcevfromtracker(sock);
    // }
    else if (commnds[0] == "download_file")
    {
        if(commnds.size()!=4)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1] + " " + commnds[2] + " " + commnds[3];
        sendtotracker(sock, strac);
        string reply = rcevdownloadinfo(sock);
        string rs = reply.substr(0, 5);
        if (rs == "Hello")
        {

            int x = 0;
            int spcount = 0;
            vector<string> ipports;
            vector<string> fpaths;
            long long sizef;

            string ipp = "";
            vector<string> infocom;
            for (int j = 0; j < reply.size(); j++)
            {
                if (reply[j] != ' ')
                {
                    ipp.push_back(reply[j]);
                }
                else
                {
                    infocom.push_back(ipp);
                    ipp = "";
                }
            }
            if(ipp!="")
            infocom.push_back(ipp);
            sizef = stoll(infocom[2]);
            for (int xx = 3; xx < infocom.size(); xx+=2)
            {
                ipports.push_back(infocom[xx]);
                if(xx+1<infocom.size())
                fpaths.push_back(infocom[xx+1]);
            }
            string clienttoip = "";
            string clienttoport = "";
            int tt = 0;
            //string recvhash=infocom[1];
            for (; tt < ipports[0].size(); tt++)
            {
                if (ipports[0][tt] == ':')
                {
                    break;
                }
                else
                    clienttoip.push_back(ipports[0][tt]);
            }
            tt++;
            for (; tt < ipports[0].size(); tt++)
            {
                clienttoport.push_back(ipports[0][tt]);
            }
            string reve = "";
            // for (int i = fpaths[0].size() - 1; i >= 0; i--)
            // {
            //     if (fpaths[0][i] == '/')
            //         break;
            //     reve.push_back(fpaths[0][i]);
            // }
            // string fnameshere = string(reve.rbegin(), reve.rend());
            int sock_client = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in serv_addr;
            if (inet_pton(AF_INET, &clienttoip[0], &serv_addr.sin_addr) <= 0)
            {
                printf("\nInvalid address/ Address not supported \n");
                return;
            }
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(atoi(&clienttoport[0]));

            if (connect(sock_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                printf("\nConnection Failed \n");
                //return;
            }
            send(sock_client, &fpaths[0][0], fpaths[0].size(), 0);
            long long blocknumber, remsize = 0;
            long long blocksize = KB(512);
            if (sizef % blocksize == 0)
            {
                blocknumber = sizef / blocksize;
            }
            else
            {
                remsize = sizef % blocksize;
                blocknumber = sizef / blocksize;
                ++blocknumber;
            }
            long long k = 0;
            char dum[10]={0};
            vector<string>bc;
            char bu[blocksize];
            char bubu[remsize];
            string opp="[D] [" + commnds[1]+"] " +commnds[2];
            downinfo.push_back(opp);
            string dest=commnds[3];
            ofstream outfile;
            while (k < blocknumber)
            {
                if (k != blocknumber - 1)
                {
                    
                    
                    //cout<<"Reached\n";
                    int bytes = read(sock_client, bu, blocksize);
                    if (bytes == 0)
                    {
                        cout << sock_client << " shutted down!\n";
                        break;
                    }
                    //cout<<"writing chunk_no "<<chunk_no<<endl;
                    if(k == 0){
                        outfile.open(dest,ios_base::binary);
                        outfile.write(bu,blocksize);
                        outfile.close();		
                    }
                    else{
                        outfile.open(dest,ios_base::app|ios_base::binary);
                        outfile.write(bu,blocksize);
                         outfile.close();
                    }
                    
                    // char chunks[SHA_DIGEST_LENGTH * 2];
                    // memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    // unsigned char arr[SHA_DIGEST_LENGTH];
                    // memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    // SHA1((unsigned char *)bu, blocksize, arr);

                    // for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    // {
                    //     sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    // }
                    // string resultsha = (chunks);
                    //bc.push_back(resultsha);
                    
                    //string trackmsg = (bu);
                    //cout<<trackmsg<<endl;
                    for (int i = 0; i < blocksize; i++)
                    {
                        bu[i] = '\0';
                    }
                    write(sock_client, &dum, 10);
                    
                }
                else
                {
                    
                    //int bytes = recv(sock, buffer, remsize, 0);
                    // char buffer[blocksize];
                    // //cout<<"Reached\n";
                    for (int i = 0; i < remsize; i++)
                    {
                        bubu[i] = '\0';
                    }
                     int bytes = read(sock_client, bubu, blocksize);
                     if (bytes == 0)
                    {
                        cout << sock_client<< " shutted down!\n";
                        break;
                    }
                     if(k == 0){
                        outfile.open(dest,ios_base::binary);
                        outfile.write(bubu,remsize);	
                        outfile.close();	
                    }
                    else{
                        outfile.open(dest,ios_base::app|ios_base::binary);
                        outfile.write(bubu,remsize);
                        outfile.close();
                    }
                    char chunks[SHA_DIGEST_LENGTH * 2];
                    memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    unsigned char arr[SHA_DIGEST_LENGTH];
                    memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    SHA1((unsigned char *)bubu, blocksize, arr);

                    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    {
                        sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    }
                    string resultsha = (chunks);
                    bc.push_back(resultsha.substr(0,20));
                    
                    string trackmsg = (bubu);
                    //cout<<trackmsg<<endl;
                     for (int i = 0; i < remsize; i++)
                    {
                        bubu[i] = '\0';
                    }
                    write(sock_client, &dum, 10);
                   
                }
                ++k;
            }
            //cout<<"\nRecieved hash\n";
            //cout<<recvhash<<endl;
            
            // for(int re=0;re<bc.size();re++)
            // {
            //     cout<<bc[re];
            // }
            // cout<<endl;
            //outfile.close();
            string shafirst=comphash(fpaths[0]);
            string shasec=comphash(dest);
            if(shafirst==shasec)
            {
                cout<<"Sha1 matched successfully\n";
            }
            string os="[C] [" + commnds[1]+"] " +commnds[2];
            cominfo.push_back(os);
            downinfo.pop_back();
            string usiz=to_string(filesize(dest));
            string uploadmsg="upload_file "+dest+" "+commnds[1]+" "+commnds[2]+" "+usiz+" ";
            for (int re = 0; re < bc.size(); re++)
            {
                uploadmsg += bc[re];
            }
            sendtotracker(sock, uploadmsg);
            rcevfromtracker(sock);
            //close(sock_client);
        }

        else
        {
            cout << reply;
        }
        }
    }
    else if (commnds[0] == "logout")
    {
        if(commnds.size()!=1)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "show_downloads")
    {
        if(commnds.size()!=1)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        //string strac = commnds[0] + " " + commnds[1] + " " + commnds[2] + " " + commnds[3];
        for(int i=0;i<downinfo.size();i++)
        {
            cout<<downinfo[i]<<endl;
        }
        for(int i=0;i<cominfo.size();i++)
        {
            cout<<cominfo[i]<<endl;
        }
        }
        // sendtotracker(sock, strac);
        // rcevfromtracker(sock);
    }
    else if (commnds[0] == "stop_share")
    {
        if(commnds.size()!=3)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string strac = commnds[0] + " " + commnds[1] + " " + commnds[2];
        sendtotracker(sock, strac);
        rcevfromtracker(sock);
        }
    }
    else if (commnds[0] == "upload_file")
    {
        if(commnds.size()!=3)
        {
            cout<<"Invalid Command\n";
        }
        else
        {
        string fpath = commnds[1];
        long long file_size = filesize(fpath);
        if (file_size == -1)
            cout << "File not found\n";
        else
        {
            vector<string> sha1;
            string rev = "";
            for (int i = fpath.size() - 1; i >= 0; i--)
            {
                if (fpath[i] == '/')
                    break;
                rev.push_back(fpath[i]);
            }
            string fname = string(rev.rbegin(), rev.rend());
            filen[fname] = fpath;
            ifstream fin;
            fin.open(fpath);
            long long blocknumber, remsize = 0;
            long long blocksize = KB(512);
            if (file_size % blocksize == 0)
            {
                blocknumber = file_size / blocksize;
            }
            else
            {
                remsize = file_size % blocksize;
                blocknumber = file_size / blocksize;
                ++blocknumber;
            }
            long long k = 0;
            int fd = open(fpath.c_str(), O_RDONLY);
            while (k < blocknumber)
            {
                if (k != blocknumber - 1)
                {
                    // char chunks[SHA_DIGEST_LENGTH * 2];
                    // memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    // unsigned char arr[SHA_DIGEST_LENGTH];
                    // memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    // void *buf = malloc(blocksize);
                    // read(fd, buf, blocksize);
                    // SHA1((unsigned char *)buf, blocksize, arr);

                    // for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    // {
                    //     sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    // }
                    // string resultsha = (chunks);
                    // //sha1.push_back(resultsha.substr(0,2));
                    // free(buf);
                }
                else
                {

                    char chunks[SHA_DIGEST_LENGTH * 2];
                    memset(chunks, 0x0, SHA_DIGEST_LENGTH * 2);
                    unsigned char arr[SHA_DIGEST_LENGTH];
                    memset(arr, 0x0, SHA_DIGEST_LENGTH);
                    void *buf = malloc(remsize);
                    read(fd, buf, remsize);
                    SHA1((unsigned char *)buf, remsize, arr);

                    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
                    {
                        sprintf((char *)&(chunks[i * 2]), "%02x", arr[i]);
                    }
                    string resultsha = chunks;
                    sha1.push_back(resultsha.substr(0,20));
                    free(buf);
                }
                ++k;
            }
            string ss = to_string(file_size);
            string msgtrack = commnds[0] + " " + commnds[1] + " " + commnds[2] + " " + fname + " " + ss + " ";
            for (int i = 0; i < sha1.size(); i++)
            {
                msgtrack += sha1[i];
            }
            sendtotracker(sock, msgtrack);
            rcevfromtracker(sock);
        }
        }
    }
    else
    {
        cout << "Invalid Command\n";
        //exit(0);
    }
}
// void *downloadhelper(void *param,)
// {

// }
void *sendchunk(void *param)
{
    int nsoc = *((int *)param);
    cout << "Listening to " << nsoc << endl;
    char buffer[100005];
    int bytes = read(nsoc, buffer, 100005);
    if (bytes == 0)
    {
        cout << nsoc << " shutted down!\n";
        //return;
    }
    string fpath = (buffer);
    long long file_size = filesize(fpath);
    if (file_size == -1)
        {
            cout << "File not found\n";
            cout<<fpath<<endl;
        }
    else
    {
        string rev = "";
        for (int i = fpath.size() - 1; i >= 0; i--)
        {
            if (fpath[i] == '/')
                break;
            rev.push_back(fpath[i]);
        }
        string fname = string(rev.rbegin(), rev.rend());
        //filen[fname] = fpath;
        ifstream fin;
        fin.open(fpath,ios_base::binary);
        long long blocknumber, remsize = 0;
        long long blocksize = KB(512);
        if (file_size % blocksize == 0)
        {
            blocknumber = file_size / blocksize;
        }
        else
        {
            remsize = file_size % blocksize;
            blocknumber = file_size / blocksize;
            ++blocknumber;
        }
        long long k = 0;
        char dummy[10];
        char buf[blocksize];
        char xyz[remsize];
        //int fd = open(fpath.c_str(), O_RDONLY);
        while (k < blocknumber)
        {
            if (k != blocknumber - 1)
            {
                
                
                fin.read(buf, blocksize);
                write(nsoc, buf, blocksize);
                //cout<<"\nsend a buffer\n";
                read(nsoc,dummy,10);
                for(int i=0;i<blocksize;i++)
                {
                    buf[i]='\0';
                }
                //free(buf);
            }
            else
            {
                
                
                fin.read(xyz, remsize);

                write(nsoc, xyz, remsize);
                //cout<<"\nsend a buffer\n";
                read(nsoc,dummy,10);
                for(int i=0;i<remsize;i++)
                {
                    xyz[i]='\0';
                }
                //free(buf);
            }
            ++k;
        }

    }
    for(int i=0;i<100005;i++)
    {
        buffer[i]='\0';
    }
    pthread_exit(NULL);
}
void *clientconnect(void *param)
{
    //int nsoc = *((int *)param);
    // cout << "Listening to " << nsoc << endl;
    socklen_t addr_size;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    int serverSocket;
    int portno = atoi(&client_port[0]);
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        // return -1;
    }
    if (inet_pton(AF_INET, &client_ip[0], &serverAddr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        // return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portno);

    // Bind the socket to the
    // address and port number.
    bind(serverSocket,
         (struct sockaddr *)&serverAddr,
         sizeof(serverAddr));

    // Listen on the socket,
    // with 40 max connection
    // requests queued
    if (listen(serverSocket, 50) == 0)
        cout << "Listening\n";
    else
        cout << "Error\n";
    while (1)
    {
        socklen_t addr_size;
        struct sockaddr_in serverAd;
        struct sockaddr_storage serverSt;
        addr_size = sizeof(serverSt);

        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket,
                           (struct sockaddr *)&serverSt,
                           &addr_size);

        pthread_t *newthread = new pthread_t();
        if (pthread_create(newthread, NULL, sendchunk, &newSocket) != 0)

            // Error in creating thread
            cout << "Failed to create thread\n";
    }
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    string file_name;

    int i = 0;
    string temp = argv[1];
    for (; i < temp.size(); i++)
    {
        if (temp[i] == ':')
        {
            break;
        }
        else
            client_ip.push_back(temp[i]);
    }
    i++;
    for (; i < temp.size(); i++)
    {
        client_port.push_back(temp[i]);
    }

    cout << "Serving at " << client_ip << ":" << client_port << endl;
    string tracker_ip, tracker_port;
    fstream file;
    file.open(argv[2]);
    file >> tracker_ip;
    file >> tracker_port;

    // if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    // {
    //     printf("\n Socket creation error \n");
    //     return -1;
    // }

    // int portno = atoi(&client_port[0]);
    // if (inet_pton(AF_INET, &client_ip[0], &serverAddr.sin_addr) <= 0)
    // {
    //     printf("\nInvalid address/ Address not supported \n");
    //     return -1;
    // }
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_port = htons(portno);
    // bind(serverSocket,
    //      (struct sockaddr *)&serverAddr,
    //      sizeof(serverAddr));
    // if (listen(serverSocket, 50) == 0)
    //     cout << "Listening\n";
    // else
    //     cout << "Error\n";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    if (inet_pton(AF_INET, &tracker_ip[0], &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        //return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(&tracker_port[0]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        // return -1;
    }
    pthread_t *newthread = new pthread_t();
    if (pthread_create(newthread, NULL, clientconnect, NULL) != 0)
    {
        // Error in creating thread
        cout << "Failed to create thread\n";
    }
    while (1)
    {
        cout << "Enter Commands\n";
        string s;
        vector<string> commnds;
        getline(cin, s);
        if (s == "close")
        {
            break;
        }
        string ww = "";
        for (int j = 0; j < s.size(); j++)
        {
            if (s[j] != ' ')
            {
                ww.push_back(s[j]);
            }
            else
            {
                commnds.push_back(ww);
                ww = "";
            }
        }
        commnds.push_back(ww);
        process_cmnds(sock, commnds, client_ip, client_port);
    }
    close(sock);
    
    return 0;
}

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <bits/stdc++.h>

using namespace std;

// Semaphore variables
// sem_t x, y;
// pthread_t tid;
// pthread_t writerthreads[100];
// pthread_t readerthreads[100];
int readercount = 0;
int serverSocket, newSocket;
struct sockaddr_in serverAddr;
struct sockaddr_storage serverStorage;
// long long KB(int con)
// {

// }
// void comphash(string a,string b)
// {
//     SHA_CTX ctx;
//     SHA1_Init(&ctx);
//     long long SIZE=KB(512);
//     int fd = open(a,O_RDONLY);
//     char data[SIZE];
//     char chunks[SHA_DIGEST_LENGTH * 2];
//     int actually_read;
//     while((actually_read = read(fd,data, SIZE)) > 0) {
//         SHA1_Update(&ctx, data, actually_read);
//         bzero(data,SIZE);
//     }

//     char hash[21];
//     SHA1_Final((unsigned char)hash, &ctx);
// }
// Reader Function
class User
{
public:
    bool loginstat;
    string ID;
    string paswd;
    string IP;
    string Port;
    User(string a, string b)
    {
        ID = a;
        paswd = b;
        loginstat = false;
    }
};
class files
{
public:
    string fname;
    string sha;
    long long int fsize;
    //unordered_map<string, User *> UsersFiles;
    unordered_map<string, pair<bool,string> > issharing; //username
    

    files(string name, long long int size, string v)
    {
        fname = name;
        fsize = size;
        sha = v;
    }
};
class Group
{
public:
    string grpID;
    User *Admin;
    vector<User *> members;
    vector<files *> file_details;
    vector<User *> pending_members;
    
    Group(string a, User *b)
    {
        grpID = a;
        Admin = b;
        members.push_back(b);
    }
    void deletemem(User *a)
    {
        for (int i = 0; i < members.size(); i++)
        {
            if (members[i]->ID == a->ID)
            {
                members.erase(members.begin() + i);
                break;
            }
        }
        for (int i = 0; i < pending_members.size(); i++)
        {
            if (pending_members[i]->ID == a->ID)
            {
                pending_members.erase(pending_members.begin() + i);
                break;
            }
        }
    }
    bool isMember(User *a)
    {
        for (int i = 0; i < members.size(); i++)
        {
            if (members[i]->ID == a->ID)
            {
                return true;
            }
        }
        return false;
    }
    bool isPending(User *a)
    {
        for (int i = 0; i < pending_members.size(); i++)
        {
            if (pending_members[i]->ID == a->ID)
            {
                return true;
            }
        }
        return false;
    }
    User *validReq(string a)
    {
        for (int i = 0; i < pending_members.size(); i++)
        {
            if (a == pending_members[i]->ID)
                return pending_members[i];
        }
        return NULL;
    }
    void acceptReq(User *a)
    {
        for (int i = 0; i < pending_members.size(); i++)
        {
            if (pending_members[i]->ID == a->ID)
            {
                pending_members.erase(pending_members.begin() + i);
                break;
            }
        }
        members.push_back(a);
    }
    files *isFile(string a)
    {
        for (int i = 0; i < file_details.size(); i++)
        {
            if (file_details[i]->fname == a)
                return file_details[i];
        }
        return NULL;
    }
};

unordered_map<string, User *> peers;
unordered_map<string, Group *> groups;
unordered_map<string, bool> loggedstat;
unordered_map<string, files *> filenames;
void *reader(void *param)
{
    User *current = NULL;
    int nsoc = *((int *)param);
    cout<<"Listening to " << nsoc << endl;
    while (1)
    {
        char buffer[100005];
        int bytes = recv(nsoc, buffer, 100005, 0);
        if (bytes == 0)
        {
            cout << nsoc << " shutted down!\n";
            break;
        }
        string msg = (buffer);
        string tp = msg.substr(0, bytes);
        //cout<<tp<<endl;
        string temp = "";
        vector<string> commnds;

        for (int j = 0; j < tp.size(); j++)
        {
            if (tp[j] != ' ')
            {
                temp.push_back(tp[j]);
            }
            else
            {
                commnds.push_back(temp);
                temp = "";
            }
        }
        commnds.push_back(temp);
        for (int i = 0; i < 100005; i++)
        {
            buffer[i] = '\0';
        }
        if (commnds[0] == "create_user")
        {
            //cout<<"IN creste\n";
            if (peers.find(commnds[1]) == peers.end())
            {
                User *newpeer = new User(commnds[1], commnds[2]);
                peers[commnds[1]] = newpeer;
                //loggedstat[commnds[1]]=false;
                string msg = "User Created Successfully";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User Created Successfully\n";
            }
            else
            {
                string msg = "User Exist";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User Exist\n";
            }
        }
        else if (commnds[0] == "login")
        {
            if (current && current->loginstat)
            {
                string msg = "Already a user logged in";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "Already a user logged in\n";
            }
            else
            {
                if (peers.find(commnds[1]) == peers.end())
                {
                    string msg = "UserID does not Exist";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "UserID does not Exist\n";
                }
                else
                {
                    if (peers[commnds[1]]->paswd == commnds[2])
                    {
                        if (peers[commnds[1]]->loginstat)
                        {
                            string msg = "User already Logged In";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                            cout << "User already Logged In\n";
                        }
                        else
                        {
                            //loggedstat[commnds[1]]=true;
                            current = peers[commnds[1]];
                            peers[commnds[1]]->loginstat = true;
                            peers[commnds[1]]->IP = commnds[3];
                            peers[commnds[1]]->Port = commnds[4];
                            string msg = "User Logged In";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                            cout << "User Logged In\n";
                        }
                    }
                    else
                    {
                        string msg = "Enter Correct Password";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                        cout << "Enter Correct Password\n";
                    }
                }
            }
        }
        else if (commnds[0] == "create_group")
        {

            if (groups.find(commnds[1]) == groups.end())
            {
                Group *newgrp = new Group(commnds[1], current);
                groups[commnds[1]] = newgrp;
                string msg = "Group created successfully!";
                send(nsoc, msg.c_str(), msg.size(), 0);
            }

            else
            {
                string msg = "Group ID Exist";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "Group ID Exist\n";
            }
        }
        else if (commnds[0] == "join_group")
        {
            if (groups.find(commnds[1]) != groups.end())
            {
                Group *currgroup = groups[commnds[1]];
                if (currgroup->isMember(current) || currgroup->isPending(current))
                {
                    string msg = "You are already a member of this group";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                }
                else
                {
                    currgroup->pending_members.push_back(current);
                    string msg = "Request to join group sent!";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Request to join group Recieved\n";
                }
            }
            else
            {
                string msg = "Group ID not Exist";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "Group ID not Exist\n";
            }
        }
        else if (commnds[0] == "leave_group")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) != groups.end())
                {
                    Group *currgroup = groups[commnds[1]];
                    if (currgroup->isMember(current) || currgroup->isPending(current))
                    {
                        if (currgroup->Admin == current)
                        {
                            groups.erase(currgroup->grpID);
                            string msg = "You were group Admin. Group is deleted";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                        else
                        {
                            currgroup->deletemem(current);
                            string msg = "You are removed from this group";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                    }
                    else
                    {
                        string msg = "You are not a member of this group";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                    }
                }
                else
                {
                    string msg = "Group ID not Exist";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Exist\n";
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "list_requests")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else
                {
                    Group *currgroup = groups[commnds[1]];
                    string msg = "NO request pending\n";
                    for (int i = 0; i < currgroup->pending_members.size(); i++)
                    {
                        if (!i)
                            msg = "";
                        msg += (currgroup->pending_members[i])->ID;
                        msg += "\n";
                    }
                    send(nsoc, msg.c_str(), msg.size(), 0);
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "accept_request")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else if (groups[commnds[1]]->Admin != current)
                {
                    //Group *currgroup = groups[commnds[1]];
                    string msg = "User is not admin";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "User is not admin";
                }
                else
                {
                    User *thisuser = groups[commnds[1]]->validReq(commnds[2]);
                    if (!thisuser)
                    {
                        // Group *currgroup = groups[commnds[1]];
                        string msg = "User Request not present";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                        cout << "User Request not present\n";
                    }

                    else
                    {
                        //Group *currgroup = groups[commnds[1]];
                        groups[commnds[1]]->acceptReq(thisuser);
                        string msg = "Request Accepted";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                        cout << msg << endl;
                    }
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "list_groups")
        {
            if (current && current->loginstat)
            {
                string msg = "NO Groups";
                for (auto i = groups.begin(); i != groups.end(); i++)
                {
                    if (i == groups.begin())
                        msg = "";
                    msg += i->first;
                    msg += "\n";
                }
                send(nsoc, msg.c_str(), msg.size(), 0);
            }
            else
            {
                cout << current->loginstat;
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "logout")
        {
            if (current && current->loginstat)
            {

                current->loginstat = false;
                current->IP = "";
                current->Port = "";
                current = NULL;
                string msg = "successfully logged out";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "successfully logged out\n";
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "upload_file")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[2]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else
                {
                    Group *currgroup = groups[commnds[2]];

                    if (currgroup->isMember(current))
                    {
                        files *checkfile = currgroup->isFile(commnds[3]);
                        if (checkfile)
                        {
                            if (checkfile->issharing.find(current->ID) == checkfile->issharing.end())
                            {
                                checkfile->issharing[current->ID].first = true;
                                checkfile->issharing[current->ID].second = commnds[1];
                                string msg = "Now You are also sharing the File";
                                send(nsoc, msg.c_str(), msg.size(), 0);
                            }
                            else
                            {
                                checkfile->issharing[current->ID].first = true;
                                checkfile->issharing[current->ID].second = commnds[1];
                                string msg = "File name was already present\nFile path updated";
                                send(nsoc, msg.c_str(), msg.size(), 0);
                            }
                        }
                        else
                        {
                            long long int we = stoll(commnds[4]);
                            files *newfile = new files(commnds[3], we, commnds[5]);
                            //newfile->UsersFiles[current->ID] = current;
                            newfile->issharing[current->ID].first = true;
                            newfile->issharing[current->ID].second=commnds[1];
                            currgroup->file_details.push_back(newfile);
                            string msg = "File uploaded successfully";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                    }

                    else
                    {

                        string msg = "You are not a member of this group";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                    }
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "list_files")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else
                {
                    Group *currgroup = groups[commnds[1]];
                    string msg = "NO Files present\n";
                    for (int i = 0; i < currgroup->file_details.size(); i++)
                    {
                        if (!i)
                            msg = "";
                        msg += (currgroup->file_details[i])->fname;
                        msg += "\n";
                    }

                    send(nsoc, msg.c_str(), msg.size(), 0);
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "download_file")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else
                {
                    Group *currgroup = groups[commnds[1]];
                    files *checkfile = currgroup->isFile(commnds[2]);
                    if (checkfile)
                    {
                        string tpsha = checkfile->sha;
                        string us = "";
                        for (auto i : checkfile->issharing)
                        {

                            if ((i.second).first && peers[i.first]->loginstat)
                            {
                                User *tpuser = peers[i.first];
                                us += tpuser->IP + ":" + tpuser->Port + " "+(i.second).second+" " ;
                            }
                        }
                        if (us == "")
                        {
                            string msg = "Users having this files are either offline or not sharing";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                        else
                        {
                            us.pop_back();
                            string msg = "Hello "+tpsha + " " + to_string(checkfile->fsize)+" "+us ;
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                    }
                    else
                    {
                        string msg = "file not present";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                    }
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        else if (commnds[0] == "stop_share")
        {
            if (current && current->loginstat)
            {
                if (groups.find(commnds[1]) == groups.end())
                {
                    string msg = "Group ID not Presesnt";
                    send(nsoc, msg.c_str(), msg.size(), 0);
                    cout << "Group ID not Present\n";
                }
                else
                {
                    Group *currgroup = groups[commnds[1]];
                    if ((currgroup->isMember(current)))
                    {
                        files *checkfile = currgroup->isFile(commnds[2]);
                        if (checkfile)
                        {
                            if (checkfile->issharing.find(current->ID) == checkfile->issharing.end())
                            {
                                string msg = "You have not uploaded this file yet!";
                                send(nsoc, msg.c_str(), msg.size(), 0);
                            }
                            else
                            {

                                checkfile->issharing[current->ID].first = false;
                                string msg = "File sharing stopped";
                                send(nsoc, msg.c_str(), msg.size(), 0);
                            }
                        }
                        else
                        {
                            string msg = "File not Presesnt";
                            send(nsoc, msg.c_str(), msg.size(), 0);
                        }
                    }
                    else
                    {
                        string msg = "You are not member of this group";
                        send(nsoc, msg.c_str(), msg.size(), 0);
                        cout << "You are not member of this group\n";
                    }
                }
            }
            else
            {
                string msg = "User not Present";
                send(nsoc, msg.c_str(), msg.size(), 0);
                cout << "User not Present\n";
            }
        }
        // else if(commnds[0]=="show_downloads")
        // {
            
        // }
        else
        {
            // string msg = "Invalid command";
            // send(nsoc, msg.c_str(), msg.size(), 0);
            // cout << "Invalid command\n";
        }

        // if (n < 0)
        // {
        //     cout << "Error in read\n";
        // }
    }
        pthread_exit(NULL);
    
}

    // Writer Function
    // void *writer(void *param)
    // {
    //     // printf("\nWriter is trying to enter");

    //     // // Lock the semaphore
    //     // sem_wait(&y);

    //     // printf("\nWriter has entered");

    //     // // Unlock the semaphore
    //     // sem_post(&y);

    //     // printf("\nWriter is leaving");
    //     // pthread_exit(NULL);
    // }

    // Driver Code
    int main(int argc, char *argv[])
    {

        socklen_t addr_size;
        string tracker_ip, tracker_port;
        fstream file;
        file.open(argv[1]);
        file >> tracker_ip;
        file >> tracker_port;

        cout << "Serving at " << tracker_ip << ":" << tracker_port << endl;
        int portno = atoi(&tracker_port[0]);
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }
        if (inet_pton(AF_INET, &tracker_ip[0], &serverAddr.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
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

        // Array for thread
        //pthread_t tid[60];

        //cout << "Finish Establishment \n";

        int i = 0;

        while (1)
        {

            struct sockaddr_in serverAd;
            struct sockaddr_storage serverSt;
            addr_size = sizeof(serverSt);

            // Extract the first
            // connection in the queue
            newSocket = accept(serverSocket,
                               (struct sockaddr *)&serverSt,
                               &addr_size);
            // string s;
            // recv(newSocket,
            //      &s[0], sizeof(s), 0);

            // if (s!="")

            // Creater readers thread
            //cout << "In choice 1\n";
            pthread_t *newthread=new pthread_t();
            if (pthread_create( newthread, NULL, reader, &newSocket) != 0)

                // Error in creating thread
                cout << "Failed to create thread\n";
        }

        return 0;
    }
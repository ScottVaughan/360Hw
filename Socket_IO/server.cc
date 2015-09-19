#include "server.h"

Server::Server() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    // create and run the server
    create();
    serve();
}

void
Server::create() {
}

void
Server::close_socket() {
}

void
Server::serve() {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {

        handle(client);
    }
    close_socket();
}

void
Server::handle(int client) {
     Message message = new Message();
    // loop to handle all requests
    while (1) {
        // get a request
        string request = get_request(client);
        // break if client is done or an error occurred
        if (request.empty())
            break;
        
        // parse request
        Message message = parse_request(request, message);
        // get more characters if needed
        if (message.needed)
            get_value(client,message);
        // send response
        bool success = send_response(client,request);
        // break if an error occurred
        if (not success)
            break;
    }
    close(client);
}

Message
Server::parse_request(string request, Message message){

    /*He had this method in getrequest
        string cache = "";
        while(chache.find("\n")) != ...){
             string chache = get_request(client);
             sring.substr();//find gets the index and you chop it off up to the new line. 
        }
        cache
    */

    string command;
    string filename;
    string numberOfBytes = 0;
    string bytes;

    std::istringstream iss(request);
    iss >> command;

    if(command == "store"){
        iss >> filename;
        iss >> numberOfBytes;

        stringstream ss(numberOfBytes);
        int numbytes;
        ss >> numbytes;
       
        message.command = "stored";
        message.params[filename, numberOfBytes];
        message.numberOfBytes = numbytes;
        message.needed = true;
       

        iss >> bytes;
        if(iss.fail())
            return message;
        else{
            message.cache += bytes;  
            return message;
        }

    }

}
void
Server::get_value(int client, Message message){
/*
        while(cache.length() <= bytes.needed){
            cache = get_request();
        }
        cache.substr()
        if()
        getvalue();
    else
        cache;*/


        int sizeOfCache = message.cache.length();
        int bytesToBeCached = message.numberOfBytes - sizeOfCache;

        While(bytesToBeCached > 0){
            sizeOfCache = message.cache.length();
            bytesToBeCached = message.numberOfBytes - sizeOfCache;

            string bytes = get_request(client);
            message.cache += bytes;  
    }
}

string
Server::get_request(int client) {
    string request = "";
    // read until we get a newline
    while (request.find("\n") == string::npos) {
        int nread = recv(client,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        request.append(buf_,nread);
    }
    // a better server would cut off anything after the newline and
    // save it in a cache
    return request;
}

bool
Server::send_response(int client, string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

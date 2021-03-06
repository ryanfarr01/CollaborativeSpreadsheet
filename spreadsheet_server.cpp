/*
 * Filename: spreadsheet_server.cpp
 * Authors: Riley Anderson, Brent Bagley, Ryan Farr, Nathan Rollins
 * Last modified: 04/26/2015
 * Version 1.0
 */

/*
 * Description: Creates a server which pairs sockets, launches a thread for
 *   each socket paired, waits for messages from  each socket, and provides a convenient function for
 *   communicating with clients via sockets.
 */

 
#include <arpa/inet.h> // inet_ntoa
#include <algorithm> // remove()
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream> // File I/O
#include <iostream> // console I/O
#include <map>
#include <mutex>
#include <netdb.h> // addrinfo/getaddrinfo
#include <netinet/in.h> // Unnecessary?
#include <pthread.h>
#include <sstream>
#include <stdio.h> // perror error message printing
#include <string> // std::strings
#include <string.h> // memset(), strlen
#include <sys/socket.h> // Unnecessary?
#include <thread>
#include <unistd.h>
#include <vector>
#include "spreadsheet.h"


#define BACKLOG 10  // Max number of queued users waiting to connect
#define INCOMING_BUFFER_SIZE 500 // Used in handle() to receive messages from sockets


// Holds all registered users.
std::map<std::string,bool> user_list;

// Holds all spreadsheets.
std::map<std::string, spreadsheet*> spreadsheets;

//Map user ID to spreadsheet name
std::map<int, std::string> user_spreadsheet;

//Map spreadsheet name to all connected users (Theses spreadsheets are open)
std::map<std::string, std::vector<int> > spreadsheet_user;

// Locks shared resources between threads.
std::mutex godlock;


// Used to send a string through a socket.
int send_message(int socket_id, std::string string_to_send);

// Splits messages into space-separated tokens.
void split_message(std::string message, std::vector<std::string> & ret);

// Registers a new user, or sends an error to the requesting client.
void register_user(int user_socket_ID, std::string user_name);

//Save the current spreadsheets contents
void save_open_spreadsheets(std::string);

//Save the current spreadsheets contents
void save_spreadsheet_names(std::string);

// Handles a client's connection/spreadsheet loading request.
void connect_requested(int user_socket_ID, std::string user_name, std::string spreadsheet_requested);

//Change the incoming cells contents
void change_cell(int user_socket_id, std::string cell_name, std::string new_cell_contents);

//Send connect command
void send_connect(const int socket_id, const int count);

//Send cell changes
void send_cell(const int socket_id, const std::string cellName, const std::string cellContents);

//Send error
void send_error(int socket_id, int error_id, std::string context);

//Remove a user from previous spreadsheets
void remove_user(int socket_id);


/* Function: send_connect
 * Params: user ID, number of cells to send
 * Return: void
 *
 * Description: Sends specified client the "connected" command
 */
void send_connect(const int socket_id, const int count)
{
    std::string message = "connected ";
    std::stringstream ss;
    ss << count;
    message += ss.str();
    message += '\n';
    send_message(socket_id, message);
}

/* Function: send_cell
 * Params: user ID, name of cell, new contents
 * Return: void
 *
 * Description: Sends specified client the "cell" command, indicating they need to update a cell
 */
void send_cell(const int socket_id, const std::string cellName, const std::string cellContents)
{
    send_message(socket_id, std::string("cell " + cellName + " " + cellContents + '\n'));
}

/* Function: send_error
 * Params: client ID, error identifier, explanation
 * Return: void
 *
 * Description: Sends specified client the "error" command, letting them know they did something wrong
 */
void send_error(int socket_id, int error_id, std::string context)
{
    std::ostringstream stream;
    stream << error_id;
    send_message(socket_id, std::string("error " + stream.str() + " " + context + '\n'));
}

/* Function: user_to_spreadsheet
 * Params: user ID, pointer to a pointer of a spreadsheet
 * Return: 0 if failed, 1 if succeeded
 *
 * Description: Finds the spreadsheet associated with the user and changes pointer to point to it
 */
int user_to_spreadsheet(int user, spreadsheet **s)
{
    if(user_spreadsheet.count(user) != 0)
    {
        std::string spreadsheet_name = user_spreadsheet[user];
        if(spreadsheets.count(spreadsheet_name) != 0)
        {
            (*s) = spreadsheets[spreadsheet_name];
            return 1;
        }
    }
    
    return 0;
}


/* Function: register_user
 * Params: user ID, new name
 * Return: void
 *
 * Description: Registers a new user if the name hasn't already been registered
 *              and the current user is registered. Sends error 4 otherwise.
 */
void register_user(int user_socket_ID, std::string user_name)
{
  if(user_spreadsheet.count(user_socket_ID) > 0)
  {
    if(user_list.count(user_name) == 0)
    {
      // Add name to map
      user_list.insert(std::make_pair(user_name, 1));
      
      // Add name to user list file
      std::ofstream user_names;
      user_names.open("users.axis", std::ios_base::app); // This is needed so the file doesnt get overwritten
      user_names << user_name << std::endl;
      user_names.close();
    }
    else
    {   
        // Username already exists.  Send error 4.
        send_error(user_socket_ID, 4, user_name);
    }
  }
  else
  {
    send_error(user_socket_ID, 3, "Must be logged in to register a new user.");
  }
}// End register_user()

/* Function: remove_user
 * Params: user ID
 * Return: void
 *
 * Description: Gets rid of a user from all data structures. This is primarily
 *              for users switching spreadsheets
 */
void remove_user(int socket_id)
{
  if(user_spreadsheet.count(socket_id) > 0)
  {
    std::string spreadsheet = user_spreadsheet[socket_id];
    user_spreadsheet.erase(socket_id);

    std::vector<int>::iterator it;
    it = find(spreadsheet_user[spreadsheet].begin(), spreadsheet_user[spreadsheet].end(), socket_id);
    spreadsheet_user[spreadsheet].erase(it);
  }
}

/* Function: connect_requested
 * Params: user ID, username identifier, name of spreadsheet
 * Return: void
 *
 * Description: Called when a client tries to connect to a spreadsheet. If all information 
 *              works out, they will be sent the "connected" command. Otherwise they'll get error(s)
 */
void connect_requested(int user_socket_ID, std::string user_name, std::string spreadsheet_requested)
{
    // If the username has been registered...
    if (user_list.find(user_name) != user_list.end())
    {
        remove_user(user_socket_ID);
        
	//If the spreadsheet exists
        if(spreadsheets.count(spreadsheet_requested) == 1)
        {
            //associate the socket with the spreadsheet
            user_spreadsheet[user_socket_ID] = spreadsheet_requested;
            
            //If its already open
            if(spreadsheet_user.count(spreadsheet_requested) == 1)
            {
	      std::vector<int> users = spreadsheet_user[spreadsheet_requested]; 
	      users.push_back(user_socket_ID);
	      spreadsheet_user[spreadsheet_requested] = users;
            }
            //Add it to the opened
            else
            {
                std::vector<int> temp;
                temp.push_back(user_socket_ID);
                spreadsheet_user[spreadsheet_requested] = temp;
            }
	    
            send_connect(user_socket_ID, spreadsheets[spreadsheet_requested]->num_cells());

            if(spreadsheets[spreadsheet_requested]->num_cells() > 0)
            {
                //Send the cells
                std::map<std::string, std::string>::iterator itCells = spreadsheets[spreadsheet_requested]->get_data_map()->begin();
                for( ; itCells != spreadsheets[spreadsheet_requested]->get_data_map()->end(); ++itCells)
                {
                    //TODO:Fix this
                    send_cell(user_socket_ID, itCells->first, itCells->second);
                }   
            }          
        }
        //Otherwise, create the spreadsheet
        else
        {
            
            spreadsheet * s = new spreadsheet(spreadsheet_requested);
	    send_connect(user_socket_ID, s->num_cells());
            spreadsheets.insert(std::pair<std::string, spreadsheet*>(spreadsheet_requested, s));
            
            std::vector<int> temp;
            temp.push_back(user_socket_ID);
            spreadsheet_user.insert(std::pair<std::string, std::vector<int> >(spreadsheet_requested, temp));
            user_spreadsheet.insert(std::pair<int, std::string>(user_socket_ID, spreadsheet_requested));
	    save_spreadsheet_names(spreadsheet_requested);
        }
    }
    // Otherwise, respond with error 4
    else
        send_error(user_socket_ID, 4, user_name);
}//End connect_requested()

/* Function: save_spreadsheet_names
 * Params: name of spreadsheet to be saved
 * Return: void
 *
 * Description: Saves the name of a new spreadsheet into the spreadsheets.axis list
 */
void save_spreadsheet_names(std::string spreadsheet_name)
{
    std::ofstream ss_names;
    ss_names.open("spreadsheets.axis", std::ios_base::app); // This is needed so the file doesnt get overwritten
    ss_names << spreadsheet_name << std::endl;
    ss_names.close();
}

/* Function: save_open_spreadsheets
 * Params: name of spreadsheet to be saved
 * Return: void
 *
 * Description: Saves the contents of specified spreadsheet
 */
void save_open_spreadsheets(std::string spreadsheet_name)
{
    //Create the file name
    std::string file_name = spreadsheet_name;
    file_name += ".axissheet";
    std::ofstream ss;
    //Open the file
    ss.open(file_name.c_str());
    
    std::map<std::string, std::string>::iterator itCells;
    for(itCells = spreadsheets[spreadsheet_name]->get_data_map()->begin(); itCells != spreadsheets[spreadsheet_name]->get_data_map()->end(); itCells++)
        
    {
        ss << itCells->first << "="<< itCells->second<< std::endl;
    }
    
    ss.close();
}

/* Function: change_cell
 * Params: user ID, cell name, new cell contents
 * Return: void
 *
 * Description: Checks for circular dependencies (returns error if there are any), then
 *              internally changes spreadsheets and sends off cell change to associated clients
 */
void change_cell(int user_socket_id, std::string cell_name, std::string new_cell_contents)
{
    std::map<int,std::string>::iterator it;

    spreadsheet *s;
    if(user_to_spreadsheet(user_socket_id, &s))
    {
        if(s->set_cell(cell_name, new_cell_contents))
        {
            std::vector<int> users = spreadsheet_user[s->get_name()];
            std::vector<int>::iterator it;
            for(it = users.begin(); it != users.end(); it++)
            {
                send_cell(*it, cell_name, new_cell_contents);
            }
            save_open_spreadsheets(s->get_name());
        }
        else
        {
            send_error(user_socket_id, 2, "Circular Dependency");
        }
    }
    else
    {
        // Failed to match user to aa spreadsheet. Send error 3.
        send_error(user_socket_id, 3, "User not logged in.");
    }
}

/* Function: undo
 * Params: user ID
 * Return: void
 *
 * Description: Called when a client wants to undo last action in spreadsheet 
 */
void undo(int socket_id)
{
    //Find the spreadsheet
    //Call undo on spreadsheet
    //Send the cell change to all other users on the spreadsheet
    spreadsheet *s;
    if(user_to_spreadsheet(socket_id, &s))
    {
        std::string cell, contents;
        if(s->undo(&cell, &contents))
        {
            std::vector<int> users = spreadsheet_user[s->get_name()];
            std::vector<int>::iterator it;
            
            for(it = users.begin(); it != users.end(); it++)
            {
                send_cell(*it, cell, contents);
            }
            save_open_spreadsheets(s->get_name());
        }
    }
    else
    {
        // Failed to match user to aa spreadsheet. Send error 3.
        send_error(socket_id, 3, "User not logged in.");
    }
}

/* Function: send_message
 * Params: user ID, message to be sent
 * Return: 1 if succeeded, 0 otherwise
 *
 * Description: Sends the specified client the specified message.
 *              Terminate message with \n
 */
int send_message(int socket_id, std::string string_to_send)
{
    // If socket_id is 0, we don't want to send the message.
    //   (It's the server sending an error to itself as though it's a client)
    if (socket_id == 0)
        return 1;
    
    // Send the message across the socket.
    const char * message = string_to_send.c_str();
    int length = strlen(message);
    ssize_t bytes_sent = send(socket_id, message, length, 0);
    
    // Return a nonzero value if we didn't send the whole message.
    if (bytes_sent != length){
        return 1;
    }
    
    // Return 0 to signal success.
    return 0;
    
} // end send_message()

/* Function: messaeg_received
 * Params: user ID, message received
 * Return: void
 *
 * Description: Splits up the message received and determines what functions
 *              to call accordingly
 */
void message_received(int socket_id, std::string & line_received)
{
    // THIS IS PLACEHOLDER CODE. TO BE REPLACED.
    // Prints the message to the server console
    std::cout << "Line received: " << line_received << std::endl;
    
    std::vector<std::string> command;
    split_message(line_received, command);

    // Call the appropriate functions for the received command.
    if (command.at(0) == "connect")
    {
        if (command.size() > 2)
        {
            std::string spreadsheet_name = "";
            std::vector<std::string>::iterator it = command.begin();
            it += 2;

            // Continue adding all tokens until the end of the line to the final parameter
            for( ; it != command.end(); it++)
            {
                spreadsheet_name += *it + " ";
            }

            // Remove the trailing space from the final token (if there is one)
            if (spreadsheet_name[spreadsheet_name.size()-1] == ' ')
                spreadsheet_name = spreadsheet_name.substr(0, spreadsheet_name.size()-1);

            connect_requested(socket_id, command.at(1), spreadsheet_name);
        }
        else
        {
            // Too few parameters. Send error 2.
            send_error(socket_id, 2, "Invalid parameters in command: " + line_received);
        }
    }
    else if (command.at(0) == "register")
    {
        if (command.size() == 2)
            register_user(socket_id, command.at(1));
        else
        {
            // Invalid parameters. Send error 2.
            send_error(socket_id, 2, "Invalid parameters in command: " + line_received);
        }
    }
    else if (command.at(0) == "cell")
    {
        if (command.size() > 2)
        {
            std::string cell_name, cell_contents = "";
            std::vector<std::string>::iterator it = command.begin();
            it++;
            cell_name = *it;
            it++;
            
            // Continue adding all tokens until the end of the line to the final parameter
            for( ; it != command.end(); it++)
            {
                cell_contents += *it + " ";
            }
            
            // Remove the trailing space from the final token (if there is one)
            if (cell_contents[cell_contents.size()-1] == ' ')
                cell_contents = cell_contents.substr(0, cell_contents.size()-1);
            
            change_cell(socket_id, cell_name, cell_contents);
        }
        else
        {
            // Invalid parameters. Send error 2.
            send_error(socket_id, 2, "Invalid parameters in command: " + line_received);
        }
    }
    else if (command.at(0) == "undo")
    {
        std::cout << "In undo else-if" << std::endl;
        undo(socket_id);
    }
    else
    {
        // Invalid parameters. Send error 2.
        send_error(socket_id, 2, "Invalid parameters in command: " + line_received);
    }
}// End message_received()

/* Function: split_message
 * Params: message to split, vector to store results
 * Return: void
 * Source: http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
 *
 * Description: Splits up a message by spaces. Helper function to help determine how to
 *              respond to an incoming message
 */
void split_message(std::string message, std::vector<std::string> & ret)
{   
    // Strip any carriage returns off of our message.
    message.erase( remove(message.begin(), message.end(), '\r'), message.end() );
    message.erase( remove(message.begin(), message.end(), '\n'), message.end() );
    
    // Add the tokens one by one, space-separated
    ret.clear();
    int pos = message.find(' ');
    int pos_init = 0;
    int hit_space = 0;
    
    while(pos != std::string::npos)
    {
        hit_space = 1;
        ret.push_back(message.substr(pos_init, pos - pos_init));
        pos_init = pos + 1;
        pos = message.find(' ', pos_init);
    }
    
    // Add the last token missed by the above loop
    ret.push_back(message.substr(pos_init, message.size() - pos_init));
    
}// End split_message()


// Called when a client disconnects.
// Removes them from any spreadsheets they were editing and closes the socket.
void client_disconnected(int socket_id)
{
    // THIS IS PLACEHOLDER CODE. TO BE HANDLED LATER.
    std::cout << "A client has disconnected." << std::endl;
    
    // Remove the user's spreadsheet associations.
    
    // Close the socket
    close(socket_id);
}// End client_disconnected()

/* Function: handle
 * Params: socket
 * Return: handle pointer
 *
 * Description: Fired when a new socket connects. Invoked in a new thread as soon as socket is paired
 */
void *handle(void *pnewsock)
{
    int newsock = *(int*)pnewsock; // Cast to string identifier of the socket
    
    // This string will be populated with what we receive from the socket, and full lines
    //   (terminated by '\n' characters) will be removed from the front as they arrive.
    std::string received_so_far = "";
    
    // Infinitely check for complete lines and continue receiving data.
    // Note that this does NOT occur on the main thread.
    // Each socket has its own thread for this purpose.
    while(1)
    {
        // Receive more data from the socket:
        // Create the buffer we'll populate with the received messages.
        char incoming_data_buffer[INCOMING_BUFFER_SIZE];
        
        // Wait for a message to be received. This code will block until we receive something.
        ssize_t bytes_received = recv(newsock, incoming_data_buffer, INCOMING_BUFFER_SIZE-1, 0);
        
        // If the client has shut down, call the client disconnection cleanup function, and return.
        if (bytes_received == 0) {
            client_disconnected(newsock);
            return NULL;
        }
        
        // If we failed to receive properly, send an error to the error stream, and return.
        if (bytes_received == -1) {
            perror("handle()'s message reception failed");
            client_disconnected(newsock);
            return NULL;
        }
        
        // Terminates the received character array properly. (May not be needed?).
        if (bytes_received != INCOMING_BUFFER_SIZE)
            incoming_data_buffer[bytes_received] = '\0';
        
        // Get a string representation of what we've just received from the socket.
        std::string received_data(incoming_data_buffer);
        
        // Add what we've just received to the received_so_far string.
        received_so_far += received_data;
        
        // While we have newline characters, strip the lines one by one and pass them to message_received().
        while(received_so_far.find('\n') != std::string::npos)
        {
            // This will hold the line at the front of received_so_far.
            std::string current_line = received_so_far.substr(0, received_so_far.find('\n')+1);
            
            // Remove the line we just found from received_so_far.
            received_so_far = received_so_far.substr(current_line.length(),received_so_far.length()-received_so_far.find('\n'));
            
            // Remove the newline character from the current_line
            current_line = current_line.substr(0, current_line.length()-1);
            
            // call message_received() with the newly received line.
            godlock.lock();
            message_received(newsock, current_line);
            godlock.unlock();
        } // End newline detection loop
    } // End infinite receive/newline detection loop
    
    return NULL;
} // End handle()

/* Function: main
 * Params: number of arguments, string arguments
 * Return: int
 *
 * Description: Starts up the server. Pulls in all saved spreadsheets and users, then waits
 *              for connecting clients.
 */
int main(int argc, char* argv[])
{
    // Holds the port number we're going to host on.   Default to port 2000 as per protocol specification.
    std::string port = "2000";
    
    // If we have exactly one argument, make it our host port.
    if (argc == 2){
        port = argv[1]; //  Port value assigned here.
        
        // Error if we could not parse the argument as an int.
        if( std::atoi(port.c_str()) == 0 ){
            fprintf(stderr, "Invalid port specified\n");
            return 1; // Terminate here
        }
    }
    
    
    // Load the list of registered users, or create one if none exists.
    // Check if file exists.
    FILE * user_list_file = fopen("users.axis", "r");
    bool exists = (user_list_file == NULL) ? false : true;
    if (exists) fclose(user_list_file);
    
    // If the users.axis file exists, add its users to the user_list.
    if (exists)
    {
        std::ifstream file_stream("users.axis");
        std::string txt;
        
        if (file_stream.is_open())
            while (file_stream.good())
            {
                getline(file_stream, txt);
                user_list.insert(std::make_pair(std::string(txt), true));
            }
        file_stream.close();
    }
    // Otherwise, create it.
    else
    {
        // Add name to map
        user_list.insert(std::make_pair(std::string("sysadmin"), true));
        
        // Add name to user list file
        std::ofstream user_names;
        user_names.open("users.axis", std::ios_base::app); // This is needed so the file doesnt get overwritten
        user_names << "sysadmin" << std::endl;
        user_names.close();
    }
    
    
    //Start the save thread
    //pthread_t save_thread;
    //pthread_create(&save_thread, NULL, &save_open_spreadsheets, NULL);
    
    
    
    //pthread_t save_thread;
    //pthread_create(&save_thread, NULL, &save_open_spreadsheets, NULL);
    //pthread_join(save_thread, NULL);
    
    // Begin loading all spreadsheets from file:
    // Check if file containing list of existing spreadsheets exists.
    FILE * sheet_list_file = fopen("spreadsheets.axis", "r");
    bool sheets_file_exists = (sheet_list_file == NULL) ? false : true;
    if (sheets_file_exists) fclose(sheet_list_file);
    
    
    if (sheets_file_exists) // If the file exists...
    {
        // Open the existing spreadsheet names file.
        std::ifstream file_stream("spreadsheets.axis");
        std::string txt;
        if (file_stream.is_open())
            // For each spreadsheet name in the file...
            while (file_stream.good())
            {            
                getline(file_stream, txt);
                
                // If there exists a file with this name...
                FILE * sheet_file = fopen("spreadsheets.axis", "r");
                bool file_exists = (sheet_file == NULL) ? false : true;
                if (file_exists)
                {
                    fclose(sheet_file);
                    
		    spreadsheets[txt] = new spreadsheet(txt);

                    // Open the file with this spreadsheet's name...
                    std::ifstream current_file_stream((txt+".axissheet").c_str());
                    std::string current_line;
                    if (current_file_stream.is_open())
                        // While new lines exist within this file...
                        while (current_file_stream.good())
                        {
                            getline(current_file_stream, current_line);
                            
                            // Separare into cell_name and cell_contents by "=" symbol.
                            std::string cell_name, cell_contents;
                            int equals_index = current_line.find('=');
                            cell_name = current_line.substr(0, equals_index);
                            cell_contents = current_line.substr(equals_index+1, current_line.length()-(equals_index-1));
                            
                            if(cell_contents == "")
                                continue;
                  
                            // Set the cell to what we just read from the file.
                            spreadsheets[txt]->set_cell(cell_name,cell_contents);
                        }
                    current_file_stream.close();
                }
            }
        file_stream.close();
    } // End loading all spreadsheets from file.
    
    
    /* Get the address info */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints); // Clear addrinfo struct
    hints.ai_family = AF_INET;       // Internet address family
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Marked for bind()ing
    // Localhost, port, addrinfo struct, list of structs
    if (getaddrinfo(NULL, port.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo"); // Error if getaddrinfo failed
        return 1;
    }
    
    /* Create the socket */
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        perror("socket"); // Error if socket creation failed
        return 1;
    }
    
    /* Enable the socket to reuse the address */
    int reuseaddr = 1; /* True */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        perror("setsockopt"); // Error if socket option setting failed
        return 1;
    }
    
    /* Bind the socket to the address */
    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind"); // Error if binding failed
        return 1;
    }
    
    /* Listen on the socket */
    if (listen(sock, BACKLOG) == -1) {
        perror("listen"); // Error if listening failed
        return 1;
    }
    
    // Release the addrinfo struct's memory
    freeaddrinfo(res);
    
    /* Main loop
     *  Threaded server implementation adapted from Martin Broadhurst's
     *  implementation:  http://martinbroadhurst.com/server-examples.html .
     *  Pairs sockets, launches a new thread for each newly paired socket */
    pthread_t thread;
    while (1){
        size_t size = sizeof(struct sockaddr_in);
        
        
        // Create structures necessary for socket acceptance.
        struct sockaddr_in their_addr;
        int newsock = accept(sock, (struct sockaddr*)&their_addr, (socklen_t*)&size);
        if (newsock == -1) {
            perror("accept");
        }
        else {
            printf("Got a connection from %s on port %d\n", inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));
            if (pthread_create(&thread, NULL, handle, &newsock) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
        }
    }
    
    // Close the socket.
    close(sock);
    
    // No errors; return exit code 0.
    return 0;
} // End main()

/*
 * Filename: spreadsheet_server.cpp
 * Authors: Riley Anderson, Brent Bagley, Ryan Farr, Nathan Rollins 
 * Last modified: 04/06/2015
 * Version 1.0
 */
 
/*
 * Description: Creates a server which pairs sockets, launches a forked thread for
 *   each socket paired, waits for messages from  each socket, and provides a convenient function for 
 *   communicating with clients via sockets.
 */
 
/*
 *  Forked server implementation adapted from Martin Broadhurst's 
 *     implementation:  http://martinbroadhurst.com/server-examples.html
 *
 *  Code utilizes the Berkely sockets API
 */


#include <fstream> // File I/O
#include <iostream> // console I/O
#include <stdio.h> // perror error message printing
#include <string> // std::strings
#include <string.h> // memset(), strlen
#include <arpa/inet.h> // inet_ntoa
#include <sys/wait.h> // wait_for_child zombie reclamation requrement
#include <vector>
#include <map>
#include <sstream>
#include <netdb.h> // addrinfo/getaddrinfo
#include <algorithm>
#include "spreadsheet.h"
//#include <sys/socket.h> // Unnecessary?
//#include <netinet/in.h> // Unnecessary?
//#include <unistd.h> // Unnecessary?
//#include <signal.h> // Unnecessary?


#define BACKLOG 10  // Max number of queued users waiting to connect
#define INCOMING_BUFFER_SIZE 500 // Used in handle() to receive messages from sockets

// Holds all registered users.
std::map<std::string,bool> user_list;

// Holds all spreadsheets.
std::map<std::string, spreadsheet> spreadsheets;

// Spreadsheet corrisponding to users socket.
std::map<int, spreadsheet> user_spreadsheet;

//Map the spreadsheets to their connected sockets
std::map<spreadsheet, std::vector<int> > spreadsheet_user;

// Used to send a string through a socket.
int send_message(int socket_id, std::string string_to_send);

// Splits messages into space-separated tokens.
void split_message(std::string message, std::vector<std::string> & ret);

// Registers a new user, or sends an error to the requesting client.
void register_user(int user_socket_ID, std::string user_name);

// Saves the current list of registered users to file.
void save_user_list();

//Save the current spreadsheets contents
void save_all_spreadsheets();

//Save the current spreadsheets contents
void save_spreadsheet_names();

// Handles a client's connection/spreadsheet loading request.
void connect_requested(int user_socket_ID, std::string user_name, std::string spreadsheet_requested);

//Change the incoming cells contents
void change_cell(int user_socket_id, std::string cell_name, std::string new_cell_contents);

//Send connect command
void send_connect(int socket_id, int count);

//Send cell changes
void send_cell(int socket_id, std::string cellName, std::string cellContents);

//Send error
void send_error(int socket_id, int error_id, std::string context);

void send_connect(int socket_id, int count)
{
  send_message(socket_id, std::string("connected " + count + '\n'));
}

void send_cell(int socket_id, std::string cellName, std::string cellContents)
{
  send_message(socket_id, std::string("cell " + cellName + " " + cellContents + '\n'));
}

void send_error(int socket_id, int error_id, std::string context)
{
  std::ostringstream stream;
  stream << error_id;
  send_message(socket_id, std::string("error " + stream.str() + " " + context + '\n'));
}

// Signal handler to reap zombie processes
static void wait_for_child(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
} // end wait_for_child()


// Processes a username registration request.
void register_user(int user_socket_ID, std::string user_name)
{
	// NOTE: THIS CODE CANNOT BE USED UNTIL SOCKETS ARE ASSOCIATED WITH USERNAMES.

	// If (user_socket_ID is logged in as a registered user)
	//{
	//	// If name isn't already registered; register it
	//	if (user_list.find(user_name) == user_list.end())
	//	{
	//		user_list.insert(std::make_pair(std::string("sysadmin"), true));
	//		save_user_list();
	//	}
	// 	else
	//		// Name is already registered. Respond with error 4.
	//		send_message(user_socket_ID, "Error 4 " + user_name + "\n");
	//}
}// End register_user()


// Handles a client's connection/spreadsheet loading request.
void connect_requested(int user_socket_ID, std::string user_name, std::string spreadsheet_requested)
{	
	// If the username has been registered...
	if (user_list.find(user_name) != user_list.end())
	{
		// If (the spreadsheet exists)
		//	associate this socket with desired spreadsheet, and proceed to send the sheet to it per protocol.
		// else
		// {
		// 	Create the spreadsheet
        //  save_spreadsheet_names(spreadsheet_requested);
		//	associate this socket with this spreadsheet, and proceed per protocol.
		// }
	}
	// Otherwise, respond with error 4
	else
	  send_error(user_socket_ID, 4, user_name);
}// End connect_requested()


//Save all of the spreadsheet names to a file. Read on server launch
void save_spreadsheet_names(std::string spreadsheet_name)
{
    std::ofstream ss_names;
    ss_names.open("spreadsheets.axis");
    ss_names << spreadsheet_name << std::endl;
    ss_names.close();
}

//Save all spreadsheets contents on server. Read on server launch
void save_all_spreadsheets()
{
    std::map<std::string,spreadsheet>::iterator it;
    
    for (it = spreadsheets.begin(); it != spreadsheets.end(); it++)
    {
        std::string file_name = it->first + ".axissheet";
        std::map<std::string, std::string>::iterator data_it;
        
        std::ofstream ss;
        ss.open(file_name.c_str());
        
        for(data_it = it->second.get_data_map().begin(); data_it != it->second.get_data_map().end(); data_it++)
        {
            
            ss << data_it->first << "="<< data_it->second<< std::endl;
            
        }
        
        ss.close();
        
    }
}

void save_all_open_spreadsheets()
{
    //Loop through all open spreadsheets saving all every 30 seconds??
    //Each spreadsheet object has its own timer
    //
}

// Saves the user_list map to file.  Will be read upon next server launch.
void save_user_list()
{
	// Create/open the file.
	std::ofstream user_list_file;
	user_list_file.open("users.axis");
	
	// Iterate over the user_list map, and write each of its vales to file.
	std::map<std::string, bool>::iterator it;
	for (it = user_list.begin(); it != user_list.end(); it++)
	{
		user_list_file << it->first << "\n";
	}
	
	// Close the file
	user_list_file.close();
} // End save_user_list()


//Change the requested cell
void change_cell(int user_socket_id, std::string cell_name, std::string new_cell_contents)
{
    std::map<int,spreadsheet>::iterator it;
    
    for(it = user_spreadsheet.begin(); it != user_spreadsheet.end(); it++)
    {
        if(it->first == user_socket_id)
        {
            if((it->second.set_cell(cell_name, new_cell_contents)) == 1)
            {
                //Success, send to rest of clients, save it
            }
            
            //returns 0. Send error
            else
            {
                send_message(user_socket_id, std::string("Error 2 Circular Dependency Error\n"));
            }
        }
    }
}


// Used to send a string through a socket.
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
        perror("send_message failed"); // Error if sending failed.
        return 1;
    }
    
    // Return 0 to signal success.
    return 0;
} // end send_message()


// Called when an entire "\n"-terminated command is received.
//   Parameter will still contain the newline character. (Easily changeable if that bothers anyone)
void message_received(int socket_id, std::string & line_received)
{
    // THIS IS PLACEHOLDER CODE. TO BE REPLACED.
    // Prints the message to the server console
    std::cout << "Line received: " << line_received << std::endl;

    std::vector<std::string> command;
    split_message(line_received, command);
    
    std::cout << "In pieces:" << std::endl;
    for(std::vector<std::string>::iterator it = command.begin(); it != command.end(); ++it)
    {
      std::cout << *it << std::endl;
    }
	
	// Call the appropriate functions for the received command.
	// NOTE: THIS IS ROUGH CODE AND IS NOT INTENDED TO BE NOT ERROR-PROOF.
	//		MUST DETECT ERRORS AS PER THE PROTOCOL.
	if (command.at(0) == "connect")
		connect_requested(socket_id, command.at(1), command.at(2));
	if (command.at(0) == "register")
		register_user(socket_id, command.at(1));
	   
    // Echos the message back to the sender.
    std::string my_string = "Echo: " + line_received + "\n";
    send_message(socket_id, my_string);
}// End message_received()


/*
 * http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
 *
 */
 // Splits a string message into its space-separated components.
void split_message(std::string message, std::vector<std::string> & ret)
{
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
  
  ret.push_back(message.substr(pos_init, std::min(pos, static_cast<int>(message.size())) - pos_init));
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


// Handler (event) fired for each new socket.
// Invoked in a forked thread as soon as the socket is paired.
void handle(int newsock)
{
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
        ssize_t bytes_received = recv(newsock, incoming_data_buffer, INCOMING_BUFFER_SIZE, 0);
        // If the client has shut down, call the client disconnection cleanup function, and return.
        if (bytes_received == 0) {
            client_disconnected(newsock);
            return;
        }
        // If we failed to receive properly, send an error to the error stream, and return.
        if (bytes_received == -1) {
            perror("handle()'s message reception failed");
            return;
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
            message_received(newsock, current_line);
        } // End newline detection loop
    } // End infinite receive/newline detection loop
} // End handle()


// Main function.  Creates a server which pairs sockets to connecting clients, and 
//   launches a callback handle in a forked thread for each.
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
		user_list.insert(std::make_pair(std::string("sysadmin"), true));
        save_user_list();
    }
    
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
                
                    // Open the file with this spreadsheet's name...
                    std::ifstream current_file_stream((txt+".axis").c_str());
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

                            // Set the cell to what we just read from the file.
                            change_cell(0,cell_name,cell_contents);
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

    /* Set up the signal (event) handler */
    struct sigaction sa;
    sa.sa_handler = wait_for_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    /* Main loop */
    /* Pairs sockets, launches forked process for each newly paired socket */
    while (1){
        // Create structures necessary for socket acceptance.
        struct sockaddr_in their_addr;
        socklen_t size = sizeof(struct sockaddr_in);
        // Get int identifier for new socket, and create new socket for connecting client.
        int newsock = accept(sock, (struct sockaddr*)&their_addr, &size);

        // Announce if we encounter an error.
        if (newsock == -1) {
            perror("accept");
            return 0;
        }

        // Print connection announcement to console.
        printf("Got a connection from %s on port %d\n", inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));

        // Fork.  Create new thread for the new socket.
        int pid = fork();
        if (pid == 0) {
            /* In child process */
            close(sock);
            handle(newsock);
            return 0;
        }
        else {
            /* Parent process */
            if (pid == -1) {
                perror("fork");
                return 1;
            }
            else {
                close(newsock);
            }
        }
    }

    // Close the socket.
    close(sock);

    // No errors; return exit code 0.
    return 0;
} // End main()

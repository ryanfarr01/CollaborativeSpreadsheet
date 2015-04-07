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


#include <iostream> // console I/O
#include <stdio.h> // perror error message printing
#include <string> // std::strings
#include <string.h> // memset(), strlen
#include <arpa/inet.h> // inet_ntoa
#include <sys/wait.h> // wait_for_child zombie reclamation requrement
#include <netdb.h> // addrinfo/getaddrinfo
//#include <sys/socket.h> // Unnecessary?
//#include <netinet/in.h> // Unnecessary?
//#include <unistd.h> // Unnecessary?
//#include <signal.h> // Unnecessary?


#define PORT "2115" // Port to host on
#define BACKLOG 10  // Max number of queued users waiting to connect
#define INCOMING_BUFFER_SIZE 500 // Used in handle() to receive messages from sockets


// Signal handler to reap zombie processes
static void wait_for_child(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
} // end wait_for_child()


// Used to send a string through a socket.
int send_message(int socket_id, std::string & string_to_send)
{
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
    
    // Echos the message back to the sender.
    std::string my_string = "Echo: " + line_received + "\n";
    send_message(socket_id, my_string);
}

// Called when a client disconnects.
// Removes them from any spreadsheets they were editing and closes the socket.
void client_disconnected(int socket_id)
{
    // THIS IS PLACEHOLDER CODE. TO BE HANDLED LATER.
    std::cout << "A client has disconnected." << std::endl;
    
    // Close the socket
    close(socket_id);
}

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
int main(void)
{
    /* Get the address info */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints); // Clear addrinfo struct
    hints.ai_family = AF_INET;       // Internet address family
    hints.ai_socktype = SOCK_STREAM; // TCP
    // Localhost, port, addrinfo struct, list of structs
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
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

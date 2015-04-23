using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;

/// <summary>
/// The command processor prepares commands to send and translates recieved messages. 
/// </summary>
public class CommandProcessor
{
    /// <summary>
    /// The associated socket handler
    /// </summary>
    public SocketHandler handler;
    public delegate void connectedFunction(int cellCount);
    public delegate void cellFunction(string cellName, string cellContents);
    public delegate void errorFunction(int errorNumber, string errorMessage);
    public delegate void invalidFunction(string badCommand, string explaination);

    connectedFunction conFunc;
    cellFunction cellFunc;
    errorFunction errorFunc;
    invalidFunction invalidFunc;

    public CommandProcessor(connectedFunction con, cellFunction cell, errorFunction error, invalidFunction invalid)
    {
        conFunc = con;
        cellFunc = cell;
        errorFunc = error;
        invalidFunc = invalid;
    }


    /// <summary>
    /// takes a received command and translates it 
    /// </summary>
    /// <param name="s">the recieved command</param>
    public void ProcessServerCommand(string s)
    {
        // Note: this method contains several console writes, these are for debugging purposes 
        // and will be removed in the final version.
        Console.WriteLine("-------------------------------------");
        Console.WriteLine("Command Recieved: " + s);
        Console.WriteLine("Tokens:");

        // split message into tokens based on white space (charater value 32)
        char[] splitter = new char[1] { ' ' };
        string[] tokens = s.Split(splitter);

        // debug write all tokens
        foreach (string t in tokens)
        {
            Console.WriteLine(t);
        }

        // determine the command based on the first token and call the apropriate method.
        if (tokens[0] == "connected")
        {
            ReceiveConnected(tokens, s);
        }
        else if (tokens[0] == "cell")
        {
            ReceiveCell(tokens, s);
        }
        else if (tokens[0] == "error")
        {
            ReceiveError(tokens, s);
        }
        else
            InvalidCommand(s, "Bad Keyword");
    }


    /////////////////////////// receive methods //////////////////////////////
    // these methods parse received messages

    private void ReceiveConnected(string[] tokens, string s)
    {
        if (tokens.Length != 2)
        {
            int cellCount;
            bool result = Int32.TryParse(tokens[1], out cellCount);
            if (result)
                ConnectionSuccess(cellCount);
            else
                InvalidCommand(s, "Cannot parse cell count.");

        }
        else
            InvalidCommand(s, "Incorrect number of tokens.");
    }

    private void ReceiveCell(string[] tokens, string s)
    {
        if (tokens.Length >= 3)
        {
            string cellName = tokens[1];
            string cellContents = tokens[2];
            for (int i = 3; i < tokens.Length; i++)
            {
                cellContents += tokens[i];
            }

            UpdateCell(cellName, cellContents);
        }
        else
            InvalidCommand(s, "Incorrect number of tokens.");
    }

    private void ReceiveError(string[] tokens, string s)
    {
        if (tokens.Length >= 3)
        {
            int errorNumber;
            bool result = Int32.TryParse(tokens[1], out errorNumber);
            if (result)
                ConnectionSuccess(errorNumber);
            else
                InvalidCommand(s, "Cannot parse Error Number.");

            string errorMessage = tokens[2];
            for (int i = 3; i < tokens.Length; i++)
            {
                errorMessage += " " + tokens[i];
            }
            ErrorCall(errorNumber, errorMessage);
        }
        else
            InvalidCommand(s, "Incorrect number of tokens.");
    }


    /////////////////////////// Do methods //////////////////////////////
    // these methods handle actually responding to a recieve. To be 
    // implemented once we know what to actually do.

    private void ConnectionSuccess(int cellCount)
    {
        conFunc(cellCount);
    }
    private void UpdateCell(string cellName, string cellContents)
    {
        cellFunc(cellName, cellContents);
    }

    private void ErrorCall(int errorNumber, string errorMessage)
    {
        errorFunc(errorNumber, errorMessage);
    }

    private void InvalidCommand(string s, string p)
    {
        invalidFunc(s, p);
    }

    /////////////////////////// send methods //////////////////////////////
    // thse methods format and send messages from input data.

    public void SendCell(string cellName, string contents)
    {
        // initialize message
        string message = "";
        // build message
        message += "cell"; // key word
        message += " ";
        message += cellName;
        message += " ";
        message += contents;


        // send message
        handler.Send(message);
    }

    public void SendConnect(string clientName, string spreadsheetName)
    {
        // initialize message
        string message = "";
        // build message
        message += "connect"; // key word
        message += " ";
        message += clientName;
        message += " ";
        message += spreadsheetName;


        // send message
        handler.Send(message);
    }

    public void SendRegisterUser(string userName)
    {
        // initialize message
        string message = "";
        // build message
        message += "register"; // key word
        message += " ";
        message += userName;


        // send message
        handler.Send(message);
    }

    public void SendUndo()
    {
        // initialize message
        string message = "";
        // build message
        message += "undo";


        // send message
        handler.Send(message);
    }

}


/// <summary>
/// The Socket Handler Class abstracts away all networking concerns, simply allowing the 
/// input of a message to be sent and outputting the messages received. Appropriate for
/// simple networked programs. All console writes will be edited out in the final version
/// and it will be converted to a library.
/// </summary>
public class SocketHandler
{
    /// <summary>
    /// An object that will parse received messages. To be replaced by an event 
    /// system in the final version that the object will hook into instead.
    /// </summary>
    private CommandProcessor parser;

    /// <summary>
    /// the underlying socket used for communication
    /// </summary>
    private Socket socket;

    /// <summary>
    ///  the byte array for incoming data
    /// </summary>
    private byte[] buffer = new byte[256];

    /// <summary>
    ///  This string is where all incoming messages are dumped. It is cut into individual messages as needed.
    /// </summary>
    private String incomingData = "";

    /// <summary>
    /// Creates the handler. This constructor may throw an unhandled exception, calling code should handle it.
    /// When creating the object, either an IP address or a host name must be provided. If both are provided, 
    /// the host name will be ignored. If neither are provided an exception will be thrown.
    /// </summary>
    /// <param name="host"></param>
    /// <param name="ip">IP address in string form, such as 192.168.1.10</param>
    /// <param name="port">Port to connect though.</param>
    public SocketHandler(string host, string ip, int port, CommandProcessor _parser)
    {
        parser = _parser;
        // first we need to know where we are connecting from
        IPHostEntry hostEntry = null;
        IPAddress address = null;

        if (IPAddress.TryParse(ip, out address)) { } // if IP address is provied, parse it
        else if (host != null) // if a host name is provided, get an IP address from it
        {
            hostEntry = Dns.Resolve(host); // this function is obsolete, but works. This might need to be changed for the final version
            address = hostEntry.AddressList[0]; // ge the actual IP address
        }
        else
            throw new Exception("No IP or host name.");

        // create tcp/ip socket.
        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        // Connect using the found address and port
        socket.BeginConnect(address, port, new AsyncCallback(ConnectCallback), socket);
    }

    /// <summary>
    /// the connect attempt callback function
    /// </summary>
    /// <param name="result"></param>
    private void ConnectCallback(IAsyncResult result)
    {
        try
        {
            // end connect
            socket.EndConnect(result);
            Console.WriteLine("connected");
            // Start listening for recieves from the server
            Receive();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    /// <summary>
    /// Tells the handler to begin receiving data.
    /// </summary>
    private void Receive()
    {
        try
        {
            // start listening for data on the socket
            socket.BeginReceive(buffer, 0, 256, 0, new AsyncCallback(ReceiveCallback), null);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    /// <summary>
    /// The recieve callback. Converts incoming data to a string and checks for completed messages
    /// </summary>
    /// <param name="result"></param>
    private void ReceiveCallback(IAsyncResult result)
    {
        try
        {
            // Read data
            int bytes = socket.EndReceive(result);

            // append data to already existing data
            if (bytes > 0)
                incomingData += Encoding.ASCII.GetString(buffer, 0, bytes);

            // split off and handle any completed messages.
            while (incomingData.Contains("\n"))
                SplitIncomingData();

            // listen for more data
            socket.BeginReceive(buffer, 0, 256, 0, new AsyncCallback(ReceiveCallback), null);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    /// <summary>
    /// breaks off a single message from the incoming data and pass it to the message parser
    /// </summary>
    private void SplitIncomingData()
    {
        int index = incomingData.IndexOf('\n');
        string message = incomingData.Substring(0, index);
        parser.ProcessServerCommand(message);
        incomingData = incomingData.Substring(index + 1);
    }


    /// <summary>
    /// sends a string over the socket.
    /// </summary>
    /// <param name="message">The message to send</param>
    public void Send(String message)
    {
        // ensures a call always ends with a new line, as per spec
        message += "\n";

        // get bytes from string
        byte[] bytes = Encoding.ASCII.GetBytes(message);

        // Begin sending the data
        socket.BeginSend(bytes, 0, bytes.Length, 0, new AsyncCallback(SendCallback), null);
    }

    /// <summary>
    /// Callback after a send
    /// </summary>
    /// <param name="result">The result data from the send</param>
    private void SendCallback(IAsyncResult result)
    {
        try
        {
            // stop the send
            int bytesSent = socket.EndSend(result);
            // print to console 
            Console.WriteLine("Send success. {0} bytes sent", bytesSent);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public void Close()
    {
        socket.Close();
    }

    /*
    /// <summary>
    /// A simple main to test the Socket Handler.
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    public static int Main(String[] args)
    {
        // create the instances
        CommandProcessor A = new CommandProcessor();
        SocketHandler B = new SocketHandler("Striker", null, 2000);

        // associate instances
        B.parser = A;
        A.handler = B;

        // testing loop
        while (true)
        {
            Thread.Sleep(1000); // The console will not write while it is waiting for a read, this allows time to write
            Console.WriteLine("Enter Command");
            string s = Console.ReadLine();
            B.Send(s);
        }
        return 0;
    }
    */
}
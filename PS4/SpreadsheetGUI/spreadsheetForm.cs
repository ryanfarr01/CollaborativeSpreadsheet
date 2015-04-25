/// Brent Bagley

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using SS;
using SpreadsheetUtilities;
using System.Text.RegularExpressions;
using System.IO;

namespace SpreadsheetGUI
{
    /// <summary>
    /// The user facing control for a spreadsheet.
    /// </summary>
    public partial class spreadsheetForm : Form
    {

        delegate void SetTextCallback(string text);

        
        private bool waitingForConnected;
        private string userName = "sysadmin";
        private string spreadsheetName = "any";
        private string IPAddress = "";
        private string hostName = "striker";
        private string port = "2115";
        /// <summary>
        /// The underlying spreadsheet data.
        /// </summary>
        private spreadsheet baseSpreadsheet;
        /// <summary>
        /// If the file has been saved or loaded.
        /// </summary>
        private Boolean saved = false;
        /// <summary>
        /// The name of this file if it was loaded or has been saved.
        /// </summary>
        private string fileName = "";
        /// <summary>
        /// keeps track of changes made so they may be undone.
        /// </summary>
        private Stack<undoRedo> undoStack = new Stack<undoRedo>();
        /// <summary>
        /// keeps track of changes undone so they may be redone.
        /// </summary>
        private Stack<undoRedo> redoStack = new Stack<undoRedo>();

        private CommandProcessor parser;
        private SocketHandler socket;
        private Boolean connected = false;


        /// <summary>
        /// spreadsheet form constructor.
        /// </summary>
        public spreadsheetForm()
        {
            InitializeComponent();
            baseSpreadsheet = new spreadsheet(isValid,s=>s.ToUpper(),"ps6");

            spreadsheetPanel1.SelectionChanged += displaySelection;
            spreadsheetPanel1.SetSelection(0, 0);
            parser = new CommandProcessor(ConnectionSuccess, CellChange, Error, InvalidCommand, CrashFunction);
        }

        /// <summary>
        /// spreadsheet form constructor.
        /// </summary>
        public spreadsheetForm(string _userName, string _spreadsheetName, string _IPAddres, string _hostName, string _port)
        {
            InitializeComponent();
            baseSpreadsheet = new spreadsheet(isValid, s => s.ToUpper(), "ps6");
            userName = _userName;
            spreadsheetName = _spreadsheetName;
            IPAddress = _IPAddres;
            hostName = _hostName;
            port = _port;

            spreadsheetPanel1.SelectionChanged += displaySelection;
            spreadsheetPanel1.SetSelection(0, 0);
            parser = new CommandProcessor(ConnectionSuccess, CellChange, Error, InvalidCommand, CrashFunction);
        }


        /// <summary>
        /// updates the cell name, cell value, and cell contents when a new selection is made.
        /// </summary>
        /// <param name="ss">The spreadsheet panel.</param>
        private void displaySelection(SpreadsheetPanel ss)
        {
            int row, col;
            String value;
            String cell;
            ss.GetSelection(out col, out row);
            ss.GetValue(col, row, out value);
            cell = "" + (char)(col + 65) + (row+1);
            
            setFormContents(cell);
        }

        /// <summary>
        /// updates the form elements to reflect the named cell;
        /// </summary>
        /// <param name="cell">name of selected cell</param>
        private void setFormContents(string cell)
        {

            if (toolStrip1.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(setFormContents);
                toolStrip1.Invoke(d, new object[] { cell });
            }
            else{
                if (baseSpreadsheet.GetCellContents(cell) is Formula)
                    cellContentsField.Text = "=" + baseSpreadsheet.GetCellContents(cell).ToString();
                else
                    cellContentsField.Text = baseSpreadsheet.GetCellContents(cell).ToString();

                if (baseSpreadsheet.GetCellValue(cell) is FormulaError)
                    cellValueField.Text = "Invalid Formula";
                else
                    cellValueField.Text = "Cell Value: " + baseSpreadsheet.GetCellValue(cell).ToString();

                cellNameField.Text = "Cell Selected: " + cell;
            }
        }


        /// <summary>
        /// Closes the spreadsheet.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (cancelDialog("This action will result is data loss. Do you wish to continue with this action?","Data Loss Warning"))
                Close();
        }

        /// <summary>
        /// Displays a message to the user and requests a boolean response. Used for data loss warning messages.
        /// </summary>
        /// <param name="p1">The Message to show to the user</param>
        /// <param name="p2">The dialog box title</param>
        /// <returns></returns>
        private bool cancelDialog(string p1, string p2)
        {
            DialogResult result = MessageBox.Show(p1, p2, MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
                return true;
            else
                return false;
        }

        /// <summary>
        /// Opens a new spreadsheet.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Tell the application context to run the form on the same
            // thread as the other forms.
            SpreadsheetApplicationContext.getAppContext().RunForm(new spreadsheetForm(userName,spreadsheetName,IPAddress,hostName,port));
        }

        /// <summary>
        /// Event of pressing return while application focus is on the cell content text box. Saves the inputted contents to the cell.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cellContentsField_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
            {
                // get cell name
                int row, col;
                String cell;
                spreadsheetPanel1.GetSelection(out col, out row);
                cell = "" + (char)(col + 65) + (row + 1);

                sendMessage("cell " + cell + " " + cellContentsField.Text);

                // set focus to the spreadsheet panel
                spreadsheetPanel1.Focus();
            }            
        }

        /// <summary>
        /// Set the named cell contents
        /// </summary>
        /// <param name="cell">Name of cell</param>
        /// <param name="contents">Contents to set</param>
        /// <returns>the previous contents of the cell</returns>
        private string setCellContent(string cell, string contents)
        {
                string previousValue;

                if (baseSpreadsheet.GetCellContents(cell) is Formula)
                    previousValue = "=" + baseSpreadsheet.GetCellContents(cell).ToString();
                else
                    previousValue = baseSpreadsheet.GetCellContents(cell).ToString();
                
                updateCellValues(baseSpreadsheet.SetContentsOfCell(cell, contents));

                return previousValue;
        }

        /// <summary>
        /// update the displayed value of all cells in the set.
        /// </summary>
        /// <param name="set">Set of cells to update</param>
        private void updateCellValues(IEnumerable<string> set)
        {
            int row, col;
            string value;
            foreach (string cell in set)
            {
                col = getCol(cell);
                row = getRow(cell);
                if (baseSpreadsheet.GetCellValue(cell) is FormulaError)
                    value = "Formula Error";
                else
                    value = baseSpreadsheet.GetCellValue(cell).ToString();

                spreadsheetPanel1.SetValue(col, row, value);
            }
        }

        /// <summary>
        /// gets the column of a cell from the cell name.
        /// </summary>
        /// <param name="cell">the cell name</param>
        /// <returns>the column of the cell</returns>
        private int getCol(string cell)
        {
            return (int)cell[0] - 65;
        }

        /// <summary>
        /// gets the row of a cell from the cell name.
        /// </summary>
        /// <param name="cell">the cell name</param>
        /// <returns>the row of the cell</returns>
        private int getRow(string cell)
        {
            return Int32.Parse(cell.Substring(1, cell.Length - 1))-1;
        }

        /// <summary>
        /// the validation deligate given to the spreadsheet storage object.
        /// </summary>
        /// <param name="s">string to check</param>
        /// <returns>true if valid, false if not</returns>
        private Boolean isValid(string s)
        {
            Regex nameChecker = new Regex("[a-zA-Z][1-9][0-9]?", 0); // meets basic rules.
            return nameChecker.IsMatch(s);
        }

        /// <summary>
        /// Opens a spreadsheet
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {

            SpreadsheetApplicationContext.getAppContext().RunForm(new ConnectionForm(userName,spreadsheetName,IPAddress,hostName,port,AttemptConnection));
            /*
            // establish if the user wants to open file in case of potential data loss.
            Boolean check = !baseSpreadsheet.Changed;
            if (!check) {
                if (cancelDialog("This action will result is data loss. Do you wish to continue with this action?", "Data Loss Warning"))
                    check = true;
            }

            // open file sequence.
            if(check){
                OpenFileDialog openDialog = new OpenFileDialog();
                openDialog.Filter = "sprd files (*.sprd)|*.sprd|All files (*.*)|*.*";
                openDialog.FilterIndex = 1;
                openDialog.RestoreDirectory = true;

                if (openDialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        // set the baseSpreadsheet as the opened spreadsheet
                        baseSpreadsheet = new spreadsheet(openDialog.FileName, isValid, s => s.ToUpper(), "ps6");
                        saved = true;
                        fileName = openDialog.FileName;
                        
                        // update all cells with new values.
                        int row, col;
                        String cell;
                        spreadsheetPanel1.GetSelection(out col, out row);
                        spreadsheetPanel1.Clear();
                        updateCellValues(baseSpreadsheet.GetNamesOfAllNonemptyCells());
                        cell = "" + (char)(col + 65) + (row + 1);
                        setFormContents(cell);

                    }
                    catch(Exception){
                        MessageBox.Show("Unable to load spreadsheet");
                    }
                }
            }
            */
        }

        /// <summary>
        /// Saves the spreadsheet.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.FileName = fileName;
            saveDialog.Filter = "sprd files (*.sprd)|*.sprd|All files (*.*)|*.*";
            saveDialog.FilterIndex = 1;
            saveDialog.RestoreDirectory = true;

            if (saveDialog.ShowDialog() == DialogResult.OK)
            {
                // auto append .sprd
                if (saveDialog.FilterIndex == 1)
                    if (!saveDialog.FileName.EndsWith(".sprd"))
                        saveDialog.FileName += ".sprd";

                // save dialog to prevent unintentional overwrite.
                if (saveDialog.FileName != fileName)
                {
                    if (File.Exists(saveDialog.FileName))
                    {
                        if (cancelDialog("This action will overwrite a file. Do you wish to continue with this action?", "Overwrite Warning"))
                            saveFile(saveDialog.FileName);
                    }
                    else
                        saveFile(saveDialog.FileName);
                }
                else
                    saveFile(saveDialog.FileName);

            }
        }

        /// <summary>
        /// helper method to save the spreadsheet.
        /// </summary>
        /// <param name="saveFileName">The file name of the saved file</param>
        private void saveFile(string saveFileName)
        {
            baseSpreadsheet.Save(saveFileName);
            saved = true;
            fileName = saveFileName;
        }

        /// <summary>
        /// Key press events for when the focus is on the spreadsheet panel. Undo, redo, jump to edit contents.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void spreadsheetPanel1_KeyDown(object sender, KeyEventArgs e)
        {
            // this will catch circular exceptions caused by undo or redo
            try
            {
                // undo
                if (e.KeyCode == Keys.Z && ModifierKeys.HasFlag(Keys.Control))
                {
                    sendMessage("undo");
                }
                else if (char.IsLetterOrDigit((char)e.KeyCode))
                {
                    // jump to editing the content field
                    cellContentsField.Focus();
                    if (!ModifierKeys.HasFlag(Keys.Shift))
                        SendKeys.Send(e.KeyCode.ToString().ToLower());
                    else
                        SendKeys.Send(e.KeyCode.ToString());
                }

            }
            catch (CircularException)
            {
                MessageBox.Show("Invalid Input: Circular Dependency");
            }

        }

        /// <summary>
        /// A data storage class for storing changes to the spreadsheet.
        /// </summary>
        private class undoRedo
        {
            /// <summary>
            /// contents pre change
            /// </summary>
            public string contents{ get; private set;}
            /// <summary>
            /// name of changed cell
            /// </summary>
            public string cellName { get; private set;}

            /// <summary>
            /// constructor
            /// </summary>
            /// <param name="name">name of cell</param>
            /// <param name="content">contents pre change</param>
            public undoRedo(string name, string content)
            {
                cellName = name;
                contents = content;
            }
        }

        private void editCellContentsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("You can edit the contents of the currently selected cell by changing the string inside the text box and then pressing return. Note: After selecting a cell, pressing any letter or digit will begin editing the cell contents automatically.");
        }

        private void undoRedoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("You may undo changes made to the spreadsheet by pressing CTRL+Z.");
        }

        private void selectNewCellToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("You may select a new cell by clicking on the cell you wish to select");
        }

        private void TestMessageBox(object sender, EventArgs e)
        {
            sendMessage(Microsoft.VisualBasic.Interaction.InputBox("Type test message to send", "Test Message", ""));
        }

        private void testReceiveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            testRecieve(Microsoft.VisualBasic.Interaction.InputBox("Type test message to receive", "Test Receive", ""));
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // new stuff!
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        public void AttemptConnection(string _userName, string _spreadsheetName, string _IPAddres, string _hostName, string _port)
        {
            userName = _userName;
            spreadsheetName = _spreadsheetName;
            IPAddress = _IPAddres;
            hostName = _hostName;
            port = _port;
            int portNum;

            if (socket != null)
            {
                socket = null;
            }

            if(!Int32.TryParse(port,out portNum))
            {
                MessageBox.Show("Invalid Port");
                return;
            }

            try
            {
                socket = new SocketHandler(hostName, IPAddress, portNum, parser);
                waitingForConnected = true;
                sendConnect("connect " + userName + " " + spreadsheetName);
            }
            catch (Exception e)
            {
                MessageBox.Show("Connection Failed \n" + e.Message);
            }
        }


        private void ConnectionSuccess(int cellCount)
        {
            connected = true;
            MessageBox.Show("Connection Successful");
        }

        private void CellChange(string cellName, string cellContents)
        {
            setCellContent(cellName, cellContents);
            setFormContents(cellName);
        }

        private void Error(int errorNumber, string errorMessage)
        {
            switch (errorNumber) {
                default: MessageBox.Show(errorMessage); break;
                case 1: MessageBox.Show("Bad cell change:\n" + errorMessage); break;
                case 2: MessageBox.Show("Invalid Command:\n" + errorMessage); break;
                case 3: MessageBox.Show("Unable to perform command in current state:\n" + errorMessage); break;
                case 4: 
                    MessageBox.Show("Invalid username:\n" + errorMessage);
                    if (waitingForConnected == true)
                    {
                        MessageBox.Show("Invalid username, connection failed:\n" + errorMessage);
                        CloseSocket();
                    }
                    else
                        MessageBox.Show("Invalid username:\n" + errorMessage); 
                break;
            }
            
        }

        private void InvalidCommand(string badCommand, string explaination)
        {
            MessageBox.Show(explaination);
        }

        private void testRecieve(string s)
        {
            parser.ProcessServerCommand(s);
        }

        private void sendMessage(string s)
        {
            if (connected)
            {
                socket.Send(s);
            }
            else
            {
                MessageBox.Show("A connection to a spreadsheet must be made to perform this action.");
            }
        }

        private void sendConnect(string s)
        {
            socket.Send(s);
        }

        private void registerUserToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string register = "register ";
            register += Microsoft.VisualBasic.Interaction.InputBox("Name of user to register", "Register User", "");
            if (register != "register ")
                sendMessage(register);
        }




        private void CrashFunction()
        {
            MessageBox.Show("Socket closed unexpectedly");
            CloseSocket();
        }

        private void CloseSocket()
        {
            socket.Close();
            connected = false;
        }


    }
}

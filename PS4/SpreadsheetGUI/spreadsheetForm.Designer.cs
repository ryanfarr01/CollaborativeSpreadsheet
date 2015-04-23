namespace SpreadsheetGUI
{
    partial class spreadsheetForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectNewCellToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editCellContentsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.undoRedoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.testMessageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.testReceiveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.cellNameField = new System.Windows.Forms.ToolStripLabel();
            this.cellValueField = new System.Windows.Forms.ToolStripLabel();
            this.cellContentsField = new System.Windows.Forms.ToolStripTextBox();
            this.spreadsheetPanel1 = new SS.SpreadsheetPanel();
            this.registerUserToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(984, 24);
            this.menuStrip1.TabIndex = 2;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.registerUserToolStripMenuItem,
            this.closeToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(77, 20);
            this.fileToolStripMenuItem.Text = "Operations";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.newToolStripMenuItem.Text = "New Window";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "Connect";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.closeToolStripMenuItem.Text = "Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectNewCellToolStripMenuItem,
            this.editCellContentsToolStripMenuItem,
            this.undoRedoToolStripMenuItem,
            this.testMessageToolStripMenuItem,
            this.testReceiveToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // selectNewCellToolStripMenuItem
            // 
            this.selectNewCellToolStripMenuItem.Name = "selectNewCellToolStripMenuItem";
            this.selectNewCellToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.selectNewCellToolStripMenuItem.Text = "Select New Cell";
            this.selectNewCellToolStripMenuItem.Click += new System.EventHandler(this.selectNewCellToolStripMenuItem_Click);
            // 
            // editCellContentsToolStripMenuItem
            // 
            this.editCellContentsToolStripMenuItem.Name = "editCellContentsToolStripMenuItem";
            this.editCellContentsToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.editCellContentsToolStripMenuItem.Text = "Edit Cell Contents";
            this.editCellContentsToolStripMenuItem.Click += new System.EventHandler(this.editCellContentsToolStripMenuItem_Click);
            // 
            // undoRedoToolStripMenuItem
            // 
            this.undoRedoToolStripMenuItem.Name = "undoRedoToolStripMenuItem";
            this.undoRedoToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.undoRedoToolStripMenuItem.Text = "Undo";
            this.undoRedoToolStripMenuItem.Click += new System.EventHandler(this.undoRedoToolStripMenuItem_Click);
            // 
            // testMessageToolStripMenuItem
            // 
            this.testMessageToolStripMenuItem.Name = "testMessageToolStripMenuItem";
            this.testMessageToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.testMessageToolStripMenuItem.Text = "Test message";
            this.testMessageToolStripMenuItem.Click += new System.EventHandler(this.TestMessageBox);
            // 
            // testReceiveToolStripMenuItem
            // 
            this.testReceiveToolStripMenuItem.Name = "testReceiveToolStripMenuItem";
            this.testReceiveToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.testReceiveToolStripMenuItem.Text = "Test receive";
            this.testReceiveToolStripMenuItem.Click += new System.EventHandler(this.testReceiveToolStripMenuItem_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.BackColor = System.Drawing.SystemColors.ControlLight;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cellNameField,
            this.cellValueField,
            this.cellContentsField});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Padding = new System.Windows.Forms.Padding(3);
            this.toolStrip1.Size = new System.Drawing.Size(984, 31);
            this.toolStrip1.TabIndex = 4;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // cellNameField
            // 
            this.cellNameField.AutoSize = false;
            this.cellNameField.Name = "cellNameField";
            this.cellNameField.Size = new System.Drawing.Size(200, 22);
            this.cellNameField.Text = "Cell Selected: A1";
            // 
            // cellValueField
            // 
            this.cellValueField.AutoSize = false;
            this.cellValueField.Name = "cellValueField";
            this.cellValueField.Size = new System.Drawing.Size(200, 22);
            this.cellValueField.Text = "Cell Value:";
            // 
            // cellContentsField
            // 
            this.cellContentsField.Name = "cellContentsField";
            this.cellContentsField.Size = new System.Drawing.Size(300, 25);
            this.cellContentsField.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cellContentsField_KeyDown);
            // 
            // spreadsheetPanel1
            // 
            this.spreadsheetPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.spreadsheetPanel1.Location = new System.Drawing.Point(0, 58);
            this.spreadsheetPanel1.Name = "spreadsheetPanel1";
            this.spreadsheetPanel1.Size = new System.Drawing.Size(984, 494);
            this.spreadsheetPanel1.TabIndex = 0;
            this.spreadsheetPanel1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.spreadsheetPanel1_KeyDown);
            // 
            // registerUserToolStripMenuItem
            // 
            this.registerUserToolStripMenuItem.Name = "registerUserToolStripMenuItem";
            this.registerUserToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.registerUserToolStripMenuItem.Text = "Register User";
            this.registerUserToolStripMenuItem.Click += new System.EventHandler(this.registerUserToolStripMenuItem_Click);
            // 
            // spreadsheetForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(984, 550);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.spreadsheetPanel1);
            this.Name = "spreadsheetForm";
            this.Text = "Spreadsheet";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private SS.SpreadsheetPanel spreadsheetPanel1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripTextBox cellContentsField;
        private System.Windows.Forms.ToolStripLabel cellNameField;
        private System.Windows.Forms.ToolStripLabel cellValueField;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editCellContentsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem undoRedoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem selectNewCellToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem testMessageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem testReceiveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem registerUserToolStripMenuItem;
    }
}


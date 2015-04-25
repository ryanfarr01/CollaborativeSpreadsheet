namespace SpreadsheetGUI
{
    partial class ConnectionForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ConnectionForm));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.UserNameText = new System.Windows.Forms.TextBox();
            this.SpreadsheetText = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.IPAddressText = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.HostNameText = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.PortText = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // UserNameText
            // 
            resources.ApplyResources(this.UserNameText, "UserNameText");
            this.UserNameText.Name = "UserNameText";
            // 
            // SpreadsheetText
            // 
            resources.ApplyResources(this.SpreadsheetText, "SpreadsheetText");
            this.SpreadsheetText.Name = "SpreadsheetText";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // IPAddressText
            // 
            resources.ApplyResources(this.IPAddressText, "IPAddressText");
            this.IPAddressText.Name = "IPAddressText";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // HostNameText
            // 
            resources.ApplyResources(this.HostNameText, "HostNameText");
            this.HostNameText.Name = "HostNameText";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // PortText
            // 
            resources.ApplyResources(this.PortText, "PortText");
            this.PortText.Name = "PortText";
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // button1
            // 
            resources.ApplyResources(this.button1, "button1");
            this.button1.Name = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // ConnectionForm
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.button1);
            this.Controls.Add(this.PortText);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.HostNameText);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.IPAddressText);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.SpreadsheetText);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.UserNameText);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "ConnectionForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox UserNameText;
        private System.Windows.Forms.TextBox SpreadsheetText;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox IPAddressText;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox HostNameText;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox PortText;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button button1;
    }
}


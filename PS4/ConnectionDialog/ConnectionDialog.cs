using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ConnectionDialog
{
    public partial class ConnectionForm : Form
    {
        public delegate void ConnectionAttempt(string u, string s, string i, string h, string p);

        private ConnectionAttempt connector;

        public ConnectionForm(string un, string ss, string ip, string hn, string pt, ConnectionAttempt con)
        {
            InitializeComponent();
            UserNameText.Text = un;
            SpreadsheetText.Text = ss;
            IPAddressText.Text = ip;
            HostNameText.Text = hn;
            PortText.Text = pt;
            
            connector = con;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            connector(UserNameText.Text, SpreadsheetText.Text, IPAddressText.Text, HostNameText.Text, PortText.Text);

            Close();
        }

    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SampleApiDllCSharpWinForm.X64_Only
{
    public partial class frmInput : Form
    {
        public frmInput()
        {
            InitializeComponent();
        }
        public delegate void TextEventHandler(string strText);

        public TextEventHandler TextHandler;

        public static DialogResult Show(out string strText)
        {
            string strTemp = string.Empty;

            frmInput inputDialog = new frmInput();
            inputDialog.TextHandler = (str) => { strTemp = str; };

            DialogResult result = inputDialog.ShowDialog();
            strText = strTemp;

            return result;
        }

        private void button_ok_Click(object sender, EventArgs e)
        {
            if (null != TextHandler)
            {
                TextHandler.Invoke(txtString.Text);
                DialogResult = DialogResult.OK;
            }
        }

        private void button_cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}

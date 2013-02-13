using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace TestingGrounds
{
    public partial class MainWindow : Form
    {
        Process activeProcess;
        int simulationIndex = 0;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void buttonGo_Click(object sender, EventArgs e)
        {
            if (!timerGo.Enabled)
            {
                timerGo.Enabled = true;
                console.Items.Add("Started process spawning");
            }
        }

        private void timerGo_Tick(object sender, EventArgs e)
        {
            if(activeProcess == null)
            {
                using (StreamWriter parameterFile = new System.IO.StreamWriter("TournamentParameters.txt"))
                {
                    parameterFile.WriteLine("random");
                    parameterFile.WriteLine("random");
                    parameterFile.WriteLine("generations=32");
                    parameterFile.WriteLine("chambers=1");
                    parameterFile.Close();
                }

                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.CreateNoWindow = false;
                startInfo.RedirectStandardOutput = false;
                startInfo.RedirectStandardInput = false;
                startInfo.WindowStyle = ProcessWindowStyle.Minimized;

                startInfo.UseShellExecute = true;
                startInfo.FileName = "TestingGroundsInternal.exe";

                activeProcess = new Process();
                activeProcess.StartInfo = startInfo;
                activeProcess.Start();

                simulationIndex++;
                console.Items.Add("simulation " + simulationIndex + " launched");
            }

            if (activeProcess.WaitForExit(100))
            {
                console.Items.Add("simulation " + simulationIndex + " finished");
                activeProcess = null;
            }
        }
    }
}

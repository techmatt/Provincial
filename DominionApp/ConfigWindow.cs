using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace BaseCodeApp
{
    public partial class ConfigWindow : Form
    {
        public MainWindow parentWindow;
        public string visualizationFilename = "";
        public string activeTournamentDirectory = "";
        public int activeTournamentGenerationCount = -1;

        public ConfigWindow(MainWindow _parentWindow)
        {
            InitializeComponent();
            parentWindow = _parentWindow;

            ResetPlayerLists();
            UpdateBuildLists();

            textBoxRequiredCards.Focus();
        }

        private void buttonNewKingdomCards_Click(object sender, EventArgs e)
        {
            string gameOptions = textBoxRequiredCards.Text.ToLower() + " |";

            if (radioButton25Split.Checked) gameOptions += "25Split|";
            else if (radioButton34Split.Checked) gameOptions += "34Split|";
            else gameOptions += "34Split|";

            if (checkBoxProsperity.Checked) gameOptions += "Prosperity|";
            else gameOptions += "NoProsperity|";
            
            parentWindow.NewKingdomCards(gameOptions);

            ResetPlayerLists();
            UpdateBuildLists();

            comboPlayer1.SelectedIndex = 0;
        }

        private void ResetPlayerLists()
        {
            for(int playerIndex = 0; playerIndex < 2; playerIndex++)
            {
                ComboBox combo = comboPlayer1;
                if (playerIndex == 1) combo = comboPlayer2;

                combo.Items.Clear();
                combo.Items.Add("Human");
                combo.Items.Add("Custom AI");
                combo.SelectedIndex = 1;
            }
        }

        public void UpdateBuildLists()
        {
            this.SuspendLayout();
            for (int playerIndex = 0; playerIndex < 2; playerIndex++)
            {
                Panel p = panelPlayer1;
                if (playerIndex == 1) p = panelPlayer2;

                p.SuspendLayout();

                p.Controls.Clear();

                int y = 0;
                for (int buildIndex = 0; buildIndex < 13; buildIndex++)
                {
                    BuildComboBox cardCombo = new BuildComboBox(buildIndex);
                    foreach(SupplyEntry s in parentWindow.supply)
                    {
                        cardCombo.Items.Add(s.c);
                    }

                    cardCombo.Left = 0;
                    cardCombo.Top = y;
                    cardCombo.Width = p.Width - 60;
                    cardCombo.DropDownStyle = ComboBoxStyle.DropDownList;
                    cardCombo.Font = p.Font;
                    cardCombo.Height = 21;
                    cardCombo.SelectedIndexChanged += cardCombo_SelectedIndexChanged;

                    p.Controls.Add(cardCombo);

                    QuantityTextBox quantityText = new QuantityTextBox(buildIndex);
                    quantityText.Left = p.Width - 50;
                    quantityText.Top = y;
                    quantityText.Width = 50;
                    quantityText.Text = "0";
                    quantityText.Font = p.Font;
                    quantityText.Height = 21;

                    quantityText.TextChanged += quantityText_TextChanged;

                    p.Controls.Add(quantityText);

                    y += 31;
                }

                p.ResumeLayout();
            }

            Player1Reset();
            Player2Reset();
            this.ResumeLayout();
        }

        void cardCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            BuildComboBox b = sender as BuildComboBox;
            if (b != null)
            {
                //if (b.Parent == panelPlayer1) comboPlayer1.SelectedIndex = 1;
                //if (b.Parent == panelPlayer2) comboPlayer2.SelectedIndex = 1;
            }
        }

        void quantityText_TextChanged(object sender, EventArgs e)
        {
            QuantityTextBox b = sender as QuantityTextBox;
            if (b != null)
            {
                int x;
                try
                {
                    x = Convert.ToInt32(b.Text);
                }
                catch (Exception)
                {
                    b.ForeColor = Color.Red;
                    return;
                }
                if (x < 0 || x > 99) b.ForeColor = Color.Red;
                else b.ForeColor = Color.Black;
            }
        }

        void SetBuildRow(Panel p, int index, String card, int quantity)
        {
            foreach(Control c in p.Controls)
            {
                BuildComboBox b = c as BuildComboBox;
                if (b != null && b.index == index)
                {
                    int supplyIndex = parentWindow.supply.FindIndex(delegate(SupplyEntry s) { return s.c.name == card; });
                    if (supplyIndex == -1) supplyIndex = 0;
                    b.SelectedIndex = supplyIndex;
                }
                QuantityTextBox t = c as QuantityTextBox;
                if (t != null && t.index == index)
                {
                    t.Text = quantity.ToString();
                }
            }
        }

        private String GetBuildRow(Panel p, int index)
        {
            String cardName = "colony";
            int count = 0;
            foreach (Control c in p.Controls)
            {
                BuildComboBox b = c as BuildComboBox;
                if (b != null && b.index == index)
                {
                    cardName = parentWindow.supply[b.SelectedIndex].c.name;
                }
                QuantityTextBox t = c as QuantityTextBox;
                if (t != null && t.index == index)
                {
                    try
                    {
                        count = Convert.ToInt32(t.Text);
                    }
                    catch (Exception)
                    {
                        
                    }
                    
                }
            }
            return cardName + "@" + count.ToString();
        }

        private String PanelToShorthandEncoding(Panel p)
        {
            String result = "";
            for (int i = 0; i < 13; i++)
            {
                result += GetBuildRow(p, i);
                if (i != 12) result += "|";
            }
            return result;
        }

        void ResetPanelToBigMoney(Panel p)
        {
            int supplyIndex = parentWindow.supply.FindIndex(delegate(SupplyEntry s) { return s.c.name == "platinum"; });
            if (supplyIndex == -1)
            {
                SetBuildRow(p, 0, "gold", 0);
                SetBuildRow(p, 1, "gold", 0);
            }
            else
            {
                SetBuildRow(p, 0, "platinum", 0);
                SetBuildRow(p, 1, "platinum", 99);
            }
            SetBuildRow(p,  2, "gold", 0);
            SetBuildRow(p,  3, "gold", 0);
            SetBuildRow(p,  4, "gold", 0);
            SetBuildRow(p,  5, "gold", 99);
            SetBuildRow(p,  6, "silver", 0);
            SetBuildRow(p,  7, "silver", 0);
            SetBuildRow(p,  8, "silver", 0);
            SetBuildRow(p,  9, "silver", 0);
            SetBuildRow(p, 10, "silver", 99);
            SetBuildRow(p, 11, "copper", 0);
            SetBuildRow(p, 12, "copper", 0);
        }

        void Player1Reset()
        {
            ResetPanelToBigMoney(panelPlayer1);
            comboPlayer1.SelectedIndex = 1;
            int supplyIndex = parentWindow.supply.FindIndex(delegate(SupplyEntry s) { return s.c.name == "platinum"; });
            if (supplyIndex == -1)
            {
                textBoxPlayer1Estate.Text = "2";
                textBoxPlayer1Duchy.Text = "4";
                textBoxPlayer1Province.Text = "8";
            }
            else
            {
                textBoxPlayer1Estate.Text = "1";
                textBoxPlayer1Duchy.Text = "2";
                textBoxPlayer1Province.Text = "4";
            }
        }

        void Player2Reset()
        {
            ResetPanelToBigMoney(panelPlayer2);
            comboPlayer2.SelectedIndex = 1;
            int supplyIndex = parentWindow.supply.FindIndex(delegate(SupplyEntry s) { return s.c.name == "platinum"; });
            if (supplyIndex == -1)
            {
                textBoxPlayer2Estate.Text = "2";
                textBoxPlayer2Duchy.Text = "4";
                textBoxPlayer2Province.Text = "8";
            }
            else
            {
                textBoxPlayer2Estate.Text = "1";
                textBoxPlayer2Duchy.Text = "2";
                textBoxPlayer2Province.Text = "4";
            }
        }

        private void buttonPlayer1Reset_Click(object sender, EventArgs e)
        {
            Player1Reset();
        }

        private void buttonPlayer2Reset_Click(object sender, EventArgs e)
        {
            Player2Reset();
        }

        private void LoadPanelShorthand(string line, Panel p)
        {
            for (int i = 0; i < 13; i++) SetBuildRow(p, i, "copper", 0);

            String[] s = line.Split('|');
            for(int i = 0; i < s.Length; i++)
            {
                SetBuildRow(p, i, s[i].Split('@')[0], Convert.ToInt32(s[i].Split('@')[1]));
            }
        }

        void LoadShorthand(string shorthand, Panel p, TextBox estate, TextBox duchy, TextBox province)
        {
            String[] s = shorthand.Split('-');
            try
            {
                estate.Text = s[0].Replace("e", "");
                duchy.Text = s[1].Replace("d", "");
                province.Text = s[2].Replace("p", "");
                LoadPanelShorthand(s[3], p);
            }
            catch (Exception)
            {
                textBoxShorthand.ForeColor = Color.Red;
                return;
            }
            textBoxShorthand.ForeColor = Color.Black;
        }

        private void buttonShorthand1_Click(object sender, EventArgs e)
        {
            LoadShorthand(textBoxShorthand.Text, panelPlayer1, textBoxPlayer1Estate, textBoxPlayer1Duchy, textBoxPlayer1Province);
        }

        private void buttonShorthand2_Click(object sender, EventArgs e)
        {
            LoadShorthand(textBoxShorthand.Text, panelPlayer2, textBoxPlayer2Estate, textBoxPlayer2Duchy, textBoxPlayer2Province);
        }

        public String AIString()
        {
            String player1String = "Human";
            if (comboPlayer1.SelectedItem.ToString() != "Human")
            {
                player1String = "e" + textBoxPlayer1Estate.Text.ToString() + "-" +
                                "d" + textBoxPlayer1Duchy.Text.ToString() + "-" +
                                "p" + textBoxPlayer1Province.Text.ToString() + "-" +
                                PanelToShorthandEncoding(panelPlayer1).Replace("@", "~");
            }

            String player2String = "Human";
            if (comboPlayer2.SelectedItem.ToString() != "Human")
            {
                player2String = "e" + textBoxPlayer2Estate.Text.ToString() + "-" +
                                "d" + textBoxPlayer2Duchy.Text.ToString() + "-" +
                                "p" + textBoxPlayer2Province.Text.ToString() + "-" +
                                PanelToShorthandEncoding(panelPlayer2).Replace("@", "~");
            }
            return player1String + "@" + player2String;
        }

        private void buttonCompareAIs_Click(object sender, EventArgs e)
        {
            String parameters = AIString();
            if (parameters.Contains("Human"))
            {
                MessageBox.Show("Humans are not valid test subjects.", "Error");
                return;
            }
            
            DLLInterface.ProcessCommand("testAIs@" + parameters);
            parentWindow.InitializeSupply();
            parentWindow.UpdateAllState();

            labelSampleGameInfo.Visible = true;
            labelSampleGameInfo.Text = "The main window contains a sample game between the two AIs.";

            double result = DLLInterface.GetDouble("AITestResult");
            labelResult.Text = "Player 1 wins " + result.ToString("P").Replace(" %", "%") + " of games for a spread of " + DominionVisualization.FormatPercentage((result * 100.0 - 50.0) * 2.0) + ".";
        }

        private void buttonRunTournament_Click(object sender, EventArgs e)
        {
            parentWindow.VisualizeFinal();
            if (parentWindow.activeProcess != null)
            {
                if(parentWindow.activeProcess.WaitForExit(100))
                {
                    parentWindow.activeProcess = null;
                    parentWindow.VisualizeFinal();
                }
                else
                {
                    MessageBox.Show("Cannot launch a new tournament while the previous one is still running.", "Error");
                    return;
                }
            }

            int chamberCount, generationCount;
            try
            {
                chamberCount = Convert.ToInt32(textBoxChambers.Text.ToString());
                generationCount = Convert.ToInt32(textBoxGenerations.Text.ToString());
                if (chamberCount < 1 || chamberCount > 50 || generationCount < 1 || generationCount > 10000) throw new Exception();
            }
            catch
            {
                textBoxChambers.ForeColor = Color.Red;
                textBoxGenerations.ForeColor = Color.Red;
                return;
            }

            textBoxChambers.ForeColor = Color.Black;
            textBoxGenerations.ForeColor = Color.Black;

            DialogResult result = MessageBox.Show("This will launch a new process that will take approximately 10 seconds per generation.  " +
                            "Results will be stored in the kingdomsIntermediate and KingdomsFinal folders.  " +
                            "You can load any of the text files into the AI building interface with the \"Load AI File\" button, then visualize them with the \"Show Leaderboard\" button.  Are you sure you want to continue?",
                            "Caution", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                activeTournamentGenerationCount = generationCount;
                activeTournamentDirectory = Directory.GetCurrentDirectory() + "/" + parentWindow.Strategize(generationCount, chamberCount);
                timerCheckProgress.Enabled = true;
                buttonLoadAndVisualize.Enabled = false;
            }
        }

        private void buttonVisualizeAll_Click(object sender, EventArgs e)
        {
            parentWindow.VisualizeAll();
        }

        private void buttonVisualizeFinal_Click(object sender, EventArgs e)
        {
            parentWindow.VisualizeFinal();
        }

        public void LoadAIFile(string filename)
        {
            List<String> lines;
            try
            {
                lines = new List<String>(File.ReadAllLines(filename));
            }
            catch
            {
                MessageBox.Show("There was an error reading from the file.");
                return;
            }

            int leaderStartIndex;
            int leaderCount;
            try
            {
                String kingdomCards = lines.Find(delegate(String s) { return s.StartsWith("Kingdom cards:"); }).Split('\t')[1];
                parentWindow.NewKingdomCards(kingdomCards.Replace("@", "|"));

                textBoxRequiredCards.Text = kingdomCards.Split('@')[0];
                checkBoxProsperity.Checked = !kingdomCards.Contains("NoProsperity");
                radioButton34Split.Checked = kingdomCards.Contains("34Split");

                leaderStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Leaders:"); });
                if (leaderStartIndex == -1) leaderStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Opponents:"); });
                leaderCount = Convert.ToInt32(lines[leaderStartIndex].Split('\t')[1]);
            }
            catch
            {
                MessageBox.Show("This is not a valid AI file. Select a text file in the kingdomsIntermediate or kingdomsFinal directories.");
                return;
            }

            ResetPlayerLists();
            for (int leaderIndex = 0; leaderIndex < leaderCount; leaderIndex++)
            {
                String shorthand = lines[leaderStartIndex + leaderIndex + 1].Split('\t')[2];
                comboPlayer1.Items.Add(shorthand);
                comboPlayer2.Items.Add(shorthand);
            }

            try
            {
                comboPlayer1.SelectedIndex = 2;
                comboPlayer2.SelectedIndex = 0;
            }
            catch { }

            visualizationFilename = filename.Replace(".txt", ".png");
            parentWindow.NewGame(AIString());
        }

        private void buttonLoadAIFile_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialog.ShowDialog();
            if (result == DialogResult.OK && File.Exists(openFileDialog.FileName))
            {
                LoadAIFile(openFileDialog.FileName);
            }
        }

        private void comboPlayer1_SelectedIndexChanged(object sender, EventArgs e)
        {
            String shorthand = comboPlayer1.SelectedItem.ToString();
            if (shorthand != "Human" && shorthand != "Custom AI")
            {
                LoadShorthand(shorthand, panelPlayer1, textBoxPlayer1Estate, textBoxPlayer1Duchy, textBoxPlayer1Province);
                textBoxShorthand.Text = shorthand;
            }
        }

        private void comboPlayer2_SelectedIndexChanged(object sender, EventArgs e)
        {
            String shorthand = comboPlayer2.SelectedItem.ToString();
            if (shorthand != "Human" && shorthand != "Custom AI")
            {
                LoadShorthand(shorthand, panelPlayer2, textBoxPlayer2Estate, textBoxPlayer2Duchy, textBoxPlayer2Province);
                textBoxShorthand.Text = shorthand;
            }
        }

        private void buttonShowLeaderboard_Click(object sender, EventArgs e)
        {
            if (visualizationFilename.Length == 0)
            {
                MessageBox.Show("This button shows the visualization corresponding to an AI file created by the \"Run Tournament\" button and loaded using the \"Load AI File\" button." +
                                "  Start by running a tournament and using the \"Load AI File\" button to load a text file in the \"leaderboard\" or \"progression\" folders.");
                return;
            }

            DominionVisualization vis = new DominionVisualization(parentWindow.database, visualizationFilename.Replace(".png", ".txt"), visualizationFilename);
            if (File.Exists(visualizationFilename))
            {
                Process.Start(visualizationFilename);
            }
            else
            {
                MessageBox.Show("The corresponding strategy visualization for " + visualizationFilename + " does not exist.  Use the \"Visualize All\" button to " +
                                "generate visualization files for leaderboard and progression AI files.");
            }
        }

        private void textBoxPlayer2_TextChanged(object sender, EventArgs e)
        {
            //comboPlayer2.SelectedIndex = 1;
        }

        private void textBoxPlayer1_TextChanged(object sender, EventArgs e)
        {
            //comboPlayer1.SelectedIndex = 1;
        }

        private void buttonNewGame_Click(object sender, EventArgs e)
        {
            labelSampleGameInfo.Visible = true;
            if (AIString().Contains("Human")) labelSampleGameInfo.Text = "New game started in the main window.";
            else labelSampleGameInfo.Text = "The main window contains a sample game between the two AIs.";
            parentWindow.NewGame(AIString());
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            pictureBoxOccluder.Visible = !checkBoxCustomizeAI.Checked;
        }

        private void ConfigWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
        }

        private void timerCheckProgress_Tick(object sender, EventArgs e)
        {
            if(activeTournamentDirectory.Length == 0) return;

            string file = Utility.FindLargestIndexInDirectory(activeTournamentDirectory);
            if (file.Length == 0)
            {
                labelLeaderboard.Text = "Tournament in progress...";
                return;
            }

            int fileIndex;
            try
            {
                fileIndex = Convert.ToInt32(file.Replace(activeTournamentDirectory, "").Replace(".txt",""));
            }
            catch
            {
                return;
            }

            if (fileIndex == activeTournamentGenerationCount)
            {
                labelLeaderboard.Text = "Tournament completed.";
                timerCheckProgress.Enabled = false;
                string visFile = file.Replace(".txt", ".png");
                DominionVisualization vis = new DominionVisualization(parentWindow.database, file, visFile);
                if (File.Exists(visFile))
                {
                    Process.Start(visFile);
                }
            }
            else
            {
                labelLeaderboard.Text = "Leaderboard for generation " + fileIndex.ToString() + " completed.";
            }
            buttonLoadAndVisualize.Enabled = true;
        }

        private void buttonLoadAndVisualize_Click(object sender, EventArgs e)
        {
            string file = Utility.FindLargestIndexInDirectory(activeTournamentDirectory);
            if (file.Length > 0)
            {
                LoadAIFile(file);
                string visFile = file.Replace(".txt", ".png");
                DominionVisualization vis = new DominionVisualization(parentWindow.database, file, visFile);
                if (File.Exists(visFile))
                {
                    Process.Start(visFile);
                }
            }
        }

        private void labelLink_Click(object sender, EventArgs e)
        {
            Process.Start("http://graphics.stanford.edu/~mdfisher/DominionAI.html");
        }
    }
}

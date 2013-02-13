using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Linq;
using System.Diagnostics;

namespace BaseCodeApp
{
    public partial class MainWindow : Form
    {
        public CardDatabase database = new CardDatabase();
        public List<SupplyEntry> supply;
        DecisionData decision = new DecisionData();

        DebugWindow debugWindow;
        ConfigWindow configWindow;

        public Process activeProcess;

        public MainWindow()
        {
            InitializeComponent();
            DLLInterface.Init();

            NewGame("");

            InitConfigWindow();

            NewKingdomCardsPressed();
        }

        private void InitConfigWindow()
        {
            if (configWindow == null || configWindow.IsDisposed)
            {
                configWindow = new ConfigWindow(this);
                configWindow.Visible = false;
            }
        }

        private void buttonConfigGame_Click(object sender, EventArgs e)
        {
            InitConfigWindow();
            configWindow.Visible = true;
            configWindow.WindowState = FormWindowState.Normal;
            configWindow.Activate();
            configWindow.Show();
        }

        public void NewKingdomCards(string gameOptions)
        {
            DLLInterface.ProcessCommand("newKingdomCards@" + gameOptions);
            InitializeSupply();
            UpdateAllState();

            if (configWindow != null)
            {
                configWindow.UpdateBuildLists();
            }
        }

        public void NewGame(string parameters)
        {
            if(parameters.Length == 0)
                DLLInterface.ProcessCommand("newGame");
            else
                DLLInterface.ProcessCommand("newGame@" + parameters);
            InitializeSupply();
            UpdateAllState();
        }

        private void buttonNewGame_Click(object sender, EventArgs e)
        {
            if (checkBoxUsePrecomputedKingdoms.Checked)
            {
                NewGame(configWindow.AIString());
                labelError.Text = "You are playing against a precomputed AI trained on these kingdom cards. Use the \"Game Options\" button to choose its strategy.";
                labelError.Visible = true;
            }
            else
            {
                NewGame("");
                labelError.Text = "You are playing against an AI that can only make random decisions. Use the \"Game Options\" button to train a competent opponent.";
                labelError.Visible = true;
            }
        }

        private void NewKingdomCardsPressed()
        {
            if (checkBoxUsePrecomputedKingdoms.Checked)
            {
                List<string> files = new List<string>(Directory.EnumerateFiles("data/kingdoms_g32_c1/", "*.txt"));
                Random r = new Random();
                InitConfigWindow();
                configWindow.LoadAIFile(Directory.GetCurrentDirectory() + "/" + files[r.Next(files.Count)]);

                labelError.Text = "You are playing against a precomputed AI trained on these kingdom cards. Use the \"Game Options\" button to choose its strategy.";
                labelError.Visible = true;
            }
            else
            {
                NewKingdomCards("");
                labelError.Text = "You are playing against an AI that can only make random decisions. Use the \"Game Options\" button to train a competent opponent.";
                labelError.Visible = true;
            }
        }

        private void buttonNewKingdom_Click(object sender, EventArgs e)
        {
            NewKingdomCardsPressed();
        }

        private void UpdateLog()
        {
            String log = DLLInterface.GetString("log") + " ";
            if (log.StartsWith("New Game"))
            {
                textBoxLog.Clear();
            }
            else
            {
                log = "\n" + log;
            }

            int textBoxStart = textBoxLog.Text.Length;
            textBoxLog.AppendText(log);

            foreach (SupplyEntry s in supply)
            {
                String paddedName = " " + s.c.PrettyName() + " ";
                foreach (var instance in log.AllIndexesOf(paddedName))
                {
                    textBoxLog.SelectionStart = textBoxStart + instance;
                    textBoxLog.SelectionLength = paddedName.Length;
                    textBoxLog.SelectionBackColor = s.c.BackColor();
                }
            }

            textBoxLog.SelectionStart = textBoxLog.Text.Length - 1;
            textBoxLog.SelectionLength = 1;
            textBoxLog.SelectionBackColor = Color.White;
            textBoxLog.ScrollToCaret();
        }

        private void ProcessDecision()
        {
            List<Card> result = new List<Card>();
            int choice = -1;
            foreach(ButtonBase b in panelDecision.Controls)
            {
                if (b is CardCheckBox      && (b as CardCheckBox     ).Checked) result.Add((b as CardCheckBox   ).c);
                if (b is CardRadioButton   && (b as CardRadioButton  ).Checked) result.Add((b as CardRadioButton).c);
                if (b is ChoiceRadioButton && (b as ChoiceRadioButton).Checked) choice = (b as ChoiceRadioButton).choice;
            }
            if (decision.type == "choice")
            {
                //
                // discrete choice decision
                //
                if (choice >= 0)
                {
                    String response = "response@" + choice.ToString();
                    DLLInterface.ProcessCommand(response);
                    UpdateAllState();
                }
                else
                {
                    labelError.Text = "You must choose an option.";
                }
            }
            else if (decision.type == "selectCard")
            {
                //
                // card select decision
                //
                if (result.Count >= decision.minimumCount && result.Count <= decision.maximumCount)
                {
                    String response = "response@";
                    foreach (Card c in result) response += c.name + "|";
                    DLLInterface.ProcessCommand(response);
                    UpdateAllState();
                }
                else
                {
                    if (decision.minimumCount == 1 && decision.minimumCount == 1)
                    {
                        labelError.Text = "You must choose a card.";
                    }
                    else if (decision.minimumCount == decision.maximumCount)
                    {
                        labelError.Text = "You must select exactly " + decision.minimumCount + " cards.";
                    }
                    else
                    {
                        labelError.Text = "You must select between " + decision.minimumCount + " and " + decision.maximumCount + " cards.";
                    }
                }
            }
        }

        private void UpdateDecision()
        {
            panelDecision.Controls.Clear();
            labelError.Text = null;
            
            int x = 0;
            int y = 0;

            List<String> s = new List<String>(DLLInterface.GetString("decision").Split('@'));
            decision.type = s[0];

            if (decision.type == "none") labelDecision.Text = "No decision?";
            else if (decision.type == "gameover")
            {
                labelActiveCard.Text = "";
                labelActiveCardName.Visible = false;
                labelDecision.Text = "Game Over!";
            }
            else if (decision.type == "selectCard")
            {
                String playerName = s[2];
                labelDecision.Text = "(" + playerName + ") " + s[3];
                decision.minimumCount = Convert.ToInt32(s[4]);
                decision.maximumCount = Convert.ToInt32(s[5]);

                String activeCardName = s[6];
                if (activeCardName.StartsWith("phase"))
                {
                    labelActiveCard.Text = Utility.PhaseName(Convert.ToInt32(activeCardName.Split('|')[1])) + " Phase";
                    labelActiveCardName.Visible = false;
                }
                else
                {
                    Card activeCard = database.GetCard(activeCardName);
                    labelActiveCard.Text = "Active Card:";
                    labelActiveCardName.Text = activeCard.PrettyName();
                    labelActiveCardName.BackColor = activeCard.BackColor();
                    labelActiveCardName.Visible = true;
                }

                var sortedList = new List<String>(s[7].Split('|'));
                var cards = sortedList.Select(z => database.GetCard(z));
                cards = cards.OrderByDescending(a => a.cost).ThenBy(a => a.name).ToList();
                foreach (Card c in cards)
                {
                    ButtonBase button;
                    if (decision.maximumCount == 1)
                    {
                        button = new CardRadioButton(c);
                        button.Click += delegate { ProcessDecision(); };
                    }
                    else
                    {
                        button = new CardCheckBox(c);
                    }

                    button.Left = x * 150 + 3;
                    button.Top = y * 25 + 3;
                    button.MaximumSize = new Size(145, 23);
                    button.Text = c.PrettyName();
                    button.BackColor = c.BackColor();
                    button.Font = panelDecision.Font;
                    button.AutoSize = true;
                    button.MouseMove += delegate { pictureBoxCardImage.BackgroundImage = c.fullImage; };
                    button.Padding = new Padding(3, 1, 1, 1);
                    panelDecision.Controls.Add(button);

                    y++;
                    if (y == 5)
                    {
                        x++;
                        y = 0;
                    }
                }
            }
            else if (decision.type == "choice")
            {
                String playerName = s[2];
                String[] parts = s[3].Split('|');
                labelDecision.Text = "(" + playerName + ") " + parts[0];

                for (int choiceIndex = 1; choiceIndex < parts.Length; choiceIndex++)
                {
                    ButtonBase button = new ChoiceRadioButton(choiceIndex - 1);
                    button.Click += delegate { ProcessDecision(); };

                    button.Left = x * 190 + 3;
                    button.Top = y * 25 + 3;
                    button.MaximumSize = new Size(185, 23);
                    button.Text = parts[choiceIndex];
                    button.Font = panelDecision.Font;
                    button.AutoSize = true;
                    button.Padding = new Padding(3, 1, 1, 1);
                    panelDecision.Controls.Add(button);

                    y++;
                    if (y == 5)
                    {
                        x++;
                        y = 0;
                    }
                }
            }
        }

        public void InitializeSupply()
        {
            List<String> supplyCards = new List<String>(DLLInterface.GetString("supplyCards").Split('|'));
            panelSupply.Controls.Clear();

            supply = new List<SupplyEntry>();

            int supplyX = 0, supplyY = 0;

            foreach(String s in supplyCards)
            {
                SupplyEntry newEntry = new SupplyEntry();
                newEntry.index = supply.Count;
                supply.Add(newEntry);

                int baseX = supplyX * 60;
                int baseY = supplyY * 85;

                newEntry.c = database.GetCard(s);
                newEntry.c.LoadBitmaps();
                newEntry.p = new PictureBox();

                newEntry.p.Size = newEntry.c.croppedImage.Size;
                newEntry.p.Left = baseX + 4;
                newEntry.p.Top = baseY + 18;
                newEntry.p.BorderStyle = BorderStyle.FixedSingle;

                newEntry.p.Image = newEntry.c.croppedImage;
                newEntry.p.MouseMove += delegate { pictureBoxCardImage.BackgroundImage = newEntry.c.fullImage; };

                newEntry.nameLabel = new Label();
                newEntry.nameLabel.Left = baseX;
                newEntry.nameLabel.Top = baseY;
                newEntry.nameLabel.Width = 58;
                newEntry.nameLabel.Height = 60;
                newEntry.nameLabel.BorderStyle = BorderStyle.FixedSingle;
                newEntry.nameLabel.Text = newEntry.c.PrettyName();
                newEntry.nameLabel.TextAlign = ContentAlignment.TopCenter;
                newEntry.nameLabel.BackColor = newEntry.c.BackColor();
                newEntry.nameLabel.Font = this.Font;

                newEntry.countLabel = new Label();
                newEntry.countLabel.Left = baseX;
                newEntry.countLabel.Top = baseY + 61;
                newEntry.countLabel.Width = 58;
                newEntry.countLabel.Height = 20;
                newEntry.countLabel.Text = "";
                newEntry.countLabel.TextAlign = ContentAlignment.TopCenter;
                newEntry.countLabel.Font = this.Font;
                newEntry.countLabel.BackColor = Color.Transparent;

                panelSupply.Controls.Add(newEntry.p);
                panelSupply.Controls.Add(newEntry.nameLabel);
                panelSupply.Controls.Add(newEntry.countLabel);

                supplyX++;

                if (supplyX == 3 && supplyY == 0 && !supplyCards.Contains("platinum")) supplyX++;
                if (supplyX == 4 && supplyY == 0 && !supplyCards.Contains("potion")) supplyX++;
                if (supplyX == 3 && supplyY == 1 && !supplyCards.Contains("colony")) supplyX++;

                if (supplyX >= 5)
                {
                    supplyX = 0;
                    supplyY++;
                }
            }
        }

        public void UpdateAllState()
        {
            UpdateBasicState();
            UpdateSupplyState();
            UpdatePlayArea();
            UpdateHand();
            UpdateDecision();
            UpdateLog();

            if (debugWindow != null && !debugWindow.IsDisposed)
            {
                debugWindow.UpdateState();
            }
        }

        private void UpdateBasicState()
        {
            String[] parts = DLLInterface.GetString("basicState").Split('|');
            labelPhase.Text = parts[1] + "'s " + Utility.PhaseName(Convert.ToInt32(parts[2])) + " Phase";
            labelActions.Text = "Actions: " + parts[3];
            labelBuys.Text = "Buys: " + parts[4];
            labelMoney.Text = "$ " + parts[5];
        }

        private void UpdateSupplyState()
        {
            String[] parts = DLLInterface.GetString("supplyState").Split('@');
            List<String> supplyCounts = new List<String>(parts[0].Split('|'));
            List<String> supplyCosts = new List<String>(parts[1].Split('|'));
            
            foreach (SupplyEntry s in supply)
            {
                s.countLabel.Text = "$" + supplyCosts[s.index] + " (" + supplyCounts[s.index] + ")";
            }
        }

        private void UpdateArea(Panel p, String cardList)
        {
            p.Controls.Clear();
            if (cardList.Length == 0) return;
            int x = 0;
            int y = 0;
            int height = 5;
            if (p == panelHand) height = 7;

            List<String> cards = new List<String>(cardList.Split('|'));
            cards.Sort();

            foreach (String s in cards)
            {
                Card c = database.GetCard(s);
                Label l = new Label();
                l.Left = x * 150 + 3;
                l.Top = y * 25 + 3;
                l.MaximumSize = new Size(148, 23);
                l.Text = c.PrettyName();
                l.BackColor = c.BackColor();
                l.BorderStyle = BorderStyle.FixedSingle;
                l.Font = p.Font;
                l.AutoSize = true;
                l.MouseMove += delegate { pictureBoxCardImage.BackgroundImage = c.fullImage; };

                p.Controls.Add(l);

                y++;
                if (y == height)
                {
                    x++;
                    y = 0;
                }
            }
        }

        private void UpdatePlayArea()
        {
            UpdateArea(panelPlayArea, DLLInterface.GetString("playArea"));
        }

        private void UpdateHand()
        {
            UpdateArea(panelHand, DLLInterface.GetString("hand"));
        }

        private void buttonDone_Click(object sender, EventArgs e)
        {
            ProcessDecision();
        }

        private void buttonDebug_Click(object sender, EventArgs e)
        {
            if (debugWindow == null || debugWindow.IsDisposed)
            {
                debugWindow = new DebugWindow();
                debugWindow.Show();
            }
            else
            {
                debugWindow.WindowState = FormWindowState.Normal;
                debugWindow.Activate();
                debugWindow.Show();
            }
            debugWindow.UpdateState();
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {

        }

        public string Strategize(int generationCount, int chamberCount)
        {
            if (supply == null || supply.Count == 0) return "";

            string directory = Utility.MD5Hash(DLLInterface.GetString("kingdomDescription"));

            Directory.CreateDirectory("kingdomsIntermediate/");
            Directory.CreateDirectory("kingdomsFinal/");
            Directory.CreateDirectory("kingdomsIntermediate/" + directory);
            Directory.CreateDirectory("kingdomsIntermediate/" + directory + "/generations");
            Directory.CreateDirectory("kingdomsIntermediate/" + directory + "/leaderboard");
            Directory.CreateDirectory("kingdomsIntermediate/" + directory + "/progression");

            using (StreamWriter parameterFile = new System.IO.StreamWriter("TournamentParameters.txt"))
            {
                parameterFile.WriteLine(DLLInterface.GetString("kingdomDescription").Replace("@","|"));
                parameterFile.WriteLine("kingdomsIntermediate/" + directory + "/");
                parameterFile.WriteLine("generations=" + generationCount.ToString());
                parameterFile.WriteLine("chambers=" + chamberCount.ToString());
                parameterFile.Close();
            }

            if (activeProcess == null)
            {
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

                return "kingdomsIntermediate/" + directory + "/leaderboard/";
            }
            else
            {
                MessageBox.Show("Cannot launch a new tournament while the previous one is still running.", "Error");
                return "";
            }

            /*DLLInterface.ProcessCommand("trainAIStart@kingdoms/" + directory + "/@1");

            for (int generationIndex = 0; generationIndex < 64; generationIndex++)
            {
                DLLInterface.ProcessCommand("trainAIStep");

                List<string> leaderboardFiles = new List<string>(Directory.EnumerateFiles("kingdoms/" + directory + "/leaderboard", "*.txt"));
                foreach (var f in leaderboardFiles)
                {
                    if (!File.Exists(f.Replace(".txt", ".png")))
                    {
                        DominionVisualization vis = new DominionVisualization(database, f, f.Replace(".txt", ".png"));
                    }
                }

                List<string> progressionFiles = new List<string>(Directory.EnumerateFiles("kingdoms/" + directory + "/progression", "*.txt"));
                foreach (var f in progressionFiles)
                {
                    if (!File.Exists(f.Replace(".txt", ".png")))
                    {
                        DominionVisualization vis = new DominionVisualization(database, f, f.Replace(".txt", ".png"));
                    }
                }
            }

            Utility.SafeFileCopy(directory + "/leaderboard/063.png", "kingdomVisualizations/" + directory + "_l.png");
            Utility.SafeFileCopy(directory + "/progression/063.png", "kingdomVisualizations/" + directory + "_p.png");*/
        }

        public void VisualizeFinal()
        {
            Directory.CreateDirectory("kingdomsIntermediate/");
            Directory.CreateDirectory("kingdomsFinal/");
            List<string> directories = new List<string>(Directory.EnumerateDirectories("kingdomsIntermediate/"));

            foreach (var dir in directories)
            {
                string dirOnly = dir.Replace("kingdomsIntermediate/", "");

                string finalLeaderboard = Utility.FindLargestIndexInDirectory(dir + "/leaderboard");
                string leaderboardVisFilename = finalLeaderboard.Replace(".txt", ".png");
                if (finalLeaderboard.Length > 0 && !File.Exists(finalLeaderboard.Replace(".txt", ".png")))
                {
                    DominionVisualization vis = new DominionVisualization(database, finalLeaderboard, leaderboardVisFilename);
                }
                Utility.SafeFileCopy(leaderboardVisFilename, "kingdomsFinal/" + dirOnly + "_l.png");
                Utility.SafeFileCopy(finalLeaderboard, "kingdomsFinal/" + dirOnly + ".txt");

                string finalProgression = Utility.FindLargestIndexInDirectory(dir + "/progression");
                string progressionVisFilename = finalProgression.Replace(".txt", ".png");
                if (finalProgression.Length > 0 && !File.Exists(finalProgression.Replace(".txt", ".png")))
                {
                    DominionVisualization vis = new DominionVisualization(database, finalProgression, progressionVisFilename);
                }
                Utility.SafeFileCopy(progressionVisFilename, "kingdomsFinal/" + dirOnly + "_p.png");
            }
        }

        public void VisualizeAll()
        {
            Directory.CreateDirectory("kingdomsIntermediate/");
            Directory.CreateDirectory("kingdomsFinal/");
            List<string> directories = new List<string>(Directory.EnumerateDirectories("kingdomsIntermediate/"));

            foreach (var dir in directories)
            {
                List<string> leaderboardFiles = new List<string>(Directory.EnumerateFiles(dir + "/leaderboard", "*.txt"));
                foreach (var f in leaderboardFiles)
                {
                    if (f.EndsWith(".txt") && !File.Exists(f.Replace(".txt", ".png")))
                    {
                        DominionVisualization vis = new DominionVisualization(database, f, f.Replace(".txt", ".png"));
                    }
                }

                List<string> progressionFiles = new List<string>(Directory.EnumerateFiles(dir + "/progression", "*.txt"));
                foreach (var f in progressionFiles)
                {
                    if (f.EndsWith(".txt") && !File.Exists(f.Replace(".txt", ".png")))
                    {
                        DominionVisualization vis = new DominionVisualization(database, f, f.Replace(".txt", ".png"));
                    }
                }
            }

            VisualizeFinal();
        }
    }
}

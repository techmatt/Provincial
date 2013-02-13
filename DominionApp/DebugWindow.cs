using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BaseCodeApp
{
    public partial class DebugWindow : Form
    {
        public DebugWindow()
        {
            InitializeComponent();
        }

        public void UpdateState(bool updatePlayerList = true)
        {
            if(updatePlayerList) UpdatePlayerList();
            
            LoadCardList("deck", listBoxDeck);
            LoadCardList("discard", listBoxDiscard);
            LoadCardList("playArea", listBoxPlayArea);
            LoadCardList("hand", listBoxHand);

            labelTotalVP.Text = "Total VP: " + Convert.ToInt32(DLLInterface.GetString("victoryPoints " + comboBoxPlayerList.SelectedIndex.ToString()));
        }

        public void LoadCardList(String command, ListBox box)
        {
            box.Items.Clear();
            String[] cards = DLLInterface.GetString(command + " " + comboBoxPlayerList.SelectedIndex.ToString()).Split('|');
            foreach (String s in cards)
            {
                box.Items.Add(s);
            }
        }

        private void UpdatePlayerList()
        {
            comboBoxPlayerList.Items.Clear();
            String[] playerInfo = DLLInterface.GetString("playerList").Split('|');
            if (playerInfo.Length <= 1) return;
            foreach (String s in playerInfo)
            {
                String[] parts = s.Split('@');
                comboBoxPlayerList.Items.Add(parts[0] + " -- " + parts[1]);
            }

            int activePlayer = Convert.ToInt32(DLLInterface.GetString("basicState").Split('|')[0]);
            comboBoxPlayerList.SelectedIndex = activePlayer;
        }

        private void DebugWindow_Load(object sender, EventArgs e)
        {

        }

        private void comboBoxPlayerList_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateState(false);
        }

        private void buttonAddCard_Click(object sender, EventArgs e)
        {
            DLLInterface.ProcessCommand("debugAddCard@" + textBoxAddCard.Text.ToLower());
            UpdateState(false);
        }
    }
}

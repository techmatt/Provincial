namespace BaseCodeApp
{
    partial class DebugWindow
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
            this.comboBoxPlayerList = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.listBoxHand = new System.Windows.Forms.ListBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.listBoxPlayArea = new System.Windows.Forms.ListBox();
            this.listBoxDeck = new System.Windows.Forms.ListBox();
            this.listBoxDiscard = new System.Windows.Forms.ListBox();
            this.label5 = new System.Windows.Forms.Label();
            this.labelVPTokens = new System.Windows.Forms.Label();
            this.labelTotalVP = new System.Windows.Forms.Label();
            this.listBoxStack = new System.Windows.Forms.ListBox();
            this.label6 = new System.Windows.Forms.Label();
            this.textBoxAddCard = new System.Windows.Forms.TextBox();
            this.buttonAddCard = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // comboBoxPlayerList
            // 
            this.comboBoxPlayerList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxPlayerList.FormattingEnabled = true;
            this.comboBoxPlayerList.Location = new System.Drawing.Point(153, 8);
            this.comboBoxPlayerList.Margin = new System.Windows.Forms.Padding(4);
            this.comboBoxPlayerList.Name = "comboBoxPlayerList";
            this.comboBoxPlayerList.Size = new System.Drawing.Size(936, 27);
            this.comboBoxPlayerList.TabIndex = 0;
            this.comboBoxPlayerList.SelectedIndexChanged += new System.EventHandler(this.comboBoxPlayerList_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 11);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(132, 19);
            this.label1.TabIndex = 1;
            this.label1.Text = "Looking at player:";
            // 
            // listBoxHand
            // 
            this.listBoxHand.FormattingEnabled = true;
            this.listBoxHand.ItemHeight = 19;
            this.listBoxHand.Location = new System.Drawing.Point(12, 69);
            this.listBoxHand.Name = "listBoxHand";
            this.listBoxHand.Size = new System.Drawing.Size(212, 308);
            this.listBoxHand.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 47);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(45, 19);
            this.label2.TabIndex = 1;
            this.label2.Text = "Hand";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(226, 47);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(74, 19);
            this.label3.TabIndex = 1;
            this.label3.Text = "Play Area";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(444, 47);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(42, 19);
            this.label4.TabIndex = 1;
            this.label4.Text = "Deck";
            // 
            // listBoxPlayArea
            // 
            this.listBoxPlayArea.FormattingEnabled = true;
            this.listBoxPlayArea.ItemHeight = 19;
            this.listBoxPlayArea.Location = new System.Drawing.Point(230, 69);
            this.listBoxPlayArea.Name = "listBoxPlayArea";
            this.listBoxPlayArea.Size = new System.Drawing.Size(212, 308);
            this.listBoxPlayArea.TabIndex = 2;
            // 
            // listBoxDeck
            // 
            this.listBoxDeck.FormattingEnabled = true;
            this.listBoxDeck.ItemHeight = 19;
            this.listBoxDeck.Location = new System.Drawing.Point(448, 69);
            this.listBoxDeck.Name = "listBoxDeck";
            this.listBoxDeck.Size = new System.Drawing.Size(212, 308);
            this.listBoxDeck.TabIndex = 2;
            // 
            // listBoxDiscard
            // 
            this.listBoxDiscard.FormattingEnabled = true;
            this.listBoxDiscard.ItemHeight = 19;
            this.listBoxDiscard.Location = new System.Drawing.Point(666, 69);
            this.listBoxDiscard.Name = "listBoxDiscard";
            this.listBoxDiscard.Size = new System.Drawing.Size(212, 308);
            this.listBoxDiscard.TabIndex = 2;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(662, 47);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(59, 19);
            this.label5.TabIndex = 1;
            this.label5.Text = "Discard";
            // 
            // labelVPTokens
            // 
            this.labelVPTokens.AutoSize = true;
            this.labelVPTokens.Location = new System.Drawing.Point(8, 391);
            this.labelVPTokens.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelVPTokens.Name = "labelVPTokens";
            this.labelVPTokens.Size = new System.Drawing.Size(94, 19);
            this.labelVPTokens.TabIndex = 1;
            this.labelVPTokens.Text = "VP Tokens: 0";
            // 
            // labelTotalVP
            // 
            this.labelTotalVP.AutoSize = true;
            this.labelTotalVP.Location = new System.Drawing.Point(130, 391);
            this.labelTotalVP.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelTotalVP.Name = "labelTotalVP";
            this.labelTotalVP.Size = new System.Drawing.Size(81, 19);
            this.labelTotalVP.TabIndex = 1;
            this.labelTotalVP.Text = "Total VP: 0";
            // 
            // listBoxStack
            // 
            this.listBoxStack.FormattingEnabled = true;
            this.listBoxStack.ItemHeight = 19;
            this.listBoxStack.Location = new System.Drawing.Point(884, 69);
            this.listBoxStack.Name = "listBoxStack";
            this.listBoxStack.Size = new System.Drawing.Size(212, 308);
            this.listBoxStack.TabIndex = 2;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(880, 47);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(46, 19);
            this.label6.TabIndex = 1;
            this.label6.Text = "Stack";
            // 
            // textBoxAddCard
            // 
            this.textBoxAddCard.Location = new System.Drawing.Point(743, 388);
            this.textBoxAddCard.Name = "textBoxAddCard";
            this.textBoxAddCard.Size = new System.Drawing.Size(200, 27);
            this.textBoxAddCard.TabIndex = 3;
            this.textBoxAddCard.Text = "village";
            // 
            // buttonAddCard
            // 
            this.buttonAddCard.Location = new System.Drawing.Point(949, 386);
            this.buttonAddCard.Name = "buttonAddCard";
            this.buttonAddCard.Size = new System.Drawing.Size(147, 29);
            this.buttonAddCard.TabIndex = 4;
            this.buttonAddCard.Text = "Add card to hand";
            this.buttonAddCard.UseVisualStyleBackColor = true;
            this.buttonAddCard.Click += new System.EventHandler(this.buttonAddCard_Click);
            // 
            // DebugWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 19F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1102, 432);
            this.Controls.Add(this.buttonAddCard);
            this.Controls.Add(this.textBoxAddCard);
            this.Controls.Add(this.listBoxStack);
            this.Controls.Add(this.listBoxDiscard);
            this.Controls.Add(this.listBoxDeck);
            this.Controls.Add(this.listBoxPlayArea);
            this.Controls.Add(this.listBoxHand);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.labelTotalVP);
            this.Controls.Add(this.labelVPTokens);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comboBoxPlayerList);
            this.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "DebugWindow";
            this.Text = "DebugWindow";
            this.Load += new System.EventHandler(this.DebugWindow_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBoxPlayerList;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListBox listBoxHand;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ListBox listBoxPlayArea;
        private System.Windows.Forms.ListBox listBoxDeck;
        private System.Windows.Forms.ListBox listBoxDiscard;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label labelVPTokens;
        private System.Windows.Forms.Label labelTotalVP;
        private System.Windows.Forms.ListBox listBoxStack;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBoxAddCard;
        private System.Windows.Forms.Button buttonAddCard;
    }
}
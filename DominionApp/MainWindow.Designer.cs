namespace BaseCodeApp
{
    partial class MainWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.buttonConfigGame = new System.Windows.Forms.Button();
            this.buttonNewGame = new System.Windows.Forms.Button();
            this.labelPhase = new System.Windows.Forms.Label();
            this.labelBuys = new System.Windows.Forms.Label();
            this.labelMoney = new System.Windows.Forms.Label();
            this.pictureBoxCardImage = new System.Windows.Forms.PictureBox();
            this.textBoxLog = new System.Windows.Forms.RichTextBox();
            this.labelActions = new System.Windows.Forms.Label();
            this.labelDecision = new System.Windows.Forms.Label();
            this.buttonDone = new System.Windows.Forms.Button();
            this.labelActiveCard = new System.Windows.Forms.Label();
            this.labelActiveCardName = new System.Windows.Forms.Label();
            this.panelDecision = new System.Windows.Forms.Panel();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.panelPlayArea = new System.Windows.Forms.Panel();
            this.label9 = new System.Windows.Forms.Label();
            this.panelHand = new System.Windows.Forms.Panel();
            this.label8 = new System.Windows.Forms.Label();
            this.panelSupply = new System.Windows.Forms.Panel();
            this.labelError = new System.Windows.Forms.Label();
            this.buttonDebug = new System.Windows.Forms.Button();
            this.buttonNewKingdom = new System.Windows.Forms.Button();
            this.checkBoxUsePrecomputedKingdoms = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCardImage)).BeginInit();
            this.panelDecision.SuspendLayout();
            this.panelPlayArea.SuspendLayout();
            this.panelHand.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonConfigGame
            // 
            this.buttonConfigGame.Location = new System.Drawing.Point(10, 5);
            this.buttonConfigGame.Margin = new System.Windows.Forms.Padding(2);
            this.buttonConfigGame.Name = "buttonConfigGame";
            this.buttonConfigGame.Size = new System.Drawing.Size(92, 24);
            this.buttonConfigGame.TabIndex = 1;
            this.buttonConfigGame.Text = "Game Options";
            this.buttonConfigGame.UseVisualStyleBackColor = true;
            this.buttonConfigGame.Click += new System.EventHandler(this.buttonConfigGame_Click);
            // 
            // buttonNewGame
            // 
            this.buttonNewGame.Location = new System.Drawing.Point(106, 5);
            this.buttonNewGame.Margin = new System.Windows.Forms.Padding(2);
            this.buttonNewGame.Name = "buttonNewGame";
            this.buttonNewGame.Size = new System.Drawing.Size(77, 24);
            this.buttonNewGame.TabIndex = 1;
            this.buttonNewGame.Text = "New Game";
            this.buttonNewGame.UseVisualStyleBackColor = true;
            this.buttonNewGame.Click += new System.EventHandler(this.buttonNewGame_Click);
            // 
            // labelPhase
            // 
            this.labelPhase.AutoSize = true;
            this.labelPhase.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPhase.Location = new System.Drawing.Point(316, 6);
            this.labelPhase.Name = "labelPhase";
            this.labelPhase.Size = new System.Drawing.Size(193, 26);
            this.labelPhase.TabIndex = 2;
            this.labelPhase.Text = "Adam\'s Action Phase";
            // 
            // labelBuys
            // 
            this.labelBuys.AutoSize = true;
            this.labelBuys.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelBuys.Location = new System.Drawing.Point(655, 6);
            this.labelBuys.Name = "labelBuys";
            this.labelBuys.Size = new System.Drawing.Size(75, 26);
            this.labelBuys.TabIndex = 2;
            this.labelBuys.Text = "Buys: 0";
            // 
            // labelMoney
            // 
            this.labelMoney.AutoSize = true;
            this.labelMoney.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelMoney.Location = new System.Drawing.Point(757, 6);
            this.labelMoney.Name = "labelMoney";
            this.labelMoney.Size = new System.Drawing.Size(39, 26);
            this.labelMoney.TabIndex = 2;
            this.labelMoney.Text = "$ 0";
            // 
            // pictureBoxCardImage
            // 
            this.pictureBoxCardImage.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBoxCardImage.Location = new System.Drawing.Point(49, 380);
            this.pictureBoxCardImage.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBoxCardImage.Name = "pictureBoxCardImage";
            this.pictureBoxCardImage.Size = new System.Drawing.Size(215, 336);
            this.pictureBoxCardImage.TabIndex = 0;
            this.pictureBoxCardImage.TabStop = false;
            // 
            // textBoxLog
            // 
            this.textBoxLog.BackColor = System.Drawing.Color.White;
            this.textBoxLog.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxLog.Location = new System.Drawing.Point(788, 37);
            this.textBoxLog.Name = "textBoxLog";
            this.textBoxLog.ReadOnly = true;
            this.textBoxLog.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.textBoxLog.Size = new System.Drawing.Size(415, 414);
            this.textBoxLog.TabIndex = 3;
            this.textBoxLog.Text = "";
            this.textBoxLog.WordWrap = false;
            // 
            // labelActions
            // 
            this.labelActions.AutoSize = true;
            this.labelActions.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelActions.Location = new System.Drawing.Point(542, 6);
            this.labelActions.Name = "labelActions";
            this.labelActions.Size = new System.Drawing.Size(98, 26);
            this.labelActions.TabIndex = 2;
            this.labelActions.Text = "Actions: 0";
            // 
            // labelDecision
            // 
            this.labelDecision.AutoSize = true;
            this.labelDecision.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDecision.Location = new System.Drawing.Point(318, 491);
            this.labelDecision.Name = "labelDecision";
            this.labelDecision.Size = new System.Drawing.Size(283, 26);
            this.labelDecision.TabIndex = 4;
            this.labelDecision.Text = "Select up to four cards to trash:";
            // 
            // buttonDone
            // 
            this.buttonDone.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonDone.Location = new System.Drawing.Point(320, 456);
            this.buttonDone.Name = "buttonDone";
            this.buttonDone.Size = new System.Drawing.Size(58, 32);
            this.buttonDone.TabIndex = 6;
            this.buttonDone.Text = "Done";
            this.buttonDone.UseVisualStyleBackColor = true;
            this.buttonDone.Click += new System.EventHandler(this.buttonDone_Click);
            // 
            // labelActiveCard
            // 
            this.labelActiveCard.AutoSize = true;
            this.labelActiveCard.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelActiveCard.Location = new System.Drawing.Point(383, 463);
            this.labelActiveCard.Name = "labelActiveCard";
            this.labelActiveCard.Size = new System.Drawing.Size(94, 19);
            this.labelActiveCard.TabIndex = 4;
            this.labelActiveCard.Text = "Active Card: ";
            // 
            // labelActiveCardName
            // 
            this.labelActiveCardName.AutoSize = true;
            this.labelActiveCardName.BackColor = System.Drawing.Color.White;
            this.labelActiveCardName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelActiveCardName.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelActiveCardName.Location = new System.Drawing.Point(474, 462);
            this.labelActiveCardName.Name = "labelActiveCardName";
            this.labelActiveCardName.Size = new System.Drawing.Size(57, 21);
            this.labelActiveCardName.TabIndex = 4;
            this.labelActiveCardName.Text = "Chapel";
            // 
            // panelDecision
            // 
            this.panelDecision.AutoScroll = true;
            this.panelDecision.Controls.Add(this.checkBox1);
            this.panelDecision.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panelDecision.Location = new System.Drawing.Point(318, 521);
            this.panelDecision.Name = "panelDecision";
            this.panelDecision.Size = new System.Drawing.Size(885, 173);
            this.panelDecision.TabIndex = 7;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.BackColor = System.Drawing.Color.Lime;
            this.checkBox1.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox1.Location = new System.Drawing.Point(3, 3);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Padding = new System.Windows.Forms.Padding(3, 1, 1, 1);
            this.checkBox1.Size = new System.Drawing.Size(73, 25);
            this.checkBox1.TabIndex = 5;
            this.checkBox1.Text = "Estate";
            this.checkBox1.UseVisualStyleBackColor = false;
            this.checkBox1.Visible = false;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(319, 44);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(94, 26);
            this.label6.TabIndex = 4;
            this.label6.Text = "Play Area";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(316, 219);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(57, 26);
            this.label7.TabIndex = 4;
            this.label7.Text = "Hand";
            // 
            // panelPlayArea
            // 
            this.panelPlayArea.AutoScroll = true;
            this.panelPlayArea.BackColor = System.Drawing.Color.White;
            this.panelPlayArea.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelPlayArea.Controls.Add(this.label9);
            this.panelPlayArea.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panelPlayArea.Location = new System.Drawing.Point(319, 73);
            this.panelPlayArea.Name = "panelPlayArea";
            this.panelPlayArea.Size = new System.Drawing.Size(463, 143);
            this.panelPlayArea.TabIndex = 8;
            // 
            // label9
            // 
            this.label9.AutoEllipsis = true;
            this.label9.AutoSize = true;
            this.label9.BackColor = System.Drawing.Color.White;
            this.label9.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label9.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(3, 3);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(54, 21);
            this.label9.TabIndex = 0;
            this.label9.Text = "Smithy";
            this.label9.Visible = false;
            // 
            // panelHand
            // 
            this.panelHand.AutoScroll = true;
            this.panelHand.BackColor = System.Drawing.Color.White;
            this.panelHand.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelHand.Controls.Add(this.label8);
            this.panelHand.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panelHand.Location = new System.Drawing.Point(319, 248);
            this.panelHand.Name = "panelHand";
            this.panelHand.Size = new System.Drawing.Size(463, 203);
            this.panelHand.TabIndex = 9;
            // 
            // label8
            // 
            this.label8.AutoEllipsis = true;
            this.label8.AutoSize = true;
            this.label8.BackColor = System.Drawing.Color.Gold;
            this.label8.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.label8.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(3, 3);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(57, 21);
            this.label8.TabIndex = 0;
            this.label8.Text = "Copper";
            this.label8.Visible = false;
            // 
            // panelSupply
            // 
            this.panelSupply.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.panelSupply.Location = new System.Drawing.Point(10, 36);
            this.panelSupply.Name = "panelSupply";
            this.panelSupply.Size = new System.Drawing.Size(303, 339);
            this.panelSupply.TabIndex = 10;
            // 
            // labelError
            // 
            this.labelError.AutoSize = true;
            this.labelError.Font = new System.Drawing.Font("Calibri", 12F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelError.ForeColor = System.Drawing.Color.Red;
            this.labelError.Location = new System.Drawing.Point(317, 697);
            this.labelError.Name = "labelError";
            this.labelError.Size = new System.Drawing.Size(269, 19);
            this.labelError.TabIndex = 11;
            this.labelError.Text = "You must select between 0 and 1 cards.";
            // 
            // buttonDebug
            // 
            this.buttonDebug.Location = new System.Drawing.Point(1104, 5);
            this.buttonDebug.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDebug.Name = "buttonDebug";
            this.buttonDebug.Size = new System.Drawing.Size(99, 24);
            this.buttonDebug.TabIndex = 1;
            this.buttonDebug.Text = "Debug Window";
            this.buttonDebug.UseVisualStyleBackColor = true;
            this.buttonDebug.Click += new System.EventHandler(this.buttonDebug_Click);
            // 
            // buttonNewKingdom
            // 
            this.buttonNewKingdom.Location = new System.Drawing.Point(187, 5);
            this.buttonNewKingdom.Margin = new System.Windows.Forms.Padding(2);
            this.buttonNewKingdom.Name = "buttonNewKingdom";
            this.buttonNewKingdom.Size = new System.Drawing.Size(126, 24);
            this.buttonNewKingdom.TabIndex = 1;
            this.buttonNewKingdom.Text = "New Kingdom Cards";
            this.buttonNewKingdom.UseVisualStyleBackColor = true;
            this.buttonNewKingdom.Click += new System.EventHandler(this.buttonNewKingdom_Click);
            // 
            // checkBoxUsePrecomputedKingdoms
            // 
            this.checkBoxUsePrecomputedKingdoms.AutoSize = true;
            this.checkBoxUsePrecomputedKingdoms.Checked = true;
            this.checkBoxUsePrecomputedKingdoms.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxUsePrecomputedKingdoms.Location = new System.Drawing.Point(921, 9);
            this.checkBoxUsePrecomputedKingdoms.Name = "checkBoxUsePrecomputedKingdoms";
            this.checkBoxUsePrecomputedKingdoms.Size = new System.Drawing.Size(178, 18);
            this.checkBoxUsePrecomputedKingdoms.TabIndex = 12;
            this.checkBoxUsePrecomputedKingdoms.Text = "Use precomputed kingdoms";
            this.checkBoxUsePrecomputedKingdoms.UseVisualStyleBackColor = true;
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1211, 725);
            this.Controls.Add(this.checkBoxUsePrecomputedKingdoms);
            this.Controls.Add(this.labelError);
            this.Controls.Add(this.panelSupply);
            this.Controls.Add(this.panelHand);
            this.Controls.Add(this.panelPlayArea);
            this.Controls.Add(this.panelDecision);
            this.Controls.Add(this.buttonDone);
            this.Controls.Add(this.labelActiveCardName);
            this.Controls.Add(this.labelActiveCard);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.labelDecision);
            this.Controls.Add(this.textBoxLog);
            this.Controls.Add(this.labelActions);
            this.Controls.Add(this.labelMoney);
            this.Controls.Add(this.labelBuys);
            this.Controls.Add(this.labelPhase);
            this.Controls.Add(this.buttonDebug);
            this.Controls.Add(this.buttonNewKingdom);
            this.Controls.Add(this.buttonNewGame);
            this.Controls.Add(this.buttonConfigGame);
            this.Controls.Add(this.pictureBoxCardImage);
            this.Font = new System.Drawing.Font("Calibri", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "MainWindow";
            this.Text = "Provinicial";
            this.Load += new System.EventHandler(this.MainWindow_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCardImage)).EndInit();
            this.panelDecision.ResumeLayout(false);
            this.panelDecision.PerformLayout();
            this.panelPlayArea.ResumeLayout(false);
            this.panelPlayArea.PerformLayout();
            this.panelHand.ResumeLayout(false);
            this.panelHand.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonConfigGame;
        private System.Windows.Forms.Button buttonNewGame;
        private System.Windows.Forms.Label labelPhase;
        private System.Windows.Forms.Label labelBuys;
        private System.Windows.Forms.Label labelMoney;
        private System.Windows.Forms.PictureBox pictureBoxCardImage;
        private System.Windows.Forms.RichTextBox textBoxLog;
        private System.Windows.Forms.Label labelActions;
        private System.Windows.Forms.Label labelDecision;
        private System.Windows.Forms.Button buttonDone;
        private System.Windows.Forms.Label labelActiveCard;
        private System.Windows.Forms.Label labelActiveCardName;
        private System.Windows.Forms.Panel panelDecision;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Panel panelPlayArea;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Panel panelHand;
        private System.Windows.Forms.Panel panelSupply;
        private System.Windows.Forms.Label labelError;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button buttonDebug;
        private System.Windows.Forms.Button buttonNewKingdom;
        private System.Windows.Forms.CheckBox checkBoxUsePrecomputedKingdoms;
    }
}


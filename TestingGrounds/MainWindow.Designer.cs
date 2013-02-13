namespace TestingGrounds
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
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.console = new System.Windows.Forms.ListBox();
            this.buttonGo = new System.Windows.Forms.Button();
            this.timerGo = new System.Windows.Forms.Timer(this.components);
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 44);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(406, 19);
            this.label1.TabIndex = 0;
            this.label1.Text = "Chances are you want to start with Provincial.exe instead.";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 70);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(941, 19);
            this.label2.TabIndex = 0;
            this.label2.Text = "Output files are saved to the kingdomsIntermediate directory.  To visualize these" +
    " files, run Provincial.exe and click \"Make Visualizations\".";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 99);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(796, 19);
            this.label3.TabIndex = 0;
            this.label3.Text = "After you run Provincial.exe, the kingdomsFinal directory shows the final generat" +
    "ion for each set of Kingdom cards.";
            // 
            // console
            // 
            this.console.FormattingEnabled = true;
            this.console.ItemHeight = 19;
            this.console.Location = new System.Drawing.Point(16, 185);
            this.console.Name = "console";
            this.console.Size = new System.Drawing.Size(934, 384);
            this.console.TabIndex = 1;
            // 
            // buttonGo
            // 
            this.buttonGo.Location = new System.Drawing.Point(16, 135);
            this.buttonGo.Name = "buttonGo";
            this.buttonGo.Size = new System.Drawing.Size(160, 35);
            this.buttonGo.TabIndex = 2;
            this.buttonGo.Text = "Go!";
            this.buttonGo.UseVisualStyleBackColor = true;
            this.buttonGo.Click += new System.EventHandler(this.buttonGo_Click);
            // 
            // timerGo
            // 
            this.timerGo.Interval = 2000;
            this.timerGo.Tick += new System.EventHandler(this.timerGo_Tick);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 20);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(928, 19);
            this.label5.TabIndex = 0;
            this.label5.Text = "This is a strategizing framework for Dominion.  It repeatedly runs different King" +
    "dom piles and determines good ways to play the cards.";
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 19F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(962, 585);
            this.Controls.Add(this.buttonGo);
            this.Controls.Add(this.console);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "MainWindow";
            this.Text = "Dominion Testing Grounds";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListBox console;
        private System.Windows.Forms.Button buttonGo;
        private System.Windows.Forms.Timer timerGo;
        private System.Windows.Forms.Label label5;
    }
}


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using System.Windows.Forms;


namespace BaseCodeApp
{
    public class Card
    {
        public override string ToString()
        {
            return PrettyName();
        }
        public void LoadBitmaps()
        {
            String filename = "data/" + set + "/" + name.Replace(" ","").Replace("'","") + ".gif";
            try
            {
                fullImage = new Bitmap(filename);
            }
            catch
            {
                MessageBox.Show("Failed to find one of the card images (" + filename + "). Cards are not checked into the Git repository; you will need to download Provincial.zip from http://graphics.stanford.edu/~mdfisher/DominionAI.html.");
                Environment.Exit(0);
            }

            croppedImage = new Bitmap(Constants.croppedWidth, Constants.croppedHeight);

            Graphics g = Graphics.FromImage(croppedImage);

            Rectangle srcRect;
            if(set == "core") srcRect = new Rectangle(24, 123, 271 - 24, 353 - 123);
            else srcRect = new Rectangle(26, 56, 270 - 26, 239 - 56);
            g.DrawImage(fullImage, new Rectangle(0, 0, Constants.croppedWidth, Constants.croppedHeight), srcRect, GraphicsUnit.Pixel);
        }

        public string PrettyName()
        {
            return char.ToUpper(name[0]) + name.Substring(1);
        }

        public Color BackColor()
        {
            if (type == "action") return Color.FromArgb(255, 255, 221);
            else if (type == "action attack") return Color.FromArgb(170, 170, 170);
            else if (type == "victory") return Color.FromArgb(0, 221, 0);
            else if (type == "action reaction") return Color.FromArgb(0, 170, 255);
            else if (type == "curse") return Color.FromArgb(204, 0, 204);
            else if (type == "treasure") return Color.FromArgb(255, 204, 0);
            else if (type == "action victory") return Color.FromArgb(128, (255 + 221) / 2, 221 / 2);
            else if (type == "treasure victory") return Color.FromArgb(128, (204 + 221) / 2, 0);
            else if (type == "action duration") return Color.FromArgb(255, 146, 88);
            else if (type == "action permanent") return Color.FromArgb(255, 136, 204);
            return Color.Fuchsia;
        }

        public string name;
        public string set;
        public string type;
        public Bitmap fullImage;
        public Bitmap croppedImage;
        public int cost;
    }

    public class CardDatabase
    {
        public Card GetCard(string s)
        {
            return cards[s];
        }

        public CardDatabase()
        {
            ProcessSet("core");
            ProcessSet("base");
            ProcessSet("intrigue");
            ProcessSet("seaside");
            ProcessSet("alchemy");
            ProcessSet("prosperity");
            ProcessSet("custom");
        }
        
        void ProcessSet(string set)
        {
            string[] lines = File.ReadAllLines("data/" + set + ".txt");
            List<string> header = new List<string>(lines[0].Split('\t'));
            foreach (String line in lines)
            {
                String[] words = line.Split('\t');
                if (line.StartsWith("name")) continue;
                
                Card newCard = new Card();
                
                newCard.name = words[header.IndexOf("name")];
                newCard.set = set;
                newCard.cost = Convert.ToInt32(words[header.IndexOf("cost")]);
                newCard.type = words[header.IndexOf("type")];
                
                cards[newCard.name] = newCard;
            }
        }

        Dictionary<string, Card> cards = new Dictionary<string, Card>();
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using System.Globalization;


namespace BaseCodeApp
{
    class DominionVisualization
    {
        const int cellWidth = 80;
        const int cellHeight = 55;

        public DominionVisualization(CardDatabase database, String filename, String saveFilename)
        {
            if (File.Exists(saveFilename)) return;

            List<String> lines;
            try
            {
                lines = new List<String>(File.ReadAllLines(filename));
            }
            catch (Exception)
            {
                //
                // File might be in use by the strategization process.
                //
                return;
            }

            if (lines.Count <= 1)
            {
                return;
            }

            LoadCards(database, (lines.Find(delegate(String s) { return s.StartsWith("Kingdom cards:"); }).Split('\t')[1]));

            if (lines[0] == "Leaderboard")
            {
                int leaderStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Leaders:"); });
                int leaderCount = Convert.ToInt32(lines[leaderStartIndex].Split('\t')[1]);

                int tournamentWidth = cellWidth * leaderCount + 75;
                int tournamentHeight = cellHeight * leaderCount + 19 + 35;

                int tournamentX = 1095;

                int imageWidth = tournamentX + tournamentWidth + 20;
                int iamgeHeight = 205 + leaderCount * 90 + 5;
                
                int tournamentY = iamgeHeight / 2 - tournamentHeight / 2;

                SetupGraphics(imageWidth, iamgeHeight);

                DrawKingdomCards(5, 4);

                g.DrawString("Dominant strategies", drawFont, blackBrush, 5, 170);

                for (int leaderIndex = 0; leaderIndex < leaderCount; leaderIndex++)
                {
                    DrawStrategy(database, lines[leaderStartIndex + leaderIndex + 1], ((char)('A' + leaderIndex)).ToString(), true, 5, 205 + leaderIndex * 90);
                }


                DrawTournament(lines, tournamentX, tournamentY);
            }
            else if (lines[0] == "Progression")
            {
                int opponentStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Opponents:"); } );
                int opponentCount = Convert.ToInt32(lines[opponentStartIndex].Split('\t')[1]);

                SetupGraphics(1170, 310 + opponentCount * 90 + 6);

                DrawKingdomCards(5, 4);

                g.DrawString("Leading strategy", drawFont, blackBrush, 5, 165);
                DrawStrategy(database, lines.Find(delegate(String s) { return s.StartsWith("Leader"); }), "", false, -15, 195);

                g.DrawString("Competing strategies", drawFont, blackBrush, 5, 275);
                for (int opponentIndex = 0; opponentIndex < opponentCount; opponentIndex++)
                {
                    DrawStrategy(database, lines[opponentStartIndex + opponentIndex + 1], "", false, -15, 310 + opponentIndex * 90);
                }
            }
            else if (lines[0] == "Generation")
            {
                int leaderStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Leaders:"); });
                int leaderCount = Convert.ToInt32(lines[leaderStartIndex].Split('\t')[1]);

                SetupGraphics(1026, 210 + leaderCount * 90 + 6);

                DrawKingdomCards(5, 4);

                g.DrawString("Dominant strategies", drawFont, blackBrush, 5, 170);

                for (int leaderIndex = 0; leaderIndex < leaderCount; leaderIndex++)
                {
                    DrawStrategy(database, lines[leaderStartIndex + leaderIndex + 1], ((char)('A' + leaderIndex)).ToString(), true, 5, 205 + leaderIndex * 90);
                }
            }
            else if (lines[0] == "Counters")
            {
                //
                // Not yet visualized; counters don't seem to be that useful since as far as I can tell there is almost always a single dominant strategy
                //
            }

            bmp.Save(saveFilename);
            g.Dispose();
            bmp.Dispose();
        }

        void SetupGraphics(int width, int height)
        {
            bmp = new Bitmap(width, height);
            g = Graphics.FromImage(bmp);
            g.Clear(Color.White);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
        }

        void LoadCards(CardDatabase database, String s)
        {
            string[] parts = s.Split('@');
            String[] cardList = parts[0].Split(',');
            foreach (String c in cardList)
            {
                Card curCard = database.GetCard(c);
                if (curCard != null) cards.Add(curCard);
            }

            splitType = "";
            if (parts.Contains("34Split")) splitType = "data/34SplitSmall.png";
            if (parts.Contains("25Split")) splitType = "data/25SplitSmall.png";
            prosperity = parts.Contains("Prosperity");
        }

        static public string FormatPercentage(double d)
        {
            //string result = (d / 100.0).ToString("P", CultureInfo.InvariantCulture).Replace(" %", "%");
            string result = Math.Round(d, 1).ToString();
            if (!result.Contains('.')) result += ".0";
            result += "%";
            if (result.StartsWith("-")) return result;
            else return "+" + result;
        }

        SolidBrush PercentageBrush(double d)
        {
            if (d >= 2.0) return greenBrush;
            if (d <= -2.0) return redBrush;
            return grayBrush;
        }

        void DrawStrategy(CardDatabase database, String s, string leaderName, bool displayRating, int x, int y)
        {
            //Leader 0:	4.18262	e1-d3-p4-gold@99|laboratory@3|militia@1|silver@99
            String[] parts = s.Split('\t');
            g.DrawString(leaderName, bigFont, blackBrush, x, y);

            x += 55;

            double d = Convert.ToDouble(parts[1]);
            g.DrawString(FormatPercentage(d), drawFont, PercentageBrush(d), x, y + 20);

            x += 90;

            int estateThreshold = Convert.ToInt32(parts[2].Split('-')[0].Replace("e",""));
            int duchyThreshold = Convert.ToInt32(parts[2].Split('-')[1].Replace("d", ""));
            int provinceThreshold = Convert.ToInt32(parts[2].Split('-')[2].Replace("p", ""));

            if (prosperity)
            {
                Image template = Bitmap.FromFile("data/prosperityBuyTemplate.png");
                double ratio = (double)template.Height / (double)template.Width;
                g.DrawImage(template, new Rectangle(x, y, 120, (int)(120.0 * ratio)), new Rectangle(0, 0, template.Width, template.Height), GraphicsUnit.Pixel);
                template.Dispose();

                int yOffset = -3;
                int ySpacing = 21;
                g.DrawString(estateThreshold.ToString(), drawFont, blackBrush, x + 60, y + yOffset);
                g.DrawString(duchyThreshold.ToString(), drawFont, blackBrush, x + 60, y + yOffset + ySpacing);
                g.DrawString(provinceThreshold.ToString(), drawFont, blackBrush, x + 60, y + yOffset + ySpacing * 2);
            }
            else
            {
                Image template = Bitmap.FromFile("data/coreBuyTemplate.png");
                double ratio = (double)template.Height / (double)template.Width;
                g.DrawImage(template, new Rectangle(x, y, 120, (int)(120.0 * ratio)), new Rectangle(0, 0, template.Width, template.Height), GraphicsUnit.Pixel);
                template.Dispose();

                int yOffset = 1;
                int ySpacing = 31;
                g.DrawString(estateThreshold.ToString(), drawFont, blackBrush, x + 60, y + yOffset);
                g.DrawString(duchyThreshold.ToString(), drawFont, blackBrush, x + 60, y + yOffset + ySpacing);
            }

            x += 145;

            String[] cardParts = parts[2].Split('-')[3].Split('|');
            for (int cardIndex = 0; cardIndex < cardParts.Length; cardIndex++)
            {
                Card c = database.GetCard(cardParts[cardIndex].Split('@')[0]);
                int count = Convert.ToInt32(cardParts[cardIndex].Split('@')[1]);
                if (c.fullImage == null) c.LoadBitmaps();
                double croppedCardRatio = 252.0 / 296.0;

                int yOffset = 0;
                if (c.set == "core") yOffset = 116;
                g.DrawImage(c.fullImage, new Rectangle(x + cardIndex * 80, y, (int)75.0, (int)(75.0 * croppedCardRatio)), new Rectangle(0, yOffset, c.fullImage.Width, 252), GraphicsUnit.Pixel);

                //if (count > 1 && count < 99)
                if (count > 1)
                {
                    int xOffset = 25;
                    if(count >= 10) xOffset = 17;
                    g.DrawString("x" + count.ToString(), smallFont, blackBrush, x + cardIndex * 80 + xOffset, y + 59);
                }
            }
        }

        void DrawKingdomCards(int x, int y)
        {
            g.DrawString("Kingdom cards", drawFont, blackBrush, 5, 4);

            Image splitImage = Bitmap.FromFile(splitType);
            double splitRatio = (double)splitImage.Width / (double)splitImage.Height;
            g.DrawImage(splitImage, new Rectangle(163, 4, (int)(25.0 * splitRatio), 25), new Rectangle(0, 0, splitImage.Width, splitImage.Height), GraphicsUnit.Pixel);
            splitImage.Dispose();

            if(prosperity)
            {
                Image prosperityImage = Bitmap.FromFile("data/ProsperityLogo.png");
                double ratio = (double)prosperityImage.Width / (double)prosperityImage.Height;
                g.DrawImage(prosperityImage, new Rectangle(363, 4, (int)(25.0 * ratio), 25), new Rectangle(0, 0, prosperityImage.Width, prosperityImage.Height), GraphicsUnit.Pixel);
                prosperityImage.Dispose();
            }

            //if (parts.Contains("34Split")) splitType = "bin/34SplitSmall.png";
            //if (parts.Contains("25Split")) splitType = "bin/25SplitSmall.png";
            //prosperity = parts.Contains("Prosperity");

            const double cardRatio = 473.0 / 296.0;
            for (int cardIndex = 0; cardIndex < cards.Count; cardIndex++)
            {
                Card c = cards[cardIndex];
                if (c.fullImage == null) c.LoadBitmaps();
                g.DrawImage(c.fullImage, new Rectangle(5 + cardIndex * 80, 35, (int)75.0, (int)(75.0 * cardRatio)), new Rectangle(0, 0, c.fullImage.Width, c.fullImage.Height), GraphicsUnit.Pixel);
            }
        }

        void DrawTournament(List<String> lines, int x, int y)
        {
            g.DrawString("Tournament", drawFont, blackBrush, x, y);

            int tableX = x;
            int tableY = y + 35;

            int leaderStartIndex = lines.FindIndex(delegate(String s) { return s.StartsWith("Leaders:"); });
            int leaderCount = Convert.ToInt32(lines[leaderStartIndex].Split('\t')[1]);

            for (int leaderIndex = 0; leaderIndex < leaderCount; leaderIndex++)
            {
                g.DrawString(((char)('A' + leaderIndex)).ToString(), bigFont, blackBrush, tableX + cellWidth * (leaderIndex + 1) + 15, tableY);
                g.DrawString(((char)('A' + leaderIndex)).ToString(), bigFont, blackBrush, tableX, tableY + cellHeight * (leaderIndex + 1));
            }

            int tournamentLineIndex = lines.FindIndex(delegate(String s) { return s == "Tournament"; });
            for (int leaderIndexA = 0; leaderIndexA < leaderCount; leaderIndexA++)
            {
                for (int leaderIndexB = 0; leaderIndexB < leaderCount; leaderIndexB++)
                {
                    double d = Convert.ToDouble(lines[tournamentLineIndex + leaderIndexB + 2].Split('\t')[leaderIndexA + 1]);
                    g.DrawString(FormatPercentage(d), drawFont, PercentageBrush(d), tableX + cellWidth * (leaderIndexA + 1), tableY + cellHeight * (leaderIndexB + 1) + 19);
                }
            }
        }

        List<Card> cards = new List<Card>();
        Bitmap bmp;
        Graphics g;

        String splitType;
        bool prosperity;

        SolidBrush blackBrush = new SolidBrush(Color.Black);
        SolidBrush grayBrush = new SolidBrush(Color.DarkGray);
        SolidBrush redBrush = new SolidBrush(Color.IndianRed);
        SolidBrush greenBrush = new SolidBrush(Color.ForestGreen);
        Font smallFont = new Font("Calibri", 14, FontStyle.Bold);
        Font drawFont = new Font("Calibri", 16, FontStyle.Bold);
        Font bigFont = new Font("Calibri", 42, FontStyle.Bold);
    }
}

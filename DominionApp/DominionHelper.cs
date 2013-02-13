using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Net;
using System.Linq;
using System.Security.Cryptography;

namespace BaseCodeApp
{
    public static class Utility
    {
        public static string FindLargestIndexInDirectory(string dir)
        {
            List<string> files = new List<string>(Directory.EnumerateFiles(dir, "*.txt"));
            string result = "";
            int largestIndexFound = -99999;
            foreach (var f in files)
            {
                try
                {
                    string[] lines = File.ReadAllLines(f);
                    if (lines.Length > 0)
                    {
                        String strippedFilename = f.Replace(dir, "").Replace(".txt", "").Replace("\\", "");
                        int index = Convert.ToInt32(strippedFilename);
                        if (index > largestIndexFound) result = f;
                    }
                }
                catch { }
            }
            return result;
        }

        public static IEnumerable<int> AllIndexesOf(this string str, string value)
        {
            for (int index = 0; ; index += value.Length)
            {
                index = str.IndexOf(value, index);
                if (index == -1)
                    break;
                yield return index;
            }
        }

        public static void SafeFileCopy(string source, string dest)
        {
            if (File.Exists(source) && !File.Exists(dest)) File.Copy(source, dest);
        }

        public static String PhaseName(int phaseIndex)
        {
            if (phaseIndex == 0) return "Action";
            if (phaseIndex == 1) return "Treasure";
            if (phaseIndex == 2) return "Buy";
            if (phaseIndex == 3) return "Cleanup";
            return "Unknown Phase";
        }

        public static string MD5Hash(string input)
        {
            // step 1, calculate MD5 hash from input
            MD5 md5 = System.Security.Cryptography.MD5.Create();
            byte[] inputBytes = System.Text.Encoding.ASCII.GetBytes(input);
            byte[] hash = md5.ComputeHash(inputBytes);

            // step 2, convert byte array to hex string
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < hash.Length; i++)
            {
                sb.Append(hash[i].ToString("X2"));
            }
            return sb.ToString().ToLowerInvariant();
        }
    }

    public class SupplyEntry
    {
        public int index;
        public PictureBox p;
        public Card c;
        public Label nameLabel;
        public Label countLabel;
    }

    class DecisionData
    {
        public string type;
        public int minimumCount;
        public int maximumCount;
    }

    class CardCheckBox : CheckBox
    {
        public CardCheckBox(Card _c) { c = _c; }
        public Card c;
    }

    class CardRadioButton : RadioButton
    {
        public CardRadioButton(Card _c) { c = _c; }
        public Card c;
    }

    class ChoiceRadioButton : RadioButton
    {
        public ChoiceRadioButton(int _choice) { choice = _choice; }
        public int choice;
    }

    class BuildComboBox : ComboBox
    {
        public BuildComboBox(int _index)
        {
            index = _index;
        }
        public int index;
    }

    class QuantityTextBox : TextBox
    {
        public QuantityTextBox(int _index)
        {
            index = _index;
        }
        public int index;
    }
}

/*! \mainpage SMILE backtracking solution
 * Solves smiles problem, using few arrays, permutations and backtracting.<br>
 * Shows smileys, one can input cards in form of id+rotation, as explained above, and will output corresponding cards.
 *
 */



 /* Algorithm - the idea
  * 
  * 9 cards, each 4 possible rotations = 9!*4^9 combinations about 90 bil, not really suited for random solving or pure brute force.
  * Backtracking implemented
  * Every card - is represented as - id of the card,            colors of pieces of the card,   eyes of the card
  *                                  0-9, randomly assigned     red, green, blue, yellow       since every card has the same layout, only one variable needed, eyes have 0 position
  *                                                                                             when there are another eyes to the right, and smile to the left, and another smile at the bottom
  * 
  * So now checking for smiles becomes extremely easy - need to only declare possible combinations when smiley is supoosed to be on the right 2^2, left 2^2, down 2^2 or up 2^2 = 32
  * eg. 00 , 10 - card id 0 and smile on the 0 pos, card id 1 and smile on the 0 pos, they make smiley face for sure, not comparing the color now
  *     03,32,.... the same layout - dont need id for smiley checks, but is useful for colors and rotations
  * 
  * Backtracking - general idea is to append smileys to the string one by one and checking whether they are in correct order, if not, remove only the last one, and try again
  *                if the last one is removed, every other possible combination depending on the wrong one is obviously not gonna cut it, so it can be skipped
  *                which significantly reduces the number of combinations we need to go thru
  *                
  * Rotations    - array containing all possible rotations of every card 9 cards, 4 rotations = 36 , containing id+rotation 00,01,02,03 10,11,12,13...
  * Rotation positions - global - starting rotations of every card - 0 base rotation -> 00,10,20,30,40,50,60,70,80 - need only indexes to the rotations array -> int
  *                      local - for every permutation of id there are local position of rotations so we can perform backtracking
  *                              starging at global - 0,0,0...,0, during each iteration over permutation local positon changes depending on string composition
  *                              once the first index given by the permutation is over 3 - there is no possible solution for this permutation
  *                              follows this pattern (3 ids for shorter explanation, 3 rotations) 
  *                                                     permutation - 210 (lets assume we cant get final string out of this)
  *                                                                   20 ok             000 - local rotations
  *                                                                   2010 wrong        000 -> 010
  *                                                                   2011 wrong        010 -> 020
  *                                                                   2012 wrong        020 -> 001
  *                                                                   2110 ok           001
  *                                                                   211000 wrong      001 -> 101
  *                                                                   211001 wrong      101 -> 201
  *                                                                   211002 wrong      201 -> 002
  *                                                                   ....
  *                                                                   until we get to 221202 222 -> 003 -> end, cant get final string out of this permutation continue with next
  *                                                                   
  * Checking for colors can also be done only based on eyes position - when checking right, color at pos 1, must match color at pos 3, when checking down its 2 and 0
  *         Since we know eyes position and every card is the same, colors will be at the same position - rotation of eyes eg
  *         13 card 1, eyes position 3
  *                 when checking down its (1-3)%4, and (3-3)%4, when checking down its (2-3)%4, and (0-3)%4
  *                 mod because we need something that acts like a circular buffer but we dont need to actually move the elements, only know their indexes, so mod is fine
  *                                                                    
  * Backtracking is done when it either finds the solution or exhausts all the permutations
  *     Solution is found if the string lenght is 18 - 9 cards every card id+eyes position and passes final checking
  *
  *	
  *
  *
  *
  *
  */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Timers;
using System.Windows.Threading;
using System.Threading;

namespace ShowSmileys
{

    public static class ExtensionMethods
    {
        private static Action EmptyDelegate = delegate () { };

        public static void Refresh(this UIElement uiElement)
        {
            uiElement.Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate);
        }
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public static Image[] foo;

        public MainWindow()
        {
            InitializeComponent();
            Program.InitColors();
            Program.InitSmiles();
            Program.InitRotations();
            InitImages();

        }

        public void InitImages()
        {
            foo = new Image[9];
            foo[0] = i1;
            foo[1] = i2;
            foo[2] = i3;
            foo[3] = i4;
            foo[4] = i5;
            foo[5] = i6;
            foo[6] = i7;
            foo[7] = i8;
            foo[8] = i9;
        }

        private void Show_Click(object sender, RoutedEventArgs e)
        {
            var per = Program.FindPermutations("012345678");
            int cnt = 0;
            foreach (var cmb in per)
            {
                if (cnt % 1000 == 0)
                {
                    tTest.Text = cnt.ToString() + "/362800";
                    tTest.Refresh();
                }
                int[] lrotationPos = (int[])Program.rotPos.Clone();

                StringBuilder cmbBuil = new StringBuilder();
                bool next = false;

                while (cmbBuil.Length != 18 && !next)
                {
                    for (int i = 0; i < cmb.Length; i++)
                    {
                        cmbBuil.Append(Program.rotations[int.Parse(cmb[i].ToString())][lrotationPos[int.Parse(cmb[i].ToString())]]);
                        if (!Program.CheckString(cmbBuil.ToString()))
                        {
                            if (!Program.CorrectIndexes(cmb[i], cmb, lrotationPos))
                            {
                                next = true;
                            }
                            else
                            {
                                cmbBuil.Clear();
                                break;
                            }
                        }
                    }
                }
                if (cmbBuil.Length == 18 && Program.CheckString(cmbBuil.ToString()))
                {
                    for(int i = 0; i < cmbBuil.Length; i+=2)
                    {
                        foo[i/2].Source = new BitmapImage(new Uri("/Images/" + cmbBuil.ToString()[i] + cmbBuil.ToString()[i + 1] + ".png", UriKind.Relative));
                    }
                    return;
                }
                cnt++;
            }
        }

        private void bShow_Click(object sender, RoutedEventArgs e)
        {
            string cards = tSmileys.Text;

            for(int i = 0; i < foo.Length; i++)
            {
                foo[i].Source = null;
            }

            try
            {
                for (int i = 0; i < cards.Length; i += 2)
                {
                    foo[i / 2].Source = new BitmapImage(new Uri("/Images/" + cards.ToString()[i] + cards.ToString()[i + 1] + ".png", UriKind.Relative));
                }
            }
            catch(Exception ex)
            {
                tTest.Text = ex.ToString();
            }

        }


        public static class Params
        {
            public const int bSize = 9;  /**< number of cards */
            public const int pDir = 4;   /**< number of possible direction of one card */
            public const int red = 0;    /**< colors of a card */
            public const int blue = 1;
            public const int green = 2;
            public const int yellow = 3;
        }

        public static class Program
        {
            public static string[] corSmileR;       /**< array of possible smile faces - right direction - (: , :)*/
            public static string[] corSmileD;       /**< array of possible smile faces - down direction - :_: , :-: , :) */
            public static string[][] rotations;     /**< rotation of every card represented as string 00,01..., 10,11,..., 20,21...*/
            public static int[] rotPos;             /**< rotation of given card on selected index */
            public static int[][] colors;           /**< colors of each card, starting at 0 index going clockwise */


            /**
            * @brief Init smiles
            *
            * Initiliases smiley faces down/up and right/left facing
            */

            public static void InitSmiles()
            {
                corSmileR = new string[8];
                corSmileD = new string[8];

                corSmileR[0] = "00";
                corSmileR[1] = "01";
                corSmileR[2] = "10";
                corSmileR[3] = "11";
                corSmileR[4] = "33";
                corSmileR[5] = "32";
                corSmileR[6] = "23";
                corSmileR[7] = "22";
                corSmileD[0] = "00";
                corSmileD[1] = "03";
                corSmileD[2] = "30";
                corSmileD[3] = "33";
                corSmileD[4] = "11";
                corSmileD[5] = "12";
                corSmileD[6] = "21";
                corSmileD[7] = "22";
            }


            /**
            * @brief Sets all cards colors
            *
            * Sets color for every card - 4 colors on every card - RED 0, GREEN 1, BLUE 2, YELLOW 3.
            */

            public static void InitColors()
            {
                colors = new int[Params.bSize][];

                for (int i = 0; i < Params.bSize; i++)
                {
                    colors[i] = new int[Params.pDir];
                }

                colors[0][0] = Params.yellow;
                colors[0][1] = Params.blue;
                colors[0][2] = Params.blue;
                colors[0][3] = Params.green;

                colors[1][0] = Params.red;
                colors[1][1] = Params.green;
                colors[1][2] = Params.blue;
                colors[1][3] = Params.yellow;

                colors[2][0] = Params.blue;
                colors[2][1] = Params.yellow;
                colors[2][2] = Params.blue;
                colors[2][3] = Params.green;

                colors[3][0] = Params.blue;
                colors[3][1] = Params.green;
                colors[3][2] = Params.red;
                colors[3][3] = Params.yellow;

                colors[4][0] = Params.yellow;
                colors[4][1] = Params.blue;
                colors[4][2] = Params.red;
                colors[4][3] = Params.blue;

                colors[5][0] = Params.red;
                colors[5][1] = Params.green;
                colors[5][2] = Params.red;
                colors[5][3] = Params.yellow;

                colors[6][0] = Params.blue;
                colors[6][1] = Params.green;
                colors[6][2] = Params.red;
                colors[6][3] = Params.red;

                colors[7][0] = Params.red;
                colors[7][1] = Params.yellow;
                colors[7][2] = Params.blue;
                colors[7][3] = Params.yellow;

                colors[8][0] = Params.green;
                colors[8][1] = Params.yellow;
                colors[8][2] = Params.red;
                colors[8][3] = Params.green;

            }


            /**
            * @brief Initializes card rotations
            *
            * Fills arrays with all possible card rotations
            */
            public static void InitRotations()
            {
                rotations = new string[Params.bSize][];
                rotPos = new int[Params.bSize];
                Random rn = new Random();
                for (int i = 0; i < Params.bSize; i++)
                {
                    rotPos[i] = rn.Next(0, 3);
                    rotations[i] = new string[Params.pDir];
                    for (int j = 0; j < Params.pDir; j++)
                    {
                        rotations[i][j] = i.ToString() + (j).ToString();
                    }
                }
            }

            /**
            * @brief Sets all cards colors
            *
            * @param s1 first card eyes position
            * @param id1 id of first card
            * @param s2 second card eyes position
            * @param id2 id of second card
            * @param pos positon 0 - smile to the right, 1 - smile down
            * 
            * Checks whether two card make smile and if the colors match
            * 
            */

            public static bool CheckSmile(string s1, string id1, string s2, string id2, int pos)
            {
                if (pos == 0)
                    if (corSmileR.Contains(s1 + s2) && colors[int.Parse(id1)][(((1 - int.Parse(s1)) % Params.pDir) + Params.pDir) % Params.pDir] == colors[int.Parse(id2)][(((3 - int.Parse(s2)) % Params.pDir) + Params.pDir) % Params.pDir])
                        return true;
                    else
                        return false;
                else
                    if (corSmileD.Contains(s1 + s2) && colors[int.Parse(id1)][(((2 - int.Parse(s1)) % Params.pDir) + Params.pDir) % Params.pDir] == colors[int.Parse(id2)][(((0 - int.Parse(s2)) % Params.pDir) + Params.pDir) % Params.pDir])
                    return true;
                return false;
            }



            /**
            * @brief Checks for smileys
            *
            * @param cmb string representing cards
            * 
            * Checks if the string is correct - every card next to each other/below makes smiley face, if one pair is off whole string is surely not correct
            */

            public static bool CheckString(string cmb)
            {
                for (int i = 0; i < cmb.Length; i += 2)
                {
                    switch (i / 2)
                    {
                        case 0:
                        case 1:
                        case 3:
                        case 4:
                            if (i + 3 <= cmb.Length) //check right
                            {
                                if (!CheckSmile(cmb[i + 1].ToString(), cmb[i].ToString(), cmb[i + 3].ToString(), cmb[i + 2].ToString(), 0))
                                    return false;
                            }
                            if (i + 1 + 6 <= cmb.Length) // check down
                            {
                                if (!CheckSmile(cmb[i + 1].ToString(), cmb[i].ToString(), cmb[i + 1 + 6].ToString(), cmb[i + 6].ToString(), 1))
                                    return false;
                            }
                            break;
                        case 2:
                        case 5:
                            if (i + 1 + 6 <= cmb.Length) // check down
                                if (!CheckSmile(cmb[i + 1].ToString(), cmb[i].ToString(), cmb[i + 1 + 6].ToString(), cmb[i + 6].ToString(), 1))
                                    return false;
                            break;
                        case 6:
                        case 7:
                            if (i + 3 <= cmb.Length) // check right
                                if (!CheckSmile(cmb[i + 1].ToString(), cmb[i].ToString(), cmb[i + 3].ToString(), cmb[i + 2].ToString(), 0))
                                    return false;
                            break;
                    }
                }
                return true;
            }



            /**
            * @brief Correct rotation positions
            *
            * @param pos current position in string
            * @param cmb string representing cards
            * @param rp array of local rotation positions
            * 
            * There are only 4 rotations - rotation positions [0-3][0-3]...., to get all the possible combinations, one has to iterate thru all of them, incrementing one it failed if 
            *  out of bounds then incrementing the previous one. If the previous one is the first in current string - then terminating.
            */

            public static bool CorrectIndexes(char pos, string cmb, int[] rp)
            {
                int foo = int.Parse(pos.ToString());
                if (cmb.IndexOf(pos) == 0 && rp[int.Parse(pos.ToString())] == 3)
                    return false;

                if (rp[foo] + 1 < Params.pDir)
                {
                    rp[foo]++;
                }
                else
                {
                    rp[foo] = 0;
                    if (!CorrectIndexes(cmb[cmb.IndexOf(pos) - 1], cmb, rp))
                        return false;
                }
                return true;
            }


            /**
            * @brief Backtracking variation on smiley faces
            *
            * 
            * Performs backtracking algorithm on all possible permutations of original ids - 012345678, finishes once it finds a solution, prints it in form of string representing cards and their rotations
            */

            public static void Backtracking()
            {
                var per = FindPermutations("012345678");
                foreach (var cmb in per)
                {
                    int[] lrotationPos = (int[])rotPos.Clone();

                    StringBuilder cmbBuil = new StringBuilder();
                    bool next = false;

                    while (cmbBuil.Length != 18 && !next)
                    {
                        for (int i = 0; i < cmb.Length; i++)
                        {
                            cmbBuil.Append(rotations[int.Parse(cmb[i].ToString())][lrotationPos[int.Parse(cmb[i].ToString())]]);
                            if (!CheckString(cmbBuil.ToString()))
                            {
                                if (!CorrectIndexes(cmb[i], cmb, lrotationPos))
                                {
                                    next = true;
                                }
                                else
                                {
                                    cmbBuil.Clear();
                                    break;
                                }
                            }
                        }
                    }
                    if (cmbBuil.Length == 18 && CheckString(cmbBuil.ToString()))
                    {
                        Console.WriteLine(cmbBuil.ToString());
                        break;
                    }
                }
                Console.ReadKey();
            }


            /**
            * @brief Permutations
            *
            * @param set set of string to find permutations
            * 
            * Finds all permutations of given string.
            */

            public static IEnumerable<string> FindPermutations(string set)
            {
                var output = new List<string>();
                switch (set.Length)
                {
                    case 1:
                        output.Add(set);
                        break;
                    default:
                        output.AddRange(from c in set let tail = set.Remove(set.IndexOf(c), 1) from tailPerms in FindPermutations(tail) select c + tailPerms);
                        break;
                }
                return output;
            }
        }
    }
}


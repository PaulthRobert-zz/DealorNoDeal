#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib> 
#include<climits>
#include <iomanip>
#include <ctime>
#include <conio.h> //used in the countdown timer
#include <fstream>
using namespace std;

/***********************************************************************************
CONSTANTS
************************************************************************************/
//int const ROW = 26;
constexpr int ROW = 26;

int const COL = 3;
int const WIDE = 2; 	//for formatting control
int const PREC = 2; 	//for formatting control - setprecision
int const MAX = 25;		//random number upper limit 
int const MIN = 0;		//random number lower limit
int const ROUNDS = 10; 	//number of rounds limit
int const TIME_LIMIT = 20; //Time control for countdown timer
double winnings = 0; //Global Declaration for winnings

double const OFFERRATE = .1;	//Bank offer calculation rate 10%
double const ROUNDPNLTY = 500;	//Bank offer round penalty
string contName; //Global declaration for multi function purpose
/***********************************************************************************
PROTOTYPES
************************************************************************************/
void intro();							//introduce game / main menu
void rules();							//give user rules
void displayRound(double[][COL], int, double[][COL], int, int);		//added 2nd array parameter to function PD 12.1.18
void setMoneyPool(double[][COL], int); 								//apply money pool values to array
void displayArray(double[][COL], int); 								//display a ROW x COL array
void assignCaseValues(double[][COL], int, double[][COL], int); 		//assign values from money pool to cases randomely

int reserveCase(double[][COL], int, double[][COL], int);			//PD 12.1.18 added 2nd array parameter
int removeCase(double[][COL], int, double[][COL], int, int, int);			//remove cases from play PD 12.1.18 added 2nd array parameter
int match(double[][COL], int, int, double);	//function  to find a matching value in an array and return the row index for that value

double bankOffer(double[][COL], int, double[][COL], int, int, int); //calculates the bank offer
bool choice(double[][COL], int, double, int); 			//each round user chooses between 3 options
bool finalChoice(double[][COL], int, double, int);

void endGame(double); //Displays end game scrreen. Also write the highscore using fstream
void HighScoreFunct(); //Displays the highscore screen

int choiceTimer(bool*);	//pointer parameter passed to function

/************************************************************************************
MAIN MODULE
*************************************************************************************/

/*
int main() {
	//cout << "hello - i love cats" +  ROW;

	cout << "Row = " << ROW;
	

}
*/

int main()
{
	srand(time(0));

	/*DECLARE ARRAY VARIABLES*/
	double moneyPool[ROW][COL];
	double cases[ROW][COL];

	/*DECLARE VARIABLES*/
	int roundNumber = 1,		//for tracking the number of rounds played so far
		reservedCase = -1;	//for storing the case number of the reserve case

	bool play = true;				//exit game control

	/*CALL FUNCTIONS*/
	intro();									//runs the intro menu, and rules
	setMoneyPool(moneyPool, ROW);				//build money pool array
	assignCaseValues(moneyPool, ROW, cases, ROW);	//randomely assigns money values to cases
	reservedCase = reserveCase(cases, ROW, moneyPool, ROW);	//PD 12.1.18  run through reserve case to take the reserve case out of play and store the case number 

	//displayArray(cases,ROW);

	/*LOOP TO CONTROL ROUNDS IN GAME*/
	do
	{
		removeCase(cases, ROW, moneyPool, ROW, roundNumber, reservedCase);  //Have the user remove cases from play each round	
		play = choice(cases, ROW, bankOffer(cases, ROW, moneyPool, ROW, reservedCase, roundNumber), reservedCase); //run choice, allow to exit on an exit choice
		roundNumber++;
	} while (play && roundNumber < ROUNDS);

	if (play && roundNumber == 10)
	{
		displayRound(cases, ROW, moneyPool, ROW, roundNumber);
		finalChoice(cases, ROW, bankOffer(cases, ROW, moneyPool, ROW, reservedCase, roundNumber), reservedCase);
	}

	endGame(winnings);

	cout << "\n\n\t\t\t\tGAME OVER\n\n\n\n";
}

void intro()
{
	int menuInput, round = 1;
	bool menuStatus = false;

	while (!menuStatus)
	{

		cout << " ***      | D E A L    O R    N O    D E A L |       ***\n";
		cout << "________________________________________________________\n";
		cout << "                      MAIN MENU\n";
		cout << "________________________________________________________\n";
		cout << "\n1. Start"
			<< "\n2. Highscore"
			<< "\n3. Quit\n";
		cout << "\nChoose a number from the menu: ";
		cin >> menuInput;

		switch (menuInput)					//menu options control
		{
		case 1: for (int i = 0; i < 3; i++)
		{
			cout << ".\n";
		}
				cout << "------ Game is now starting -----" << endl; //Welcome to DEAL OR NO DEAL
				menuStatus = true; //set bool to true
				break;
		case 2: HighScoreFunct();
			break;
		case 3: for (int i = 0; i < 3; i++)
		{
			cout << ".\n";
		}
				cout << "Goodbye loser...ending program\n";
				exit(0); //Terminate Program Instantly
		default: cout << "You can only select 1 or 2 from the main menu. Try Again." << endl << endl;
		}
	}

	cout << "\n\nWhat is the name of the contestant?";
	cin.ignore(); //When getline() reads from the input, there is a newline character left in the input                  stream, so it doesn't read your c-string
	getline(cin, contName);
	cout << "\n\nWelcome to the show " << contName << "!\n______________________\n";

	rules();
}

void rules()
{
	int tellRules;
	cout << "\nDo you know how to play deal or no deal?\n\n1. Yes, let's begin!\n2. No, tell me the rules!";
	cin >> tellRules;

	while (tellRules != 1 && tellRules != 2)
	{
		while (cin.fail())
		{
			cin.clear(); // clear input buffer to restore cin to a usable state
			cin.ignore(INT_MAX, '\n'); // ignore last input
			cout << "Please enter 1 or 2\n";
			cin >> tellRules;
		}
		cout << "You can only enter 1 or 2"; //notifies user their input was invalid
		cin >> tellRules;
	}
	if (tellRules == 2) {
		cout << "\nDeal or No Deal is a game of chance and risk management. \n\nYou are first presented with 26 options - each one is hiding a dollar value. \n\nWhen you start the game, you will select one option to become your 'reserve' - you are not allowed to know what this option is, but it is NOT removed from play. You can choose to take whatever value is in this reserve option whenever you are presented with a deal from the banker.\n\nThere are nine rounds of play. Each round will have you select from the remaining options, with each option you selected being removed from play. The dollar values hidden in each option will no longer be factored into your banker offer, or otherwise be accessible. The first round you must select six options to remove from play, and following rounds will have you select fewer options, until you are left with only two options to pick from.\n\nAt the end of each round, you will be presented the banker's offer, which is an average of all the remaining dollar values left in play. You will have 20 seconds to decide whether to go for another round and try to eliminate low value options, take the banker's offer, or to take a chance on your reserve option. If you do not decide in 20 seconds, then you must continue on to another round and choose more options to disqualify.\n\nThe goal of the game is to either end up with a high value option, or to take a high value banker's offer. \n\nIf luck is with you, you will disqualify the low dollar values and either get a high value banker's offer or to take your reserve. If your luck is not so good, you might disqualify high value options and doom yourself to a small deal.\n\nDo you understand?\n\n1. I understand, lets begin!\n2. I do not understand.";
		cin >> tellRules;
		while (tellRules != 1 && tellRules != 2)
		{
			while (cin.fail())
			{
				cin.clear(); // clear input buffer to restore cin to a usable state
				cin.ignore(INT_MAX, '\n'); // ignore last input
				cout << "Please enter 1 or 2\n";
				cin >> tellRules;
			}
			cout << "You can only enter 1 or 2"; //notifies user their input was invalid
			cin >> tellRules;
		}
		if (tellRules == 2)
		{
			cout << "__________________\n\nUgh, okay.... Uhhh, look, this is just a game of chance, nevermind the details - just say you understand, follow the prompts, and when the banker is making an offer to you, just try to pick the bigger number, okay? Is that good enough?\n\n__________________\n\n1. Of course, I understand completely.\n2. I do not understand.";

			cin >> tellRules;
			while (tellRules != 1 && tellRules != 2) {
				while (cin.fail())
				{
					cin.clear(); // clear input buffer to restore cin to a usable state
					cin.ignore(INT_MAX, '\n'); // ignore last input
					cout << "Please enter 1 or 2\n";
					cin >> tellRules;
				}
				cout << "You can only enter 1 or 2"; //notifies user their input was invalid
				cin >> tellRules;
			}
			if (tellRules == 2)
			{
				cout << "__________________\n\nOkay... You are the contestant that's supposed to be here, right? You didn't just wander into the studio by mistake?\n\n__________________\n\n1. I'm the contestant. I'm just messin' with you, y'know, as a joke.\n2. I don't understand.";
				cin >> tellRules;
				while (tellRules != 1 && tellRules != 2)
				{
					while (cin.fail())
					{
						cin.clear(); // clear input buffer to restore cin to a usable state
						cin.ignore(INT_MAX, '\n'); // ignore last input
						cout << "Please enter 1 or 2\n";
						cin >> tellRules;
					}
					cout << "You can only enter 1 or 2"; //notifies user their input was invalid
					cin >> tellRules;
				}
				if (tellRules == 2)
				{
					cout << "__________________\n\nYou know what? No deal for you, just go home";
					exit(0); //Terminate Program Instantly
				}
			}
		}
	}
}

void displayRound(double cases[][COL], int SIZE, double money[][COL], int SIZE2, int round)
{
	if (round > 0)
	{
		cout << "\nWelcome to Round " << round << "!!!" << endl << "Here are the cases and dollar amounts left in play!" << endl << endl << endl; //For each loop of the game, the round counter should increase by 1.
	}
	cout << "\n\n   CASES                           DOLLARS" << endl << "------------------------        -------------------------" << endl; //table labels
	for (int i = 0; i < 13; i++)
	{
		//CASE DISPLAY SECTION 
		//	COLUMN 1
		cout.setf(ios::fixed);
		if (cases[i][1] == -1)	//<<update by PD - if the value in column 2 of cases array = 0, it has not been selected yet if -1 it has
			cout << "|" << setprecision(0) << setw(8) << "" << "   ";
		else
			cout << "|" << setprecision(0) << setw(8) << i + 1 << "   ";//These are written so that if the value of the array's element is equal to 0, it will display as a null value.

		//	COLUMN 2
		if (cases[i + 13][1] == -1) //<<update by PD - if the value in column 2 of cases array = 0, it has not been selected yet if -1 it has
			cout << "|" << setprecision(0) << setw(8) << "";
		else
			cout << "|" << setprecision(0) << setw(8) << i + 14; //Each if statement here does essentially the same thing

		cout << "  |        "; //just a spacer

		//MONEY DISPLAY SECTION
		if (money[i][1] == -1) //<<update by PD - if the value in column 2 of money array = -1, the case holding that value has been selected

			cout << "|" << setprecision(0) << setw(8) << "";
		else
			cout << "|" << setprecision(0) << setw(8) << money[i][2];

		cout << "   |  ";

		//	COLUMN 2
		if (money[i + 13][1] == -1) //<<update by PD - if the value in column 2 of money array = -1, the case holding that value has been selected
			cout << setprecision(0) << setw(8) << "" << " |";
		else
			if (money[i + 13][2] < 1)
				cout << setprecision(2) << setw(8) << money[i + 13][2] << " |";
			else
				cout << setprecision(0) << setw(8) << money[i + 13][2] << " |";
		cout << endl << "------------------------        -------------------------" << endl; //to create the illusion of table cells
	}
}

void setMoneyPool(double mPool[][COL], int ROWl)
{
	//pool holds the values that belong in moneyPool - but in order to 
	//keep move this to a function, I needed to seperate the arrays
	double pool[ROW][COL] =
	{ { 1,0,1000000, }, // the money pool
	  { 2,0,750000, },
	  { 3,0,500000, },
	  { 4,0,400000, },
	  { 5,0,300000, },
	  { 6,0,200000, },
	  { 7,0,100000, },
	  { 8,0,75000, },
	  { 9,0,50000, },
	  { 10,0,25000, },
	  { 11,0,10000, },
	  { 12,0,5000, },
	  { 13,0,1000, },
	  { 14,0,750, },
	  { 15,0,500, },
	  { 16,0,400, },
	  { 17,0,300, },
	  { 18,0,200, },
	  { 19,0,100, },
	  { 20,0,75, },
	  { 21,0,50, },
	  { 22,0,25, },
	  { 23,0,10, },
	  { 24,0,5, },
	  { 25,0,1, },
	  { 26,0,0.01, }, };

	for (int i = 0; i < ROW; i++)			//loop an assign the values of this array to the array that was passed to this function
	{
		for (int x = 0; x < COL; x++)
		{
			mPool[i][x] = pool[i][x];
		}
	}
}

void displayArray(double array[ROW][COL], int ROW)
{
	//Control formatting
	cout << fixed << setw(WIDE) << setprecision(PREC) << showpoint;

	for (int i = 0; i < ROW; i++)
	{
		for (int x = 0; x < COL; x++)
		{
			cout << array[i][x] << "\t";
		}
		cout << endl;
	}
}

void assignCaseValues(double moneyArray[][COL], int row1, double caseArray[][COL], int row2)
{

	double cases[ROW][COL];

	int random1 = (rand() % (MAX - MIN + 1)) + MIN; 			//set random1 to a any number between 0-26
	int i = 0;
	for (; i < row1;)									//outer for loop to loop through 0-26
	{
		if (i == 0)										//on the first iteration of the for loop
		{
			cases[i][1] = random1;						//send the value of the random number to the array cases[][]
			caseArray[i][0] = i + 1;						//send the value of i+1 to the caseArray **Set Case Number in Case Array
			caseArray[i][1] = 0;							//set the value of column 2 of casrArray to 0
			moneyArray[random1][1] = i + 1;					//set the value of moneyArray col 2 to case number **memory for which case holds this value
			caseArray[i][2] = moneyArray[random1][2];		//assign the money value from moneyArray **array parameter2 t ** to caseArray column3
			i++;										//incriment i
		}
		else									//on all other loops of the outer for loop **i**
		{
			for (int x = 0; x < i;)					//inner for loop loop for each iteration of i that has occured so far
			{
				if (random1 == cases[x][1])		//check to make sure that random1 isn't a repeat
				{
					random1 = (rand() % (MAX - MIN + 1)) + MIN;	//if random 1 is a repeat the re-set random1 to a any number between 0-26
					x = 0;						//if random1 was a repeat, after resetting it, recheck that it is not another repeat
				}
				else
				{
					x++;						//else random1 was != to this array element so incriment
				}
			}
			cases[i][1] = random1;				//send the value of the random number to the array cases[][]
			caseArray[i][0] = i + 1;				//send the value of i+1 to the caseArray **array parameter 2
			caseArray[i][1] = 0;					//set the value of column 2 of casrArray to 0
			moneyArray[random1][1] = i + 1;			//set the value of moneyArray col 2 to case number **memory for which case holds this value
			caseArray[i][2] = moneyArray[random1][2]; //assign the money value from moneyArray **array parameter2 t ** to caseArray column3
			i++;								//incriment i
		}
	}
}
//int reserveCase(double cases[][COL],int ROW)
int reserveCase(double cases[][COL], int ROW, double money[][COL], int ROW2)
{
	int rCase = -1,
		i = 0,
		j = 0;
	displayRound(cases, ROW, money, ROW, 0); //PD 12.1 Inserted KD's display function

	cout << endl << "Please choose a case to RESERVE: ";
	cin >> rCase;
	while (rCase < 1 || rCase > ROW)
	{
		cout << endl << "<!> ERROR that is an choice! <!>" << endl;
		cout << "Please choose a case to reserve: ";
		cin >> rCase;
	}
	cases[rCase - 1][1] = -1;
	return rCase;
}

int removeCase(double cases[][COL], int ROW, double money[][COL], int ROW2, int roundNumber, int reserveCase)
{
	int removeCase,
		i = 0,
		j = 0,
		howmany = 0, // how many cases to remove
		count = 1;

	displayRound(cases, ROW, money, ROW, roundNumber); //PD 12.1 Inserted KD's display function

	// the switch checks the round # and sets the amount of cases removed in each round
	switch (roundNumber)
	{
	case 1:
		howmany = 6;
		break;
	case 2:
		howmany = 5;
		break;
	case 3:
		howmany = 4;
		break;
	case 4:
		howmany = 3;
		break;
	case 5:
		howmany = 2;
		break;
	default:
		howmany = 1;
	}
	cout << endl << "This is round #" << roundNumber << "!" << endl;
	cout << "You will be removing " << howmany << " cases from play!" << endl;
	while (howmany != 0)
	{
		cout << count << ". Please choose a case to remove: ";
		cin >> removeCase;					//get case to remove

		while (removeCase < 1 || removeCase > ROW || cases[removeCase - 1][1] == -1) //PD 12.1.18 changes ==1 to ==-1 & added reserve case
		{
			cout << endl << "<!> ERROR that is not an option! <!>" << endl;
			cout << "Please choose a case to remove: ";
			cin >> removeCase;					//get case to remove
		}
		cases[removeCase - 1][1] = -1;			//remove chosen case from play
		money[match(money, ROW, 2, cases[removeCase - 1][2])][1] = -1; //call the match function to match the value of the case chosen with its original index in the moneyPool
																//in order to set the money pool flag for that value to -1

		cout << "\nYou chose to remove case #" << removeCase << " which had $" << cases[removeCase - 1][2] << " in it!" << endl;
		howmany = howmany - 1;
		count++;
	}
	return removeCase;
}

int match(double array[][COL], int SIZE, int searchCol, double searchVal)
{
	int i = 0;				//counter variable and arrray index
	bool found = false;		//exit control

	do						//loop to find a match
	{
		if (array[i][searchCol] == searchVal)	//test for match
			found = true;						//if a match is found, exit loop						
		else
			i++;							//if a match is not found - incriment
	} while (!found && i < SIZE);				//exit on found or at end of array

	if (found)
		return(i);							//return the array row index if a match is found			
	else
		return(-1);							//retunrn -1 if a match is not found
}

double bankOffer(double cases[][COL], int ROW, double money[][COL], int ROW2, int reserveCase, int roundNum)
{
	double 	sumMoney = 0,		//variable to hold the value the bank will offer
		countMoney = 0,		//variable to count the number of cases left in play
		avgMoney = 0,			//average in each case
		offer = 0;			//bank offer

//Bank offer = (AVG of $ in remaining Cases * 10%)+(10%*Reserve Case Value)	-(Round Penalty(200) * roundNumber)

	for (int i = 0; i < ROW; i++)
	{
		if ((cases[i][1] != -1) && (i != reserveCase))	//for each case that has not yet been removed from play, including the reserve case
		{
			sumMoney += cases[i][2];					//total the value in each case
			countMoney++;							//count the case
		}
	}
	avgMoney = sumMoney / countMoney;				//calculate the average value in each remaining case
	//Bank offer = (AVG of $ in remaining Cases * 10%)+(10%*Reserve Case Value)	
	offer = ((avgMoney * OFFERRATE) + (cases[reserveCase - 1][2] * OFFERRATE) - (ROUNDPNLTY * (ROUNDS - roundNum)));

	return(offer);			//value in the reserve case


}

//void userPhase() //Phase to Deal or No Deal
bool choice(double cases[][COL], int ROW, double bankOffer, int reserveCase)
{
	int 	choiceDeal,
		deal;
	bool 	play = true,				//variable to allow exit
		timeExp = false,
		goBack = false;


	cout << "\t\n\nThe Bank Offers $ " << bankOffer << endl;
	cout << "\t\nDo you want to: \n"
		<< "\t\t1. Accept the Bank Offer\n"
		<< "\t\t2. No Deal . . . Keep on Playing!!\n\n";
	choiceDeal = choiceTimer(&timeExp);

	// loop so that if time expires - choice is forced

	while (!timeExp && !goBack)
	{
		//Options menue for choice statement
		switch (choiceDeal)
		{
		case 1: //Deal
			winnings = bankOffer;
			cout << "\t\t\nDeal, You have chosen take the bank's offer. You are now going home with $" << winnings << endl << endl;
			play = false;		//set play to false to end game
			break;

		case 2: //No Deal
			cout << "\t\t\nYou have chosen No Deal. You will keep on playing.\n";
			cout << "________________________________________________________\n\n";
			play = true;
			break;
		default: cout << "Your input: " << choiceDeal
			<< "\t\t\nYou may only choose between Deal (1) Reserve Case (2) or No Deal (3)\n>>"; //PD 12.1.18 modified for 3 option
			cout << "\t\n\nThe Bank Offers $ " << bankOffer << endl;
			cout << "\t\nDo you want to: \n"//"take the offer or keep playing?\n1. Deal\n2.No Deal\n\n>> "; PD 12.1/18 modified to allow 3 options
				<< "\t\t1. Accept the Bank Offer\n"
				<< "\t\t2. Go home with your Reserve Case\n"
				<< "\t\t3. No Deal . . . Keep on Playing!!\n\n";
			choiceDeal = choiceTimer(&timeExp);
		}
		goBack = true;
		return(play);
	}
	if (timeExp)
	{
		cout << "\n\n\t\ttimeExp: " << timeExp;
		cout << "\n\n\t\tYou have run out of time! You are now going home with nothing!!\n";
		play = false;
	}

	return(play);
}

bool finalChoice(double cases[][COL], int ROW, double bankOffer, int reserveCase)
{
	int 	choiceDeal,
		deal;
	bool 	play = true,				//variable to allow exit
		timeExp = false,
		goBack = false;


	cout << "\t\n\nThe Bank Offers $ " << bankOffer << endl;
	cout << "\t\nDo you want to: \n"//"take the offer or keep playing?\n1. Deal\n2.No Deal\n\n>> "; PD 12.1/18 modified to allow 3 options
		<< "\t\t1. Accept the Bank Offer\n"
		<< "\t\t2. Go home with your Reserve Case\n"
		<< "\t\t3. Go home with the final case\n\n";
	cin >> choiceDeal;			//PD 12/1/18 removed choice input - replaced it with countdown timer function

   // loop so that if time expires - choice is forced

		   //Options menue for choice statement
	switch (choiceDeal)
	{
	case 1: //Deal
		winnings = bankOffer;
		cout << "\t\t\nDeal, You have chosen take the bank's offer. You are now going home with $" << winnings << endl << endl;
		play = false;		//set play to false to end game
		break;
	case 2: //Reserve Case
		winnings = cases[reserveCase][2];
		cout << "\n\t\tDeal, You have chosen take home your reserve case. You are now going home with $" << winnings << endl << endl;
		play = false;	//set play to false to end game
		break;

	case 3: //Final case
		for (int i = 0; i < ROW; i++)
		{
			if ((cases[i][1] != -1) && (i != reserveCase))
				winnings = cases[i][2];
		}
		cout << "\n\t\tDeal, You have chosen take home the final case. You are now going home with $" << winnings << endl << endl;
		play = false;	//set play to false to end game
		break;
	default: cout << "Your input: " << choiceDeal
		<< "\t\t\nYou may only choose between Deal (1) Reserve Case (2) or No Deal (3)\n>>"; //PD 12.1.18 modified for 3 option
		cout << "\t\n\nThe Bank Offers $ " << bankOffer << endl;
		cout << "\t\nDo you want to: \n"//"take the offer or keep playing?\n1. Deal\n2.No Deal\n\n>> "; PD 12.1/18 modified to allow 3 options
			<< "\t\t1. Accept the Bank Offer\n"
			<< "\t\t2. Go home with your Reserve Case\n"
			<< "\t\t3. No Deal . . . Keep on Playing!!\n\n";
	}
	goBack = true;
	return(play);
}


void endGame(double winCash)
{
	double currentScore = winCash;
	cout << "Congratulations! You won $" << currentScore << " from Deal Or No Deal\nAdding to High Scores...\n\n";

	//writing using fstream. Writing Name for Highscore list.
	ofstream oFile;
	oFile.open("HighScores.txt", ios::app);
	oFile << contName << setw(5) << "$ " << currentScore << endl; //write contestant's name and score.
	oFile.close();
}

void HighScoreFunct() //find a way to sort by score (not required but would be nice)
{
	ifstream inFile; //Open to Read
	string name, score;
	inFile.open("HighScores.txt");

	cout << "________________________________________________________\n" << setw(32) << "HIGH SCORE" << "\n________________________________________________________\n\n";

	for (int i = 1; i <= 5; i++)
	{
		getline(inFile, name);
		cout << i << ". " << name << endl;
	}
	inFile.close();
	cout << "________________________________________________________\n\n";
}

int choiceTimer(bool* expired)		//pointer parameter used to indicate if the time has expired
{
	double startTime = time(0); 		//get time the counter starts
	int second = 0,						//get seconds elapsed
		memory = -1,						//used to trigger output if second changes
		numChoice = 0;					//converts char to int to return an int

	char choice;					//user input choice

	while (second < TIME_LIMIT && !_kbhit())			//Exit while loop when time is up
	{
		second = time(0) - startTime;					//set begining time
		while (memory != second)						//compare memory to begining time - if a second has passed run the loop
		{
			cout << "\t\t\tTime to Decide: ";
			cout << TIME_LIMIT - second << " \r";  		//Display count down timer
			memory = second;					 		//Store the second value to control while loop only when second changes
		}
	}

	if (second == TIME_LIMIT)							//if time expires
	{
		cout << "\n\n\t\tsecond:" << second;
		*expired = true;								//send pointer value exiped to calling function
	}
	else											//if time did not expire - accept user input
	{

		*expired = false;
		choice = _getch();								//store user input
		numChoice = static_cast<int>(choice) - 48;		//convert char input to int and adjust for ascii (only tested on inpute 1,2,3)
		return(numChoice);							//return choice to calling function
	}

}


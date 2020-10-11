/**
 * Author: Lucas Yamamoto
 * Year: 2020
 */

#include <iostream>
#include <fstream>
#include <array>
#include <thread>
#include <chrono>

namespace EscapeCode {
	const char getPosition[]        = "\033[6F";
	const char savePosition[]       = "\033[s";
	const char returnPosition[]     = "\033[u";
	const char setForeground[]      = "\033[30m";
	const char setBackground[]      = "\033[107m";
	const char setEmptyBackground[] = "\033[40m";
	const char resetColor[]         = "\033[m";
}

struct Position
{
	int x, y;
};

void ShowGameScreen(const std::array<std::array<char,3>,3>& game);
void SetupScreen(const std::array<std::array<char,3>,3>& game, const std::string& solution);
void UpdateScreen(const std::array<std::array<char,3>,3>& game, char move);
bool MakeMove(std::array<std::array<char,3>,3>& game, char move, Position& pos);
bool TestSolution(const std::array<std::array<char,3>,3>& game);
void ShowFinalResults(int numCorrect, int numTests);

/**
 * Print game state on screen
 * 
 * @param game Game state matrix
 */
void ShowGameScreen(const std::array<std::array<char,3>,3>& game)
{
	for(size_t i = 0; i < game.size(); i++)
	{
		for(size_t j = 0; j < game[i].size(); j++)
		{
			/* Set color */
			std::cout << EscapeCode::setForeground;
			if(game[j][i] == 'x')
				std::cout << EscapeCode::setEmptyBackground;
			else
				std::cout << EscapeCode::setBackground;
			/* Print tile */
			std::cout << ' ' << ((game[j][i] != 'x') ? game[j][i] : ' ') << ' ';
			/* Reset color */
			std::cout << EscapeCode::resetColor;
		}
		std::cout << std::endl;
	}
}

/**
 * Generate a new game screen
 * 
 * @param game Game state matrix
 * @param solution String with all steps to solve. Valid characters: 'u', 'd', 'l', 'r'
 */
void SetupScreen(const std::array<std::array<char,3>,3>& game, const std::string& solution)
{
	std::cout << '\n';
	ShowGameScreen(game);
	std::cout << '\n' << solution << "\n\n\033[F" << EscapeCode::savePosition << std::endl;
}

/**
 * Update screen created with SetupScreen
 * 
 * @param game Game state matrix
 * @param move Last move. Valid characters: 'u', 'd', 'l', 'r'
 */
void UpdateScreen(const std::array<std::array<char,3>,3>& game, char move)
{
	std::cout << EscapeCode::getPosition;
	ShowGameScreen(game);
	std::cout << EscapeCode::returnPosition << move << EscapeCode::savePosition << std::endl;
}

/**
 * Make a move in the game
 * 
 * @param game Game state matrix
 * @param move Move direction. Valid characters: 'u', 'd', 'l', 'r'
 * @return True if it was successful else false
 */
bool MakeMove(std::array<std::array<char,3>,3>& game, char move, Position& pos)
{
	Position newPos = pos;

	/* Calculates new position */
	switch(move)
	{
	case 'u':
		newPos.y -= 1;
		if(newPos.y < 0)
			return false;
		break;
	case 'd':
		newPos.y += 1;
		if(newPos.y >= (int)game[newPos.x].size())
			return false;
		break;
	case 'l':
		newPos.x -= 1;
		if(newPos.x < 0)
			return false;
		break;
	case 'r':
		newPos.x += 1;
		if(newPos.x >= (int)game.size())
			return false;
		break;
	default:
		return false;
	}
	/* Make move */
	std::swap(game[newPos.x][newPos.y], game[pos.x][pos.y]);
	pos = newPos;

	return true;
}

/**
 * Test if the game was solved
 * 
 * @param game Game state matrix
 * @return True if it's solved else false
 */
bool TestSolution(const std::array<std::array<char,3>,3>& game)
{
	for(int i = 0; i < (int)game.size(); i++)
		for(int j = 0; j < (int)game[i].size(); j++)
			if(game[j][i] != '1' + (j + (int)game.size() * i) && game[j][i] != 'x')
				return false;
	return true;
}

/**
 * Print final results
 * 
 * @param numCorrect Number of successful tests
 * @param numTests Total number of tests
 */
void ShowFinalResults(int numCorrect, int numTests)
{
	/* Print header */
	std::cout << "\033[41m----------[Results]----------\033[m\n";
	/* Set color */
	if(numCorrect == numTests)
		std::cout << "\033[32m";
	else if(numCorrect == 0)
		std::cout << "\033[31m";
	else
		std::cout << "\033[34m";
	/* Print results */
	std::cout << "Your results: " << numCorrect << '/' << numTests << " tests successfully completed\033[m" << std::endl;
}

/**
 * Usage:
 * ./thisprogram testinput.in testoutput.out
 */
int main(int argc, char *argv[])
{
	/* Check args */
	if(argc != 3) {
		std::cerr << "\033[31mUsage:\n" << argv[0] << " testinput.in testoutput.out\033[m" << std::endl;
		exit(1);
	}

	/* Try to open the files */
	std::ifstream inputFile(argv[1]);
	std::ifstream outputFile(argv[2]);
	if(!(inputFile.is_open() && outputFile.is_open())) {
		std::cerr << "Failed to open input and output files." << std::endl;
		exit(2);
	}
	
	/* Read number of tests */
	int numTests;
	inputFile >> numTests;

	/* Execute tests */
	std::array<std::array<char,3>,3> game;
	Position pos;
	std::string solution;
	int correct = 0;
	for(int i = 0; i < numTests; i++)
	{
		/* Display test message */
		std::cout << "\033[41m----------[Test #" << i+1 << "]----------\033[m" << std::endl;

		/* Read array */
		for(int j = 0; j < (int)game.size(); j++)
		{
			for(int k = 0; k < (int)game[j].size(); k++)
			{
				inputFile >> game[k][j];
				if(game[k][j] == 'x')
					pos = {k, j};
			}
		}
		/* Read solution */
		outputFile >> solution;

		/* Display solution */
		bool valid = true;
		SetupScreen(game, solution);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		/* Check if game was unsolvable */
		if(solution == "unsolvable") {
			std::cout << "\033[34mYour output says it doesn't have a solution. Skipping test...\033[m" << std::endl;
			correct++;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		/* Run solution */
		for(char move : solution)
		{
			/* Make move */
			if(!MakeMove(game, move, pos))
			{
				std::cout << "\033[31mInvalid move! Ending this test...\033[m" << std::endl;
				valid = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				break;
			}
			/* Show on screen */
			UpdateScreen(game, move);
			/* Wait to make the next move */
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		/* Display results */
		if(valid) {
			if(TestSolution(game))
			{
				std::cout << "\033[32mTest completed\033[m" << std::endl;
				correct++;
			} else {
				std::cout << "\033[34mTest incompleted\033[m" << std::endl;
			}
		}
	}

	/* Display final results */
	ShowFinalResults(correct, numTests);

	inputFile.close();
	outputFile.close();

	return 0;
}
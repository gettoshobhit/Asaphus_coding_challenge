/**
 * @file asaphus_coding_challenge.cpp
 * @version 1.1
 * @copyright Copyright (c) 2022 Asaphus Vision GmbH
 *
 * The goal is to implement the mechanics of a simple game and write test cases for them.
 * The rules of the game are:
 * - There are two types of boxes, green and blue.
 * - Both can absorb tokens of a given weight, which they add to their own total weight.
 * - Both are initialized with a given initial weight.
 * - After a box absorbs a token weight, it outputs a score.
 * - Green and blue boxes calculate the score in different ways:
 * - A green box calculates the score as the square of the mean of the 3 weights that it most recently absorbed (square of mean of all absorbed weights if there are fewer than 3).
 * - A blue box calculates the score as Cantor's pairing function of the smallest and largest weight that it has absorbed so far, i.e. pairing(smallest, largest), where pairing(0, 1) = 2
 * - The game is played with two green boxes with initial weights 0.0 and 0.1, and two blue boxes with initial weights 0.2 and 0.3.
 * - There is a list of input token weights. Each gets used in one turn.
 * - There are two players, A and B. Both start with a score of 0.
 * - The players take turns alternatingly. Player A starts.
 * - In each turn, the current player selects one of the boxes with the currently smallest weight, and lets it absorb the next input token weight. Each input weight gets only used once.
 * - The result of the absorption gets added to the current player's score.
 * - When all input token weights have been used up, the game ends, and the player with highest score wins.
 *
 * Task:
 * - Create a git repo for the implementation. Use the git repository to have a commit history.
 * - Implement all missing parts, marked with "TODO", including the test cases.
 * - Split the work in reasonable commits (not just one commit).
 * - Make sure the test cases succeed.
 * - Produce clean, readable code.
 *
 * Notes:
 * - Building and running the executable: g++ --std=c++14 asaphus_coding_challenge.cpp -o challenge && ./challenge
 * - Feel free to add a build system like CMake, meson, etc.
 * - Feel free to add more test cases, if you would like to test more.
 * - This file includes the header-only test framework Catch v2.13.9.
 * - A main function is not required, as it is provided by the test framework.
 */

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <vector>
#include <cmath>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


class Box {
public:
	explicit Box(double initial_weight) : weight_(initial_weight) {}
	static std::unique_ptr<Box> makeGreenBox(double initial_weight);
	static std::unique_ptr<Box> makeBlueBox(double initial_weight);
	bool operator<(const Box& rhs) const { return weight_ < rhs.weight_; }

	double getWeight() const { return weight_; }
	double calculateScoreForGreenBox(double weight);
	double calculateScoreForBlueBox(double weight);


protected:

	double weight_;
	std::deque<double> absorbed_weights_;
};

double Box::calculateScoreForGreenBox(double weight)
{
	double score = 0.0;
	absorbed_weights_.emplace_back(weight);
	double mean = 0;

	if (absorbed_weights_.size() > 3) {
		absorbed_weights_.pop_front();
	}

	mean = std::accumulate(absorbed_weights_.begin(), absorbed_weights_.end(), 0.0) / absorbed_weights_.size();
	score = std::pow(mean, 2);
	this->weight_ += weight;
	return score;
}

double Box::calculateScoreForBlueBox(double weight)
{
	double score = 0.0;
	if (!absorbed_weights_.empty()) {
		if (absorbed_weights_.front() > weight) { absorbed_weights_.emplace_front(weight); } // adding smallest weight as first element of deque
		else if (absorbed_weights_.back() < weight) { absorbed_weights_.emplace_back(weight); } // adding largest weight as last element of deque
	}
	else {
		absorbed_weights_.emplace_back(weight);  //adding first element to the deque
	}
	auto smallest = absorbed_weights_.front();
	auto largest = absorbed_weights_.back();
	double sum = smallest + largest;
	score = ((sum) * (sum + 1)) / 2 + largest; //calculating Cantor's pairing function of the smallest and largest weight 
	this->weight_ += weight;
	return score;
}


//Initializing a green box
std::unique_ptr<Box> Box::makeGreenBox(double initial_weight) {
	return std::make_unique<Box>(initial_weight);
}

//Initializing a blue box
std::unique_ptr<Box> Box::makeBlueBox(double initial_weight) {
	return std::make_unique<Box>(initial_weight);
}

class Player {
public:
	void takeTurn(uint32_t input_weight, std::vector<std::unique_ptr<Box>>& boxes) {
		//finding the box with the lowest weight
		auto min_box = std::min_element(boxes.begin(), boxes.end(), [](const auto& a, const auto& b) {
			return *a < *b;
			});

		auto index = std::distance(boxes.begin(), min_box);
		if (index == 0 || index == 1)//green box
		{
			score_ += (*min_box)->calculateScoreForGreenBox(static_cast<double>(input_weight));
			
		}
		else // blue box
		{
			score_ += (*min_box)->calculateScoreForBlueBox(static_cast<double>(input_weight));
			
		}
	}

	double getScore() const { return score_; }

private:
	double score_ = 0.0;
};

std::pair<double, double> play(const std::vector<uint32_t>& input_weights) {
	std::vector<std::unique_ptr<Box>> boxes;
	boxes.emplace_back(Box::makeGreenBox(0.0));
	boxes.emplace_back(Box::makeGreenBox(0.1));
	boxes.emplace_back(Box::makeBlueBox(0.2));
	boxes.emplace_back(Box::makeBlueBox(0.3));

	Player player_A, player_B;

	//Logic for Players taking alternate turns
	for (int i = 0; i < input_weights.size(); i++)
	{

		if (i % 2 == 0)
		{
			player_A.takeTurn(input_weights[i], boxes);
		}
		else
		{
			player_B.takeTurn(input_weights[i], boxes);
		}
	}


	std::cout << "Scores: player A " << player_A.getScore() << ", player B " << player_B.getScore() << std::endl;
	return std::make_pair(player_A.getScore(), player_B.getScore());
}

TEST_CASE("Final scores for first 4 Fibonacci numbers", "[fibonacci4]") {
	std::vector<uint32_t> inputs{ 1, 1, 2, 3 };
	auto result = play(inputs);
	REQUIRE(result.first == 13.0);
	REQUIRE(result.second == 25.0);
}

TEST_CASE("Final scores for first 8 Fibonacci numbers", "[fibonacci8]") {
	std::vector<uint32_t> inputs{ 1, 1, 2, 3, 5, 8, 13, 21 };
	auto result = play(inputs);
	REQUIRE(result.first == 155.0);
	REQUIRE(result.second == 366.25);
}

TEST_CASE("Test absorption of green box", "[green]") {
	// TODO
}

TEST_CASE("Test absorption of blue box", "[blue]") {
	// TODO
}

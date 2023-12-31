#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

const int n = 3;
const int m = 3;
const float cellSize = 50.0f;

int maxCoins(int board[][m], vector<vector<int>>& coin_tracker, int i, int j) {
    if (i >= n || j >= m) {
        return 0;
    }

    if (coin_tracker[i][j] != -1) {
        return coin_tracker[i][j];
    }

    int fromBotom = maxCoins(board, coin_tracker, i + 1, j);
    int fromRight = maxCoins(board, coin_tracker, i, j + 1);

    coin_tracker[i][j] = max(fromBotom, fromRight) + board[i][j];
    return coin_tracker[i][j];
    //worst case is 2^(n+m)
    //doesn't need to recompute any value due to memoization.
   // every cell's value is computed and stored during the first visit, and subsequent recursive calls directly retrieve the value from coin_tracker. 
//O(n * m)
}

void findPath(int board[][m], vector<vector<int>>& dp, int i, int j, vector<pair<int, int>>& path) {
    if (i > n - 1 || j > m - 1) {
        return;
    }

    if (i == n - 1 && j == m - 1) {
        path.push_back({ i, j });
        return;
    }

    if ((i >= 0 && i < n - 1) && (j >= 0 && j < m - 1)) {

        if (dp[i + 1][j] > dp[i][j + 1]) {
            findPath(board, dp, i + 1, j, path);
        }
        else {
            findPath(board, dp, i, j + 1, path);
        }
    }

    if (j == m - 1) {
        findPath(board, dp, i + 1, j, path);
    }

    if (i == n - 1) {
        findPath(board, dp, i, j + 1, path);
    }

    path.push_back({ i, j });
    //worst case 2^(n+m)
    //best case 1
}

void drawBoard(RenderWindow& window, int board[][m], const vector<vector<int>>& coin_tracker) {

    // Define cell size and colors
    const float cellSize = 50.0f;
    Color colorWithCoin(255, 255, 0); // Yellow for cells with coins
    Color colorWithoutCoin(169, 169, 169); // Gray for cells without coins
    Color borderColor(0, 0, 0); // Black for cell borders

    // Draw the board
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            RectangleShape cell(Vector2f(cellSize, cellSize));
            cell.setPosition(j * cellSize, i * cellSize);

            // Set color based on the presence of coins
            if (board[i][j] == 1) {
                cell.setFillColor(colorWithCoin); // Cell with coin
            }
            else {
                cell.setFillColor(colorWithoutCoin); // Cell without coin
            }

            // Set cell border
            cell.setOutlineThickness(2); // Border thickness
            cell.setOutlineColor(borderColor);

            window.draw(cell);
        }
    }
}


void displayMenu(RenderWindow& window) {
    // Load the font
    Font font;
    if (!font.loadFromFile("Fonts/arial.ttf")) {
        cout << "Error loading font" << endl;
        return;
    }



    Text title("Coin Collector Game", font, 100);
    title.setPosition(560, 200);
    title.setFillColor(Color::Black);

    RectangleShape startButton(Vector2f(200, 70));
    startButton.setPosition(860, 700);
    startButton.setFillColor(sf::Color::Blue);

    Text buttonText("Start", font, 30);
    buttonText.setPosition(925, 715);
    buttonText.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed) {
                //returns current position of the mouse cursor
                //by the vector2f map pixel cordinadare onverts oixels to sfml coordinates
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                //to get start  button bounds
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    return; // Return from the function to start the game
                }
            }
        }

        window.clear(Color::White);
        window.draw(title);
        window.draw(startButton);
        window.draw(buttonText);
        window.display();
    }
}

int main() {
    Font font;
    if (!font.loadFromFile("Fonts/arial.ttf")) {
        cout << "Error loading font" << endl;
    }

    RenderWindow window(VideoMode::getDesktopMode(), "Coin Collector Game");

    displayMenu(window); // Show the menu and wait for the start button to be pressed


    int board[n][m] = {
        0, 1, 0,
        0,0,1,
        1,0,0
    };

    vector<vector<int>> dp(n, vector<int>(m, -1));

    int maxCoinsCollected = maxCoins(board, dp, 0, 0);
    vector<pair<int, int>> path;
    findPath(board, dp, 0, 0, path);

    int currentCoinsCollected = 0;

    CircleShape player(25);
    player.setFillColor(Color::Black);
    player.setPosition(0, 0);

    if (board[0][0] == 1) {
        currentCoinsCollected++;
        board[0][0] = 0;
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::KeyPressed) {
                //capturing x coordinste
                float newX = player.getPosition().x;
                float newY = player.getPosition().y;

                if (event.key.code == Keyboard::Right && newX + cellSize < cellSize * m) {
                    newX += cellSize;
                }
                else if (event.key.code == Keyboard::Down && newY + cellSize < cellSize * n) {
                    newY += cellSize;
                }

                int cellX = int(newX / cellSize);
                int cellY = int(newY / cellSize);

               
                float epsilon = 0.01f;
                player.setPosition(cellX * cellSize - epsilon, cellY * cellSize - epsilon);

                if (board[cellY][cellX] == 1) {
                    currentCoinsCollected++;
                    board[cellY][cellX] = 0;
                }

                player.setPosition(newX, newY);

                if (cellX == m - 1 && cellY == n - 1) {
                    // Display max coins and path in a new window
                    RenderWindow resultWindow(VideoMode::getDesktopMode(), "Game Result");
                    /* Text resultText("Max Coins Collected: " + to_string(maxCoinsCollected), font, 20);
                    resultText.setPosition(20, 30);
                    resultText.setFillColor(Color::Black);


                    * Text pathText("Path Followed: ", font, 20);
                    pathText.setPosition(20, 70);

                    for (int i = path.size() - 1; i >= 0; --i) {
                        pathText.setString(pathText.getString() + "(" + to_string(path[i].first) + ", " + to_string(path[i].second) + ") ");
                        pathText.setFillColor(Color::Black);
                    }
                    */

                    while (resultWindow.isOpen()) {
                        Event resultEvent;
                        while (resultWindow.pollEvent(resultEvent)) {
                            if (resultEvent.type == Event::Closed) {
                                resultWindow.close();
                                window.close(); // Close the main window after displaying the result
                            }
                        }

                        resultWindow.clear(Color::White);
                        // resultWindow.draw(resultText);
                         //resultWindow.draw(pathText);

                         // Display "Game Over!" and maximum coins collected
                        Text gameOverText("Game Over!", font, 70);
                        gameOverText.setPosition((resultWindow.getSize().x - gameOverText.getLocalBounds().width) / 2, 245);
                        gameOverText.setFillColor(Color::Red);
                        resultWindow.draw(gameOverText);

                        Text maxCoinsText("Current Coins Collected: " + to_string(currentCoinsCollected), font, 30);
                        maxCoinsText.setPosition((resultWindow.getSize().x - maxCoinsText.getLocalBounds().width) / 2, 350);
                        maxCoinsText.setFillColor(Color::Black);
                        resultWindow.draw(maxCoinsText);

                        // Display total maximum coins and path
                        Text totalMaxCoinsText("Total Maximum Coins: " + to_string(dp[0][0]), font, 30);
                        totalMaxCoinsText.setPosition(800, 420);
                        totalMaxCoinsText.setFillColor(Color::Black);
                        resultWindow.draw(totalMaxCoinsText);

                        Text pathText("Path to Follow to collect maximum coins: ", font, 20);
                        sf::FloatRect pathBounds = pathText.getLocalBounds();
                        pathText.setPosition(70, 500);

                        for (int i = path.size() - 1; i >= 0; --i) {
                            pathText.setString(pathText.getString() + "(" + to_string(path[i].first) + ", " + to_string(path[i].second) + ") ");
                            pathText.setFillColor(Color::Black);
                        }

                        resultWindow.draw(pathText);
                        resultWindow.display();
                    }

                }
            }
        }

        window.clear(Color::White);
        drawBoard(window, board, dp);
        window.draw(player);
        window.display();
    }

    return 0;
}

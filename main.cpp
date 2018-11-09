// Including everything
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <array>
#include <chrono>
#include <thread>

// Create a window surface to draw everything to
sf::RenderWindow window(sf::VideoMode(960, 480), "Snake", sf::Style::Close);

// This function will shift all selections of the snake forward one, and move the first segment in the passed direction
std::array<sf::RectangleShape, 40> movesnake(char dir, std::array<sf::RectangleShape, 40> snake, int len) {
    for (int a = len - 1; a > 0; a--) {
        snake[a] = snake[a - 1];
    }
    switch(dir) {
        case 'u':
            snake[0].move(0, -16);
            break;
        case 'd':
            snake[0].move(0, 16);
            break;
        case 'l':
            snake[0].move(-16, 0);
            break;
        case 'r':
            snake[0].move(16, 0);
            break;
    }
    return snake;
}

// Pseudorandom number generation to create a random position for any food items
// The 58 and 28 are the number of 16*16 tiles
sf::Vector2f newfood() {
    return sf::Vector2f((rand() % 58 + 1)*16, (rand() % 28 + 1)*16);
}

// This function does everything necessary to check the snake has done anything interesting, like hitting the edge or itself
std::array<sf::RectangleShape, 40> checksnake (std::array<sf::RectangleShape, 40> snake, int *length, char *dir, bool *food) {
    sf::Music gameover, victory;
    if (!gameover.openFromFile("GameOver.wav")) {
        std::cout << "ERROR" << std::endl;
    }
    if (!victory.openFromFile("Victory.wav")) {
        std::cout << "ERROR" << std::endl;
    }
    if (snake[0].getPosition().x < 16 || snake[0].getPosition().x > 928 || snake[0].getPosition().y < 16 || snake[0].getPosition().y > 448) {
        window.clear(sf::Color::Red);
        window.display();
        gameover.play();
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        *length = 2;
        *food = false;
        *dir = 'r';
        snake[0].setPosition(sf::Vector2f(256, 256));
        snake[1].setPosition(sf::Vector2f(240, 256));
    }
    for (int a = 1; a < *length; a++) {
        if (snake[0].getPosition() == snake[a].getPosition()) {
            window.clear(sf::Color::Red);
            window.display();
            gameover.play();
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
            *length = 2;
            *food = false;
            *dir = 'r';
            snake[0].setPosition(sf::Vector2f(256, 256));
            snake[1].setPosition(sf::Vector2f(240, 256));
        }
    }
    // Check for winning conditions of snake being 40 segments long
    if (*length == 40) {
        window.clear(sf::Color::Green);
        window.display();
        victory.play();
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        *length = 2;
        *food = false;
        *dir = 'r';
        snake[0].setPosition(sf::Vector2f(256, 256));
        snake[1].setPosition(sf::Vector2f(240, 256));
        window.setFramerateLimit(5 + (*length / 2));
    }
    return snake;
}

// Function to add a segment to the snake; it duplicates the size and colour properties then places it on top of the last segment
// The last segment then moves off into its next position during the same iteration of the loop
std::array<sf::RectangleShape, 40> add_segment(std::array<sf::RectangleShape, 40> snake, int length) {
    snake[length] = snake[0];
    snake[length].setPosition(sf::Vector2f(snake[length - 1].getPosition()));
    return snake;
}


// Everything comes together here
int main() {
    sf::Music chomp;
    if (!chomp.openFromFile("Chomp.wav")) {
        return -1;
    }
    // Seed the pseudorandom number generator with the system time
    srand(time(NULL));
    // Set the window location to the top left of the screen
    window.setPosition(sf::Vector2i(0, 0));
    // Set the FPS/Game loop iterations to 10 per second
    int snakelength = 2;
    window.setFramerateLimit(5 + (snakelength / 2));
    // Don't allow holding of keys to generate enormous amounts of key pressed events
    window.setKeyRepeatEnabled(false);
    // Create SFML textures for the stone and apple, which is essentially a holder for an image file
    sf::Texture mcstone, mcapple;
    if (!mcstone.loadFromFile("MC_Stone.png")) {
        std::cout << "ERROR" << std::endl;
    }
    if (!mcapple.loadFromFile("MC_Apple.png")) {
        std::cout << "ERROR" << std::endl;
    }
    // Tell the stone to tile
    mcstone.setRepeated(true);
    // Create variables including the std::array of RectangleShapes
    bool food = false;
    char snakedir = 'r';
    std::array<sf::RectangleShape, 40> snake;
    // Create the first segment of snake then make a second copy of it and move them to the correct starting positions
    snake[0] = sf::RectangleShape(sf::Vector2f(16, 16));
    snake[0].setFillColor(sf::Color(128, 0, 0));
    snake[0].setPosition(sf::Vector2f(256, 256));
    snake[0].setOutlineThickness(-2);
    snake[0].setOutlineColor(sf::Color(64, 0, 0));
    snake[1] = snake[0];
    snake[1].setPosition(sf::Vector2f(240, 256));
    // Create an SFML sprite which allows a texture mcapple to be drawn to the screen
    sf::Sprite apple;
    apple.setTexture(mcapple);
    // Create an SFML RectangleShape tiled with the mcstone texture
    sf::RectangleShape stone(sf::Vector2f(928, 448));
    stone.setTexture(&mcstone);
    stone.setTextureRect(sf::IntRect(0, 0, 928, 448));
    // To add the border of grey colour, begin drawing the stone background one tile in from the edge
    // Then add an outline 16 pixels thick going outwards of grey colour
    stone.setPosition(sf::Vector2f(16, 16));
    stone.setOutlineThickness(16);
    stone.setOutlineColor(sf::Color(32, 32, 32));
    // Main game loop
    while(window.isOpen()) {
        // Event handling
        sf::Event event;
        while(window.pollEvent(event)) {
            switch(event.type){
                // Close the window if someone clicks the close button
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    // Handle all key presses
                    if (event.key.code == sf::Keyboard::Up && snakedir != 'd') {
                        snakedir = 'u';
                    }
                    else if (event.key.code == sf::Keyboard::Down  && snakedir != 'u') {
                        snakedir = 'd';
                    }
                    else if (event.key.code == sf::Keyboard::Left && snakedir != 'r') {
                        snakedir = 'l';
                    }
                    else if (event.key.code == sf::Keyboard::Right && snakedir != 'l') {
                        snakedir = 'r';
                    }
                    break;
                default:
                    break;
            }
        }
        // Empty the window with an arbitrary black background which nobody will see
        window.clear(sf::Color::Black);
        // Draw the large stone RectangleShape and its border
        window.draw(stone);
        // Move the snake
        snake = movesnake(snakedir, snake, snakelength);
        // Check the snake hasn't done anything interesting
        snake = checksnake(snake, &snakelength, &snakedir, &food);
        // If the snake has moved onto the food
        if (apple.getPosition() == snake[0].getPosition()) {
            // Make the snake longer and set food to false so a new food is generated
            snake = add_segment(snake, snakelength);
            snakelength += 1;
            food = false;
            chomp.play();

        }
        if (food == false) {
            // Make a new food for the snake to eat
            apple.setPosition(newfood());
            food = true;
        }
        // Draw the apple and the snake
        for (int a = 0; a < snakelength; a++) {
            window.draw(snake[a]);
        }
        window.draw(apple);
        // Finally, draw the window buffer to the actual screen
        window.display();
        window.setFramerateLimit(5 + (snakelength / 4));
    }
    return 0;
}

#include <SFML/Graphics.hpp>

constexpr int FPS = 30;

class Game {
private:
    sf::RenderWindow window;
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    sf::Vector2i windowOffset;
public:
    Game(int width, int height, const std::string& name, int posX = 0, int posY = 0);
    void Play(void);

private:
    void LoadTextureFromFile(const std::string& filePath, sf::Texture& texture, sf::Sprite& sprite);
    void HandleEvents(void);
    void HandleKeyEvent(sf::Event::KeyEvent& keyEvent);
    void Render(void);
};

Game::Game(int width, int height, const std::string &name, int posX, int posY) 
    : window{sf::VideoMode(width, height), name}, windowOffset{sf::Vector2i{posX, posY}}
{
    window.setFramerateLimit(FPS);
    window.setPosition(windowOffset);
    LoadTextureFromFile("imagine.png", playerTexture, playerSprite);
}

void Game::Play(void)
{
    while(window.isOpen())
    {
        HandleEvents();
        // UpdateState();
        Render();
    }
}

void Game::LoadTextureFromFile(const std::string &filePath, sf::Texture &texture, sf::Sprite &sprite)
{
    if(texture.loadFromFile(filePath) == false)
    {
        exit(EXIT_FAILURE);
    }
    sprite.setTexture(texture);
}

void Game::HandleEvents(void)
{
    // event loop
    sf::Event event;
    printf("Event loop\n");
    while(window.pollEvent(event) == true)
    {
        switch(event.type)
        {
            case sf::Event::EventType::Closed: {
                window.close();
                break;
            }
            case sf::Event::EventType::KeyPressed: {
                sf::Event::KeyEvent& keyEvent = event.key;
                HandleKeyEvent(keyEvent);
                break;
            }
            case sf::Event::EventType::MouseButtonPressed: {
                playerSprite.setPosition(sf::Vector2f{(float)event.mouseButton.x, (float)event.mouseButton.y});
                break;
            }
            default: {
                break;
            }
        }

    }
}

void Game::HandleKeyEvent(sf::Event::KeyEvent &keyEvent)
{
    sf::Vector2f position = playerSprite.getPosition();
    int dx = 10;
    int dy = 10;
    switch(keyEvent.code)
    {

        case sf::Keyboard::Key::Left: 
        case sf::Keyboard::Key::A: 
        {
            playerSprite.setPosition(sf::Vector2f(position.x - dx, position.y));
            break;
        }
        case sf::Keyboard::Key::Right: 
        case sf::Keyboard::Key::D: 
        {
            playerSprite.setPosition(sf::Vector2f(position.x + dx, position.y));
            break;
        }
        case sf::Keyboard::Key::Up: 
        case sf::Keyboard::Key::W: 
        {
            playerSprite.setPosition(sf::Vector2f(position.x, position.y - dy));
            break;
        }
        case sf::Keyboard::Key::Down: 
        case sf::Keyboard::Key::S: 
        {
            playerSprite.setPosition(sf::Vector2f(position.x, position.y + dy));
            break;
        }
    }
}

void Game::Render(void)
{
    window.clear();
    window.draw(playerSprite);
    window.display();
}

int main()
{
    // TODO - constexpr pentru fiecare "numar magic"
    Game game{800, 600, "Test", 100, 100};
    game.Play();
    return 0;
}


#include "Game.h"


constexpr int NUMTILES = 16;

// aici initializam toate variabilele din clasa
// spre exemplu, asset-urile nu dorim sa le incarcam de pe disc la fiecare frame, 
// ar fi costisitor, si am solicita discul degeaba
Game::Game(unsigned int width, unsigned int height, std::string title) : window(sf::VideoMode(width, height), title)
{
    window.setFramerateLimit(FRAMES_PER_SECOND);
    window.setPosition(sf::Vector2i(WINDOW_POS_X, WINDOW_POS_Y));
    LoadAssets();
}

void Game::Play(void)
{
    while (window.isOpen() == true)
    {
        // intre frame-uri, utilizatorul face diverse actiuni cu fereastra (click-uri de mouse, tastatura, etc)
        // acestea se inregistreaza drept evenimente si sunt tratate la fiecare frame
        HandleEvents();

        // starea jocului s-a schimbat, spre exemplu actualizam pozitia unui obiect in urma apasarii unei taste, etc.
        UpdateGameState();

        // desenam sprite-urile frame-ului curent dupa ce au fost actualizate
        Render();
    }
}

// de la ultimul frame pana acum, este posibil sa se fi intamplat diverse evenimente
// ne ocupam de fiecare in parte, rand pe rand
void Game::HandleEvents(void)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
            {
                window.close();
                break;
            }
            default:
            {
                std::cerr << "Unhandled event" << EventTypeToString(event.type) << std::endl;
            }
        }
    }
}

// aici punem logica, ce se schimba de la frame la frame
void Game::UpdateGameState(void)
{

}

// aici incarcam asset-urile de pe disc in variabile (RAM) la initializarea jocului
void Game::LoadAssets(void)
{
    std::string backgroundSpritePath = "assets/images/background.png";
    LoadGameObjectFromFile(backgroundSpritePath, background);

    for (int i = 0; i < NUMTILES; i++)
    {
        char tilePath[128];
        sprintf_s(tilePath, "assets/tiles/tile%02d.png", i);
        std::cout << tilePath << std::endl;
        std::string tilePathStr = tilePath;
        GameObject tile;
        LoadGameObjectFromFile(std::string(tilePath), tile);
        tiles.push_back(tile);
    }
}

// nu exista o functie directa in libraria SFML ca sa putem incarca un sprite (ce desenam) din fisier
// deci trebuie o implementare in cadrul proiectului pentru a nu tot repeta acelasi cod pentru fiecare sprite
void Game::LoadGameObjectFromFile(const std::string& filePath, GameObject& gameObject)
{
    if (gameObject.texture.loadFromFile(filePath) == false)
    {
        // un mesaj de eroare implicit este printat 
        exit(EXIT_FAILURE);
    }
    gameObject.sprite.setTexture(gameObject.texture);
}

// aici desenam fiecare frame
void Game::Render(void)
{
    // stergem ecranul pentru a desena de la zero noul frame
    ClearWindow();

    // desenam sprite-urile pe ecran
    DrawBackground();

    // afisam continutul desenat in fereastra
    DisplayWindow();
}

void Game::DrawBackground(void)
{
    window.draw(background.sprite);
}

// afisam ce am desenat
void Game::DisplayWindow(void)
{
    window.display();
}

// stergem continutul ferestrei astfel incat sa putem desena urmatorul frame
void Game::ClearWindow(void)
{
    window.clear();
}

std::string Game::EventTypeToString(sf::Event::EventType& eventType)
{
    switch (eventType) {
    case sf::Event::Closed:                 return "Closed";
    case sf::Event::Resized:                return "Resized";
    case sf::Event::LostFocus:              return "LostFocus";
    case sf::Event::GainedFocus:            return "GainedFocus";
    case sf::Event::TextEntered:            return "TextEntered";
    case sf::Event::KeyPressed:             return "KeyPressed";
    case sf::Event::KeyReleased:            return "KeyReleased";
    case sf::Event::MouseWheelMoved:        return "MouseWheelMoved (deprecated)";
    case sf::Event::MouseWheelScrolled:     return "MouseWheelScrolled";
    case sf::Event::MouseButtonPressed:     return "MouseButtonPressed";
    case sf::Event::MouseButtonReleased:    return "MouseButtonReleased";
    case sf::Event::MouseMoved:             return "MouseMoved";
    case sf::Event::MouseEntered:           return "MouseEntered";
    case sf::Event::MouseLeft:              return "MouseLeft";
    case sf::Event::JoystickButtonPressed:  return "JoystickButtonPressed";
    case sf::Event::JoystickButtonReleased: return "JoystickButtonReleased";
    case sf::Event::JoystickMoved:          return "JoystickMoved";
    case sf::Event::JoystickConnected:      return "JoystickConnected";
    case sf::Event::JoystickDisconnected:   return "JoystickDisconnected";
    case sf::Event::TouchBegan:             return "TouchBegan";
    case sf::Event::TouchMoved:             return "TouchMoved";
    case sf::Event::TouchEnded:             return "TouchEnded";
    case sf::Event::SensorChanged:          return "SensorChanged";
    default:                                return "UnknownEventType";
    }
}
#include "Game.h"

// avem 16 imagini diferite in total
constexpr int NUMTILES = 7;

// dimensiunea uni tile este 140x140 pixeli
constexpr int TILE_WIDTH = 140;
constexpr int TILE_HEIGHT = TILE_WIDTH;

// o sa avem o bara de scor jos
constexpr int SCORE_OFFSET = 90;

// daca doriti, puteti sa il scalati diferit
constexpr float SCALE_X = 0.5f;
constexpr float SCALE_Y = SCALE_X;


// pozitia de la care incepe tabla 
constexpr float START_X = 100;
constexpr float START_Y = 100;


// dimensiunea actuala a tablei tinand cont de numarul de piese, dimensiunea unei piese, si coeficientul de scalare
const int BOARD_SIZE_X_PIXELS = BOARD_SIZE_X * TILE_WIDTH * SCALE_X;
const int BOARD_SIZE_Y_PIXELS = BOARD_SIZE_Y * TILE_HEIGHT * SCALE_Y;


// padding intre tiles ca sa nu fie inghesuite
const int PADDING = 5; // pixeli

// aici initializam toate variabilele din clasa
// spre exemplu, asset-urile nu dorim sa le incarcam de pe disc la fiecare frame, 
// ar fi costisitor, si am solicita discul degeaba
Game::Game(unsigned int width, unsigned int height, std::string title) : window(sf::VideoMode(width, height), title)
{
    srand(time(NULL));

    // jocul va avea 60 FPS si pozitia ferestrei este setata mai jos
    constexpr int FRAMES_PER_SECOND = 30;
    window.setFramerateLimit(FRAMES_PER_SECOND);
    window.setPosition(sf::Vector2i(WINDOW_POS_X, WINDOW_POS_Y));

    // aici vom memora toate piesele, si stim ca in cadrul jocului sunt NUMTILES piese, prealocam memoria
    tiles.resize(NUMTILES);

    shouldUpdateState = true;

    level = 1;
    // incarcam de pe disc toate imaginile necesare in joc
    LoadAssets();

    // pregatim tabla pentru nivelul 1
    SetupBoard();
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
            case sf::Event::Resized:
            {
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                sf::Event::MouseButtonEvent mouseEvent = event.mouseButton;
                if (mouseEvent.button == sf::Mouse::Button::Left)
                {
                    printf("Left Mouse button pressed at (%d, %d)\n", mouseEvent.x, mouseEvent.y);
                    shouldUpdateState = true;
                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                sf::Event::KeyEvent keyEvent = event.key;

                if (keyEvent.code == sf::Keyboard::Key::A)
                {
                    std::cout << "Am apasat tasta a" << std::endl;
                }
                break;
            }
            default:
            {
                // std::cerr << "Unhandled event" << EventTypeToString(event.type) << std::endl;
                break;
            }
        }
    }
}

// aici punem logica, ce se schimba de la frame la frame (daca s-a schimbat ceva)
void Game::UpdateGameState(void)
{
    if (shouldUpdateState == false)
    {
        return;
    }

    SetupBoard();

    shouldUpdateState = false;
}

void Game::SetupBoard(void)
{
    // pozitia curenta a fiecarei piese de pe tabla
    sf::Vector2f currentTilePosition;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            currentTilePosition.x = START_X + i * TILE_WIDTH  * SCALE_X + i * PADDING;
            currentTilePosition.y = START_Y + j * TILE_HEIGHT * SCALE_Y + j * PADDING;

            int index = rand() % NUMTILES;
            board[i][j] = tiles[index];
            board[i][j].position = sf::Vector2f(currentTilePosition.x, currentTilePosition.y);
            board[i][j].scale = sf::Vector2f(SCALE_X, SCALE_Y);
            board[i][j].sprite.setPosition(board[i][j].position);
            board[i][j].sprite.setScale(board[i][j].scale);
        }
    }
}

// aici desenam fiecare frame
void Game::Render(void)
{
    // stergem ecranul pentru a desena de la zero noul frame
    ClearWindow();

    // desenam sprite-urile pe ecran, incepand cu fundalul
    DrawBackground();

    // desenam tabla de joc
    DisplayBoard();

    // afisam continutul desenat in fereastra
    DisplayWindow();
}

void Game::DisplayBoard(void)
{
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            window.draw(board[i][j].sprite);
        }
    }
};


// aici incarcam asset-urile de pe disc in variabile (RAM) la initializarea jocului
void Game::LoadAssets(void)
{
    std::string backgroundSpritePath = "assets/images/background.jpg";
    LoadGameObjectFromFile(backgroundSpritePath, backgroundImage);

    char currentTilePath[64];
    for (int i = 0; i < NUMTILES; i++)
    {
        sprintf_s(currentTilePath, "assets/tiles/tile%02d.png", i);
        LoadGameObjectFromFile(currentTilePath, tiles[i]);
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

void Game::DrawBackground(void)
{
    window.draw(backgroundImage.sprite);
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
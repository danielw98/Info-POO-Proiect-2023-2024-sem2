#include "Game.h"
// avem 7 imagini diferite in total
constexpr int NUMTILES = 7;

// dimensiunea unui tile este 140x140 pixeli
constexpr int TILE_WIDTH = 140;
constexpr int TILE_HEIGHT = TILE_WIDTH;

// o sa avem o bara de scor jos
constexpr int SCORE_OFFSET = 90;

// daca doriti, puteti sa il scalati diferit
constexpr float SCALE_X = 0.5f;
constexpr float SCALE_Y = SCALE_X;


// pozitia de la care incepe tabla 
constexpr int START_X = 100;
constexpr int START_Y = 100;


// dimensiunea actuala a tablei tinand cont de numarul de piese, dimensiunea unei piese, si coeficientul de scalare
constexpr int BOARD_SIZE_X_PIXELS = static_cast<int>(BOARD_SIZE_X * TILE_WIDTH * SCALE_X);
constexpr int BOARD_SIZE_Y_PIXELS = static_cast<int>(BOARD_SIZE_Y * TILE_HEIGHT * SCALE_Y);


// padding intre tiles ca sa nu fie inghesuite
constexpr int PADDING = 5; // pixeli

// culoare val max
constexpr int RGBA_MAX[4] = { 255, 255, 255, 255 };

// aici initializam toate variabilele din clasa
// spre exemplu, asset-urile nu dorim sa le incarcam de pe disc la fiecare frame, 
// ar fi costisitor, si am solicita discul degeaba
Game::Game(unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, std::string title) 
    : window(sf::VideoMode(width, height), title)
{

    // PRNG cu seed timpul curent pentru a avea o tabla de joc diferita mereu
    srand(time(NULL));

    // jocul va avea 60 FPS si pozitia ferestrei este setata mai jos
    constexpr int FRAMES_PER_SECOND = 30;
    window.setFramerateLimit(FRAMES_PER_SECOND);
    window.setPosition(sf::Vector2i(posX, posY));
    
    // aici vom memora toate piesele, si stim ca in cadrul jocului sunt NUMTILES piese, prealocam memoria
    tiles.resize(NUMTILES);
    shouldUpdateState = false;
    isSwapping = false;
    isMovingAnimationOn = false;
    level = 1;
    score = 0;
    numClicks = 0;
    boardRect = sf::IntRect(START_X, START_Y, NUMTILES*TILE_WIDTH, NUMTILES*TILE_HEIGHT);
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
                sf::Event::MouseButtonEvent mouseButtonEvent = event.mouseButton;
                sf::Vector2i mouseCrtPos = sf::Vector2i(mouseButtonEvent.x, mouseButtonEvent.y);
                if (mouseButtonEvent.button == sf::Mouse::Button::Left)
                {
                    // am dat click in afara tablei, n-are sens sa facem nimic
                    if (boardRect.contains(mouseCrtPos) == false)
                    {
                        isSwapping = false;
                        numClicks = 0;
                        break;
                    }

                    shouldUpdateState = true;

                    mousePos = mouseCrtPos;
                    
                    if (isSwapping == false && isMovingAnimationOn == false)
                        numClicks++;
                }
                break;
            }
            /*case sf::Event::KeyPressed:
            {
                sf::Event::KeyEvent keyEvent = event.key;

                if (keyEvent.code == sf::Keyboard::Key::A)
                {
                    std::cout << "Am apasat tasta a" << std::endl;
                }
                break;
            }*/
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
        return;
    printf("ShouldUpdateState()\n");
    MouseClickUpdateCallback();
    MatchFinding();
    MovingAnimationOnSwap(); // TODO: de verificat
    DeletingAnimation(); // TODO: de verificat
    CheckNoMatch();
    UpdateBoard();

    if(isMovingAnimationOn == false && isSwapping == false)
        shouldUpdateState = false;
}

// aici desenam fiecare frame
void Game::Render(void)
{
    // stergem ecranul pentru a desena de la zero noul frame
    ClearWindow();

    // desenam sprite-urile pe ecran, incepand cu fundalul
    DrawBackground();

    // desenam tabla de joc
    DrawBoard();

    // afisam continutul desenat in fereastra
    DisplayWindow();
}

void Game::SetupBoard(void)
{
    // pozitia curenta a fiecarei piese de pe tabla
    sf::Vector2f currentTilePosition;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            int index;
            currentTilePosition.x = START_X + i * (TILE_WIDTH  * SCALE_X + PADDING);
            currentTilePosition.y = START_Y + j * (TILE_HEIGHT * SCALE_Y + PADDING);
            while (true)
            {
                index = rand() % NUMTILES;
                if (IsCollisionOnCreation(tiles[index], i, j) == false)
                    break;
            }
            board[i][j] = tiles[index];
            board[i][j].sprite.setScale(sf::Vector2f(SCALE_X, SCALE_Y));
            board[i][j].sprite.setPosition(currentTilePosition);
            board[i][j].pieceType = static_cast<PieceType>(index);
            board[i][j].pieceModifierType = PieceModifierType::PIECEMODIFIER_NONE;
            board[i][j].isMatch = false;
            board[i][j].alpha = RGBA_MAX[3];
        }
    }
}

void Game::DrawBoard(void)
{
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            window.draw(board[i][j].sprite);
        }
    }
};


void Game::MovingAnimationOnSwap(void)
{
    int dx = 0, dy = 0;
    printf("MovingAnimationOnSwap()\n");
    isMovingAnimationOn = false;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            GameTile& tile = board[i][j];
            sf::Vector2f position = tile.sprite.getPosition();

            if (tile.shouldMove == false)
                continue;

            if (destinationAfterSwap[0].x == i && destinationAfterSwap[0].y == j)
            {
                dx = position.x - (START_X + destinationAfterSwap[1].x * (TILE_WIDTH  * SCALE_X + PADDING));
                dy = position.y - (START_Y + destinationAfterSwap[1].y * (TILE_HEIGHT * SCALE_Y + PADDING));

                if (sqrtf(dx * dx + dy * dy) <= 6)
                {
                    tile.shouldMove = false;
                    board[destinationAfterSwap[1].x][destinationAfterSwap[1].y].shouldMove = false;
                    isMovingAnimationOn = false;
                    return;
                }
                printf("Distance: %.2f\n", sqrtf(dx * dx + dy * dy));

            }
            if (destinationAfterSwap[1].x == i && destinationAfterSwap[1].y == j)
            {
                dx = position.x - (START_X + destinationAfterSwap[0].x * (TILE_WIDTH * SCALE_X + PADDING));
                dy = position.y - (START_Y + destinationAfterSwap[0].y * (TILE_HEIGHT * SCALE_Y + PADDING));
                if (sqrtf(dx * dx + dy * dy) <= 6)
                {
                    tile.shouldMove = false;
                    board[destinationAfterSwap[0].x][destinationAfterSwap[0].y].shouldMove = false;
                    isMovingAnimationOn = false;
                    return;
                }
                printf("Distance: %.2f\n", sqrtf(dx * dx + dy * dy));

            }
            printf("dx = %d\tdy = %d\n", dx, dy);

            for (int speed = 0; speed < 4; speed++)
            {
                if (dx != 0)
                    position.x -= dx/abs(dx);
                if (dy != 0)
                    position.y -= dy/abs(dy);
            }
            tile.sprite.setPosition(position);
            if (dx != 0 || dy != 0)
                isMovingAnimationOn = true;
            
           
        }
    }
    printf("isMovingAnimationOn = %s\n", isMovingAnimationOn ? "true" : "false");
}

// trebuie sa verificam daca piesa pusa provoaca un "combo" in stanga sau in sus,
// deoarece in dreapta si in jos nu este inca populata tabla de joc
bool Game::IsCollisionOnCreation(GameTile& tile, int i, int j) const
{
    // verificam coliziune deasupra, cele 2 piese de deasupra de pe coloana curenta 
    // sa nu aiba aceeasi valoare precum piesa pusa
    if (i >= 2)
    {
        if (board[i-2][j].pieceType == tile.pieceType && board[i-1][j].pieceType == tile.pieceType)
            return true;
    }
    // verificam coliziune in stanga, cele 2 piese din stanga de pe linia curenta
    // sa nu aiba aceeasi valoare precum piesa pusa
    if (j >= 2)
    {
        if (board[i][j-2].pieceType == tile.pieceType && board[i][j-1].pieceType == tile.pieceType)
            return true;
    }
    return false;
}

bool Game::CheckCombo(GameTile& tile, int x1, int y1, int x0, int y0) const
{
    // Luam piesa curenta in considerare la combo
    int comboCount = 1;
    PieceType type = tile.pieceType;
    printf("Tipul piesei in checkCombo: %d\n", type);
    // Verificam combo orizontal la stanga si la dreapta cu 2 piese (daca exista)
    printf("La stanga: ");
    int cnt = 0;
    for (int i = x1 - 1; i >= x1 - 2 && i >= 0; i--)
    {
        if (i == x0 && board[x1][y1].pieceType != type)
            break;
        if (board[i][y1].pieceType != type)
            break;
     
        
        printf("(%d, %d) = %d\t", y1, i, type);
        comboCount++;
        cnt++;
    }
    if (cnt == 0)
        printf("Nimic\n");
    cnt = 0;
    printf("\nLa dreapta: ");
    for (int i = x1 + 1; i <= x1 + 2 && i < BOARD_SIZE_X; i++)
    {
        if (i == x0 && board[x1][y1].pieceType != type)
            break;
        if (board[i][y1].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", y1, i, type);
        comboCount++;
        cnt++;
    }
    if (cnt == 0)
        printf("Nimic\n");
    cnt = 0;
    if (comboCount >= 3)
    {
        printf("\n");
        return true; // Am gasit combo pe linie
    }
    

    // resetare contor
    comboCount = 1;

    // Verificam combo vertical in sus si in jos cu 2 piese (daca exista)
    printf("\nSus: ");
    for (int j = y1 - 1; j >= y1 - 2 && j >= 0; j--)
    {
        if (j == y0 && board[x1][y1].pieceType != type)
            break;
        if (board[x1][j].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", j, x1, type);
        comboCount++;
        cnt++;
    }
    if (cnt == 0)
        printf("Nimic\n");
    cnt = 0;
    printf("\nJos: ");
    for (int j = y1 + 1; j <= y1 + 2 && j < BOARD_SIZE_Y; j++)
    {
        if (j == y0 && board[x1][y1].pieceType != type)
            break;
        // ignor warning fals-pozitiv out of bounds
#pragma warning(push)
#pragma warning(disable:6385)
        if (board[x1][j].pieceType != type)
            break;
#pragma warning(pop)
        printf("(%d, %d) = %d\t", j, x1, type);
        comboCount++;
        cnt++;
    }
    printf((cnt == 0) ? "nimic\n" : "\n");
    if (comboCount >= 3) 
        return true; // Am gasit combo pe coloana

    return false; // Nu am gasit combo
}

void Game::MouseClickUpdateCallback(void)
{

    // currentTilePosition.x = START_X + i * (TILE_WIDTH  * SCALE_X + PADDING);
    // currentTilePosition.y = START_Y + j * (TILE_HEIGHT * SCALE_Y + PADDING);
    // aflam prin formula inversa
    int x = (mousePos.x - START_X) / (SCALE_X * TILE_WIDTH  + PADDING);
    int y = (mousePos.y - START_Y) / (SCALE_Y * TILE_HEIGHT + PADDING);

    printf("Click (%d, %d) - %d \n", y, x, board[x][y].pieceType);
    if (numClicks == 1)
    {
        x0 = x;
        y0 = y;
    }
    if (numClicks == 2 && (abs(x - x0) + abs(y - y0) == 1))
    {
        x1 = x;
        y1 = y;
        if (CheckCombo(board[x0][y0], x1, y1, x0, y0) == true)
        {
            printf("Trebuie swap (%d, %d) cu (%d, %d)\n", y0, x0, y1, x1);
            board[x0][y0].shouldMove = true;
            board[x1][y1].shouldMove = true;
            
            GameTilePosition tile1{x1, y1, board[x1][y1].sprite.getPosition()};
            GameTilePosition tile2{x0, y0, board[x0][y0].sprite.getPosition()};
            
            destinationAfterSwap[0] = tile1;
            destinationAfterSwap[1] = tile2;

            isSwapping = true;
            isMovingAnimationOn = true;
        }
        numClicks = 0;
    }
    
}

void Game::SwapPieces(GameTile& piece1, GameTile& piece2)
{
   /* sf::Texture* tempTexture = &piece1.texture;
    piece1.sprite.setTexture(piece2.texture);
    piece2.sprite.setTexture(*tempTexture);*/

}

void Game::MatchFinding(void)
{
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            const GameTile& tile = board[i][j];

            if (i == BOARD_SIZE_X - 1 || j == BOARD_SIZE_Y - 1)
                continue;
            if (i == 0 || j == 0)
                continue;

            // check for match on row
            if (tile.pieceType == board[i-1][j].pieceType && tile.pieceType == board[i+1][j].pieceType)
            {
                for (int offsetX = -1; offsetX <= 1; offsetX++)
                {
                    board[i+offsetX][j].isMatch = true;
                    printf("Match(%d, %d)\n", i + offsetX, j);
                }
            }
            // check for match on column
            if (tile.pieceType == board[i][j-1].pieceType && tile.pieceType == board[i][j+1].pieceType)
            {
                for (int offsetY = -1; offsetY <= 1; offsetY++)
                {
                    board[i][j+offsetY].isMatch == true;
                    printf("Match(%d, %d)\n", i, j + offsetY);
                }
            }
        }
    }
}

void Game::DeletingAnimation(void)
{
    if (isMovingAnimationOn == true)
        return;
    printf("Deleting animation\n");
    int dAlpha = 10;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            if(board[i][j].isMatch == true && board[i][j].alpha > dAlpha)
            {
                sf::Color color = board[i][j].sprite.getColor();
                color.a -= dAlpha;
                board[i][j].sprite.setColor(color);
                isMovingAnimationOn = true;
            }
        }
    }
}

// TODO: De actualizat scorul mai "destept" decat cu doar 1 per piesa
void Game::CheckNoMatch(void)
{
    int currentMoveScore = 0;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            currentMoveScore += (board[i][j].isMatch == true) ? 1 : 0;
        }
    }
    if (isSwapping == true && isMovingAnimationOn == false)
    {
        if (currentMoveScore != 0)
        {
            printf("CheckNoMatch() swapping pieces\n");
            //std::swap(board[x0][y0], board[x1][y1]);
        }
        isSwapping = false;
    }
}

void Game::UpdateBoard(void)
{
    if (shouldUpdateState == false)
        return;
    if (isMovingAnimationOn == false)
        return;

    printf("UpdateBoard()\n");
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            if (board[i][j].isMatch == false)
                continue;

            for (int n = i; n > 0; n--)
            {
                if (board[n][j].isMatch == false)
                {
                    std::swap(board[n][j], board[i][j]);
                    break;
                }
            }
        }
    }
    for (int j = 0; j < BOARD_SIZE_Y; j++)
    {
        for (int i = BOARD_SIZE_X - 1, n = 0; i >= 0; i--)
        {
            if (board[i][j].isMatch == true)
            {
                sf::Vector2f pos = board[i][j].sprite.getPosition();
                board[i][j].pieceType = static_cast<PieceType>(rand() % NUMTILES);
                board[i][j].isMatch = false;
                board[i][j].alpha = RGBA_MAX[3];
                pos.y = -TILE_HEIGHT * n++;
                board[i][j].sprite.setPosition(pos);
            }
        }
    }
}


// aici incarcam asset-urile de pe disc in variabile (RAM) la initializarea jocului
void Game::LoadAssets(void)
{
    std::string backgroundSpritePath = "assets/images/background.jpg";
    LoadGameObjectFromFile(backgroundSpritePath, backgroundImage);

    char currentTilePath[64];
    for (int i = 0; i < NUMTILES; i++)
    {
        sprintf(currentTilePath, "assets/tiles/tile%02d.png", i);
        LoadGameObjectFromFile(currentTilePath, tiles[i]);
        tiles[i].pieceModifierType = PieceModifierType::PIECEMODIFIER_NONE;
        tiles[i].pieceType = static_cast<PieceType>(i);
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
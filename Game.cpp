#include "Game.h"
#include <list>


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

    // jocul va avea 30 FPS si pozitia ferestrei este setata mai jos
    constexpr int FRAMES_PER_SECOND = 30;
    window.setFramerateLimit(FRAMES_PER_SECOND);
    window.setPosition(sf::Vector2i(posX, posY));
    
    // aici vom memora toate piesele, si stim ca in cadrul jocului sunt NUMTILES piese, prealocam memoria
    tiles.resize(NUMTILES);

    level = 1;
    score = 0;

    // dreptunghiul tablei pentru a afla usor daca am dat click pe una din piese
    boardRect = sf::IntRect(START_X, START_Y, NUMTILES*TILE_WIDTH, NUMTILES*TILE_HEIGHT);

    // initializarea tuturor variabilelor boolene care spun cand trebuie sa apelam o functie
    // sau care marcheaza o conditie, sau contoare, la zero
    ResetBoardState();

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
                // TODO: Handle resize
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
                        shouldHandleMouseClick = false;
                        numClicks = 0;
                        break;
                    }

                    shouldHandleMouseClick = true;

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
    if (shouldHandleMouseClick == true)
        MouseClickUpdateCallback();

    if (shouldFindMatch == true)
        MatchFindingCallback();

    if (isMovingAnimationOn == true)
        MovingAnimationOnSwapCallback();

    if (shouldDeleteCombo == true)
        DeleteComboAnimationCallback();

    if (shouldUpdateBoard == true)
        UpdateBoard();
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
            int randomTileIndex;
            currentTilePosition.x = START_X + i * (TILE_WIDTH  * SCALE_X + PADDING);
            currentTilePosition.y = START_Y + j * (TILE_HEIGHT * SCALE_Y + PADDING);
            while (true)
            {
                randomTileIndex = rand() % NUMTILES;
                if (IsCollisionOnCreation(tiles[randomTileIndex], i, j) == false)
                    break;
            }
            GenerateTile(randomTileIndex, currentTilePosition, &board[i][j]);
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

// trebuie sa verificam daca piesa pusa provoaca un "combo" in stanga sau in sus,
// deoarece in dreapta si in jos nu este inca populata tabla de joc
bool Game::IsCollisionOnCreation(GameTile& tile, int i, int j) const
{
    // verificam coliziune deasupra, cele 2 piese de deasupra de pe coloana curenta 
    // sa nu aiba aceeasi valoare precum piesa pusa
    if (i >= 2)
    {
        if (board[i - 2][j].pieceType == tile.pieceType && board[i - 1][j].pieceType == tile.pieceType)
            return true;
    }
    // verificam coliziune in stanga, cele 2 piese din stanga de pe linia curenta
    // sa nu aiba aceeasi valoare precum piesa pusa
    if (j >= 2)
    {
        if (board[i][j - 2].pieceType == tile.pieceType && board[i][j - 1].pieceType == tile.pieceType)
            return true;
    }
    return false;
}

bool Game::CheckCombo(GameTile& tile, int x1, int y1, int x0, int y0) const
{
    // tipul piesei curente
    PieceType type = tile.pieceType;

    // Luam piesa curenta in considerare la combo
    int comboCount = 1;

    // variabile care tin minte daca am avut combo pe linie/coloana
    bool lineCombo = false;
    bool rowCombo = false;

    if (type == PieceType::PIECE_NONE)
        return false;

    printf("Tipul piesei in checkCombo: %d\n", type);

    // Verificam combo orizontal la stanga si la dreapta cu 2 piese (daca exista)
    printf("La stanga: ");
    for (int i = x1 - 1; i >= x1 - 2 && i >= 0; i--)
    {
        if (board[i][y1].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", y1, i, type);
        comboCount++;
    }
    printf("\nLa dreapta: ");
    for (int i = x1 + 1; i <= x1 + 2 && i < BOARD_SIZE_X; i++)
    {
        if (board[i][y1].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", y1, i, type);
        comboCount++;
    }
    
    // daca avem macar 3 piese pe linie de acelasi tip, avem combo pe linie
    if (comboCount >= 3)
          lineCombo = true; // Am gasit combo pe linie
    else
        printf((comboCount < 3) ? "nimic\n" : "\n");

    // resetare contor pentru combo vertical
    comboCount = 1;

    // Verificam combo vertical in sus si in jos cu 2 piese (daca exista)
    printf("\nSus: ");
    for (int j = y0 - 1; j >= y0 - 2 && j >= 0; j--)
    {
        if (board[x1][j].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", j, x1, type);
        comboCount++;
    }

    printf("\nJos: ");
    for (int j = y1 + 1; j <= y1 + 2 && j < BOARD_SIZE_Y; j++)
    {
        if (board[x1][j].pieceType != type)
            break;
        printf("(%d, %d) = %d\t", j, x1, type);
        comboCount++;
    }
    
    // daca avem macar 3 piese pe coloana de acelasi tip, avem combo pe coloana
    if (comboCount >= 3)
        rowCombo = true; // Am gasit combo pe coloana
    else 
        printf((comboCount < 3) ? "nimic\n" : "\n");

    // daca am gasit combo pe linie si/sau pe coloana, returnam true
    return rowCombo || lineCombo;
}

void Game::MouseClickUpdateCallback(void)
{
    // currentTilePosition.x = START_X + i * (TILE_WIDTH  * SCALE_X + PADDING);
    // currentTilePosition.y = START_Y + j * (TILE_HEIGHT * SCALE_Y + PADDING);
    // aflam prin formula inversa
    int x = (mousePos.x - START_X) / (SCALE_X * TILE_WIDTH  + PADDING);
    int y = (mousePos.y - START_Y) / (SCALE_Y * TILE_HEIGHT + PADDING);
    int distanceToNeighbour = -1;

    printf("Click (%d, %d) - %d \n", y, x, board[x][y].pieceType);

    // la primul sau al doilea click avem o celula anterioara la care am dat click
    if (numClicks >= 1)
        distanceToNeighbour = abs(x - x0) + abs(y - y0);

    // la primul click salvam coordonatele celulei in care am dat click
    if (numClicks == 1)
    {
        x0 = x;
        y0 = y;
    }
   
    // la al doilea click pe o celula care nu este vecina resetam contorul de click-uri
    if (numClicks == 2 && distanceToNeighbour != 1)
        numClicks = 0;

    // al doilea click si am dat pe o celula alaturata stanga/dreapta/sus/jos
    if (numClicks == 2 && distanceToNeighbour == 1)
    {
        if (CheckCombo(board[x0][y0], x, y, x0, y0) == true || CheckCombo(board[x][y], x0, y0, x, y) == true)
        {
            printf("Trebuie swap (%d, %d) cu (%d, %d)\n", y0, x0, y, x);
            board[x0][y0].shouldSwap = true;
            board[x][y].shouldSwap = true;

            SwapPieces(board[x0][y0], board[x][y]);

            GameTilePosition tile1{ x, y, board[x][y].sprite.getPosition() };
            GameTilePosition tile2{ x0, y0, board[x0][y0].sprite.getPosition() };

            destinationAfterSwap[0] = tile1;
            destinationAfterSwap[1] = tile2;

            isSwapping = true;
            isMovingAnimationOn = true;
            shouldFindMatch = true;
        }
        numClicks = 0;
    }

    // functia de mouse click si-a facut treaba, nu mai trebuie apelata
    shouldHandleMouseClick = false;
}

void Game::SwapPieces(GameTile& piece1, GameTile& piece2)
{
    // fac swap la piese
    std::swap(piece1, piece2);

    //// nu fac swap la texturi, texturile o sa faca swap cu tranzitia
    //sf::Texture* tempTexture = piece1.texture;
    //piece1.sprite.setTexture(*piece2.texture);
    //piece2.sprite.setTexture(*tempTexture);

    ////// nu fac swap la isMatch, ca piesa curenta are match-ul
    //bool tempIsMatch = piece1.isMatch;
    //piece1.isMatch = piece2.isMatch;
    //piece2.isMatch = tempIsMatch;
}

void Game::MovingAnimationOnSwapCallback(void)
{
    int dx = 1, dy = 1;
    int remaining_dx = 0, remaining_dy = 0;

    isMovingAnimationOn = false;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            GameTile& tile = board[i][j];
            sf::Vector2f position = tile.sprite.getPosition();

            // ne ocupam doar de piesele pentru care trebuie sa facem swap, si le ingoram pe restul
            if (tile.shouldSwap == false)
                continue;


            if (destinationAfterSwap[0].x == i && destinationAfterSwap[0].y == j)
            {
                remaining_dx = position.x - (START_X + destinationAfterSwap[0].x * (TILE_WIDTH * SCALE_X + PADDING));
                remaining_dy = position.y - (START_Y + destinationAfterSwap[0].y * (TILE_HEIGHT * SCALE_Y + PADDING));
                if ((abs(remaining_dx) + abs(remaining_dy)) == 0)
                {
                    printf("Piesa 1 a ajuns la noua destinatie\n");
                    tile.shouldSwap = false;
                    board[destinationAfterSwap[0].x][destinationAfterSwap[0].y].shouldSwap = false;
                    shouldDeleteCombo = true;
                }
            }
            else if (destinationAfterSwap[1].x == i && destinationAfterSwap[1].y == j)
            {
                remaining_dx = position.x - (START_X + destinationAfterSwap[1].x * (TILE_WIDTH  * SCALE_X + PADDING));
                remaining_dy = position.y - (START_Y + destinationAfterSwap[1].y * (TILE_HEIGHT * SCALE_Y + PADDING));
                if ((abs(remaining_dx) + abs(remaining_dy)) == 0)
                {
                    printf("Piesa 2 a ajuns la noua destinatie\n");
                    tile.shouldSwap = false;
                    board[destinationAfterSwap[1].x][destinationAfterSwap[1].y].shouldSwap = false;
                    shouldDeleteCombo = true;
                }
            }

            for (int speed = 0; speed < 5; speed++)
            {
                if (remaining_dx != 0)
                    position.x -= dx * remaining_dx / (abs(remaining_dx));
                if (remaining_dy != 0)
                    position.y -= dy * remaining_dy / (abs(remaining_dy));
            }

            tile.sprite.setPosition(position);

            if (remaining_dx != 0 || remaining_dy != 0)
                isMovingAnimationOn = true;
            else
            {
                tile.shouldSwap = false;
                isMovingAnimationOn = false;
                printf("Finished moving sprite properly (%d, %d)\n", remaining_dx, remaining_dy);
            }
        }
    }
}

void Game::MatchFindingCallback(void)
{
    bool matchFound = false;
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            const GameTile& tile = board[i][j];
            if (i - 1 >= 0 && i + 1 < BOARD_SIZE_X)
            {
                // check for match on row
                if (tile.pieceType == board[i-1][j].pieceType && tile.pieceType == board[i+1][j].pieceType)
                {
                    matchFound = true;
                    for (int offsetX = -1; offsetX <= 1; offsetX++)
                    {
                        board[i+offsetX][j].isMatch = true;
                        matchedPiecesIndexes.insert(std::make_pair(i + offsetX, j));
                        printf("Match(%d, %d)\n", i + offsetX, j);
                    }
                }
            }
            if (j - 1 >= 0 && j + 1 < BOARD_SIZE_Y)
            {
                // check for match on column
                if (tile.pieceType == board[i][j-1].pieceType && tile.pieceType == board[i][j+1].pieceType)
                {
                    matchFound = true;
                    for (int offsetY = -1; offsetY <= 1; offsetY++)
                    {
                        board[i][j+offsetY].isMatch = true;
                        matchedPiecesIndexes.insert(std::make_pair(i, j + offsetY));
                        printf("Match(%d, %d)\n", i, j + offsetY);
                    }
                }
            }
        }
    }
    if(matchFound == true && isMovingAnimationOn == false)
        shouldDeleteCombo = true;

    // functia de gasit un match/combo si-a facut treaba, nu mai trebuie apelata
    shouldFindMatch = false;
}

void Game::DeleteComboAnimationCallback(void)
{
    static int numCalls = 0;
    int dAlpha = 15; // trebuie sa fie divizor a lui 255 pentru a nu avea underflow/overflow
    bool finishedDeleting = true;

    if (numCalls == 0)
        printf("Deleting animation\n");

    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y; j++)
        {
            if(board[i][j].isMatch == true && board[i][j].alpha > 0)
            {
                sf::Color color = board[i][j].sprite.getColor();
                color.a -= dAlpha;
                board[i][j].sprite.setColor(color);
                finishedDeleting = false;
                if (color.a == 0)
                    board[i][j] = GameTile();
            }
        }
    }

    if (finishedDeleting == true)
    {
        shouldDeleteCombo = false;
        shouldUpdateBoard = true;
        numCalls = 0;
        printf("Deleting animation done\n");
    }

    numCalls++;
}

void Game::UpdateBoard(void)
{
    bool finishedUpdating = true;
    // pentru fiecare coloana tin minte cu cate piese trebuie sa mut in jos piesele de deasupra
    int numPositionsToMoveDown[BOARD_SIZE_X] = { 0 };
    // pentru fiecare coloana tin minte indexul ultimei piese pe care trebuie sa o mut in jos
    int lastIndexY[BOARD_SIZE_X] = { 0 };

    printf("Updating Board()\n");
    
    // Pas 1: numar pentru fiecare coloana cu cate piese trebuie sa mut in jos si pana la ce piesa
    for (auto& pieceIdx : matchedPiecesIndexes)
    {
        // piesa curenta nu mai este match, resetez
        board[pieceIdx.first][pieceIdx.second].isMatch = false;

        // contor pentru cate pozitii trebuie sa se mute in jos piesele de deasupra unei piese sterse
        numPositionsToMoveDown[pieceIdx.first]++;
        if (pieceIdx.second > lastIndexY[pieceIdx.first])
        {
            // index-ul ultimei piese de mutat in jos, pentru a le putea muta de la ultima la prima
            lastIndexY[pieceIdx.first] = pieceIdx.second;
        }
    }

    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        if (lastIndexY[i] == 0 && numPositionsToMoveDown[i] == 0)
            continue;
        printf("Mut in jos pe coloana %d pana la piesa %d cu %d pozitii\n", i, lastIndexY[i], numPositionsToMoveDown[i]);
    }

    // Pas 2: mut in jos toate piesele care trebuie sa cada pentru a umple golurile
    for (int i = 0; i < BOARD_SIZE_X; i++)
    {
        if (numPositionsToMoveDown[i] == 0)
            continue;

        for (int j = lastIndexY[i]; j >= 0; j--)
        {
            GameTile& srcPiece = board[i][j];
            GameTile& dstPiece = board[i][j + numPositionsToMoveDown[i]];

            sf::Vector2f dstPosition = srcPiece.sprite.getPosition();
            sf::Vector2f srcPosition = dstPiece.sprite.getPosition();

            dstPosition.y += numPositionsToMoveDown[i] * (TILE_HEIGHT * SCALE_Y + PADDING);

            SwapPieces(srcPiece, dstPiece);

            srcPiece.sprite.setPosition(srcPosition);
            dstPiece.sprite.setPosition(dstPosition);
        }
    }

    // Pas 3: generez noi piese pentru spatiile libere de sus
    //  TODO - sunt spatiile libere ok? verifica cu debugger; 
    // ceva variable au ramas setate, eventual o functie de reset pt tiles
    // partea de generare piese + eventual animatie cadere de sus
    if (finishedUpdating == true)
    {
        printf("Finished Updating\n");
        // am terminat de generat noile piese, resetez complet starea tablei
        ResetBoardState();
        //// posibil sa fie match-uri facute de noile piese
        //shouldFindMatch = true;
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

// nu exista o functie directa in libraria SFML ca sa putem incarca un sprite (obiectul pe care desenam) din fisier
// deci trebuie incarcata textura si asociata sprite-ului pentru a nu tot repeta acelasi cod pentru fiecare sprite
void Game::LoadGameObjectFromFile(const std::string& filePath, GameObject& gameObject)
{
    gameObject.texture = new sf::Texture();
    if (gameObject.texture->loadFromFile(filePath) == false)
    {
        // un mesaj de eroare implicit este printat 
        exit(EXIT_FAILURE);
    }
    gameObject.sprite.setTexture(*gameObject.texture);
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

void Game::ResetBoardState(void)
{
    numClicks = 0;
    shouldUpdateBoard = false;
    shouldHandleMouseClick = false;
    shouldDeleteCombo = false;
    shouldFindMatch = false;
    isSwapping = false;
    isMovingAnimationOn = false;
    matchedPiecesIndexes.clear();
}

void Game::GenerateTile(int tileIdx, sf::Vector2f tilePosition, GameTile *tile)
{
    *tile = tiles[tileIdx];
    tile->sprite.setScale(sf::Vector2f(SCALE_X, SCALE_Y));
    tile->sprite.setPosition(tilePosition);
    tile->pieceType = static_cast<PieceType>(tileIdx);
    tile->pieceModifierType = PieceModifierType::PIECEMODIFIER_NONE;
    tile->isMatch = false;
    tile->alpha = RGBA_MAX[3];
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

Game::~Game()
{
    for (GameTile tile : tiles)
        delete tile.texture;
    tiles.clear();
}

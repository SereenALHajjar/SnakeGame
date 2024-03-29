#include <raylib.h>
#include <deque>
#include <raymath.h>
#include <iostream>
using namespace std;

const int CELL_SIZE = 20;
const int CELL_COUNT = 25;
Color DARK_BLUE = {0, 0, 26, 255};
double last_update = 0;
int offset = 65 ;
bool FindInDeque(Vector2 food, deque<Vector2> body)
{
    for (auto element : body)
    {
        if (Vector2Equals(food, element))
            return true;
    }
    return false;
}
class Snake
{
public:
    deque<Vector2> body = {Vector2{5 , 6}};
    Vector2 dir;
    Color color;

    void Draw()
    {
        for (auto cell : body)
        {
            Rectangle rect = Rectangle{cell.x * CELL_SIZE +offset,offset+ cell.y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            DrawRectangleRounded(rect, 0.5, 6, WHITE);
        }
    }
    void Update()
    {
        body.pop_back();
        body.push_front(Vector2Add(body[0], dir));
    }
    void Grow()
    {
        body.push_front(Vector2Add(body[0], dir));
    }
    void Reset()
    {
        body.clear();
        body.push_back(Vector2{5 , 6});
        dir = {0, 0};
    }
};
class Food
{
public:
    Vector2 position;
    int width;
    int height;
    Texture2D texture;
    Food()
    {
        Image img = LoadImage("graphics/food2.png");
        ImageResize(&img, 20, 20); 
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
        position = GeneratePosition();
    }
    ~Food()
    {
        UnloadTexture(texture);
    }
    void Draw()
    {
        DrawTexture(texture, offset + position.x * CELL_SIZE, offset + position.y * CELL_SIZE, WHITE);
    }
    Vector2 GeneratePosition()
    {
        int x = GetRandomValue(0, CELL_COUNT - 1);
        int y = GetRandomValue(0, CELL_COUNT - 1);
        return {(float)x, (float)y};
    }
    void Update(Snake snake)
    {
        Vector2 pos = GeneratePosition();
        while (FindInDeque(pos, snake.body))
        {
            pos = GeneratePosition();
        }
        position = pos;
    }
};
bool CheckInterval(double interval)
{
    if (GetTime() - last_update >= interval)
    {
        last_update = GetTime();
        return true;
    }
    return false;
}
class Game
{
public:
    Food food = Food();
    Snake snake = Snake();
    bool running = true;
    Sound wall_sound ; 
    Sound eat_sound ;
    int score = 0 ;
    Game()
    {
        InitAudioDevice() ;
        wall_sound = LoadSound("sounds/wall.mp3") ;
        eat_sound = LoadSound("sounds/eat.mp3") ;
    }
    ~Game()
    {
        UnloadSound(wall_sound) ;
        UnloadSound(eat_sound) ;
        CloseAudioDevice() ;
    }
    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionSnakeWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }
    void Draw()
    {
        food.Draw();
        snake.Draw();
    }
    void CheckCollisionSnakeWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.Update(snake);
            snake.Grow();
            PlaySound(eat_sound) ;
            score ++ ;
        }
    }
    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == -1 || snake.body[0].x == CELL_COUNT)
        {
            GameOver();
        }
        if (snake.body[0].y == -1 || snake.body[0].y == CELL_COUNT)
        {
            GameOver();
        }
    }
    void CheckCollisionWithTail()
    {
        deque<Vector2>tmp = snake.body ;
        tmp.pop_front() ;
        if (FindInDeque(snake.body[0], tmp))
            GameOver();
    }
    void GameOver()
    {
        cout << "game over";
        snake.Reset();
        food.Update(snake);
        running = false;
        PlaySound(wall_sound) ;
        score = 0 ;
    }
};

int main()
{

    InitWindow( 2*offset + CELL_COUNT * CELL_SIZE, 2*offset+ CELL_COUNT * CELL_SIZE, "Snake");
    SetTargetFPS(60);
    Game game = Game();

    while (!WindowShouldClose())
    {
        // event handling
        if (IsKeyPressed(KEY_LEFT) && game.snake.dir.x != 1)
        {
            game.snake.dir = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.dir.x != -1)
        {
            game.snake.dir = {1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_UP) && game.snake.dir.y != 1)
        {
            game.snake.dir = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.dir.y != -1)
        {
            game.snake.dir = {0, 1};
            game.running = true;
        }
        // edit position
        if (CheckInterval(0.2))
            game.Update();
        // draws
        BeginDrawing();
        ClearBackground(DARK_BLUE);
        DrawRectangleLinesEx(Rectangle{(float)offset -3 ,(float) offset-3 , CELL_COUNT * CELL_SIZE +6  , CELL_COUNT*CELL_SIZE+6} , 3 , WHITE) ;
        DrawText("Snake Game" , offset - 3 , 20 , 30 , WHITE) ;
        DrawText(TextFormat("Score : %i" , game.score) , offset -3  , CELL_COUNT*CELL_SIZE+offset + 6 , 30 , WHITE) ;
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
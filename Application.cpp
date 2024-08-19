#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Chess.h"
#include "classes/Zobrist.h"

namespace ClassGame {
        //
        // our global variables
        //
        Chess *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;
        int AIPlayer = 1;
        bool selectedColor = false;
        Zobrist zobrist;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new Chess();
            game->setUpBoard();
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

                ImGui::Begin("Settings");

                if (!selectedColor) {
                    if (ImGui::Button("Play WHITE")) {
                        AIPlayer = 1;
                        selectedColor = true;
                        game = new Chess();
                        game->_gameOptions.AIPlayer = AIPlayer;
                        game->setAI('B');
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Play BLACK")) {
                        AIPlayer = 0;
                        selectedColor = true;
                        game = new Chess();
                        game->_gameOptions.AIPlayer = AIPlayer;
                        game->setAI('W');
                        game->setUpBoard();
                    }
                    ImGui::End();
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("");
                    std::string state = game->stateString();
                    //
                    // break state string into 8 rows of 8 characters
                    //
                    if (state.length() == 64) {
                        for (int y=0; y<8; y++) {
                            std::string row = state.substr(y*8, 8);
                            ImGui::Text("%s", row.c_str());
                        }
                        ImGui::Text("");
                        int64_t hash = zobrist.ZobristHash(state.c_str(),64);
                        ImGui::Text("zobrist hash: %llx", hash);
                        ImGui::Text("");
                        ImGui::Text("board evaluation value: %d", game->evaluateBoard(state.c_str()));
                    } else {
                            ImGui::Text("%s", state.c_str());
                    }
                    if (game->gameHasAI()) {
                        ImGui::Text("");
                        ImGui::Text("AI Depth Searches: %d", game->getAIDepathSearches());
                    }
                    if (gameOver) {
                        ImGui::Text("Game Over!");
                        std::cout<<"got to if gameover\n";
                        ImGui::Text("Winner: %d", gameWinner);
                        if (ImGui::Button("Reset Game")) {
                            game->stopGame();
                            game->setUpBoard();
                            gameOver = false;
                            gameWinner = -1;

                        }
                    }
                    ImGui::End();
                }
                ImGui::Begin("GameWindow");
                if (selectedColor) {
                    if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer()){
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}

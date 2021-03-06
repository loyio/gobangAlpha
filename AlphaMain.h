//
//  AlphaMain.h
//  gobangAlpha
//
//  Created by Loyio Macbook on 2020/11/15.
//

#ifndef AlphaMain_h
#define AlphaMain_h

#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "alphaAlgo.h"




int AlphaApp() {
    //Initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        return 1;
    }
    //Set up the screen
    SDL_Surface *screen = NULL;
    SDL_Window *window = SDL_CreateWindow("alpha AI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 150,
                                          SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);

    //If there was an error in setting up the screen
    if (screen == NULL) {
        return 1;
    }

    //Initialize SDL_ttf
    if (TTF_Init() == -1) {
        return 1;
    }

    //Initialize SDL_net
    if (SDLNet_Init() == -1) {
        return 1;
    }

    TTF_Font *font;

    //Open the font
    font = TTF_OpenFont("resource/font/font.ttf", 19);

    if (font == NULL) {
        return 1;
    }

    //Set Font Style
    TTF_SetFontStyle(font, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);

    //The color of the font
    SDL_Color textColor = {215, 215, 215};

    SDL_Surface *msg = NULL;

    msg = TTF_RenderText_Blended(font, "Connecting", textColor);

    SDL_BlitSurface(msg, NULL, screen, NULL);

    IPaddress ip;
    TCPsocket tcpsock;

    // create a listening TCP socket on port 4700 (client)
    if (SDLNet_ResolveHost(&ip, "localhost", 4700) == -1) {
        return 1;
    }

    bool quit = false;
    bool connected = false;
    bool success = false;
    bool finish = false;
    SDL_Event event;


    while (!quit) {
        //While there's an event to handle
        while (SDL_PollEvent(&event)) {
            //If the user has Xed out the window
            if (event.type == SDL_QUIT) {
                //Quit the program
                quit = true;
            }
        }
        if (!connected) {
            tcpsock = SDLNet_TCP_Open(&ip);
            if (tcpsock) {
                msg = TTF_RenderText_Blended(font, "Connected", textColor);
                connected = true;
            }
        } else {
            //success = false;
            char data[257] = {0};
            char response[3] = {0};
            if (SDLNet_TCP_Recv(tcpsock, data, 257) > 0) {
                if (data[0] != 'e') {
                    int gameData[15][15] = {{0}};
                    int addX = -1, addY = -1;
                    for (int i = 0; i < 255; i++) {
                        int temp = 0;
                        if (data[i + 1] != 0)
                            temp = (data[i + 1] == data[0] ? 1 : 2);
                        gameData[i / 15][i % 15] = temp;
                    }
                    AIwork(gameData, addX, addY);
                    response[0] = addX;
                    response[1] = addY;
                    if (SDLNet_TCP_Send(tcpsock, response, 3) == 3)
                        success = true;
                } else {
                    finish = true;
                    quit = true;
                }
            }
            if (success)
                msg = TTF_RenderText_Blended(font, "Working", textColor);
        }
        SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
        SDL_BlitSurface(msg, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    //Quit SDL_ttf
    TTF_Quit();

    //Close Network
    if (!finish && connected) {
        SDLNet_TCP_Send(tcpsock, "e", 2);
        SDLNet_TCP_Close(tcpsock);
    }

    //Quit SDL_net
    SDLNet_Quit();

    //Quit SDL
    SDL_Quit();
    
    return 0;
}

#endif /* AlphaMain_h */

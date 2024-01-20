//---------------------------------------------------------------------------------
// App.cpp
// Implementation of the calls that are exposed via the App namespace.
//---------------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------------
#include <string>
#include "main.h"
#include "app.h"
#include "SimpleSound.h"
#include "SimpleController.h"
#include "SimpleSprite.h"
//---------------------------------------------------------------------------------
// Utils and externals for system info.

namespace App
{
    void DrawTriangle(float ax, float ay, float bx, float by, float cx, float cy, float r, float g, float b)
    {
#if APP_USE_VIRTUAL_RES
        APP_VIRTUAL_TO_NATIVE_COORDS(ax, ay);
        APP_VIRTUAL_TO_NATIVE_COORDS(bx, by);
        APP_VIRTUAL_TO_NATIVE_COORDS(cx, cy);
#endif
        glBegin(GL_TRIANGLES);
        glColor3f(r, g, b); // Yellow
        glVertex2f(ax, ay);
        glVertex2f(bx, by);
        glVertex2f(cx, cy);
        glEnd();
    }

    void DrawPoint(float sx, float sy, float r, float g, float b)
    {
#if APP_USE_VIRTUAL_RES
        APP_VIRTUAL_TO_NATIVE_COORDS(sx, sy);
#endif
        glBegin(GL_POINTS);
        glColor3f(r, g, b); // Yellow
        glVertex2f(sx, sy);
        glEnd();
    }

    void DrawCircle(float sx, float sy, float radius, float r, float g, float b)
    {
#if APP_USE_VIRTUAL_RES
        APP_VIRTUAL_TO_NATIVE_COORDS(sx, sy);
#endif

        int num_segments = 32;

        glBegin(GL_POLYGON);
        for (int i = 0; i < num_segments; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // get the current angle 
            float x = radius * cosf(theta); // calculate the x component 
            float y = radius * sinf(theta); // calculate the y component
            glColor3f(r, g, b);
            glVertex2f(x + sx, y + sy); // output vertex 
        }
        glEnd();
    }


    void DrawLine(float sx, float sy, float ex, float ey, float r, float g, float b)
    {
#if APP_USE_VIRTUAL_RES
        APP_VIRTUAL_TO_NATIVE_COORDS(sx, sy);
        APP_VIRTUAL_TO_NATIVE_COORDS(ex, ey);
#endif
        glBegin(GL_LINES);
        glColor3f(r, g, b); // Yellow
        glVertex2f(sx, sy);
        glVertex2f(ex, ey);
        glEnd();
    }


    CSimpleSprite* CreateSprite(const char* fileName, int columns, int rows)
    {
        return new CSimpleSprite(fileName, columns, rows);
    }

    bool IsKeyPressed(int key)
    {
        return ((GetAsyncKeyState(key) & 0x8000) != 0);
    }

    void GetMousePos(float& x, float& y)
    {
        POINT mousePos;
        GetCursorPos(&mousePos); // Get the mouse cursor 2D x,y position			
        ScreenToClient(MAIN_WINDOW_HANDLE, &mousePos);
        x = (float)mousePos.x;
        y = (float)mousePos.y;
        x = (x * (2.0f / WINDOW_WIDTH) - 1.0f);
        y = -(y * (2.0f / WINDOW_HEIGHT) - 1.0f);

#if APP_USE_VIRTUAL_RES
        APP_NATIVE_TO_VIRTUAL_COORDS(x, y);
#endif
    }

    void PlaySound(const char* fileName, bool looping)
    {
        DWORD flags = (looping) ? DSBPLAY_LOOPING : 0;
        CSimpleSound::GetInstance().PlaySound(fileName, flags);
    }

    void StopSound(const char* fileName)
    {
        CSimpleSound::GetInstance().StopSound(fileName);
    }

    bool IsSoundPlaying(const char* fileName)
    {
        return CSimpleSound::GetInstance().IsPlaying(fileName);
    }

    // This prints a string to the screen
    void Print(float x, float y, const char* st, float r, float g, float b, void* font)
    {
#if APP_USE_VIRTUAL_RES
        APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
#endif
        // Set location to start printing text
        glColor3f(r, g, b); // Yellow
        glRasterPos2f(x, y);
        int l = (int)strlen(st);
        for (int i = 0; i < l; i++)
        {
            glutBitmapCharacter(font, st[i]); // Print a character on the screen
        }
    }

    const CController& GetController(int pad)
    {
        return CSimpleControllers::GetInstance().GetController(pad);
    }

    GLuint textureID;

    void SetupGL()
    {
        glGenTextures(1, &textureID); // Generate texture ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderTexture(unsigned char* imageData)
    {
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     imageData);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f); // Bottom Left
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f); // Bottom Right
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f); // Top Right
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f); // Top Left
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

#include "stdafx.h"
#include "RenderDirect.h"
#include "../App/app.h"

GLuint textureID;

void DirectRender::SetupGL()
{
    glGenTextures(1, &textureID); // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectRender::RenderTexture(unsigned char* imageData)
{
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
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
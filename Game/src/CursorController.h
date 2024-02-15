#pragma once
#include <memory>

#include "Camera.h"
#include "Cursors.h"


class CursorController
{
public:
    void Setup();

    void Update();

private:
    std::shared_ptr<Camera> m_Cam;
    std::shared_ptr<Cursor> m_Cursor;
};

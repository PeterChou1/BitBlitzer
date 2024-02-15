//---------------------------------------------------------------------------------
// Scene.h
//---------------------------------------------------------------------------------
//
// A Scene represents a discrete chunk in the game such as a level or a menu every
// Scene is the game must subclass this class
//
#pragma once

class Scene
{
public:

    Scene() = default;

    virtual ~Scene() = default;

    /**
     * \brief Start is called once onm start up of this is used to register any
     *        Resources and setup any Systems a Scene might need
     */
    virtual void Start() = 0;

    /**
     * \brief Setup is called every time 
     */
    virtual void Setup() = 0;

    /**
     * \brief calls every iteration of the game loop
     * \param deltaTime time in ms between this frame and the previous
     */
    virtual void Update(float deltaTime) = 0;


    /**
     * \brief called after pipeline render is done use this to render some
     *        custom graphics you need
     */
    virtual void Render() = 0;


};

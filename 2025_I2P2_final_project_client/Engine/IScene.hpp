#ifndef ISCENE_HPP
#define ISCENE_HPP

#include "Group.hpp"
#include <ctime>
#include <iostream>
namespace Engine {
    /// <summary>
    /// This class should be inherited whenever you want to define a new scene.
    /// Responsible for delegating draw, update, events to the objects and controls in scene.
    /// Acts like a prototype, so all initialize and terminate shouldn't be put in constructor or destructor.
    /// </summary>
    class IScene : public Group {
    protected:
        /// <summary>
        /// The interface cannot be instantiated directly, must be inherited.
        /// </summary>
        explicit IScene() = default;

    public:
        /// <summary>
        /// Copy constructor is deleted, no copying allowed.
        /// </summary>
        IScene(IScene const &) = delete;
        /// <summary>
        /// Copy assignment operator is deleted, no copy assignment allowed.
        /// </summary>
        IScene &operator=(IScene const &) = delete;
        /// <summary>
        /// The default virtual destructor to support polymorphism destruction.
        /// </summary>
        virtual ~IScene() = default;
        /// <summary>
        /// Put all initialize code here instead of constructor to make change scene more simple.
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
        /// Put all terminate code here instead of destructor to make change scene more simple.
        /// </summary>
        virtual void Terminate();
        /// <summary>
        /// Draw to window display.
        /// This is called when the game should redraw the window.
        /// Delegate the draw event to all visible objects.
        /// </summary>
        void Draw() const override;
        
    };
   
}
// TODO-scoreboard
class PlayerInformation{
public:
    std::string name;
    int score;
    std::time_t Time;
    int gap;
    PlayerInformation(std::string name, int score, std::time_t Time, int gap):name(name), score(score), Time(Time), gap(gap){}
    PlayerInformation():name("notfound"), score(0), Time(0), gap(0){}
    ~PlayerInformation() = default;
    bool operator()(const PlayerInformation & rhs) const{
        if(score == rhs.score)
            if(gap == rhs.gap)
                return Time < rhs.Time;
            else 
                return gap < rhs.gap;
        else return score < rhs.score;
    }
    bool operator>(const PlayerInformation & rhs) const{
        if(score == rhs.score)
            if(gap == rhs.gap)
                return Time > rhs.Time;
            else 
                return gap > rhs.gap;
        else return score > rhs.score;
    }
    std::string TimeToString_Time() {
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&Time));
        return std::string(buf);
    }
    
};  
#endif   // ISCENE_HPP

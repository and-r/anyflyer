#ifndef SIMEVENTRECEIVER_H
#define SIMEVENTRECEIVER_H
using namespace irr;


class SimEventReceiver : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        }
        if (event.EventType == irr::EET_GUI_EVENT)
        {
           cout<<"GUI event!"<<endl;
        }
        // Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;
            case EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;
            case EMIE_RMOUSE_PRESSED_DOWN:
                MouseState.RightButtonDown = true;
                break;
            case EMIE_RMOUSE_LEFT_UP:
                MouseState.RightButtonDown = false;
                break;
            case EMIE_MMOUSE_PRESSED_DOWN:
                MouseState.MidButtonDown = true;
                break;
            case EMIE_MMOUSE_LEFT_UP:
                MouseState.MidButtonDown = false;
                break;
            case EMIE_MOUSE_WHEEL:
            {
                MouseState.Wheel=event.MouseInput.Wheel;
                MouseState.WheelIsRead = false;
                break;
            }
//            case EMIE_MOUSE_MOVED:
//                MouseState.Position.X = event.MouseInput.X;
//                MouseState.Position.Y = event.MouseInput.Y;
//                break;

            default:
                // We won't use the wheel
                break;
            }
        }


        return false;
    }

    // This is used to check whether a key is being held down
    bool IsKeyDown(EKEY_CODE keyCode)
    {
        if (KeyIsDown[keyCode])
        {
            KeyIsDownBefore[keyCode]=true;
            return true;
        }
        else
        {
            KeyIsDownBefore[keyCode]=false;
            return false;
        }
    }

    bool IsKeyPressed(EKEY_CODE keyCode)//sprawdza czy klawisz jest wciśnięty pierwszy raz
    {
        if (KeyIsDown[keyCode] )
        {
            if (KeyIsDownBefore[keyCode]==false)
            {
                KeyIsDownBefore[keyCode]=true;
                return true;
            }
            return false;
        }
        else
        {
            KeyIsDownBefore[keyCode]=false;
            return false;
        }
    }
    bool IsMouseLeftPressed()
    {
        if (MouseState.LeftButtonDown)
        {
            if (MouseState.LeftDownBefore == false)
            {
                MouseState.LeftDownBefore = true;
                return true;
            }
            return false;
        }
        else
        {
            MouseState.LeftDownBefore = false;
            return false;
        }
    }
    bool IsMouseRightPressed()
    {
        if (MouseState.RightButtonDown)
        {
            if (MouseState.RightDownBefore == false)
            {
                MouseState.RightDownBefore = true;
                return true;
            }
            return false;
        }
        else
        {
            MouseState.RightDownBefore = false;
            return false;
        }
    }

    bool IsWheelMoved(float& value)
    {
        if (MouseState.WheelIsRead == false)
        {
            value=MouseState.Wheel;
            MouseState.Wheel = 0.0f;
            MouseState.WheelIsRead = true;
            return true;
        }
        else
        {
            return false;
        }


    }

    struct SMouseState  //definicja zagnieżdżona, ale publiczna
        {
            //core::position2di Position;
            bool LeftButtonDown = false;
            bool RightButtonDown = false;
            bool MidButtonDown = false;
            bool LeftDownBefore = false;
            bool RightDownBefore = false;
            bool MidDownBefore = false;
            float Wheel = 0.0f;
            bool WheelIsRead = true;
        };
    SMouseState MouseState;  //obiekt klasy powyższej zagnieżdżonej, publiczny

    SimEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
        {
            KeyIsDown[i] = false;//zerowanie tablicy
            KeyIsDownBefore[i]=false;//zerowanie drugiej tablicy
        }
    }

private:
    // We use this array(s) to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
    bool KeyIsDownBefore[KEY_KEY_CODES_COUNT];


};

#endif // SIMEVENTRECEIVER_H

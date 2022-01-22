/*
 * @file    CUserActionsStack.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_USER_ACTION_STACK_H_
#define _SBX_USER_ACTION_STACK_H_

namespace sbx
{
    class IUserAction;
    class CUserActionsStack
    {
    public:
                CUserActionsStack    ();
               ~CUserActionsStack    ();

        void    PushAction          (IUserAction* aAction);
        void    Undo                ();
        void    Redo                ();
        bool    CanUndo             () const;
        bool    CanRedo             () const;
        void    Clear               ();
    private:
        struct TImpl;
        TImpl* mImpl;
    };
} // sbx

#endif //_SBX_USER_ACTION_STACK_H_
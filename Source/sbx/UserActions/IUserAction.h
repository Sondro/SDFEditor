/*
 * @file    IUserAction.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_USER_ACTION_H_
#define _SBX_USER_ACTION_H_

namespace sbx
{
    class IUserAction
    {
        SBX_DISALLOW_COPY(IUserAction)
    public:
        virtual ~IUserAction() = default;
        virtual TUniqueIdCS GetTag() = 0;
        virtual void        Perform() = 0;
        virtual void        Undo() = 0;
    protected:
        IUserAction(){};
    };
}

#endif _SBX_USER_ACTION_H_
/*
 * @file    CUserActionsManager.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_USER_ACTIONS_MANAGER_H_
#define _SBX_USER_ACTIONS_MANAGER_H_

#include <sbx/UserActions/TUserActionMessages.h>
#include <sbx/DataStructures/CPropertySet.h>

namespace sbx { namespace ngr
{
    class CNodeGraph;
    class CNode;
    class CNodePin;
}}

namespace sbx
{
    class IUserAction;
    class CUserActionsManager : public CSingleton< CUserActionsManager >
    {
    public:
                CUserActionsManager ();
               ~CUserActionsManager ();
        bool    Init                ();
        void    End                 ();

    public:
//#       define USER_ACTION(_ActionName, ...) typedef IUserAction*(*F##_ActionName)(__VA_ARGS__);
//#       include <UserActions/UserActionsRegistry.def>
//#       define USER_ACTION(_ActionName, ...) F##_ActionName AllocUserAction##_ActionName;
//#       include <UserActions/UserActionsRegistry.def>

    private:
        struct TImpl;
        TImpl* mImpl;
    };
}

#endif //_SBX_USER_ACTION_STACK_H_
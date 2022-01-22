/*
 * @file    TUserActionMessages.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_USER_ACTION_MESSAGES_H_
#define _SBX_USER_ACTION_MESSAGES_H_

namespace sbx
{
    class IUserAction;
}

struct TMsgUndo{};
struct TMsgRedo{};
struct TMsgPushUserAction
{
    TMsgPushUserAction() : mUserAction(nullptr) {;}
    TMsgPushUserAction(sbx::IUserAction* aUserAction)
        : mUserAction(aUserAction)
    {;}
    sbx::IUserAction* mUserAction;
};
struct TMsgClearUserActionsStack{};
struct TMsgDocumentSaved{};
struct TMsgQueryUnsavedActions
{
    mutable bool mHaveUnsavedActions { false };
};
struct TMsgPermanentUserActionPerformed{};


struct TMsgQueryUserActionsStatus //TODO: implement listener for this message (to enable/disable undo/redo)
{
    mutable bool mCanUndo { false };
    mutable bool mCanRedo { false };
};

#endif //_SBX_USER_ACTION_MESSAGES_H_
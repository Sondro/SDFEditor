/*
 * @file    CUserActionsStack.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CUserActionsStack.h"
#include "IUserAction.h"
#include "TUserActionMessages.h"

#include <sbx/Library/FList.h>

namespace
{
    const static size_t kStackLimit = 50;
}

namespace sbx
{
    struct CUserActionsStack::TImpl
    {
        FList<IUserAction*> mDoneActions;
        FList<IUserAction*> mUndoneActions;

        TImpl() = default;
        ~TImpl()
        {
            ClearDoneActions();
            ClearUndoneActions();
        }

        void ClearDoneActions()
        {
            for(IUserAction* lAction : mDoneActions)
            {
                SBX_DELETE(lAction);
            }

            mDoneActions.clear();
        }

        void ClearUndoneActions()
        {
            for(IUserAction* lAction : mUndoneActions)
            {
                SBX_DELETE(lAction);
            }

            mUndoneActions.clear();
        }
    };

    CUserActionsStack::CUserActionsStack()
    {
        mImpl = SBX_NEW(TImpl);
    }

    CUserActionsStack::~CUserActionsStack()
    {
        SBX_DELETE(mImpl);
    }

    void CUserActionsStack::PushAction(IUserAction* aAction)
    {
        mImpl->ClearUndoneActions();
        aAction->Perform();
        mImpl->mDoneActions.push_back(aAction);
    
        while(mImpl->mDoneActions.size() > kStackLimit)
        {
            IUserAction* lPrevAction = mImpl->mDoneActions.front();
            mImpl->mDoneActions.pop_front();
            SBX_DELETE(lPrevAction);
        }
    }

    void CUserActionsStack::Undo()
    {
        if(CanUndo())
        {   
            IUserAction* aAction = mImpl->mDoneActions.back();
            mImpl->mDoneActions.pop_back();
            aAction->Undo();
            mImpl->mUndoneActions.push_back(aAction);
        }
    }

    void CUserActionsStack::Redo()
    {
        if(CanRedo())
        {
            IUserAction* aAction = mImpl->mUndoneActions.back();
            mImpl->mUndoneActions.pop_back();
            aAction->Perform();
            mImpl->mDoneActions.push_back(aAction);
        }
    }

    bool CUserActionsStack::CanUndo() const
    {
        return !mImpl->mDoneActions.empty();
    }

    bool CUserActionsStack::CanRedo() const
    {
        return !mImpl->mUndoneActions.empty();
    }

    void CUserActionsStack::Clear()
    {
        mImpl->ClearUndoneActions();
        mImpl->ClearDoneActions();
    }

}
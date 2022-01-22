/*
 * @file    CUserActionsManager.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CUserActionsManager.h"
#include "CUserActionsStack.h"
#include "TUserActionMessages.h"
#include "IUserAction.h"

#include <sbx/System/CApplication.h>
#include <sbx/Messaging/CChannel.h>


/*
#include <sbx/NodeGraph/CNodeGraph.h>
#include <sbx/NodeGraph/CNode.h>
#include <sbx/NodeGraph/CNodePin.h>
#include <sbx/NodeGraph/CNodeFactory.h>
#include <sbx/NodeGraph/Components/TCommonNodeMessages.h>
*/

//#include "UserActionsImpl.inl"
namespace sbx
{
    template <typename TCmd, typename... TArgs> IUserAction* AllocGenericTemplateUserAction(TArgs... args)
    {
        return SBX_NEW(TCmd, args...);
    }

    void InitUserActionManager(CUserActionsManager* aUserActionManager)
    {
        //Initialize command manager stubs
//#       define USER_ACTION(_ActionName, ...) aUserActionManager->AllocUserAction##_ActionName = AllocGenericTemplateUserAction< CUserAction##_ActionName, __VA_ARGS__ >;
//#       include <sbx/UserActions/UserActionsRegistry.def>
    }
}

namespace sbx
{
    struct CUserActionsManager::TImpl
    {
        CUserActionsStack mStack;
        int32_t           mUnsavedActions;
        bool              mUnsavedPermanentAction;

        TImpl()
            : mUnsavedActions(0)
            , mUnsavedPermanentAction(false)
        {
        }

        ~TImpl()
        {
            End();
        }

        void Init()
        {
            CChannel* lAppChannel = CApplication::GetInstance()->GetChannel();
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnUndo);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnRedo);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnPushAction);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnClearStack);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnDocumentSaved);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnQueryUnsavedActions);
            lAppChannel->RegisterCallback(this, &CUserActionsManager::TImpl::OnPermanentUserActionPerformed);
        }

        void End()
        {
            CChannel* lAppChannel = CApplication::GetInstance()->GetChannel();
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnUndo);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnRedo);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnPushAction);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnClearStack);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnDocumentSaved);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnQueryUnsavedActions);
            lAppChannel->UnregisterCallback(this, &CUserActionsManager::TImpl::OnPermanentUserActionPerformed);
        }

        void OnUndo(const TMsgUndo* aMsg)
        {
            if(mStack.CanUndo())
            {   
                --mUnsavedActions;
                mStack.Undo();
            }
        }

        void OnRedo(const TMsgRedo* aMsg)
        {
            if(mStack.CanRedo())
            {
                ++mUnsavedActions;
                mStack.Redo();
            }
        }

        void OnPushAction(const TMsgPushUserAction* aMsg)
        {
            ++mUnsavedActions;
            mStack.PushAction(aMsg->mUserAction);
        }

        void OnClearStack(const TMsgClearUserActionsStack* aMsg)
        {
            mUnsavedActions = 0;
            mStack.Clear();
        }

        void OnDocumentSaved(const TMsgDocumentSaved* aMsg)
        {
            mUnsavedActions = 0;
            mUnsavedPermanentAction = false;
        }

        void OnQueryUnsavedActions(const TMsgQueryUnsavedActions* aMsg)
        {
            aMsg->mHaveUnsavedActions = mUnsavedPermanentAction || (mUnsavedActions != 0);
        }

        void OnPermanentUserActionPerformed(const TMsgPermanentUserActionPerformed* aMsg)
        {
            mUnsavedPermanentAction = true;
        }
    };

    CUserActionsManager::CUserActionsManager()
        : mImpl(SBX_NEW(TImpl))
    {
    }

    CUserActionsManager::~CUserActionsManager()
    {
        SBX_DELETE(mImpl);
        mImpl = 0;
    }

    bool CUserActionsManager::Init()
    {
        InitUserActionManager(this);
        mImpl->Init();

        return true;
    }

    void CUserActionsManager::End()
    {
        
    }
} // sbx
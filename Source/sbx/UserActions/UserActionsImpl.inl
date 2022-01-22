/*
 * @file    UserActionsImpl.inl
 * @author  David Gallardo Moreno
 */

#include <set>

namespace
{
    //UIDCS_TEXT(CreateNode, "Create Node");
    //UIDCS_TEXT(ConnectPin, "Connect Pin");
}

namespace sbx
{
    using namespace ngr;

    //--------------------------------------------------------------
    // CREATE NODE
    //--------------------------------------------------------------
    class CUserActionCreateNode : public IUserAction
    {
    public:
        CUserActionCreateNode(CNodeGraph* aGraph, TUniqueId aNodeType, float aX, float aY, CNodePin* aDefaultSourcePin)
            : mGraph(aGraph)
            , mCreatedNode(nullptr)
            , mNodeType(aNodeType)
            , mX(aX)
            , mY(aY)
            , mDefaultSourcePin(aDefaultSourcePin)
        {
        }

        ~CUserActionCreateNode()
        {

        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag( std::string("Create Node ").append(mNodeType.GetStr()).c_str() ); 
            return sTag; 
        }

        void Perform() override
        {  
            if(!mCreatedNode)
            {
                mCreatedNode = CNodeFactory::GetInstance()->CreateNode(mGraph, mNodeType, CNodeFactory::eCreateNodeFlags_UseGlobalIds);
                mCreatedNode->GetChannel()->BroadcastMessage(TMsgSetNodePosition(mX, mY));
                mCreatedNode->GetChannel()->BroadcastMessage(TMsgGenerateNodeTexture());
            }
            else
            {
                mGraph->AddNode(mCreatedNode);
                mCreatedNode->GetChannel()->BroadcastMessage(TMsgNodeReAddedToGraph());
                mCreatedNode->GetChannel()->BroadcastMessage(TMsgGenerateNodeTexture());
            }
            
            if(mDefaultSourcePin)
            {
                if(mCreatedNode->GetInputPinsCount() > 0)
                {
                    CNodePin* lInputPin = mCreatedNode->GetInputPinAt(0);
                    mCreatedNode->GetInputPinAt(0)->AddConnection(mDefaultSourcePin);
                    mDefaultSourcePin->AddConnection(mCreatedNode->GetInputPinAt(0));
                    TMsgNodeConnected lNodeConnectedMsg;
                    lNodeConnectedMsg.mInputPin = lInputPin;
                    lNodeConnectedMsg.mOutputPin = mDefaultSourcePin;
                    lInputPin->GetParentNode()->GetChannel()->BroadcastMessage(lNodeConnectedMsg);
                }
            }

            SBX_LOG("[CUserActionCreateNode::Perform] Create %s node", mNodeType.GetStr());
        }

        void Undo() override
        {  
            mGraph->RemoveNode(mCreatedNode);
            mGraph->GetChannel()->BroadcastMessage(TMsgNodeRemoved(mCreatedNode.GetPtr()));

            if(mDefaultSourcePin)
            {
                if(mCreatedNode->GetInputPinsCount() > 0)
                {
                    mCreatedNode->GetInputPinAt(0)->RemoveConnection(mDefaultSourcePin);
                    mDefaultSourcePin->RemoveConnection(mCreatedNode->GetInputPinAt(0));
                    //No need to send a modified node message as this is creation
                }
            }
        }

    private:
        CNodeGraph* mGraph;
        CNode::Ref  mCreatedNode;
        TUniqueId   mNodeType; 
        float       mX;
        float       mY;
        CNodePin*   mDefaultSourcePin;
    };

    //--------------------------------------------------------------
    // CONNECT PIN
    //--------------------------------------------------------------
    class CUserActionConnectPin : public IUserAction
    {
    public:
        CUserActionConnectPin(CNodePin* aInputPin, CNodePin* aOutputPin)
            : mInputPin(aInputPin)
            , mOutputPin(aOutputPin)
        {
        }

        ~CUserActionConnectPin()
        {

        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag( std::string("Connect Pin ")
                                     .append(mOutputPin->GetName().GetStr())
                                     .append("->")
                                     .append(mInputPin->GetName().GetStr()).c_str() ); 
            return sTag; 
        }

        void Perform() override
        {  
            //remove my input connections with my input pin (just 1)
            for(uint32_t i = 0; i < mInputPin->GetConnectionsCount(); ++i)
            {
                mPreviousConnections.push_back(mInputPin->GetConnectionAt(i));
                mInputPin->GetConnectionAt(i)->RemoveConnection(mInputPin);
            }

            //clear pin connections (just 1)
            mInputPin->ClearConnections();

            //perform the connection
            mInputPin->AddConnection(mOutputPin);
            mOutputPin->AddConnection(mInputPin);
                    
            TMsgNodeConnected lNodeConnectedMsg;
            lNodeConnectedMsg.mInputPin = mInputPin;
            lNodeConnectedMsg.mOutputPin = mOutputPin;
            mInputPin->GetParentNode()->GetChannel()->BroadcastMessage(lNodeConnectedMsg);
            
            SBX_LOG("[CUserActionConnectPin::Perform]");
        }

        void Undo() override
        {  
           mInputPin->RemoveConnection(mOutputPin);
           mOutputPin->RemoveConnection(mInputPin);

            for(CNodePin* lPin : mPreviousConnections)
            {
                mInputPin->AddConnection(lPin);
                lPin->AddConnection(mInputPin);
                TMsgNodeConnected lNodeConnectedMsg;
                lNodeConnectedMsg.mInputPin = mInputPin;
                lNodeConnectedMsg.mOutputPin = lPin;
                mInputPin->GetParentNode()->GetChannel()->BroadcastMessage(lNodeConnectedMsg);
            }

            if(mPreviousConnections.size() == 0)
            {
                mInputPin->GetParentNode()->GetChannel()->BroadcastMessage(TMsgNodeEdited());
            }

            mPreviousConnections.clear();
            SBX_LOG("[CUserActionConnectPin::Undo]");
        }

    private:
        CNodePin* mInputPin;
        CNodePin* mOutputPin;
        FVector< CNodePin* > mPreviousConnections;
    };

    //--------------------------------------------------------------
    // DISCONNECT PIN
    //--------------------------------------------------------------
    class CUserActionDisconnectPin : public IUserAction
    {
    public:
        CUserActionDisconnectPin(CNodePin* aPin)
            : mPin(aPin)
            , mIsInputPin(false)
        {
            mIsInputPin = mPin->GetParentNode()->GetInputPinIndex(mPin) != UINT_MAX;
        }

        ~CUserActionDisconnectPin()
        {

        }

        TUniqueIdCS GetTag() override  
        { 
            static TUniqueIdCS sTag( std::string("Disconnect Pin ")
                                     .append(mPin->GetName().GetStr()).c_str() ); 
            return sTag; 
        }

        void Perform() override
        {  
            //remove my input connections with my input pin (just 1)
            for(uint32_t i = 0; i < mPin->GetConnectionsCount(); ++i)
            {
                mPreviousConnections.push_back(mPin->GetConnectionAt(i));
                mPin->GetConnectionAt(i)->RemoveConnection(mPin);
                if(!mIsInputPin)
                {
                     mPin->GetConnectionAt(i)->GetParentNode()->GetChannel()->BroadcastMessage(TMsgNodeEdited());
                }
            }

            //clear pin connections
            mPin->ClearConnections();
            if(mIsInputPin)
            {
                mPin->GetParentNode()->GetChannel()->BroadcastMessage(TMsgNodeEdited());
            }
            SBX_LOG("[CUserActionDisconnectPin::Perform]");
        }

        void Undo() override
        {  
            for(CNodePin* lOtherPin : mPreviousConnections)
            {
                mPin->AddConnection(lOtherPin);
                lOtherPin->AddConnection(mPin);

                if(mIsInputPin)
                {
                    TMsgNodeConnected lNodeConnectedMsg;
                    lNodeConnectedMsg.mInputPin = mPin;
                    lNodeConnectedMsg.mOutputPin = lOtherPin;
                    mPin->GetParentNode()->GetChannel()->BroadcastMessage(lNodeConnectedMsg);
                }
                else
                {
                    TMsgNodeConnected lNodeConnectedMsg;
                    lNodeConnectedMsg.mInputPin = lOtherPin;
                    lNodeConnectedMsg.mOutputPin = mPin;
                    lOtherPin->GetParentNode()->GetChannel()->BroadcastMessage(lNodeConnectedMsg);
                }
            }

            mPreviousConnections.clear();

            SBX_LOG("[CUserActionDisconnectPin::Undo]");
        }

    private:
        CNodePin* mPin;
        bool mIsInputPin;
        FVector< CNodePin* > mPreviousConnections;

    };

    //--------------------------------------------------------------
    // DELETE NODE
    //--------------------------------------------------------------
    class CUserActionDeleteNodes : public IUserAction
    {
    public:
        CUserActionDeleteNodes(CNode** aNodeList, uint32_t aCount)
        {
            mNodeList.reserve(aCount);
            for(uint32_t i = 0; i < aCount; ++i)
            {
                //TODO: allocate from a pool
                mNodeList.push_back(SBX_NEW(TRemovedNodeInfo, aNodeList[i]));
            }
        }

        ~CUserActionDeleteNodes()
        {
            for(TRemovedNodeInfo* lNodeInfo : mNodeList)
            {
                SBX_DELETE(lNodeInfo);
            }
            mNodeList.clear();
            mConnectionList.clear();
        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag("Delete Nodes"); 
            return sTag; 
        }
        void Perform() override
        {
            std::set<CNode*> lEditedNodes;
            for(TRemovedNodeInfo* lNodeInfo : mNodeList)
            {
                CNode* lNode = lNodeInfo->mNode.GetPtr();
                for(uint32_t i = 0; i < lNode->GetInputPinsCount(); ++i)
                {
                    CNodePin* lPin = lNode->GetInputPinAt(i);
                    for(uint32_t j = 0; j < lPin->GetConnectionsCount(); ++j)
                    {
                        lPin->GetConnectionAt(j)->RemoveConnection(lPin);
                        mConnectionList[lPin] = lPin->GetConnectionAt(j);
                    }
                    lPin->ClearConnections();
                }

                for(uint32_t i = 0; i < lNode->GetOutputPinsCount(); ++i)
                {
                    CNodePin* lPin = lNode->GetOutputPinAt(i);
                    for(uint32_t j = 0; j < lPin->GetConnectionsCount(); ++j)
                    {
                        lPin->GetConnectionAt(j)->RemoveConnection(lPin);
                        mConnectionList[lPin->GetConnectionAt(j)] = lPin;
                        lEditedNodes.insert(lPin->GetConnectionAt(j)->GetParentNode());
                        //lPin->GetConnectionAt(j)->GetParentNode()->GetChannel()->BroadcastMessage(TMsgNodeEdited());
                    }
                    lPin->ClearConnections();
                }

                lNode->GetParentGraph()->RemoveNode(lNode);
                lNode->GetParentGraph()->GetChannel()->BroadcastMessage(TMsgNodeRemoved(lNode));
            }

            for(TRemovedNodeInfo* lNodeInfo : mNodeList)
            {
                lEditedNodes.erase(lNodeInfo->mNode.GetPtr());
            }

            for(CNode* lNode : lEditedNodes)
            {
                lNode->GetChannel()->BroadcastMessage(TMsgNodeEdited());
            }
            
            SBX_LOG("[CUserActionDeleteNodes::Perform]");
        }

        void Undo() override
        {  
            std::set<CNode*> lEditedNodes;
            TMsgSetSelectedNodes lSetSelectedNodes;
            lSetSelectedNodes.mNodes.reserve(mNodeList.size32());
            for(TRemovedNodeInfo* lNodeInfo : mNodeList)
            {
                CNode* lNode = lNodeInfo->mNode.GetPtr();
                lSetSelectedNodes.mNodes.push_back(lNode);
                lEditedNodes.insert(lNode);

                lNode->GetParentGraph()->AddNode(lNode);
                lNode->GetChannel()->BroadcastMessage(TMsgNodeReAddedToGraph());
                //lNode->GetChannel()->BroadcastMessage(TMsgGenerateNodeTexture());
            
                for(auto & lOutputChannel : lNodeInfo->mOutputChannels)
                {
                    TMsgSetNodeAsOutput lSetAsOutput(lOutputChannel, lNode);
                    lNode->GetParentGraph()->GetChannel()->BroadcastMessage(lSetAsOutput);
                }
            }

            //Restore all connections
            for(auto & lConnPair : mConnectionList)
            {
                lConnPair.first->AddConnection(lConnPair.second);
                lConnPair.second->AddConnection(lConnPair.first);
                lEditedNodes.insert(lConnPair.first->GetParentNode());
            }

            //Refresh node textures
            for(CNode* lNode : lEditedNodes)
            {
                lNode->GetChannel()->BroadcastMessage(TMsgNodeEdited());
            }

            if(lSetSelectedNodes.mNodes.size() > 0)
            {
                lSetSelectedNodes.mNodes.at(0)->GetParentGraph()->GetChannel()->BroadcastMessage(lSetSelectedNodes);
            }

            SBX_LOG("[CUserActionDeleteNodes::Undo]");
        }

    private:
        struct TRemovedNodeInfo
        {
            FVector<gfx::EGfxOutputChannel::Type> mOutputChannels;
            CNode::Ref mNode;

            TRemovedNodeInfo(CNode* aNode)
            {
                SBX_ASSERT(aNode);
                if(aNode)
                {
                    mNode = aNode;
                    TMsgQueryNodeOutputChannels lNodeChannels(mNode);
                    mNode->GetParentGraph()->GetChannel()->BroadcastMessage(lNodeChannels);
                    mOutputChannels = lNodeChannels.mOutputChannels;
                }
            }
        };
        struct TRemovedConnection
        {
            CNodePin* mInputPin;
            CNodePin* mOutputPin;
            TRemovedConnection (CNodePin* aInputPin,
                                CNodePin* aOutputPin)
                : mInputPin(aInputPin)
                , mOutputPin(aOutputPin)
            {;}
        };
        FVector< TRemovedNodeInfo* > mNodeList;
        FMap< CNodePin*, CNodePin* > mConnectionList;
    };

    //--------------------------------------------------------------
    // MOVE NODE
    //--------------------------------------------------------------
    class CUserActionMoveNodes : public IUserAction
    {
    public:
        CUserActionMoveNodes(CNode** aNodeList, uint32_t aCount, float aOffsetX, float aOffsetY)
        {
            mNodeList.reserve(aCount);
            for(uint32_t i = 0; i < aCount; ++i)
            {
                TMsgQueryNodePosition lNodePosMsg;
                aNodeList[i]->GetChannel()->BroadcastMessage(lNodePosMsg);
                float lStoredX = lNodePosMsg.mX + aOffsetX;
                float lStoredY = lNodePosMsg.mY + aOffsetY;
                //TODO: allocate from a pool
                mNodeList.push_back(SBX_NEW(TMovedNodeInfo, aNodeList[i], lStoredX, lStoredY, lNodePosMsg.mX, lNodePosMsg.mY));
            }
        }

        ~CUserActionMoveNodes()
        {
            for(TMovedNodeInfo* lNodeInfo : mNodeList)
            {
                SBX_DELETE(lNodeInfo);
            }
            mNodeList.clear();
        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag("Move Nodes"); 
            return sTag; 
        }
        void Perform() override
        {  
  
            for(TMovedNodeInfo* lNodeInfo : mNodeList)
            {
                CNode* lNode = lNodeInfo->mNode.GetPtr();
                lNode->GetChannel()->BroadcastMessage(TMsgSetNodePosition(lNodeInfo->mStoredX, lNodeInfo->mStoredY));
            }
            SBX_LOG("[CUserActionMoveNode::Perform]");
        }

        void Undo() override
        {  
            for(TMovedNodeInfo* lNodeInfo : mNodeList)
            {
                CNode* lNode = lNodeInfo->mNode.GetPtr();
                lNode->GetChannel()->BroadcastMessage(TMsgSetNodePosition(lNodeInfo->mX, lNodeInfo->mY));
            }
            SBX_LOG("[CUserActionMoveNode::Undo]");
        }

    private:
        struct TMovedNodeInfo
        {
            CNode::Ref  mNode;
            float       mStoredX;
            float       mStoredY;
            float       mX;
            float       mY;

            TMovedNodeInfo( CNode* aNode,
                            float aStoredX,
                            float aStoredY,
                            float aX,
                            float aY)
                : mNode(aNode)
                , mStoredX(aStoredX)
                , mStoredY(aStoredY)
                , mX(aX)
                , mY(aY)
            {;}
        };

        FVector< TMovedNodeInfo* > mNodeList;
    };

    //--------------------------------------------------------------
    // EDIT NODE
    //--------------------------------------------------------------
    class CUserActionEditNode : public IUserAction
    {
    public:
        CUserActionEditNode(CNode* aNode, CPropertySet::Ref aStoredProperties, CPropertySet::Ref aProperties)
            : mNode(aNode)
            , mFirstTime(true)
        {
            mPrevProperties.Init(*aStoredProperties);
            mProperties.Init(*aProperties);
        }

        ~CUserActionEditNode()
        {

        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag( std::string("Edit Node ").append(mNode->GetName().GetStr()).c_str()); 
            return sTag; 
        }
        void Perform() override
        {  
            /*if(mFirstTime)
            {
                mFirstTime = false;
            }
            else
            {*/
                TMsgQueryPropertySetRef lQueryPropertySet;
                mNode->GetChannel()->BroadcastMessage(lQueryPropertySet);
                lQueryPropertySet.mPropertySet->Init(mProperties);
                lQueryPropertySet.mStoredPropertySet->Init(mProperties);
                mNode->GetChannel()->BroadcastMessage(TMsgNodeEdited());
                SBX_LOG("[CUserActionEditNode::Perform]");
            //}
        }

        void Undo() override
        {  
            TMsgQueryPropertySetRef lQueryPropertySet;
            mNode->GetChannel()->BroadcastMessage(lQueryPropertySet);
            lQueryPropertySet.mPropertySet->Init(mPrevProperties);
            lQueryPropertySet.mStoredPropertySet->Init(mPrevProperties);
            mNode->GetChannel()->BroadcastMessage(TMsgNodeEdited());
            SBX_LOG("[CUserActionEditNode::Undo]");
        }

    private:
        CNode::Ref mNode;
        CPropertySet mPrevProperties; 
        CPropertySet mProperties;
        bool mFirstTime;
    };

    //--------------------------------------------------------------
    // SET AS OUTPUT
    //--------------------------------------------------------------
    class CUserActionSetAsOutput : public IUserAction
    {
    public:
         CUserActionSetAsOutput(CNode* aNode, sbx::gfx::EGfxOutputChannel::Type aOutputChannel, bool aEnable)
            : mNode(aNode)
            , mOutputChannel(aOutputChannel)
            , mEnable(aEnable)
            , mFirstTime(true)
        {

        }

        ~CUserActionSetAsOutput()
        {

        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag("Set Node as Output"); 
            return sTag; 
        }
        void Perform() override
        {  
            if(mFirstTime)
            {
                mFirstTime = false;
                if(mEnable)
                {
                    TMsgQueryNodeForOutputChannel lMsgQuery(mOutputChannel);
                    mNode->GetParentGraph()->GetChannel()->BroadcastMessage(lMsgQuery);
                    mPrevNode = lMsgQuery.mNode;
                }
            }

            TMsgSetNodeAsOutput lMsg;
            lMsg.mOutputChannel = mOutputChannel;
            lMsg.mNode = mNode;
            lMsg.mEnable = mEnable;
            mNode->GetParentGraph()->GetChannel()->BroadcastMessage(lMsg);
            SBX_LOG("[CSetAsOutput::Perform]");
        }

        void Undo() override
        {  
            TMsgSetNodeAsOutput lMsg;
            lMsg.mOutputChannel = mOutputChannel;

            if(mPrevNode)
            {
                lMsg.mNode = mPrevNode;
                lMsg.mEnable = true;        
            }
            else
            {
                lMsg.mNode = mNode;
                lMsg.mEnable = !mEnable;
            }
            mNode->GetParentGraph()->GetChannel()->BroadcastMessage(lMsg);
            SBX_LOG("[CSetAsOutput::Undo]");
        }
    private:
        CNode::Ref mNode;
        CNode::Ref mPrevNode;
        gfx::EGfxOutputChannel::Type mOutputChannel;
        bool mEnable;
        bool mFirstTime;
    };

    //--------------------------------------------------------------
    // PASTE NODE
    //--------------------------------------------------------------
    class CUserActionPasteNodes : public IUserAction
    {
    public:
        CUserActionPasteNodes(float aOffsetX, float aOffsetY)
            : mFirstTime(true)
            , mOffsetX(aOffsetX)
            , mOffsetY(aOffsetY)
        {

        }

        ~CUserActionPasteNodes()
        {

        }

        TUniqueIdCS GetTag() override 
        { 
            static TUniqueIdCS sTag("Paste Nodes"); 
            return sTag; 
        }

        void Perform() override
        {  
            SBX_LOG("[CUserActionPasteNodes::Perform]");
            CNodeFactory* lFactory = CNodeFactory::GetInstance();
            CNodeGraph* lGraph = lFactory->GetCurrentGraph();
            if(mFirstTime)
            {
                mFirstTime = false;
                lFactory->UnseralizeFromClipboard(lGraph, mOffsetX, mOffsetY, mPastedNodes);
            }
            else
            {
                for(auto & lNode : mPastedNodes)
                {
                    lGraph->AddNode(lNode);
                }
            }

            TMsgSetSelectedNodes lSetSelectedNodes;
            for(auto & lNode : mPastedNodes)
            {
                lNode->GetChannel()->BroadcastMessage(TMsgNodeEdited());
                lSetSelectedNodes.mNodes.push_back(lNode.GetPtr());
            }
            lGraph->GetChannel()->BroadcastMessage(lSetSelectedNodes);
        }

        void Undo() override
        {  
            SBX_LOG("[CUserActionPasteNodes::Undo]");
            for(auto & lNode : mPastedNodes)
            {
                lNode->GetParentGraph()->RemoveNode(lNode);
                lNode->GetParentGraph()->GetChannel()->BroadcastMessage(TMsgNodeRemoved(lNode.GetPtr()));
            }
        }

    private:
        bool mFirstTime;
        CNodeGraph* mGraph;
        float mOffsetX, mOffsetY;
        FVector< CNode::Ref > mPastedNodes;
    };

} // sbx
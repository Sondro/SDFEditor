/*
 * @file    GLDataBuffersCommands.inl
 * @author  David Gallardo Moreno
 */

namespace sbx { namespace gl 
{
    //-----------------------------------------------------------
    // CREATE SHARED BLOCK
    //-----------------------------------------------------------
    class GLCreateSharedBlockCmd : public TGPUCommand
    {
    public:
        GLCreateSharedBlockCmd( TGPUResourceRef const & aBlockResRef, 
                                CRawBuffer::Ref const & aBlockData)
            : mBlockRef (aBlockResRef)
            , mBlockData(aBlockData)
        {

        }

        void Execute()
        {
            TGPUHandle lBlockHandle = 0;

            glGenBuffers(1, &lBlockHandle);
            glBindBuffer(GL_UNIFORM_BUFFER, lBlockHandle);
            glBufferData(GL_UNIFORM_BUFFER, mBlockData->GetSize(), mBlockData->GetByteArray(), GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            mBlockRef.SetHandle(lBlockHandle);
        }
    private:
        TGPUResourceRef mBlockRef;
        CRawBuffer::Ref mBlockData;
    };

    //-----------------------------------------------------------
    // UPDATE SHARED BLOCK
    //-----------------------------------------------------------
    class GLUpdateSharedBlockCmd : public TGPUCommand
    {
    public:
        GLUpdateSharedBlockCmd( TGPUResourceRef const & aBlockResRef, 
                                CRawBuffer::Ref const & aBlockData,
                                uint32_t aOffset,
                                uint32_t aSize)
            : mBlockRef (aBlockResRef)
            , mBlockData(aBlockData)
            , mOffset(aOffset)
            , mSize(aSize)
        {

        }

        void Execute()
        {
            TGPUHandle lBlockHandle = mBlockRef.GetHandle();
            glBindBuffer(GL_UNIFORM_BUFFER, lBlockHandle);
            glBufferSubData(GL_UNIFORM_BUFFER, mOffset, mSize, mBlockData->GetByteArray() + mOffset);
            glBindBuffer(GL_UNIFORM_BUFFER, mOffset);
        }
    private:
        TGPUResourceRef mBlockRef;
        CRawBuffer::Ref mBlockData;
        uint32_t        mOffset;
        uint32_t        mSize;
    };

    //-----------------------------------------------------------
    // DESTORY SHARED BLOCK
    //-----------------------------------------------------------
    class GLDestroySharedBlockCmd : public TGPUCommand
    {
    public:
        GLDestroySharedBlockCmd( TGPUResourceRef const & aBlockResRef )
            : mBlockRef(aBlockResRef)
        {

        }

        void Execute()
        {
            TGPUHandle lBlockHandle = mBlockRef.GetHandle();
            glDeleteBuffers(1, &lBlockHandle);
        }
    private:
        TGPUResourceRef mBlockRef;
    };

}} // sbx::gl
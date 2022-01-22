/*
 * @file    LangUtils.h
 * @author  David Gallardo Moreno
 */


#ifndef SBX_LANG_UTILS
#define SBX_LANG_UTILS

//-----------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------

#define SBX_DISALLOW_COPY(_T) public: _T(_T const &) = delete; _T& operator=(_T const &) = delete; protected: 
#define SBX_DELETE_DEFAULT_CTORS(_T) public: _T() = delete; _T(_T const &) = delete; _T& operator=(_T const &) = delete; protected: 




namespace sbx
{
    //-----------------------------------------------------------------------
    // GetTypeId
    //-----------------------------------------------------------------------
    struct __type_id_counter
    {
        static uint32_t sTypeIdCounter;
    };// __type_id_counter


    template <typename T> uint32_t GetTypeId()
    {
        static uint32_t sTypeId = __type_id_counter::sTypeIdCounter++;
        return sTypeId;
    }

    template <typename T> const char* GetTypeName(T* aObj)
    {
        return typeid(*aObj).name();
    }

    void ManageObjectAllocationInfo(uint32_t aTypeId, const char* aTypeIdName, size_t aSize, void* aObj);
    void ManageObjectDeleteInfo(uint32_t aTypeId, const char* aTypeIdName, void* aObj);

    template <typename T> T* LogObjectAllocation(T* aObj)
    {
        ManageObjectAllocationInfo(GetTypeId<T>(), GetTypeName(aObj), sizeof(T), aObj);
        return aObj;
    }

    template <typename T> T* LogObjectDeletion(T* aObj)
    {
        ManageObjectDeleteInfo(GetTypeId<T>(), GetTypeName(aObj), aObj);
        aObj->~T();
        /*aObj->~T();
        sbxFree(aObj);*/
        return aObj;
        //delete aObj;
    }

    //-----------------------------------------------------------------------
    // CSingleton
    //-----------------------------------------------------------------------
    template <typename T> class CSingleton
    {
    public:
        static T* GetInstance()
        {
            static T sInstance;
            return &sInstance;
        }

        CSingleton(){};

        SBX_DISALLOW_COPY(CSingleton)
    };

   
} //sbx

#endif //SBX_LANG_UTILS

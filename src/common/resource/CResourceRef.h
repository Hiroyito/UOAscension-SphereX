/**
* @file CResourceRef.h
*
*/

#ifndef _INC_CRESOURCEREF_H
#define _INC_CRESOURCEREF_H

#include "../sphere_library/CSPtrTypeArray.h"
#include "CResourceLink.h"
#include "CResourceID.h"

class CSString;
class CResourceLink;
class CScript;


class CResourceRef
{
private:
    CResourceLink* m_pLink;
public:
    static const char *m_sClassName;
    CResourceRef()
    {
        m_pLink = nullptr;
    }
    CResourceRef(CResourceLink* pLink) : m_pLink(pLink)
    {
        ASSERT(pLink);
        pLink->AddRefInstance();
    }
    CResourceRef(const CResourceRef& copy)
    {
        m_pLink = copy.m_pLink;
        if (m_pLink != nullptr)
            m_pLink->AddRefInstance();
    }
    ~CResourceRef()
    {
        if (m_pLink != nullptr)
            m_pLink->DelRefInstance();
    }
    CResourceRef& operator=(const CResourceRef& other)
    {
        if (this != &other)
            SetRef(other.m_pLink);
        return *this;
    }

public:
    inline CResourceLink* GetRef() const
    {
        return m_pLink;
    }
    void SetRef(CResourceLink* pLink)
    {
        if (m_pLink != nullptr)
            m_pLink->DelRefInstance();

        m_pLink = pLink;

        if (pLink != nullptr)
            pLink->AddRefInstance();
    }
    inline operator CResourceLink*() const
    {
        return GetRef();
    }
};

class CResourceRefArray : public CSPtrTypeArray<CResourceRef>
{
    // Define a list of pointer references to resource. (Not owned by the list)
    // An indexed list of CResourceLink s.
private:
    lpctstr GetResourceName( size_t iIndex ) const;
public:
    static const char *m_sClassName;
    CResourceRefArray() = default;
    CResourceRefArray(const CResourceRefArray& copy);
    CResourceRefArray& operator=(const CResourceRefArray& other);
    size_t FindResourceType( RES_TYPE type ) const;
    size_t FindResourceID( const CResourceID & rid ) const;
    size_t FindResourceName( RES_TYPE restype, lpctstr ptcKey ) const;

    void WriteResourceRefList( CSString & sVal ) const;
    bool r_LoadVal( CScript & s, RES_TYPE restype );
    void r_Write( CScript & s, lpctstr ptcKey ) const;

    inline bool ContainsResourceID( const CResourceID & rid ) const
    {
        return FindResourceID(rid) != SCONT_BADINDEX;
    }
    inline bool ContainsResourceName( RES_TYPE restype, lpctstr & ptcKey ) const
    {
        return FindResourceName(restype, ptcKey) != SCONT_BADINDEX;
    }
};


#endif // _INC_CRESOURCEREF_H

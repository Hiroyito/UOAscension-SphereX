/**
* @file CResourceLink.h
*
*/

#ifndef _INC_CRESOURCELINK_H
#define _INC_CRESOURCELINK_H

#include "../CScriptContexts.h"
#include "CResourceDef.h"

class CResourceScript;


#define MAX_TRIGGERS_ARRAY	6

class CResourceLink : public CResourceDef
{
    // A single resource object that also has part of itself remain in resource file.
    // A pre-indexed link into a script file.
    // This is a CResourceDef not fully read into memory at index time.
    // We are able to lock it and read it as needed
private:
    CResourceScript * m_pScript;	// we already found the script.
    CScriptLineContext m_Context;

    dword m_dwRefInstances;	// How many CResourceRef objects refer to this ?
public:
    static const char *m_sClassName;
    dword m_dwOnTriggers[MAX_TRIGGERS_ARRAY];

#define XTRIG_UNKNOWN 0	// bit 0 is reserved to say there are triggers here that do not conform.

public:
    inline void AddRefInstance()
    {
        ++m_dwRefInstances;
    }
    inline void DelRefInstance()
    {
#ifdef _DEBUG
        ASSERT(m_dwRefInstances != (dword)-1);    // catching underflows
#endif
        --m_dwRefInstances;
    }
    inline dword GetRefInstances() const
    {
        return m_dwRefInstances;
    }

    bool IsLinked() const;	// been loaded from the scripts ?
    CResourceScript * GetLinkFile() const;
    int GetLinkOffset() const;
    void SetLink( CResourceScript * pScript );
    void CopyTransfer( CResourceLink * pLink );
    void ScanSection( RES_TYPE restype );
    void ClearTriggers();
    void SetTrigger( int i );
    bool HasTrigger( int i ) const;
    bool ResourceLock( CResourceLock & s );

public:
    CResourceLink( CResourceID rid, const CVarDefContNum * pDef = nullptr );
    virtual ~CResourceLink() = default;

private:
    CResourceLink(const CResourceLink& copy);
    CResourceLink& operator=(const CResourceLink& other);
};

#endif // _INC_CRESOURCELINK_H

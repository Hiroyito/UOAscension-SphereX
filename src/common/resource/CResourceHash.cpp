/**
* @file CResourceHash.cpp
*
*/

#include "CResourceDef.h"
#include "CResourceHash.h"


size_t CResourceHash::FindKey(const CResourceID& rid) const
{
    return m_Array[GetHashArray(rid)].FindKey(rid);
}
CResourceDef* CResourceHash::GetAt(const CResourceID& rid, size_t index) const
{
    return m_Array[GetHashArray(rid)].at(index);
}
size_t CResourceHash::AddSortKey(const CResourceID& rid, CResourceDef* pNew)
{
    return m_Array[GetHashArray(rid)].AddSortKey(pNew, rid);
}
void CResourceHash::SetAt(const CResourceID& rid, size_t index, CResourceDef* pNew)
{
    m_Array[GetHashArray(rid)].assign(index, pNew);
}


int CResourceHashArray::CompareKey( CResourceID rid, CResourceDef * pBase, bool fNoSpaces ) const
{
    UNREFERENCED_PARAMETER(fNoSpaces);
    dword dwID1 = rid.GetPrivateUID();
    ASSERT( pBase );
    const CResourceID& baseResID = pBase->GetResourceID();
    dword dwID2 = baseResID.GetPrivateUID();
    if (dwID1 > dwID2)
        return 1;
    if (dwID1 == dwID2)
    {
        ushort uiPage1 = rid.GetResPage();
        ushort uiPage2 = baseResID.GetResPage();
        if (uiPage1 == UINT16_MAX)  //rid page == UINT16_MAX: search independently from the page
            return 0;
        if (uiPage1 > uiPage2)
            return 1;
        else if (uiPage1 == uiPage2)
            return 0;
        else
            return -1;
    }
    return -1;
}



#include "../../common/CException.h"
#include "CItemMessage.h"
#include "CItemVendable.h"

CItemMessage::CItemMessage( ITEMID_TYPE id, CItemBase * pItemDef ) :
    CCTimedObject(PROFILE_ITEMS), CItemVendable( id, pItemDef )
{
}

CItemMessage::~CItemMessage()
{
    DeletePrepare();	// Must remove early because virtuals will fail in child destructor.
    UnLoadSystemPages();
}

lpctstr const CItemMessage::sm_szLoadKeys[CIC_QTY+1] = {
    "AUTHOR",
    "BODY",
    "PAGES",	// (W)
    "TITLE",	// same as name
    nullptr,
};

void CItemMessage::r_Write(CScript & s)
{
    ADDTOCALLSTACK_INTENSIVE("CItemMessage::r_Write");
    CItemVendable::r_Write(s);
    s.WriteKey("AUTHOR", m_sAuthor);

    // Store the message body lines. MAX_BOOK_PAGES
    TemporaryString tsTemp;
	tchar* pszTemp = static_cast<tchar *>(tsTemp);
    for ( word i = 0; i < GetPageCount(); ++i )
    {
        sprintf(pszTemp, "BODY.%" PRIu16, i);
        lpctstr pszText = GetPageText(i);
        s.WriteKey(pszTemp, pszText != nullptr ? pszText : "");
    }
}

bool CItemMessage::r_LoadVal(CScript &s)
{
    ADDTOCALLSTACK("CItemMessage::r_LoadVal");
    EXC_TRY("LoadVal");
        // Load the message body for a book or a bboard message.
        if ( s.IsKeyHead("BODY", 4) )
        {
            AddPageText(s.GetArgStr());
            return true;
        }

        switch ( FindTableSorted(s.GetKey(), sm_szLoadKeys, CountOf(sm_szLoadKeys) - 1) )
        {
            case CIC_AUTHOR:
                if ( s.GetArgStr()[0] != '0' )
                    m_sAuthor = s.GetArgStr();
                return true;
            case CIC_BODY:		// handled above
                return false;
            case CIC_PAGES:		// not settable (used for resource stuff)
                return false;
            case CIC_TITLE:
                SetName(s.GetArgStr());
                return true;
        }
        return CItemVendable::r_LoadVal(s);
    EXC_CATCH;

    EXC_DEBUG_START;
            EXC_ADD_SCRIPT;
    EXC_DEBUG_END;
    return false;
}

bool CItemMessage::r_WriteVal(lpctstr pszKey, CSString &sVal, CTextConsole *pSrc)
{
    ADDTOCALLSTACK("CItemMessage::r_WriteVal");
    EXC_TRY("WriteVal");
        // Load the message body for a book or a bboard message.
        if ( !strnicmp(pszKey, "BODY", 4) )
        {
            pszKey += 4;
            size_t iPage = Exp_GetVal(pszKey);
            if ( m_sBodyLines.IsValidIndex(iPage) == false )
                return false;
            sVal = *m_sBodyLines[iPage];
            return true;
        }

        switch ( FindTableSorted(pszKey, sm_szLoadKeys, CountOf(sm_szLoadKeys) - 1) )
        {
            case CIC_AUTHOR:
                sVal = m_sAuthor;
                return true;
            case CIC_BODY:		// handled above
                return false;
            case CIC_PAGES:		// not settable (used for resource stuff)
                sVal.FormatSTVal(m_sBodyLines.size());
                return true;
            case CIC_TITLE:
                sVal = GetName();
                return true;
        }
        return CItemVendable::r_WriteVal(pszKey, sVal, pSrc);
    EXC_CATCH;

    EXC_DEBUG_START;
            EXC_ADD_KEYRET(pSrc);
    EXC_DEBUG_END;
    return false;
}

lpctstr const CItemMessage::sm_szVerbKeys[] =
        {
                "ERASE",
                "PAGE",
                nullptr,
        };

bool CItemMessage::r_Verb(CScript & s, CTextConsole *pSrc)
{
    ADDTOCALLSTACK("CItemMessage::r_Verb");
    EXC_TRY("Verb");
        ASSERT(pSrc);
        if ( s.IsKey(sm_szVerbKeys[0]) )
        {
            // 1 based pages
            word wPage = (s.GetArgStr()[0] && toupper(s.GetArgStr()[0]) != 'A') ? s.GetArgWVal() : 0;
            if ( wPage <= 0 )
            {
                m_sBodyLines.clear();
                return true;
            }
            else if ( wPage <= m_sBodyLines.size() )
            {
                m_sBodyLines.erase(wPage - 1);
                return true;
            }
        }
        if ( s.IsKeyHead("PAGE", 4) )
        {
            word wPage = (word)ATOI(s.GetKey() + 4);
            if ( wPage <= 0 )
                return false;

            SetPageText(wPage - 1, s.GetArgStr());
            return true;
        }
        return CItemVendable::r_Verb(s, pSrc);
    EXC_CATCH;

    EXC_DEBUG_START;
            EXC_ADD_SCRIPTSRC;
    EXC_DEBUG_END;
    return false;
}

void CItemMessage::DupeCopy(const CItem *pItem)
{
    ADDTOCALLSTACK("CItemMessage::DupeCopy");
    CItemVendable::DupeCopy(pItem);

    const CItemMessage *pMsgItem = dynamic_cast<const CItemMessage *>(pItem);
    if ( pMsgItem == nullptr )
        return;

    m_sAuthor = pMsgItem->m_sAuthor;
    for ( word i = 0; i < pMsgItem->GetPageCount(); ++i )
        SetPageText(i, pMsgItem->GetPageText(i));
}

word CItemMessage::GetPageCount() const
{
    size_t sz = m_sBodyLines.size();
    ASSERT(sz < RES_PAGE_MAX);
    return (word)sz;
}

lpctstr CItemMessage::GetPageText( word wPage ) const
{
    if ( m_sBodyLines.IsValidIndex(wPage) == false )
        return nullptr;
    if ( m_sBodyLines[wPage] == nullptr )
        return nullptr;
    return m_sBodyLines[wPage]->GetPtr();
}

void CItemMessage::SetPageText( word wPage, lpctstr pszText )
{
    if ( pszText == nullptr )
        return;
    m_sBodyLines.assign_at_grow(wPage, new CSString(pszText));
}

void CItemMessage::AddPageText( lpctstr pszText )
{
    m_sBodyLines.emplace_back(new CSString(pszText) );
}

void CItemMessage::UnLoadSystemPages()
{
    m_sAuthor.Empty();
    m_sBodyLines.clear();
}
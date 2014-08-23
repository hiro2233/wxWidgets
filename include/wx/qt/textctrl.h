/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/textctrl.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TEXTCTRL_H_
#define _WX_QT_TEXTCTRL_H_

#include "wx/qt/pointer_qt.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
    wxTextCtrl();
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString &value = wxEmptyString,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxTextCtrlNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &value = wxEmptyString,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxTextCtrlNameStr);

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual void MarkDirty();
    virtual void DiscardEdits();

    virtual bool SetStyle(long start, long end, const wxTextAttr& style);
    virtual bool GetStyle(long position, wxTextAttr& style);
    virtual bool SetDefaultStyle(const wxTextAttr& style);
    
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;

    virtual void ShowPosition(long pos);

    virtual void SetInsertionPoint(long pos);
    virtual long GetInsertionPoint() const;
    virtual void SetSelection( long from, long to );

    virtual wxString DoGetValue() const;
    virtual void DoSetValue(const wxString &text, int flags = 0);
    virtual void WriteText(const wxString& text);

    virtual QWidget *GetHandle() const;

protected:
    virtual wxSize DoGetBestSize() const;

    virtual bool DoLoadFile(const wxString& file, int fileType);
    virtual bool DoSaveFile(const wxString& file, int fileType);
    
private:
    wxQtPointer< QLineEdit > m_qtLineEdit;
    wxQtPointer< QTextEdit > m_qtTextEdit;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxTextCtrl );
};

#endif // _WX_QT_TEXTCTRL_H_

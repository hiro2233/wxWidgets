/////////////////////////////////////////////////////////////////////////////
// Name:        plot.cpp
// Purpose:     wxPlotWindow
// Author:      Robert Roebling
// Modified by:
// Created:     12/01/2000
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "plot.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dcclient.h"
#endif

#include "wx/generic/plot.h"

#include <math.h>

//-----------------------------------------------------------------------------
// wxPlotCurve
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPlotCurve, wxObject)

wxPlotCurve::wxPlotCurve( int offsetY, double startY, double endY )
{
    m_offsetY = offsetY;
    m_startY = startY;
    m_endY = endY;
}

//-----------------------------------------------------------------------------
// wxPlotArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPlotArea, wxWindow)

BEGIN_EVENT_TABLE(wxPlotArea, wxWindow)
  EVT_PAINT(        wxPlotArea::OnPaint)
  EVT_LEFT_DOWN(    wxPlotArea::OnMouse)
END_EVENT_TABLE()

wxPlotArea::wxPlotArea( wxPlotWindow *parent )
        : wxWindow( parent, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, "plotarea" )
{
    m_owner = parent;

    SetBackgroundColour( *wxWHITE );
}

void wxPlotArea::OnMouse( wxMouseEvent &event )
{
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    int view_x;
    int view_y;
    m_owner->GetViewStart( &view_x, &view_y );
    view_x *= 10;
    view_y *= 10;
    
    int x = event.GetX();
    int y = event.GetY();
    x += view_x;
    y += view_y;
    
    wxNode *node = m_owner->m_curves.First();
    while (node)
    {
        wxPlotCurve *curve = (wxPlotCurve*)node->Data();
            
        double double_client_height = (double)client_height;
        double range = curve->GetEndY() - curve->GetStartY();
        double end = curve->GetEndY();
        wxCoord offset_y = curve->GetOffsetY();
            
        double dy = (end - curve->GetY( x )) / range;
        wxCoord curve_y = (wxCoord)(dy * double_client_height) - offset_y - 1;
                
        if ((y-curve_y < 4) && (y-curve_y > -4))
        {
            m_owner->SetCurrent( curve );
            return;
        }
            
        node = node->Next();
    }
}

void wxPlotArea::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    int view_x;
    int view_y;
    m_owner->GetViewStart( &view_x, &view_y );
    view_x *= 10;
    view_y *= 10;

    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );

    wxRegionIterator upd( GetUpdateRegion() );
    
    while (upd)
    {
        int update_x = upd.GetX();
        int update_y = upd.GetY();
        int update_width = upd.GetWidth();
        
        update_x += view_x;
        update_y += view_y;
        
/*
        if (m_owner->m_current)
        {
            dc.SetPen( *wxLIGHT_GREY_PEN );
            int base_line = client_height - m_owner->m_current->GetOffsetY();
            dc.DrawLine( update_x-1, base_line-1, update_x+update_width+2, base_line-1 );
        }
*/
        
        wxNode *node = m_owner->m_curves.First();
        while (node)
        {
            wxPlotCurve *curve = (wxPlotCurve*)node->Data();
            
            if (curve == m_owner->GetCurrent())
                dc.SetPen( *wxBLACK_PEN );
            else
                dc.SetPen( *wxLIGHT_GREY_PEN );

            int start_x = wxMax( update_x-1, curve->GetStartX() );
            int end_x = wxMin( update_x+update_width+2, curve->GetEndX() );
            
            double double_client_height = (double)client_height;
            double range = curve->GetEndY() - curve->GetStartY();
            double end = curve->GetEndY();
            wxCoord offset_y = curve->GetOffsetY();
            
            wxCoord y=0,last_y=0;
            for (int x = start_x; x < end_x; x++)
            {
                double dy = (end - curve->GetY( x )) / range;
                y = (wxCoord)(dy * double_client_height) - offset_y - 1;
            
                if (x != start_x)
                    dc.DrawLine( x-1, last_y, x, y );
            
                last_y = y;
            }
            node = node->Next();
        }
        upd ++;
    }
}

//-----------------------------------------------------------------------------
// wxPlotWindow
//-----------------------------------------------------------------------------

#define  ID_ENLARGE_100   1000
#define  ID_ENLARGE_50    1001
#define  ID_SHRINK_33     1002
#define  ID_SHRINK_50     1003

#define  ID_MOVE_UP       1006
#define  ID_MOVE_DOWN     1007


IMPLEMENT_DYNAMIC_CLASS(wxPlotWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxPlotWindow, wxScrolledWindow)
  EVT_PAINT(        wxPlotWindow::OnPaint)
END_EVENT_TABLE()

wxPlotWindow::wxPlotWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, int flag )
        : wxScrolledWindow( parent, id, pos, size, flag, "plotcanvas" )
{
    m_area = new wxPlotArea( this );
    
    wxBoxSizer *mainsizer = new wxBoxSizer( wxHORIZONTAL );
    
    wxBoxSizer *buttonlist = new wxBoxSizer( wxVERTICAL );
    buttonlist->Add( new wxButton( this, ID_ENLARGE_100, _("+ 100%") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( new wxButton( this, ID_ENLARGE_50, _("+ 50%") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( new wxButton( this, ID_SHRINK_33, _("- 33%") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( new wxButton( this, ID_SHRINK_50, _("- 50%") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( 20,20, 0 );
    buttonlist->Add( new wxButton( this, ID_MOVE_UP, _("Up") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( new wxButton( this, ID_MOVE_DOWN, _("Down") ), 0, wxEXPAND|wxALL, 5 );
    buttonlist->Add( 20,20, 1 );
    
    mainsizer->Add( buttonlist, 0, wxEXPAND );
    
    mainsizer->Add( m_area, 1, wxEXPAND|wxLEFT, 50 );
    
    SetAutoLayout( TRUE );
    SetSizer( mainsizer );

    SetTargetWindow( m_area );

    SetBackgroundColour( *wxWHITE );
    
    m_current = (wxPlotCurve*) NULL;
}

wxPlotWindow::~wxPlotWindow()
{
}

void wxPlotWindow::Add( wxPlotCurve *curve )
{
    m_curves.Append( curve );
    if (!m_current) m_current = curve;
}

size_t wxPlotWindow::GetCount()
{
    return m_curves.GetCount();
}

wxPlotCurve *wxPlotWindow::GetAt( size_t n )
{
    wxNode *node = m_curves.Nth( n );
    if (!node)
        return (wxPlotCurve*) NULL;
        
    return (wxPlotCurve*) node->Data();
}

void wxPlotWindow::SetCurrent( wxPlotCurve* current )
{
    m_current = current;
    m_area->Refresh( FALSE );
    
    wxPoint pos( m_area->GetPosition() );
    
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    wxRect rect(pos.x-45,0,45,client_height);
    Refresh(TRUE,&rect);
}

wxPlotCurve *wxPlotWindow::GetCurrent()
{
    return m_current;
}

void wxPlotWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    
    if (!m_current) return;
    
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    dc.SetPen( *wxBLACK_PEN );
    
    wxPoint pos( m_area->GetPosition() );
    
    double range = m_current->GetEndY() - m_current->GetStartY();
    double offset = ((double) m_current->GetOffsetY() / (double)client_height ) * range;
    double start = m_current->GetStartY() - offset;
    double end = m_current->GetEndY() - offset;
    int int_log_range = (int)floor( log10( range ) );
    double step = 1.0;
    if (int_log_range > 0)
    {
        for (int i = 0; i < int_log_range; i++)
           step *= 10; 
    }
    if (int_log_range < 0)
    {
        for (int i = 0; i < -int_log_range; i++)
           step /= 10; 
    }
    double lower = ceil(start / step) * step;
    double upper = floor(end / step) * step;
    int steps = (int)ceil((upper-lower)/step);
    if (steps < 3)
    {
        step /= 2;
        if (lower-step > start) lower -= step;
    }
    
    double current = lower;
    while (current < upper+(step/2))
    {
        int y = (int)((m_current->GetEndY()-current) / range * (double)client_height) - 1;
        y -= m_current->GetOffsetY();
        if ((y > 10) && (y < client_height-7))
        {
            dc.DrawLine( pos.x-15, y, pos.x-7, y );
            wxString label;
            label.Printf( "%.1f", current );
            dc.DrawText( label, pos.x-45, y-7 );
        }

        current += step;
    }
    
    dc.DrawLine( pos.x-15, 5, pos.x-15, client_height-5 );
    dc.DrawLine( pos.x-19, 9, pos.x-15, 5 );
    dc.DrawLine( pos.x-10, 10, pos.x-15, 5 );
    
}


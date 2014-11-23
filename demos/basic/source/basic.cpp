#include <wx/wx.h>
#include "vibrant/vibrant.hpp"

class SimpleVibrantApp : public wxApp
{
public:
	bool OnInit() override;
};


class SimpleVibrantFrame: public wxFrame
{
public:
    SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void onPaint(wxPaintEvent& event);
private:
    wxDECLARE_EVENT_TABLE();
};


wxBEGIN_EVENT_TABLE(SimpleVibrantFrame, wxFrame)
	EVT_PAINT(SimpleVibrantFrame::onPaint)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(SimpleVibrantApp);


bool SimpleVibrantApp::OnInit()
{
    SimpleVibrantFrame *frame = new SimpleVibrantFrame( "Simple Vibrant Demo", wxPoint(50, 50), wxSize(1280, 720) );
    frame->Show( true );
    return true;
}


SimpleVibrantFrame::SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
}

void SimpleVibrantFrame::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	dc.GetHDC();
}
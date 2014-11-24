#include <wx/wx.h>
#include "cairo/cairo.h"
#if defined(CAIRO_HAS_WIN32_SURFACE)
#	include "cairo/cairo-win32.h"
#endif
#if defined(CAIRO_HAS_QUARTZ_SURFACE)
#	include "cairo/cairo-quartz.h"
#endif
#if defined(CAIRO_HAS_XLIB_SURFACE)
#	include "cairo/cairo-xlib.h"
#endif

#include "vibrant/vibrant.hpp"
#include "vibrant/cairo/render.hpp"

using namespace entityx;
using namespace vibrant;

class SimpleVibrantApp : public wxApp
{
public:
	bool OnInit() override;
};

class BasicEntities : public EntityX {
public:
	BasicEntities()
	{
		render_system = std::make_shared<CairoRenderSystem>();
		systems.add(render_system);
		systems.configure();

		for (int i = 0; i < 100; ++i)
		{
			entityx::Entity entity = entities.create();
			entity.assign<Renderable>(Line(Vector2d(rand() % 1260, rand() % 700), Vector2d(rand() % 1260, rand() % 700)));
		}
	}

	void update(TimeDelta dt, cairo_t* context)
	{
		render_system->setContext(context);
		systems.update<CairoRenderSystem>(dt);
	}

private:
	std::shared_ptr<CairoRenderSystem> render_system;
};


class SimpleVibrantFrame: public wxFrame
{
public:
    SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void onPaint(wxPaintEvent& event);

private:
	BasicEntities basic_entities;


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
#if defined(_MSC_VER)
	wxPaintDC paint_dc(this); // mark as painted
	wxClientDC dc(this);  // unclipped
#else
	wxPaintDC dc(this); // mark as painted
#endif

#if defined(CAIRO_HAS_WIN32_SURFACE)
	cairo_surface_t* surface = cairo_win32_surface_create((HDC)dc.GetHDC());
#elif defined(CAIRO_HAS_QUARTZ_SURFACE)
	CGContextRef cg_context = (CGContextRef)dc.GetGraphicsContext()->GetNativeContext();
	assert(cg_context != 0);

	wxSize dc_size = dc.GetSize();
	cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(cg_context, dc_size.GetWidth(), dc_size.GetHeight());
#endif

	cairo_t* context = cairo_create(surface);
	cairo_set_source_rgb(context, 1, 1, 1);
	cairo_paint(context);

	basic_entities.update(1, context);

	cairo_destroy(context);
	cairo_surface_destroy(surface);
}
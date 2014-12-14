#include <wx/wx.h>
#include <chrono>
#include <functional>
#include "wx/dcbuffer.h"
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
using namespace std;
using namespace std::placeholders;

class SimpleVibrantApp : public wxApp
{
public:
	bool OnInit() override;
};



class BasicEntities : public EntityX {
public:
	BasicEntities()
	{
		systems.add<EasingSystem<double, Vector2d, Body>>();
		systems.add<EasingSystem<double, Radians, Body>>();
		render_system = std::make_shared<CairoRenderSystem>();
		systems.add(render_system);
		systems.configure();


		const int ENTITY_COUNT = 750;
		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			entityx::Entity entity = entities.create();

			entity.assign<Body>(Vector2d(sin(i/(double)ENTITY_COUNT * 2*M_PI)*300 + 640-50,
										 cos(i/(double)ENTITY_COUNT * 2*M_PI)*300 + 360-50),
								Vector2d(100, 100),
								rand() % 360 / 360.0 * 2 * M_PI);
			entity.assign<Renderable>(vibrant::Rectangle({ 0, Hsv(0,		0,	0, 0) },
														 {    Hsv(i/(double)ENTITY_COUNT,	1,	1, 0.015) }));

			// TODO: DSL for easing because this is clunky
			entity.assign<Ease<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return body->position; },
														entity.component<Body>()->position,
														Vector2d(640-50, 360-50),
														9000,
														&ease_out_elastic<double, Vector2d>);
			/* TODO: Support more than one easing for a given type
			entity.assign<Ease<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return body->size; },
														entity.component<Body>()->size,
														Vector2d(5, 5),
														10000,
														&ease_out_quad<double, Vector2d>);*/
			entity.assign<Ease<double, Radians, Body>>([](Body::Handle body) -> Radians& { return body->rotation; },
														entity.component<Body>()->rotation,
														20*M_PI,
														10000,
														&ease_inout_sine<double, Radians>);
		}

		/*entityx::Entity entity = entities.create();
		double y = rand() % int(720 * 0.8) + 20;
		entity.assign<Body>(Vector2d(640-50, 360-50),
							Vector2d(10, 10),
							rand() % 360 / 360.0 * 2*M_PI);
		entity.assign<Renderable>(vibrant::Rectangle({ 0, Hsv(0,		0,	0) },
													 {    Rgb(1,	1,	1) }));
		entity.assign<Ease<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return body->size; },
													entity.component<Body>()->size,
													Vector2d(100, 100),
													10000,
													&ease_out_quad<double, Vector2d>);
		entity.assign<Ease<double, Radians, Body>>([](Body::Handle body) -> Radians& { return body->rotation; },
													entity.component<Body>()->rotation,
													20*M_PI,
													10000,
													&ease_inout_sine<double, Radians>);*/

	}

	void update(TimeDelta dt, cairo_t* context)
	{
		render_system->setContext(context);
		systems.update<EasingSystem<double, Vector2d, Body>>(dt);
		systems.update<EasingSystem<double, Radians, Body>>(dt);
		systems.update<CairoRenderSystem>(dt);
	}

private:
	std::shared_ptr<CairoRenderSystem> render_system;
};

class RefreshTimer : public wxTimer
{
public:

	RefreshTimer(wxWindow* window) : wxTimer(), window(window) { }

	void Notify()
	{
		window->Refresh();
	}

	wxWindow* window;
};


class SimpleVibrantFrame: public wxFrame
{
public:
    SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void onPaint(wxPaintEvent& event);
	void onRefreshTimer(wxTimerEvent& event) { Refresh(); }
	void onIdle(wxIdleEvent& event);
	void draw(wxDC& dc);

private:
	BasicEntities basic_entities;
	cairo_surface_t* backbuffer = nullptr;
	Vector2u backbuffer_size;
	wxTimer refresh_timer;
	bool first_frame = true;
	std::chrono::steady_clock::time_point last_frame_end;


    wxDECLARE_EVENT_TABLE();
};


wxBEGIN_EVENT_TABLE(SimpleVibrantFrame, wxFrame)
	EVT_PAINT(SimpleVibrantFrame::onPaint)
	//EVT_TIMER(-1, SimpleVibrantFrame::onRefreshTimer)
	EVT_IDLE(SimpleVibrantFrame::onIdle)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(SimpleVibrantApp);


bool SimpleVibrantApp::OnInit()
{
    SimpleVibrantFrame *frame = new SimpleVibrantFrame( "Simple Vibrant Demo", wxPoint(50, 50), wxSize(1280, 720) );
    frame->Show(true);
    return true;
}


SimpleVibrantFrame::SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size), refresh_timer(this)
{
	//refresh_timer.Start(15);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SimpleVibrantFrame::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this); // mark as painted
	dc.DestroyClippingRegion();

	draw(dc);
}

void SimpleVibrantFrame::draw(wxDC& dc)
{
	auto current_time = std::chrono::steady_clock::now();
	if (first_frame)
	{
		last_frame_end = current_time;
		first_frame = false;
	}

	auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_frame_end).count();
	if (delta_ms < 16.67)
		return;
	last_frame_end = std::chrono::steady_clock::now();

	// Create or recreate the back buffer
	wxSize client_size = GetClientSize();
	if( backbuffer == nullptr || backbuffer_size.x < (unsigned int)client_size.GetWidth() || backbuffer_size.y < (unsigned int)client_size.GetHeight())
	{
		if(backbuffer)
			cairo_surface_destroy(backbuffer);

		backbuffer = cairo_image_surface_create(CAIRO_FORMAT_RGB24, client_size.GetWidth(), client_size.GetHeight());
		backbuffer_size = Vector2u(client_size.GetWidth(), client_size.GetHeight());
	}


	cairo_t* context = cairo_create(backbuffer);
	cairo_set_source_rgb(context, 1, 1, 1);
	cairo_paint(context);


	// Update systems to render
	basic_entities.update(delta_ms, context);


#if defined(CAIRO_HAS_WIN32_SURFACE)
	cairo_surface_t* surface = cairo_win32_surface_create((HDC)dc.GetHDC());
#elif defined(CAIRO_HAS_QUARTZ_SURFACE)
	CGContextRef cg_context = (CGContextRef)dc.GetGraphicsContext()->GetNativeContext();
	assert(cg_context != 0);

	wxSize dc_size = dc.GetSize();
	cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(cg_context, dc_size.GetWidth(), dc_size.GetHeight());
#endif
	cairo_t* dc_context = cairo_create(surface);
	cairo_set_source_surface(dc_context, backbuffer, 0, 0);
	cairo_set_operator(dc_context, CAIRO_OPERATOR_SOURCE);
	cairo_paint(dc_context);

	cairo_destroy(dc_context);
	cairo_surface_destroy(surface);

	cairo_destroy(context);

}

void SimpleVibrantFrame::onIdle(wxIdleEvent& event)
{
	wxClientDC dc(this);
    draw(dc);
    event.RequestMore(); // render continuously, not only once on idle
}


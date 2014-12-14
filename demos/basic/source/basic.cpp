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

/*struct Wiggle : entityx::Component<Wiggle>
{
	Wiggle(double size, double period, double time=0) : size(size), period(period), time(time) { }

	double size;
	double period;
	double time;
};

class WiggleSystem : public entityx::System < WiggleSystem >
{
public:
	WiggleSystem() { }

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
	{
		Wiggle::Handle wiggle;
		Body::Handle body;

		for (entityx::Entity entity : es.entities_with_components(wiggle, body))
		{
			wiggle->time += dt;
			body->position.x += sin(fmod(wiggle->time, wiggle->period) / wiggle->period * 2*M_PI) * wiggle->size/2;
			body->position.y += cos(fmod(wiggle->time, wiggle->period) / wiggle->period * 2*M_PI) * wiggle->size/2;
			//body->rotation += 0.01 * dt;
		}
	}
};*/

class BasicEntities : public EntityX {
public:
	BasicEntities()
	{
		//systems.add<WiggleSystem>();
		systems.add<EasingSystem<double, Vector2d, Body>>();
		systems.add<EasingSystem<double, Radians, Body>>();
		render_system = std::make_shared<CairoRenderSystem>();
		systems.add(render_system);
		systems.configure();

		for (int i = 0; i < 500; ++i)
		{
			entityx::Entity entity = entities.create();
			if (false && i % 2)
			{
				double y = rand() % 700 * 0.8 + 70;
				entity.assign<Body>(Vector2d(rand() % 1260*0.8 + 126, y),
									Vector2d(rand() % 5, rand() % 50),
									rand() % 360 / 360.0 * 2*M_PI);
				entity.assign<Renderable>(Line({ 2, Rgb(y/700.0,
														0,//rand() % 255 / 255.0,
														0) }));// rand() % 255 / 255.0) }));
				//entity.assign<Wiggle>(10, 1000, y/700*1000);
			}
			else
			{
				double y = rand() % int(720 * 0.8) + 20;
				entity.assign<Body>(Vector2d(rand() % 1280, y),
									Vector2d(100, 100),
									rand() % 360 / 360.0 * 2*M_PI);
				entity.assign<Renderable>(vibrant::Rectangle({ 2, Rgb(y/700.0, 0, 0) },
																	  //rand() % 255 / 255.0,
																	  //rand() % 255 / 255.0) },
															 { Rgb(y/700.0, 0, 0) }));
																   //rand() % 255 / 255.0,
																   //rand() % 255 / 255.0) }));
				/*entity.assign<Ease<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return body->position; },
															entity.component<Body>()->position,
															Vector2d(640, 360),
															3000,
															&ease_out_quad<double, Vector2d>);*/
				entity.assign<Ease<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return body->size; },
															entity.component<Body>()->size,
															Vector2d(5, 5),
															5000,
															&ease_out_quad<double, Vector2d>);
				entity.assign<Ease<double, Radians, Body>>([](Body::Handle body) -> Radians& { return body->rotation; },
															entity.component<Body>()->rotation,
															6*M_PI,
															5000,
															&ease_inout_sine<double, Radians>);
				//entity.assign<Wiggle>(10, 1000, y/700*1000);
			}
		}
	}

	void update(TimeDelta dt, cairo_t* context)
	{
		render_system->setContext(context);
		//systems.update<WiggleSystem>(dt);
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


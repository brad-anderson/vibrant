#include <wx/wx.h>
#include <chrono>
#include <functional>
#include "wx/dcbuffer.h"
#include "cairo/cairo.h"
#if defined(CAIRO_HAS_WIN32_SURFACE)
#include "cairo/cairo-win32.h"
#endif
#if defined(CAIRO_HAS_QUARTZ_SURFACE)
#include "cairo/cairo-quartz.h"
#endif
#if defined(CAIRO_HAS_XLIB_SURFACE)
#include "cairo/cairo-xlib.h"
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

class LayoutEntities : public EntityX
{
 public:
  LayoutEntities()
  {
    systems.add<EasingSystem<Body>>();
    systems.add<EasingSystem<Renderable>>();
    render_system = std::make_shared<CairoRenderSystem>();
    systems.add(render_system);
    mouse_system = std::make_shared<MouseSystem>();
    systems.add(mouse_system);
    layout_system = std::make_shared<LayoutSystem>();
    systems.add(layout_system);


    systems.configure();

    entityx::Entity button1 = entities.create();
    button1.assign<Body>(Vector2d(0, 0), Vector2d(0, 0));
    button1.assign<Renderable>(
        vibrant::Rectangle({0, Rgb(0, 0, 0)}, {Rgb(33 / 255.0, 150 / 255.0, 243 / 255.0)}), 1);
    button1.assign<Mouseable>();
    button1.assign<Layout>(0, 0, 0, 0);

    layout_system->solver.add_constraints(
        {button1.component<Layout>()->width >= 100,
         // button1.component<Layout>()->width == 100 | rhea::strength::strong(),

         button1.component<Layout>()->height >= 25,
         // button1.component<Layout>()->height == 25 | rhea::strength::strong(),
         button1.component<Layout>()->x ==
             layout_system->left_limit + layout_system->right_limit / 2.0,
         button1.component<Layout>()->y ==
             layout_system->top_limit + layout_system->bottom_limit / 2.0});
  }

  void update(TimeDelta dt, cairo_t* context)
  {
    systems.update<EasingSystem<Body>>(dt);
    systems.update<EasingSystem<Renderable>>(dt);
    systems.update<LayoutSystem>(dt);

    render_system->setContext(context);
    systems.update<CairoRenderSystem>(dt);
  }

  void updateMouse(MouseUpdate mouse) { mouse_system->update(entities, events, mouse); }

  std::shared_ptr<CairoRenderSystem> render_system;
  std::shared_ptr<MouseSystem> mouse_system;
  std::shared_ptr<LayoutSystem> layout_system;
};

class RefreshTimer : public wxTimer
{
 public:
  RefreshTimer(wxWindow* window) : wxTimer(), window(window) {}

  void Notify() { window->Refresh(); }

  wxWindow* window;
};

class SimpleVibrantFrame : public wxFrame
{
 public:
  SimpleVibrantFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  void onPaint(wxPaintEvent& event);
  void onRefreshTimer(wxTimerEvent& event) { Refresh(); }
  void onIdle(wxIdleEvent& event);
  void onMouse(wxMouseEvent& event);
  void onSize(wxSizeEvent& event);
  void draw(wxDC& dc);

 private:
  LayoutEntities layout_entities;
  cairo_surface_t* backbuffer = nullptr;
  Vector2u backbuffer_size;
  wxTimer refresh_timer;
  bool first_frame = true;
  std::chrono::steady_clock::time_point last_frame_end;

  wxDECLARE_EVENT_TABLE();
};

// clang-format off
wxBEGIN_EVENT_TABLE(SimpleVibrantFrame, wxFrame)
  EVT_PAINT(SimpleVibrantFrame::onPaint)
  // EVT_TIMER(-1, SimpleVibrantFrame::onRefreshTimer)
  EVT_IDLE(SimpleVibrantFrame::onIdle)
  EVT_MOUSE_EVENTS(SimpleVibrantFrame::onMouse)
  EVT_SIZE(SimpleVibrantFrame::onSize)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(SimpleVibrantApp);
// clang-format on

bool SimpleVibrantApp::OnInit()
{
  SimpleVibrantFrame* frame =
      new SimpleVibrantFrame("Simple Vibrant Demo", wxPoint(50, 50), wxSize(1280, 720));
  frame->Show(true);
  wxSize size = frame->GetClientSize();

  return true;
}

SimpleVibrantFrame::SimpleVibrantFrame(const wxString& title, const wxPoint& pos,
                                       const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), refresh_timer(this)
{
  // refresh_timer.Start(15);
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SimpleVibrantFrame::onPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);  // mark as painted
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

  auto delta_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_frame_end).count();
  // if (delta_ms < 16.67)
  if (delta_ms < 5.0) return;
  last_frame_end = std::chrono::steady_clock::now();

  // Create or recreate the back buffer
  wxSize client_size = GetClientSize();
  if (backbuffer == nullptr || backbuffer_size.x < (unsigned int)client_size.GetWidth() ||
      backbuffer_size.y < (unsigned int)client_size.GetHeight())
  {
    if (backbuffer) cairo_surface_destroy(backbuffer);

    backbuffer = cairo_image_surface_create(CAIRO_FORMAT_RGB24, client_size.GetWidth(),
                                            client_size.GetHeight());
    backbuffer_size = Vector2u(client_size.GetWidth(), client_size.GetHeight());
  }

  cairo_t* context = cairo_create(backbuffer);
  cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
  cairo_paint(context);

  // Update systems to render
  layout_entities.update(delta_ms, context);

#if defined(CAIRO_HAS_WIN32_SURFACE)
  cairo_surface_t* surface = cairo_win32_surface_create((HDC)dc.GetHDC());
#elif defined(CAIRO_HAS_QUARTZ_SURFACE)
  CGContextRef cg_context = (CGContextRef)dc.GetGraphicsContext()->GetNativeContext();
  assert(cg_context != 0);

  wxSize dc_size = dc.GetSize();
  cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(
      cg_context, dc_size.GetWidth(), dc_size.GetHeight());
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
  event.RequestMore();  // render continuously, not only once on idle
}

void SimpleVibrantFrame::onMouse(wxMouseEvent& event)
{
  MouseUpdate mouse;

  mouse.position = Vector2d(event.GetX(), event.GetY());

  if (event.LeftDown())
    mouse.left = ButtonState::Pressed;
  else if (event.LeftUp())
    mouse.left = ButtonState::Released;
  else if (event.LeftIsDown())
    mouse.left = ButtonState::Down;
  else if (event.LeftDClick())
    mouse.left = ButtonState::DoubleClicked;

  layout_entities.updateMouse(mouse);
}

void SimpleVibrantFrame::onSize(wxSizeEvent& event)
{
  layout_entities.layout_system->setSize(Vector2u(GetClientSize().x, GetClientSize().y));
  event.Skip();
}

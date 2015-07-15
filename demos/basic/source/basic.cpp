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

struct HoverColor : public System<HoverColor>, public Receiver<HoverColor>
{
  void configure(EventManager& events) override { events.subscribe<LeftClick>(*this); }

  void receive(const LeftClick& hover) { hovered.push_back(hover.entity); }

  void update(EntityManager& es, EventManager& events, TimeDelta dt) override
  {
    /*if (!hovered.empty())
    {
            fill_color_to(hovered.back(), Rgb(1,1,1), 400, Ease::OutSine);
            resize_to(hovered.back(), { 2000, 2000 }, 400, Ease::OutSine);
    }*/
    if (hovered.empty()) return;

    const int kleidoscope_count = 40;
    for (int i = 0; i < hovered.size(); ++i)
    {
      auto entity = hovered[hovered.size() - i - 1];
      if (i > kleidoscope_count)
      {
        entity.destroy();
        continue;
      }

      fill_color_to(entity, Hsv(i / (double)kleidoscope_count, 1, i % 2), 1000, Ease::InOutSine);
      resize_to(entity, {(double)i * 30, (double)i * 30}, 1000, Ease::InOutLinear);
      rotate_to(entity, M_TAU * 100 * i, 10000000, Ease::InOutLinear);
    }
    hovered.clear();
  }

  std::vector<Entity> hovered;
};

class BasicEntities : public EntityX
{
 public:
  BasicEntities()
  {
    systems.add<EasingSystem<Body>>();
    systems.add<EasingSystem<Renderable>>();
    render_system = std::make_shared<CairoRenderSystem>();
    systems.add(render_system);
    mouse_system = std::make_shared<MouseSystem>();
    systems.add(mouse_system);
    systems.add<HoverColor>();
    systems.configure();

    const int ENTITY_COUNT = 750;
    for (int i = 0; i < ENTITY_COUNT; ++i)
    {
      entityx::Entity entity = entities.create();

      entity.assign<Body>(Vector2d(sin(i / (double)ENTITY_COUNT * M_TAU) * 270 + 632,
                                   cos(i / (double)ENTITY_COUNT * M_TAU) * 270 + 340),
                          Vector2d(100, 100), rand() % 360 / 360.0 * M_TAU);
      entity.assign<Renderable>(
          vibrant::Rectangle({0, Hsv(0, 0, 0, 0)},
                             {Hsv(i / (double)ENTITY_COUNT, 1, 1, 750.0 / ENTITY_COUNT * 0.015)}),
          i);

      /*
      entity.assign<FastEase<double, Vector2d, Body>>([](Body::Handle body) -> Vector2d& { return
      body->position; },
                                                                                              entity.component<Body>()->position,
                                                                                              Vector2d(640-50,
      360-50),
                                                                                              10000,
                                                                                              &ease_out_elastic<double,
      Vector2d>);
      entity.assign<FastEase<double, Radians, Body>>([](Body::Handle body) -> Radians& { return
      body->rotation; },
                                                                                              entity.component<Body>()->rotation,
                                                                                              10*M_TAU
      + M_TAU/8,
                                                                                              10000,
                                                                                              &ease_inout_sine<double,
      Radians>);
      */

      move_to(entity, {632, 340}, 10000, Ease::OutElastic, 5000);
      rotate_to(entity, 10 * M_TAU + M_TAU / 8, 15000, Ease::OutSine);

      entity.assign<Mouseable>();
    }
  }

  void update(TimeDelta dt, cairo_t* context)
  {
    // systems.update<FastEasingSystem<double, Vector2d, Body>>(dt);
    // systems.update<FastEasingSystem<double, Radians, Body>>(dt);
    systems.update<EasingSystem<Body>>(dt);
    systems.update<EasingSystem<Renderable>>(dt);
    systems.update<HoverColor>(dt);

    render_system->setContext(context);
    systems.update<CairoRenderSystem>(dt);
  }

  void updateMouse(MouseUpdate mouse) { mouse_system->update(entities, events, mouse); }

 private:
  std::shared_ptr<CairoRenderSystem> render_system;
  std::shared_ptr<MouseSystem> mouse_system;
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

wxBEGIN_EVENT_TABLE(SimpleVibrantFrame, wxFrame) EVT_PAINT(SimpleVibrantFrame::onPaint)
    // EVT_TIMER(-1, SimpleVibrantFrame::onRefreshTimer)
    EVT_IDLE(SimpleVibrantFrame::onIdle) EVT_MOUSE_EVENTS(SimpleVibrantFrame::onMouse)
        wxEND_EVENT_TABLE()

            wxIMPLEMENT_APP(SimpleVibrantApp);

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
  basic_entities.update(delta_ms, context);

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

  basic_entities.updateMouse(mouse);
}

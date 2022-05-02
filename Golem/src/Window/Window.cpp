/*
 * Window.cpp
 *
 *  Created on: 01-May-2022
 *      Author: sujak
 */

#include <iostream>

#include "Window.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
Window *Window::Instance = nullptr;

void GLAPIENTRY
MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar *message,
				const void *userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}

Window::Window()
{
	// TODO Auto-generated constructor stub
	if (Instance != nullptr)
	{
		std::cout << "Window already exists.\n";
		return;
	}
	else
	{
		Instance = this;
	}
}

Window::~Window() {}

void Window::init()
{
	// FIXME: This shouldn't be here, should be in application etc.
	// glewInit();
	m_sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, m_flags);

	if (m_sdlWindow == nullptr)
	{
		std::cout << SDL_GetError();
		return;
	}

	m_windowId = SDL_GetWindowID(m_sdlWindow);

	SDL_SetWindowMinimumSize(m_sdlWindow, m_minWidth, m_minHeight);

	hide();

	m_context = SDL_GL_CreateContext(m_sdlWindow);
	int ver = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &ver);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << ver << std::endl;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &ver);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION : " << ver << std::endl;
	SDL_GL_SetSwapInterval(m_interval);

	glewInit();
	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	std::cout << "init" << std::endl;
	//	glDebugMessageCallback(MessageCallback, 0);

	show();

	// Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.f, 0.f, 0.f, 1.f);

	// Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		// printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		return;
	}
}
void Window::open()
{
	show();
	awake();
}
void Window::close()
{
	SDL_DestroyWindow(m_sdlWindow);
	m_sdlWindow = nullptr;

	clean();
}

void Window::focus()
{
	SDL_RaiseWindow(m_sdlWindow);
}

void Window::render()
{
	SDL_GL_MakeCurrent(this->m_sdlWindow, this->m_context);
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	{
		glClearColor(.3, .1, .2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static int prevWidth, prevHeight;

		if (prevWidth != m_width || prevHeight != m_height)
		{
			// gameCam->resizeTextures();

			prevWidth = m_width;
			prevHeight = m_height;
		}
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(-0.5f, 0.5f);
		glEnd();
	}

	SDL_GL_SwapWindow(m_sdlWindow);
}

// Updates windows contents.
void Window::update() {}

// Window dimensions.
int Window::getWidth()
{
	return m_width;
}
int Window::getHeight()
{
	return m_height;
}

// bool Window::hasMouseFocus() { ; }
// bool Window::hasKeyboardFocus() { ; }
// bool Window::isMinimized() { ; }
// bool Window::isShown() { ; }
// bool Window::isClosed() { ; }

void Window::hide() { SDL_HideWindow(m_sdlWindow); }
void Window::show() { SDL_ShowWindow(m_sdlWindow); }
void Window::maximise() { SDL_MaximizeWindow(m_sdlWindow); }
void Window::minimise() { SDL_MinimizeWindow(m_sdlWindow); }
void Window::restore() { SDL_RestoreWindow(m_sdlWindow); }

void Window::awake() { std::cout << "Calling Window::Awake().\n"; }

void Window::clean() { m_cleaned = true; }

void Window::gameLoop()
{
	std::cout << "Window::GameLoop() started" << std::endl;

	// start subsystems.

	// start game loop.

	while (!m_closed)
	{
		// Process Events
		processEvents();

		// update
		update();

		// render
		render();
	}
}

void Window::processEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{

		std::cout << "Window::processEvents() collected" << std::endl;
		if (e.type == SDL_WINDOWEVENT)
		{
			handleWindowEvent(e);

			// //FIXME: This should prolly not be here.
			// if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			//     ImGuiIO& io = ImGui::GetIO();
			//     io.DisplaySize = ImVec2(e.window.data1, e.window.data1);
			// }
		}
	}
}

void Window::handleWindowEvent(SDL_Event &e)
{
	// TODO: remove unnecessary window ID check.

	// If an event was detected for this window
	if (e.type == SDL_WINDOWEVENT && e.window.windowID == m_windowId) // && e.window.windowID == m_windowId)
	// TODO: Avoid unnecessarity searching and calling for events from window.
	{
		// Caption update flag.
		// bool updateCaption = false;

		switch (e.window.event)
		{
		// Window appeared.
		case SDL_WINDOWEVENT_SHOWN:
			m_shown = true;
			printf("W: Show\n");
			break;

		// Window disappeared.
		case SDL_WINDOWEVENT_HIDDEN:
			m_shown = false;
			printf("W: Hide\n");
			break;

		// Get new dimensions and repaint.
		case SDL_WINDOWEVENT_SIZE_CHANGED:

			m_width = e.window.data1;
			m_height = e.window.data2;

			// SDL_RenderPresent( m_renderer );
			// TODO: Rewrite above line for surface render...
			glViewport(0, 0, m_width, m_height);

			break;
		// Repaint on expose.
		case SDL_WINDOWEVENT_EXPOSED:
			// SDL_RenderPresent(m_renderer);

			printf("W: Exposed\n");
			break;

		// Mouse enter.
		case SDL_WINDOWEVENT_ENTER:
			m_mouseFocus = true;

			printf("M: Enter\n");
			break;

		// Mouse exit.
		case SDL_WINDOWEVENT_LEAVE:
			m_mouseFocus = false;

			printf("M: Leave\n");
			break;

		// Keyboard focus gained.
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			m_keyboardFocus = true;

			printf("K: focus Gained\n");
			break;

		// Keyboard focus lost.
		case SDL_WINDOWEVENT_FOCUS_LOST:
			m_keyboardFocus = false;

			printf("W: focus Lost\n");
			break;

		// Window minimized.
		case SDL_WINDOWEVENT_MINIMIZED:
			m_minimized = true;

			printf("W: MINIMIZED\n");
			break;

		// Window maxized.
		case SDL_WINDOWEVENT_MAXIMIZED:
			m_minimized = false;
			printf("W: MAXIMIZED\n");
			break;

		// Window restored.
		case SDL_WINDOWEVENT_RESTORED:
			m_minimized = false;

			printf("W: RESTORED\n");
			break;

		// Hide on close.
		case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow(m_sdlWindow);

			SDL_DestroyRenderer(m_sdlRenderer);
			SDL_DestroyWindow(m_sdlWindow);

			m_sdlWindow = NULL;
			m_sdlRenderer = NULL;
			m_closed = true;
			break;
		}
	}
}
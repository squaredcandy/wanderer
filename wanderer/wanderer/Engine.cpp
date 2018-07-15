#include "Engine.h"

namespace Wanderer::Engine
{
	SDL_Window * window;
	SDL_GLContext context;
	
	ImVec4 backgroundColor = ImColor(114, 144, 154);

	std::function<void()> initFunction = EmptyScene;
	std::function<void()> sceneFunction = EmptyScene;
	std::function<void()> tickFunction = EmptyScene;

	bool doTick = true;
	
	void APIENTRY glErrorCallback(GLenum source, GLenum type,
								  GLuint id, GLenum severity,
								  GLsizei length, const GLchar * message,
								  const void * userParam)
	{
		std::string sev;
		if (severity == GL_DEBUG_SEVERITY_LOW) 
			sev = "DEBUG SEVERITY LOW";
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
			sev = "DEBUG SEVERITY MEDIUM";
		else if (severity == GL_DEBUG_SEVERITY_HIGH) 
			sev = "DEBUG SEVERITY HIGH";
		else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) 
			sev = "DEBUG SEVERITY NOTIFICATION";
		fprintf(stderr, "%s\n%s\n", sev.c_str(), message);
		if (severity == GL_DEBUG_SEVERITY_HIGH)
		{
			std::cout << (char) 7 << std::endl;
			std::cin.get();
		}
	}

	void GLSettings()
	{
		// Enable Face Culling
		glEnable(GL_CULL_FACE);

		// Enable alpha blending for textures
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE);

		// Enable multisampling for our game
		glEnable(GL_MULTISAMPLE);

		// Enable Depth Testing for ordering primitives
		glEnable(GL_DEPTH_TEST);

		// Enable Debug output
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, 
							  GL_DONT_CARE, 0, nullptr, true);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, 
							  GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, 
							  false);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, 
							  GL_DONT_CARE, 0, nullptr, false);
		// 	glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//glPatchParameteri(GL_PATCH_VERTICES, 3);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void InitializeEngine()
	{
		// Init SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			printf("SDL Error: %s\n", SDL_GetError());
		}

		// Setup window
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG 
			| SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
			SDL_GL_CONTEXT_PROFILE_CORE);
		
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		//SDL_DisplayMode current;
		//SDL_GetCurrentDisplayMode(0, &current);
		window = SDL_CreateWindow("Wanderer",
								  SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED,
								  1600, 900,
								  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		context = SDL_GL_CreateContext(window);
		SDL_GL_SetSwapInterval(1); // Enable vsync
		gl3wInit();

		// Setup ImGui binding
		ImGui::CreateContext();
		ImGui_ImplSdlGL3_Init(window);

		// Load Icon
// 		SDL_Surface *surface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
// 		Uint16 pixels[16 * 2] = {  // ...or with raw pixel data:
// 			0xFFFC, 0x777F, 0xFFFC, 0x777F, 
//			0xFFFC, 0x777F, 0xFFFC, 0x777F,
//			0xFFFC, 0x777F, 0xFFFC, 0x777F,
//			0xFFFC, 0x777F, 0xFFFC, 0x777F, 
//			0xFFFC, 0x777F, 0xFFFC, 0x777F, 
//			0xFFFC, 0x777F, 0xFFFC, 0x777F,
//			0xFFFC, 0x777F, 0xFFFC, 0x777F, 
// 			0xFFFC, 0x777F, 0xFFFC, 0x777F
// 		};
// 
// 		surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 16 * 2, 0x0f00, 0x00f0, 0x000f, 0xf000);
// 
// 		// The icon is attached to the window pointer
// 		SDL_SetWindowIcon(window, surface);
// 
// 		// ...and the surface containing the icon pixel data is no longer required.
// 		SDL_FreeSurface(surface);


		Font::InitFontLibrary();
		Font::LoadFontFace("ProggyClean.ttf");

		GLSettings();

		initFunction();
	}

	void GameLoop()
	{
		bool done = false;
		while (!done)
		{
			auto totalTimer = HR_Clock::now();
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				ImGui_ImplSdlGL3_ProcessEvent(&event);
				if (event.type == SDL_QUIT)
					done = true;
			}
			ImGui_ImplSdlGL3_NewFrame(window);

			auto startTimer = HR_Clock::now();
			if(doTick) tickFunction();
			Debug::UpdateTime(TIMER_UPDATE, startTimer);
			
			startTimer = HR_Clock::now();
			sceneFunction();
			Debug::UpdateTime(TIMER_UI, startTimer);
			
			startTimer = HR_Clock::now();
			Render::RenderDungeon();
			Debug::UpdateTime(TIMER_GRAPHICS, startTimer);

			ClearWindow();
			ImGui::Render();
			ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
			Debug::UpdateTime(TIMER_TOTAL, totalTimer);
			SDL_GL_SwapWindow(window);
			//DrawWindow();

			Debug::renderTimeIdx = (Debug::renderTimeIdx + 1) % DEBUG_LENGTH;
		}
	}

	void EmptyScene()
	{
	}

	void ClearWindow()
	{
		glViewport(0, 0,
				   (int) ImGui::GetIO().DisplaySize.x,
				   (int) ImGui::GetIO().DisplaySize.y);
		glClearColor(backgroundColor.x, backgroundColor.y, 
					 backgroundColor.z, backgroundColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DrawWindow()
	{
		ImGui::Render();
		ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	void Cleanup()
	{
		Shaders::Cleanup();
		Render::Cleanup();
		Textures::Cleanup();
		Meshes::Cleanup();
		Dungeon::Cleanup();
		
		FT_Done_Face(Font::fontFace);
		FT_Done_FreeType(Font::fontLib);

		ImGui_ImplSdlGL3_Shutdown();
		ImGui::DestroyContext();

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}
#include "duckWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <stb_image.h>

#include <iostream>
#include <cmath>

std::string duckWindow::m_shaderBasePath = "shaders/duck/";

duckWindow::duckWindow()
    : m_gen(m_rd()), 
      m_uniformZeroToOne(0.0f, 1.0f),

      m_BSpline(
        {0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, 0.0f},
        {-0.5f, 0.0f, 0.5f},
        {0.0f, 0.0f, 0.5f},
        {-0.95f, 0.0f, -0.95f},
        {0.95f, 0.0f, 0.95f}
      )
{}

duckWindow::~duckWindow()
{}

void duckWindow::RunInit()
{
    // GUI 
    // ======================
    m_gui_FrameNumToSumCounter = 0;
    m_gui_FrameNumRenderTimeCounter = 0.0f;
    m_gui_FrameNumToSum = 100;
    m_gui_AvarageFrameNumRenderTime = 0.0f;

    const GLubyte* renderer = glGetString(GL_RENDERER);
    m_gui_renderer = std::string((const char*)renderer);

    // GLFW 
    // ======================

    SetUpWindowsCallbacks();
    m_windowState = wState::DEFALUT;

    // Scene
    // ======================
    
    // camera
    m_camera.UpdateRotation(glm::radians(-45.0f), glm::radians(-30.0f));

    // Lights
    m_obj_lights.reserve(m_maxLightsNum);
    m_ambientColor = glm::vec4(1.0f);
    light l; 
    l.InitGL(); 

    l.m_position = glm::vec4(0.4f, 0.5f, 0.4f, 1.0f);
    l.m_diffuseColor = glm::vec4(0.7f, 0.7f, 1.0f, 1.0f);
    l.m_specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_obj_lights.push_back(l);

    l.m_position = glm::vec4(-0.4f, 0.5f, -0.4f, 1.0f);
    l.m_diffuseColor = glm::vec4(1.0f, 0.7f, 0.7f, 1.0f);
    l.m_specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_obj_lights.push_back(l);

    m_sh_light.Init();
    m_sh_light.AttachShader(shPath("lightBillboard.vert"), GL_VERTEX_SHADER);
    m_sh_light.AttachShader(shPath("lightBillboard.frag"), GL_FRAGMENT_SHADER);
    m_sh_light.Link();

    // Materials
    material m; 
    m.ka = glm::vec3(0.2f);
    m.kd = glm::vec3(1.0f);
    m.ks = glm::vec3(0.5f);
    m.shininess = 128.0f;
    m_materials.insert(std::make_pair("water", m));

    m.ka = glm::vec3(0.2f);
    m.kd = glm::vec3(1.0f);
    m.ks = glm::vec3(0.5f);
    m.shininess = 32.0f;
    m_materials.insert(std::make_pair("duck", m));

    // Water
    m_obj_water.InitGL(m_shaderBasePath);
    m_waterColor = glm::vec3(0.0288, 0.960, 0.882);

    m_sh_water.Init();
    m_sh_water.AttachShader(shPath("water.vert"), GL_VERTEX_SHADER);
    m_sh_water.AttachShader(shPath("water.frag"), GL_FRAGMENT_SHADER);
    m_sh_water.Link();

    m_sh_water.Use();
    m_sh_water.set1i("numberOfLights", m_obj_lights.size());
    m_sh_water.set1f("a", m_obj_water.GetA());

    // skyBox
    m_obj_skyBox.InitGL();
    
    m_sh_skyBox.Init();
    m_sh_skyBox.AttachShader(shPath("skyBox.vert"), GL_VERTEX_SHADER);
    m_sh_skyBox.AttachShader(shPath("skyBox.frag"), GL_FRAGMENT_SHADER);
    m_sh_skyBox.Link();

    // std::vector<std::string> defaultSkyBox = {
    //     "resources/textures/CM_skybox/right.jpg",
    //     "resources/textures/CM_skybox/left.jpg",
    //     "resources/textures/CM_skybox/top.jpg",
    //     "resources/textures/CM_skybox/bottom.jpg",
    //     "resources/textures/CM_skybox/front.jpg",
    //     "resources/textures/CM_skybox/back.jpg"
    // }; 

    std::vector<std::string> cadcam = {
        "resources/textures/CM_310/px.png",
        "resources/textures/CM_310/nx.png",
        "resources/textures/CM_310/py.png",
        "resources/textures/CM_310/ny.png",
        "resources/textures/CM_310/pz.png",
        "resources/textures/CM_310/nz.png"
    }; 

    PrepareCubeMapTexture(cadcam);

    // duck 
    m_obj_duck.InitGLFromFile("resources/meshes/duck/duck.txt");
    
    m_sh_duck.Init();
    m_sh_duck.AttachShader(shPath("duck.vert"), GL_VERTEX_SHADER);
    m_sh_duck.AttachShader(shPath("duck.frag"), GL_FRAGMENT_SHADER);
    m_sh_duck.Link();

    m_sh_duck.Use();
    m_sh_duck.set1i("numberOfLights", m_obj_lights.size());

    PrepareDuckTexture("resources/meshes/duck/ducktex.jpg");

    // debug bezier curve
    m_obj_debugBezier.InitGL();
    glm::vec3 points[4] = {m_BSpline.m_p0, m_BSpline.m_p1, m_BSpline.m_p2, m_BSpline.m_p3};
    m_obj_debugBezier.UpdatePoints(points);

    m_sh_debugBezier.Init();
    m_sh_debugBezier.AttachShader(shPath("bezierCurve.vert"), GL_VERTEX_SHADER);
    m_sh_debugBezier.AttachShader(shPath("bezierCurve.geom"), GL_GEOMETRY_SHADER);
    m_sh_debugBezier.AttachShader(shPath("bezierCurve.frag"), GL_FRAGMENT_SHADER);
    m_sh_debugBezier.Link();

    // OpenGL initial configuration
    // ============================

    // subroutines
    m_duckLightFun.Phong = glGetSubroutineIndex(m_sh_duck.GetID(), GL_FRAGMENT_SHADER, "Phong");
    m_duckLightFun.PhongAnisotropic = glGetSubroutineIndex(m_sh_duck.GetID(), GL_FRAGMENT_SHADER, "PhongAnisotropic");

    // Bind UBOs
    m_UBO_viewProjection.CreateUBO(2 * sizeof(glm::mat4));
    m_UBO_viewProjection.BindBufferBaseToBindingPoint(0);

    m_sh_water.BindUniformBlockToBindingPoint("MatricesBlock", 0);
    m_sh_skyBox.BindUniformBlockToBindingPoint("MatricesBlock", 0);
    m_sh_duck.BindUniformBlockToBindingPoint("MatricesBlock", 0);
    m_sh_light.BindUniformBlockToBindingPoint("MatricesBlock", 0);
    m_sh_debugBezier.BindUniformBlockToBindingPoint("MatricesBlock", 0);

    m_UBO_lights.CreateUBO((1 + 3 * m_maxLightsNum) * sizeof(glm::vec4));
    m_UBO_lights.BindBufferBaseToBindingPoint(1);

    m_sh_water.BindUniformBlockToBindingPoint("LightsBlock", 1);
    m_sh_duck.BindUniformBlockToBindingPoint("LightsBlock", 1);

    // Bind Textures
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, m_obj_water.GetNormalTex());
    
    glActiveTexture(GL_TEXTURE1); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_gl_cubeMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gl_duckTex);

    // Set Clear colors and blending
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}

void duckWindow::PrepareCubeMapTexture(std::vector<std::string> files)
{
    glGenTextures(1, &m_gl_cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_gl_cubeMap);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < files.size() && i < 6; i++) {
        unsigned char *pixels = stbi_load(files[i].c_str(), &width, &height, &nrChannels, 0);
        if (pixels) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        } 
        else {
            std::cout << "Cube map read error: \n\t[file]:" << files[i] << std::endl;
        }
        stbi_image_free(pixels);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
}

void duckWindow::PrepareDuckTexture(std::string path)
{
    glGenTextures(1, &m_gl_duckTex);
    glBindTexture(GL_TEXTURE_2D, m_gl_duckTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (pixels) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
    } 
    else {
        std::cout << "Duck texture read error: \n\t[file]:" << path << std::endl;
    }
    stbi_image_free(pixels);
}

void duckWindow::RunRenderTick()
{
    // Process FrameCallback
    // =============================
    void EveryFrameCallback();

    // Render Scene
    // =============================

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateUBOs();
    UpdateDuckPosition();
    DisturbWater();

    DrawWater();
    DrawSkyBox();
    DrawDuck();
    DrawLights();
    DrawDebugCurve();

    RenderGUI();
}

void duckWindow::UpdateUBOs()
{
    float aspect = static_cast<float>(m_width)/m_height;

    glm::mat4 view = m_camera.GetViewMatrix();
    glm::mat4 projection = m_camera.GetProjectionMatrix(aspect);
    glm::mat4 viewProj[2] = {view, projection};

    m_UBO_viewProjection.BindUBO();
    m_UBO_viewProjection.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

    m_UBO_lights.BindUBO();
    m_UBO_lights.SetBufferData(0, &m_ambientColor, sizeof(glm::vec4));
    m_UBO_lights.SetBufferData(sizeof(glm::vec4), m_obj_lights.data(), 3 * sizeof(glm::vec4) * m_obj_lights.size());
}

void duckWindow::UpdateDuckPosition()
{
    m_duckPosition = m_BSpline.GetCurvePosition(m_duckTime);
    m_duckViewDir = m_BSpline.GetCurveTangent(m_duckTime);
    m_duckTime += m_deltaTime * m_duckSpeed;
    if (m_duckTime >= 1.0f) {
        m_duckTime = 0.0f;
        m_BSpline.GenerateSubsequentCurve();
        glm::vec3 points[4] = {m_BSpline.m_p0, m_BSpline.m_p1, m_BSpline.m_p2, m_BSpline.m_p3};
        m_obj_debugBezier.UpdatePoints(points);
    }
}

void duckWindow::DrawLights()
{
    m_sh_light.Use();
    m_sh_light.set2i("screenSize", m_width, m_height);

    for (int i = 0; i < m_obj_lights.size(); i++) { 
        m_sh_light.set3fv("colorDiffuse", m_obj_lights[i].m_diffuseColor);
        m_sh_light.set3fv("colorSpecular", m_obj_lights[i].m_specularColor);
        m_sh_light.set3fv("billboardPos", m_obj_lights[i].m_position);

        m_obj_lights[i].Draw();
    }
}

void duckWindow::DrawWater()
{   
    m_obj_water.SimulateWater(m_deltaTime);

    // Draw water
    // =============================
    m_sh_water.Use();
    
    const material& mat = m_materials["water"]; 
    m_sh_water.set3fv("material.ka", mat.ka);
    m_sh_water.set3fv("material.kd", mat.kd);
    m_sh_water.set3fv("material.ks", mat.ks);
    m_sh_water.set1f("material.shininess", mat.shininess);

    m_sh_water.set3fv("cameraPos", m_camera.m_worldPos);
    m_sh_water.set3fv("objectColor", m_waterColor);

    m_obj_water.Draw();
}

void duckWindow::DrawSkyBox()
{
    m_sh_skyBox.Use();

    glEnable(GL_CULL_FACE);
    m_obj_skyBox.Draw();
    glDisable(GL_CULL_FACE);
}

void duckWindow::DrawDuck()
{
    glm::vec3 V = glm::normalize(m_duckViewDir);
    glm::vec3 D = glm::vec3(-1.0f, 0.0f, 0.0f);  // Default duck's view direction
    glm::vec3 DcrossV = glm::cross(D, V);
    float DdotV = glm::dot(D, V);
    float angle = glm::acos(DdotV);

    glm::mat4 model = 
        glm::translate(glm::mat4(1.0f), m_duckPosition) *  
        glm::rotate(glm::mat4(1.0f), angle, DcrossV) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(0.001f));

    m_sh_duck.Use();
    m_sh_duck.setM4fv("model", GL_FALSE, model);

    GLuint* shadingFunction = m_gui_bAnisotropicDuck ? 
        &m_duckLightFun.PhongAnisotropic :
        &m_duckLightFun.Phong;

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, shadingFunction);
    m_sh_duck.set1f("anisoA", m_aniso_a);
    m_sh_duck.set1f("anisoB", m_aniso_b);
    m_sh_duck.set1f("anisoShininess", m_aniso_shininess);

    const material& mat = m_materials["duck"]; 
    m_sh_duck.set3fv("material.ka", mat.ka);
    m_sh_duck.set3fv("material.kd", mat.kd);
    m_sh_duck.set3fv("material.ks", mat.ks);
    m_sh_duck.set1f("material.shininess", mat.shininess);

    m_sh_duck.set3fv("cameraPos", m_camera.m_worldPos);
    
    m_obj_duck.Draw();
}

void duckWindow::DrawDebugCurve()
{
    if (m_gui_bDrawCurve) {
        m_sh_debugBezier.Use();
        m_obj_debugBezier.Draw();
    }
}

void duckWindow::DisturbWater()
{
    //drops
    float a = m_obj_water.GetA();

    if (!m_bDrop && m_uniformZeroToOne(m_gen) <= m_dropProbability) {
        m_bDrop = true;
        m_dropPos = glm::vec2(m_uniformZeroToOne(m_gen), m_uniformZeroToOne(m_gen));
        m_dropPos *= a;
        m_dropPos -= a/2;
        m_curretDropDepth = 0.0f;
    }    
    
    if (m_bDrop) {
        m_curretDropDepth -= (-m_dropMaxDepth / m_dropDepthTime) * m_deltaTime;
        m_obj_water.DisturbWaterAt(m_dropPos, m_curretDropDepth, 1);
        if (m_curretDropDepth <= m_dropMaxDepth) {
            m_bDrop = false;
        }
    }

    // duck
    glm::vec2 coords(m_duckPosition.x, m_duckPosition.z);
    m_obj_water.DisturbWaterAt(coords, m_duckMaxDepth, 0);
}


void duckWindow::RunClear()
{
    // Lights
    if (m_obj_lights.size() != 0) {
        m_obj_lights[0].DeInitGL();
    }
    m_sh_light.DeInitGL();

    // water
    m_obj_water.DeInitGL();
    m_sh_water.DeInitGL();
    
    // skyBox 
    m_obj_skyBox.DeInitGL();
    m_sh_skyBox.DeInitGL();

    // duck
    m_obj_duck.DeInitGL();
    m_sh_duck.DeInitGL();

    // bezier curve
    m_obj_debugBezier.DeInitGL();
    m_sh_debugBezier.DeInitGL();

    // UBOs
    m_UBO_viewProjection.DeleteUBO();
    m_UBO_lights.DeleteUBO();
}

void duckWindow::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, -1), ImVec2(FLT_MAX, -1), &duckWindow::InfoWindowSizeCallback, (void*)this);
    ImGui::Begin("Project: Duck", (bool*)0, flags);
    GenGUI_AppStatistics();
    GenGUI_Light();
    GenGUI_Materials();
    GenGUI_Duck();
    GenGUI_Simulation();
    //ImGui::ShowDemoWindow();
    ImGui::End();

    // *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void duckWindow::GenGUI_AppStatistics()
{
    if (m_gui_FrameNumToSumCounter >= m_gui_FrameNumToSum) {
        m_gui_FrameNumToSumCounter = 0;
        m_gui_AvarageFrameNumRenderTime = m_gui_FrameNumRenderTimeCounter / m_gui_FrameNumToSum;
        m_gui_FrameNumRenderTimeCounter = 0.0f;
        m_gui_FrameNumToSum = static_cast<int>(std::max(std::ceilf(0.5f / m_gui_AvarageFrameNumRenderTime), 1.0f));
    }
    else {
        m_gui_FrameNumToSumCounter++;
        m_gui_FrameNumRenderTimeCounter += m_deltaTime;
    }

    auto resetFPSCounter = [&](int swapValue) {
        glfwSwapInterval(swapValue);
        m_gui_FrameNumToSumCounter = 1;
        m_gui_FrameNumToSum = 1;
    };

    static bool fpsLimit = true;
    bool prevFpsLimit = fpsLimit;

    const char* swapIntervals[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    static int currentSwapInterval = 0;

    if(ImGui::CollapsingHeader("App Statistics"))
    {
        ImGui::TextColored(ImVec4(0.231f, 0.820f, 0.0984f, 1.0f), m_gui_renderer.c_str());

        if (ImGui::BeginTable("split", 2))
        {
            ImGui::TableNextColumn(); 
            ImGui::Text("FPS = %.2f", 1.0f / m_gui_AvarageFrameNumRenderTime);
            ImGui::TableNextColumn(); 
            ImGui::Checkbox("FPS limit", &fpsLimit);
            if (ImGui::BeginItemTooltip())
            {
                ImGui::TextColored(ImVec4(0.231f, 0.820f, 0.0984f, 1.0f), "With the limit maximum FSP number is bounded by a screen refresh rate.");
                ImGui::TextColored(ImVec4(0.231f, 0.820f, 0.0984f, 1.0f), "Without the limit maximum GPU and CPU generate as much FPS as it possible");
                ImGui::EndTooltip();
            }
            ImGui::TableNextColumn(); 
            ImGui::Text("MS = %.6f", m_gui_AvarageFrameNumRenderTime);

            if (prevFpsLimit != fpsLimit) {
                resetFPSCounter(fpsLimit ? currentSwapInterval + 1 : 0);
            }

            ImGui::TableNextColumn(); 
            ImGui::BeginDisabled(!fpsLimit);
            if (ImGui::BeginCombo("swap", swapIntervals[currentSwapInterval])) 
            {
                for (int i = 0; i < IM_ARRAYSIZE(swapIntervals); i++) {
                    const bool is_selected = (i == currentSwapInterval);
                    if (ImGui::Selectable(swapIntervals[i], is_selected)) 
                    {
                        currentSwapInterval = i;
                        resetFPSCounter(currentSwapInterval + 1);
                    }

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            } 
            ImGui::EndDisabled();
            
            ImGui::EndTable();
        }
    }
}

void duckWindow::GenGUI_Light()
{
    if(ImGui::CollapsingHeader("Lights")) 
    {
        ImGui::ColorEdit3("ambient color", (float*)&m_ambientColor, ImGuiColorEditFlags_NoInputs);
        for (int i = 0; i < m_obj_lights.size() && i < m_maxLightsNum; i++) {
            std::string separatorName = "light ";
            separatorName += std::to_string(i);
            
            ImGui::PushID(i);
            ImGui::SeparatorText(separatorName.c_str());
            if (ImGui::BeginTable("split", 2))
            {
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("diffuse", (float*)&m_obj_lights[i].m_diffuseColor, ImGuiColorEditFlags_NoInputs);
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("specular", (float*)&m_obj_lights[i].m_specularColor, ImGuiColorEditFlags_NoInputs);
                ImGui::EndTable();
            }
            ImGui::DragFloat3("position", (float*)&m_obj_lights[i].m_position, 0.1f);
            ImGui::PopID();
        }
    }
}

void duckWindow::GenGUI_Materials()
{
    if(ImGui::CollapsingHeader("Materials")) 
    {   
        int i = 0;
        for (auto& [key, value] : m_materials) {
            ImGui::PushID(i);
            ImGui::SeparatorText(key.c_str());
            if (ImGui::BeginTable("split", 3)) 
            {
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("ka", (float*)&value.ka, ImGuiColorEditFlags_NoInputs);
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("kd", (float*)&value.kd, ImGuiColorEditFlags_NoInputs);
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("ks", (float*)&value.ks, ImGuiColorEditFlags_NoInputs);
                ImGui::EndTable();
            }
            ImGui::DragFloat("shinness", &value.shininess, 1.0f);
            ImGui::PopID();
            i++;
        }
    }
}

void duckWindow::GenGUI_Duck()
{
    if(ImGui::CollapsingHeader("Duck")) 
    {   
        ImGui::SliderFloat("speed", &m_duckSpeed, 0.001f, 1.0f);
        ImGui::Checkbox("draw Curve", &m_gui_bDrawCurve);

        ImGui::Checkbox("anisotropic duck", &m_gui_bAnisotropicDuck);
        
        ImGui::BeginDisabled(!m_gui_bAnisotropicDuck);
        ImGui::DragFloat("shinnes", &m_aniso_shininess);
        ImGui::SliderFloat("a", &m_aniso_a, 0.0f, 1.0f);
        ImGui::SliderFloat("b", &m_aniso_b, 0.0f, 1.0f);
        ImGui::EndDisabled();
    }
}

void duckWindow::GenGUI_Simulation()
{
    static float maxWaterDepth = 20.0f;

    if (ImGui::CollapsingHeader("Simulation"))
    {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("drops' depth", &m_dropMaxDepth, 0.01f, -maxWaterDepth, maxWaterDepth);
        ImGui::DragFloat("duck's depth", &m_duckMaxDepth, 0.01f, -maxWaterDepth, maxWaterDepth);
        ImGui::PopItemWidth();
    }
}

void duckWindow::InfoWindowSizeCallback(ImGuiSizeCallbackData* data)
{
    duckWindow* win = reinterpret_cast<duckWindow*>(data->UserData);
    data->DesiredSize.y = win->m_height;    
}

void duckWindow::SetUpWindowsCallbacks()
{
    glfwSetFramebufferSizeCallback(m_window, &duckWindow::FramebufferSizeCallback);
    glfwSetMouseButtonCallback(m_window, &duckWindow::MouseButtonCallback);
    glfwSetScrollCallback(m_window, &duckWindow::ScrollCallback);
    glfwSetCursorPosCallback(m_window, &duckWindow::CursorPosCallback);
}   

void duckWindow::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    duckWindow* win = GW(window);

    win->m_width = static_cast<float>(width);
    win->m_height = static_cast<float>(height);
    
    glViewport(0, 0, width, height);
}

void duckWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // (1) GUI callback handling
    // =========================
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);
    if (io.WantCaptureMouse)
        return;

    // (2) App callback handling
    // =========================
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_RIGHT:
        MouseButton_RIGHT_Callback(window, action, mods);    
        break;
    case GLFW_MOUSE_BUTTON_LEFT:
        MouseButton_LEFT_Callback(window, action, mods);
        break;

    default:
        break;
    }
}

void duckWindow::MouseButton_RIGHT_Callback(GLFWwindow* window, int action, int mods)
{
    duckWindow* win = GW(window);

    if (win->GetState() == wState::DEFALUT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        win->m_mouse_prevPos.x = xpos;
        win->m_mouse_prevPos.y = ypos;

        win->SetState(wState::CAMERA_MOVE);
    }
    else if (win->GetState() == wState::CAMERA_MOVE && action == GLFW_RELEASE) {
        win->SetState(wState::DEFALUT);
    }
}

void duckWindow::MouseButton_LEFT_Callback(GLFWwindow* window, int action, int mods)
{
    duckWindow* win = GW(window);

    if (win->GetState() == wState::DEFALUT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        win->m_mouse_prevPos.x = xpos;
        win->m_mouse_prevPos.y = ypos;

        win->SetState(wState::CAMERA_ROTATE);
    }
    else if (win->GetState() == wState::CAMERA_ROTATE && action == GLFW_RELEASE) {
        win->SetState(wState::DEFALUT);
    }
}


void duckWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // (1) GUI callback handling
    // =========================
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(xoffset, yoffset);
    if (io.WantCaptureMouse)
        return;

    // (2) App callback handling
    // =========================

}

void duckWindow::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // (1) GUI callback handling
    // =========================
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(xpos, ypos);
    if (io.WantCaptureMouse)
        return;

    // (2) App callback handling
    // =========================
    duckWindow* win = GW(window);

    float deltaX = xpos - win->m_mouse_prevPos.x;
    float deltaY = ypos - win->m_mouse_prevPos.y;
    win->m_mouse_prevPos.x = xpos;
    win->m_mouse_prevPos.y = ypos;

    switch (win->GetState())
    {
    case wState::CAMERA_MOVE:
        {
            win->m_camera.UpdatePosition(deltaY * win->m_camera.m_cameraSpeed);
            glm::vec3 v = win->m_camera.m_worldPos;
        }
        break;
    
    case wState::CAMERA_ROTATE:
        win->m_camera.UpdateRotation(-deltaX * win->m_camera.m_cameraSpeed, -deltaY * win->m_camera.m_cameraSpeed);
        break;

    default:
        break;
    }
}

void duckWindow::EveryFrameCallback()
{

}

duckWindow* duckWindow::GW(GLFWwindow* window)
{
    return reinterpret_cast<duckWindow*>(glfwGetWindowUserPointer(window));
}

void duckWindow::SetState(wState newState)
{
    m_windowState = newState;   
}

duckWindow::wState duckWindow::GetState()
{
    return m_windowState;
}

std::string duckWindow::shPath(std::string fileName)
{
    return m_shaderBasePath + fileName;
}

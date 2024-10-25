#include "kolosal.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//-----------------------------------------------------------------------------
// [SECTION] Constants and Configurations
//-----------------------------------------------------------------------------

namespace Config
{
    // Using the same constants as defined in kolosal.h
    // Ensuring consistency between header and source files
    using namespace ::Config; // Importing all from Config namespace
}

//-----------------------------------------------------------------------------
// [SECTION] Global Variables
//-----------------------------------------------------------------------------

// Define global instance of ChatBot
ChatBot chatBot;

// Define global instance of MarkdownFonts
MarkdownFonts g_mdFonts;

// Define global instance of IconFonts
IconFonts g_iconFonts;

//-----------------------------------------------------------------------------
// [SECTION] Message Class Implementations
//-----------------------------------------------------------------------------

/**
 * @brief Constructs a new Message object.
 *
 * @param content The content of the message.
 * @param isUser Indicates if the message is from a user.
 */
Message::Message(std::string content, bool isUser)
    : content(std::move(content)), isUser(isUser), timestamp(std::chrono::system_clock::now()) {}

/**
 * @brief Gets the content of the message.
 *
 * @return std::string The content of the message.
 */
auto Message::getContent() const -> std::string
{
    return content;
}

/**
 * @brief Checks if the message is from a user.
 *
 * @return bool True if the message is from a user, false otherwise.
 */
auto Message::isUserMessage() const -> bool
{
    return isUser;
}

/**
 * @brief Gets the timestamp of the message in a formatted string.
 *
 * @return std::string The formatted timestamp of the message.
 */
auto Message::getTimestamp() const -> std::string
{
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm timeInfo;

#ifdef _WIN32
    localtime_s(&timeInfo, &time);
#else
    localtime_r(&time, &timeInfo);
#endif

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

//-----------------------------------------------------------------------------
// [SECTION] ChatHistory Class Implementations
//-----------------------------------------------------------------------------

/**
 * @brief Adds a message to the chat history.
 * @param message The message to add.
 */
void ChatHistory::addMessage(const Message &message)
{
    messages.push_back(message);
}

/**
 * @brief Retrieves the chat history.
 * @return A constant reference to the vector of messages.
 */
auto ChatHistory::getMessages() const -> const std::vector<Message> &
{
    return messages;
}

//-----------------------------------------------------------------------------
// [SECTION] ChatBot Class Implementations
//-----------------------------------------------------------------------------

/**
 * @brief Processes user input and generates a response.
 *
 * @param input The user's input string.
 */
void ChatBot::processUserInput(const std::string &input)
{
    chatHistory.addMessage(Message(input, true));
    std::string response = "Bot: " + input;
    chatHistory.addMessage(Message(response, false));
}

/**
 * @brief Retrieves the chat history.
 *
 * @return const ChatHistory& Reference to the chat history.
 */
auto ChatBot::getChatHistory() const -> const ChatHistory &
{
    return chatHistory;
}

//-----------------------------------------------------------------------------
// [SECTION] MarkdownRenderer Function Implementations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] GLFW and OpenGL Initialization Functions
//-----------------------------------------------------------------------------

/**
 * @brief Initializes the GLFW library.
 *
 * @return true if the GLFW library is successfully initialized, false otherwise.
 */
auto initializeGLFW() -> bool
{
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Creates a GLFW window with an OpenGL context.
 *
 * @return A pointer to the created GLFW window, or nullptr if creation fails.
 */
auto createWindow() -> GLFWwindow *
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ImGui Chatbot", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    return window;
}

/**
 * @brief Initializes the GLAD library to load OpenGL function pointers.
 *
 * @return true if GLAD is successfully initialized, false otherwise.
 */
auto initializeGLAD() -> bool
{
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGui Setup and Main Loop
//-----------------------------------------------------------------------------

/**
 * @brief Loads a font from a file and adds it to the ImGui context.
 * @param imguiIO The ImGuiIO object containing the font data.
 * @param fontPath The path to the font file.
 * @param fallbackFont The fallback font to use if loading fails.
 * @param fontSize The size of the font.
 * @return The loaded font, or the fallback font if loading fails.
 * @note If loading fails, an error message is printed to the standard error stream.
 * @note The fallback font is used if the loaded font is nullptr.
 *
 * This function loads a font from a file and adds it to the ImGui context.
 */
auto LoadFont(ImGuiIO &imguiIO, const char *fontPath, ImFont *fallbackFont, float fontSize) -> ImFont *
{
    ImFont *font = imguiIO.Fonts->AddFontFromFileTTF(fontPath, fontSize);
    if (font == nullptr)
    {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
        return fallbackFont;
    }
    return font;
}

/**
 * @brief Loads an icon font from a file and adds it to the ImGui context.
 * @param io The ImGuiIO object containing the font data.
 * @param iconFontPath The path to the icon font file.
 * @param fontSize The size of the font.
 * @return The loaded icon font, or the regular font if loading fails.
 * @note If loading fails, an error message is printed to the standard error stream.
 *
 * This function loads an icon font from a file and adds it to the ImGui context.
 */
auto LoadIconFont(ImGuiIO &io, const char *iconFontPath, float fontSize) -> ImFont *
{
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;            // Enable merging
    icons_config.PixelSnapH = true;           // Enable pixel snapping
    icons_config.GlyphMinAdvanceX = fontSize; // Use fontSize as min advance x

    // First load the regular font if not already loaded
    if (!g_mdFonts.regular)
    {
        g_mdFonts.regular = LoadFont(io, IMGUI_FONT_PATH_INTER_REGULAR, io.Fonts->AddFontDefault(), fontSize);
    }

    // Load and merge icon font
    ImFont *iconFont = io.Fonts->AddFontFromFileTTF(iconFontPath, fontSize, &icons_config, icons_ranges);
    if (iconFont == nullptr)
    {
        std::cerr << "Failed to load icon font: " << iconFontPath << std::endl;
        return g_mdFonts.regular;
    }

    return iconFont;
}

/**
 * @brief Sets up the ImGui context and initializes the platform/renderer backends.
 *
 * @param window A pointer to the GLFW window.
 */
void setupImGui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &imguiIO = ImGui::GetIO();

    // Set up the ImGui style
    g_mdFonts.regular = LoadFont(imguiIO, IMGUI_FONT_PATH_INTER_REGULAR, imguiIO.Fonts->AddFontDefault(), Config::Font::DEFAULT_FONT_SIZE);
    g_mdFonts.bold = LoadFont(imguiIO, IMGUI_FONT_PATH_INTER_BOLD, g_mdFonts.regular, Config::Font::DEFAULT_FONT_SIZE);
    g_mdFonts.italic = LoadFont(imguiIO, IMGUI_FONT_PATH_INTER_ITALIC, g_mdFonts.regular, Config::Font::DEFAULT_FONT_SIZE);
    g_mdFonts.boldItalic = LoadFont(imguiIO, IMGUI_FONT_PATH_INTER_BOLDITALIC, g_mdFonts.bold, Config::Font::DEFAULT_FONT_SIZE);
    g_mdFonts.code = LoadFont(imguiIO, IMGUI_FONT_PATH_FIRACODE_REGULAR, g_mdFonts.regular, Config::Font::DEFAULT_FONT_SIZE);

    // Load icon fonts
    g_iconFonts.regular = LoadIconFont(imguiIO, IMGUI_FONT_PATH_FA_REGULAR, Config::Icon::DEFAULT_FONT_SIZE);
    g_iconFonts.brands = LoadIconFont(imguiIO, IMGUI_FONT_PATH_FA_BRANDS, Config::Icon::DEFAULT_FONT_SIZE);

    imguiIO.FontDefault = g_mdFonts.regular;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

/**
 * @brief The main loop of the application, which handles rendering and event polling.
 *
 * @param window A pointer to the GLFW window.
 */
void mainLoop(GLFWwindow *window)
{
    bool focusInputField = true;
    float inputHeight = Config::INPUT_HEIGHT; // Set your desired input field height here

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ChatWindow::renderChatWindow(focusInputField, inputHeight);
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(Config::BackgroundColor::R, Config::BackgroundColor::G, Config::BackgroundColor::B, Config::BackgroundColor::A);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

/**
 * @brief Cleans up ImGui and GLFW resources before exiting the application.
 *
 * @param window A pointer to the GLFW window to be destroyed.
 */
void cleanup(GLFWwindow *window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

//-----------------------------------------------------------------------------
// [SECTION] Custom UI Functions
//-----------------------------------------------------------------------------

/**
 * @brief Renders a single button with the specified configuration.
 *
 * @param config The configuration for the button.
 */
void renderSingleButton(const ButtonConfig &config)
{
    std::string buttonText;
    bool hasIcon = !config.icon.empty();
    bool hasLabel = config.label.has_value();

    // Set the border radius (rounding) for the button
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Config::Button::RADIUS);

    if (hasIcon)
    {
        // Push icon font for icon rendering
        ImGui::PushFont(g_iconFonts.regular);
    }

    if (hasIcon && hasLabel)
    {
        buttonText = config.icon;
        if (ImGui::Button(buttonText.c_str(), ImVec2(config.size.x / 4, config.size.y)))
        {
            if (config.onClick)
                config.onClick();
        }

        ImGui::PopFont(); // Pop icon font
        ImGui::SameLine(0, config.padding);

        // Use regular font for label
        buttonText = config.label.value();
        if (ImGui::Button(buttonText.c_str(), ImVec2(3 * config.size.x / 4, config.size.y)))
        {
            if (config.onClick)
                config.onClick();
        }
    }
    else if (hasIcon)
    {
        buttonText = config.icon;
        if (ImGui::Button(buttonText.c_str(), config.size))
        {
            if (config.onClick)
                config.onClick();
        }
        ImGui::PopFont(); // Pop icon font
    }
    else if (hasLabel)
    {
        buttonText = config.label.value();
        if (ImGui::Button(buttonText.c_str(), config.size))
        {
            if (config.onClick)
                config.onClick();
        }
    }

    // Pop the border radius style
    ImGui::PopStyleVar();
}

/**
 * @brief Renders a group of buttons with the specified configurations.
 *
 * @param buttons The configurations for the buttons.
 * @param startX The X-coordinate to start rendering the buttons.
 * @param startY The Y-coordinate to start rendering the buttons.
 * @param spacing The spacing between buttons.
 */
void renderButtonGroup(const std::vector<ButtonConfig> &buttons, float startX, float startY, float spacing)
{
    ImGui::SetCursorPosX(startX);
    ImGui::SetCursorPosY(startY);

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        renderSingleButton(buttons[i]);

        if (i < buttons.size() - 1)
        {
            ImGui::SameLine(0.0f, spacing);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Chat Window Rendering Functions
//-----------------------------------------------------------------------------

/**
 * @brief Pushes an ID and sets the colors for the ImGui context based on the message type.
 *
 * @param msg The message object containing information about the message.
 * @param index The index used to set the ImGui ID.
 */
void ChatWindow::MessageBubble::pushIDAndColors(const Message &msg, int index)
{
    ImGui::PushID(index);

    ImVec4 userColor = ImVec4(Config::UserColor::COMPONENT, Config::UserColor::COMPONENT, Config::UserColor::COMPONENT, 1.0F); // #2f2f2f for user
    ImVec4 transparentColor = ImVec4(0.0F, 0.0F, 0.0F, 0.0F);                                                                  // Transparent for bot

    ImVec4 bgColor = msg.isUserMessage() ? userColor : transparentColor;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0F, 1.0F, 1.0F, 1.0F)); // White text
}

/**
 * @brief Calculates the dimensions for the message bubble.
 *
 * @param msg The message object containing information about the message.
 * @return A tuple containing:
 *         - windowWidth: The width of the ImGui window content region.
 *         - bubbleWidth: The width of the message bubble.
 *         - bubblePadding: The padding inside the message bubble.
 *         - paddingX: The horizontal padding to align the bubble.
 */
auto ChatWindow::MessageBubble::calculateDimensions(const Message &msg, float windowWidth) -> std::tuple<float, float, float>
{
    float bubbleWidth = windowWidth * Config::Bubble::WIDTH_RATIO; // 75% width for both user and bot
    float bubblePadding = Config::Bubble::PADDING;                 // Padding inside the bubble
    float paddingX = msg.isUserMessage()
                         ? (windowWidth - bubbleWidth - Config::Bubble::RIGHT_PADDING)
                         : Config::Bubble::BOT_PADDING_X;

    return {bubbleWidth, bubblePadding, paddingX};
}

/**
 * @brief Renders the message content inside the bubble.
 *
 * @param msg The message object containing the content to be displayed.
 * @param bubbleWidth The width of the message bubble.
 * @param bubblePadding The padding inside the message bubble.
 */
void ChatWindow::MessageBubble::renderMessageContent(const Message &msg, float bubbleWidth, float bubblePadding)
{
    ImGui::SetCursorPosX(bubblePadding);
    ImGui::SetCursorPosY(bubblePadding);
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + bubbleWidth - (bubblePadding * 2));
    ImGui::TextWrapped("%s", msg.getContent().c_str());
    ImGui::PopTextWrapPos();
}

/**
 * @brief Renders the timestamp of a message in the UI.
 *
 * @param msg The message object containing the timestamp to render.
 * @param bubblePadding The padding to apply on the left side of the timestamp.
 */
void ChatWindow::MessageBubble::renderTimestamp(const Message &msg, float bubblePadding)
{
    // Set timestamp color to a lighter gray
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7F, 0.7F, 0.7F, 1.0F)); // Light gray for timestamp

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() - (bubblePadding - Config::Timing::TIMESTAMP_OFFSET_Y)); // Align timestamp at the bottom
    ImGui::SetCursorPosX(bubblePadding);                                                                                                           // Align timestamp to the left
    ImGui::TextWrapped("%s", msg.getTimestamp().c_str());

    ImGui::PopStyleColor(); // Restore original text color
}

/**
 * @brief Renders interactive buttons for a message bubble.
 *
 * @param msg The message object containing the content and metadata.
 * @param index The index of the message in the message list.
 * @param bubbleWidth The width of the message bubble.
 * @param bubblePadding The padding inside the message bubble.
 */
void ChatWindow::MessageBubble::renderButtons(const Message &msg, int index, float bubbleWidth, float bubblePadding)
{
    ImVec2 textSize = ImGui::CalcTextSize(msg.getContent().c_str(), nullptr, true, bubbleWidth - bubblePadding * 2);
    float buttonPosY = textSize.y + bubblePadding;

    if (msg.isUserMessage())
    {
        ButtonConfig copyButtonConfig{
            .label = std::nullopt,
            .icon = ICON_FA_COPY,
            .size = ImVec2(Config::Button::WIDTH, 0),
            .padding = Config::Button::SPACING,
            .onClick = [&msg]()
            {
                ImGui::SetClipboardText(msg.getContent().c_str());
                std::cout << "Copied message content to clipboard" << std::endl;
            }};

        std::vector<ButtonConfig> userButtons = {copyButtonConfig};

        renderButtonGroup(
            userButtons,
            bubbleWidth - bubblePadding - Config::Button::WIDTH,
            buttonPosY);
    }
    else
    {
        ButtonConfig likeButtonConfig{
            .label = std::nullopt,
            .icon = ICON_FA_THUMBS_UP,
            .size = ImVec2(Config::Button::WIDTH, 0),
            .padding = Config::Button::SPACING,
            .onClick = [index]()
            {
                std::cout << "Like button clicked for message " << index << std::endl;
            }};

        ButtonConfig dislikeButtonConfig{
            .label = std::nullopt,
            .icon = ICON_FA_THUMBS_DOWN,
            .size = ImVec2(Config::Button::WIDTH, 0),
            .padding = Config::Button::SPACING,
            .onClick = [index]()
            {
                std::cout << "Dislike button clicked for message " << index << std::endl;
            }};

        std::vector<ButtonConfig> assistantButtons = { likeButtonConfig, dislikeButtonConfig };

        renderButtonGroup(
            assistantButtons,
            bubbleWidth - bubblePadding - (2 * Config::Button::WIDTH + Config::Button::SPACING),
            buttonPosY);
    }
}

/**
 * @brief Renders a message bubble with associated UI elements.
 *
 * @param msg The message object to be rendered.
 * @param index The index of the message in the list.
 */
void ChatWindow::MessageBubble::renderMessage(const Message &msg, int index, float contentWidth)
{
    ChatWindow::MessageBubble::pushIDAndColors(msg, index);

    float windowWidth = contentWidth; // Use contentWidth instead of ImGui::GetWindowContentRegionMax().x

    auto [bubbleWidth, bubblePadding, paddingX] = ChatWindow::MessageBubble::calculateDimensions(msg, windowWidth);

    ImVec2 textSize = ImGui::CalcTextSize(msg.getContent().c_str(), nullptr, true, bubbleWidth - bubblePadding * 2);
    float estimatedHeight = textSize.y + bubblePadding * 2 + ImGui::GetTextLineHeightWithSpacing(); // Add padding and spacing for buttons

    ImGui::SetCursorPosX(paddingX);

    if (msg.isUserMessage())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, Config::Style::CHILD_ROUNDING); // Adjust the rounding as desired
    }

    ImGui::BeginGroup();

    // Corrected BeginChild call
    ImGui::BeginChild(
        ("MessageCard" + std::to_string(index)).c_str(),
        ImVec2(bubbleWidth, estimatedHeight),
        false,                       // No border
        ImGuiWindowFlags_NoScrollbar // Supported flag
    );

    ChatWindow::MessageBubble::renderMessageContent(msg, bubbleWidth, bubblePadding);
    ChatWindow::MessageBubble::renderTimestamp(msg, bubblePadding);
    ChatWindow::MessageBubble::renderButtons(msg, index, bubbleWidth, bubblePadding);

    ImGui::EndChild();
    ImGui::EndGroup();

    if (msg.isUserMessage())
    {
        ImGui::PopStyleVar(); // Pop ChildRounding
    }

    ImGui::PopStyleColor(2); // Pop Text and ChildBg colors
    ImGui::PopID();
    ImGui::Spacing(); // Add spacing between messages
}

/**
 * @brief Renders the chat history with an auto-scroll feature.
 *
 * @param chatHistory The chat history object containing the messages to be displayed.
 */
void ChatWindow::renderChatHistory(const ChatHistory &chatHistory, float contentWidth)
{
    static size_t lastMessageCount = 0;
    size_t currentMessageCount = chatHistory.getMessages().size();

    // Check if new messages have been added
    bool newMessageAdded = currentMessageCount > lastMessageCount;

    // Save the scroll position before rendering
    float scrollY = ImGui::GetScrollY();
    float scrollMaxY = ImGui::GetScrollMaxY();
    bool isAtBottom = (scrollMaxY <= 0.0F) || (scrollY >= scrollMaxY - 1.0F);

    // Render messages
    const auto &messages = chatHistory.getMessages();
    for (size_t i = 0; i < messages.size(); ++i)
    {
        ChatWindow::MessageBubble::renderMessage(messages[i], static_cast<int>(i), contentWidth);
    }

    // If the user was at the bottom and new messages were added, scroll to bottom
    if (newMessageAdded && isAtBottom)
    {
        ImGui::SetScrollHereY(1.0F);
    }

    // Update the last message count
    lastMessageCount = currentMessageCount;
}

/**
 * @brief Renders the chat window to cover the full width and height of the application window.
 *
 * @param focusInputField A boolean reference to control the focus state of the input field.
 * @param inputHeight The height of the input field.
 */
void ChatWindow::renderChatWindow(bool &focusInputField, float inputHeight)
{
    ImGuiIO &imguiIO = ImGui::GetIO();

    // Set window to cover the entire display
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(imguiIO.DisplaySize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Chatbot", nullptr, window_flags);

    // Calculate available width and set max content width
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float contentWidth = (availableWidth < Config::CHAT_WINDOW_CONTENT_WIDTH) ? availableWidth : Config::CHAT_WINDOW_CONTENT_WIDTH;
    float paddingX = (availableWidth - contentWidth) / 2.0F;

    // Center the content horizontally
    if (paddingX > 0.0F)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingX);
    }

    // Begin a child window to contain the chat history and input field
    ImGui::BeginChild("ContentRegion", ImVec2(contentWidth, 0), false, ImGuiWindowFlags_NoScrollbar);

    // Calculate available height for the input field and chat history
    float availableHeight = ImGui::GetContentRegionAvail().y;

    // Adjust the height of the scrolling region
    float scrollingRegionHeight = availableHeight - inputHeight - Config::BOTTOM_MARGIN;

    // Ensure the scrolling region height is not negative
    if (scrollingRegionHeight < 0.0F)
    {
        scrollingRegionHeight = 0.0F;
    }

    // Begin the child window for the chat history
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollingRegionHeight), false, ImGuiWindowFlags_NoScrollbar);

    // Render chat history
    ChatWindow::renderChatHistory(chatBot.getChatHistory(), contentWidth);

    ImGui::EndChild();

    // Render the input field at the bottom
    ChatWindow::InputField::renderInputField(focusInputField, inputHeight, contentWidth);

    ImGui::EndChild(); // End of ContentRegion child window

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Input Field Functions
//-----------------------------------------------------------------------------

/**
 * @brief Sets the custom style for the input field.
 */
void ChatWindow::InputField::setInputFieldStyle()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Config::Style::FRAME_ROUNDING);                                                                                      // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Config::FRAME_PADDING_X, Config::FRAME_PADDING_Y));                                                            // Padding inside input field
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(Config::Style::INPUT_FIELD_BG_COLOR, Config::Style::INPUT_FIELD_BG_COLOR, Config::Style::INPUT_FIELD_BG_COLOR, 1.0F)); // Background color of the input field
}

/**
 * @brief Restores the original style settings for the input field.
 */
void ChatWindow::InputField::restoreInputFieldStyle()
{
    ImGui::PopStyleColor(1); // Restore FrameBg
    ImGui::PopStyleVar(2);   // Restore frame rounding and padding
}

/**
 * @brief Handles the submission of the input text.
 *
 * @param inputText The input text buffer.
 * @param focusInputField A reference to the focus input field flag.
 */
void ChatWindow::InputField::handleInputSubmission(char *inputText, bool &focusInputField)
{
    std::string inputStr(inputText);
    inputStr.erase(0, inputStr.find_first_not_of(" \n\r\t"));
    inputStr.erase(inputStr.find_last_not_of(" \n\r\t") + 1);

    if (!inputStr.empty())
    {
        chatBot.processUserInput(inputStr);
        inputText[0] = '\0'; // Empty the input after submission
    }

    focusInputField = true;
}

/**
 * @brief Renders the input field with text wrapping and no horizontal scrolling.
 *
 * @param focusInputField A reference to the focus input field flag.
 * @param inputHeight The height of the input field.
 */
void ChatWindow::InputField::renderInputField(bool &focusInputField, float inputHeight, float inputWidth)
{
    ChatWindow::InputField::setInputFieldStyle();

    static std::array<char, Config::InputField::TEXT_SIZE> inputText = {0};

    if (focusInputField)
    {
        ImGui::SetKeyboardFocusHere();
        focusInputField = false;
    }

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue |
                                ImGuiInputTextFlags_CtrlEnterForNewLine |
                                ImGuiInputTextFlags_ShiftEnterForNewLine;

    float availableWidth = ImGui::GetContentRegionAvail().x;
    float actualInputWidth = (inputWidth < availableWidth) ? inputWidth : availableWidth;
    float paddingX = (availableWidth - actualInputWidth) / Config::HALF_DIVISOR;

    if (paddingX > 0.0F)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingX);
    }

    // Draw the input field
    ImVec2 inputSize = ImVec2(actualInputWidth, inputHeight);

    // Begin a group to keep the draw calls together
    ImGui::BeginGroup();

    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + inputWidth - 15);

    if (ImGui::InputTextMultiline("##input", inputText.data(), inputText.size(), inputSize, flags))
    {
        ChatWindow::InputField::handleInputSubmission(inputText.data(), focusInputField);
    }

    ImGui::PopTextWrapPos();

    // If the input field is empty and not focused, draw the placeholder text
    bool isEmpty = (strlen(inputText.data()) == 0);

    if (isEmpty)
    {
        // Get the context and window information
        ImGuiContext &g = *ImGui::GetCurrentContext();
        ImGuiWindow *window = g.CurrentWindow;

        // Use the foreground draw list for the window
        ImDrawList *drawList = ImGui::GetForegroundDrawList(window);

        // Get the position and size of the input field
        ImVec2 inputFieldPos = ImGui::GetItemRectMin();

        const ImGuiStyle &style = ImGui::GetStyle();
        ImVec2 textPos = inputFieldPos;
        textPos.x += style.FramePadding.x;
        textPos.y += style.FramePadding.y;

        // Set the placeholder text color (light gray)
        ImU32 placeholderColor = ImGui::GetColorU32(ImVec4(0.7F, 0.7F, 0.7F, 1.0F));

        // Draw the placeholder text over the input field
        drawList->AddText(
            textPos,
            placeholderColor,
            "Type a message and press Enter to send (Ctrl+Enter for new line)");
    }

    ImGui::EndGroup();

    ChatWindow::InputField::restoreInputFieldStyle();
}

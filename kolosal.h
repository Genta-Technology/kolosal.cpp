#ifndef KOLASAL_H
#define KOLASAL_H

#include <string>
#include <vector>
#include <chrono>
#include <tuple>
#include <stack>
#include <regex>
#include <array>
#include <optional>
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5Brands.h"

//-----------------------------------------------------------------------------
// [SECTION] Constants and Configurations
//-----------------------------------------------------------------------------

namespace Config
{
    // Global constants for padding
    constexpr float FRAME_PADDING_X = 10.0F;
    constexpr float FRAME_PADDING_Y = 10.0F;

    // Constants to replace magic numbers
    namespace Font
    {
        constexpr float DEFAULT_FONT_SIZE = 16.0F;
    }

    namespace Icon
    {
        constexpr float DEFAULT_FONT_SIZE = 10.0F;
    }

    namespace BackgroundColor
    {
        constexpr float R = 0.1F;
        constexpr float G = 0.1F;
        constexpr float B = 0.1F;
        constexpr float A = 1.0F;
    }

    namespace UserColor
    {
        constexpr float COMPONENT = 47.0F / 255.0F;
    }

    namespace Bubble
    {
        constexpr float WIDTH_RATIO = 0.75F;
        constexpr float PADDING = 15.0F;
        constexpr float RIGHT_PADDING = 20.0F;
        constexpr float BOT_PADDING_X = 20.0F;
    }

    namespace Timing
    {
        constexpr float TIMESTAMP_OFFSET_Y = 5.0F;
    }

    namespace Button
    {
        constexpr float WIDTH = 30.0F;
        constexpr float SPACING = 10.0F;
        constexpr float RADIUS = 5.0F;
    }

    namespace Style
    {
        constexpr float CHILD_ROUNDING = 10.0F;
        constexpr float FRAME_ROUNDING = 12.0F;
        constexpr float INPUT_FIELD_BG_COLOR = 0.15F;
    }

    namespace InputField
    {
        constexpr size_t TEXT_SIZE = 1024;
    }

    constexpr float HALF_DIVISOR = 2.0F;
    constexpr float BOTTOM_MARGIN = 10.0F;
    constexpr float INPUT_HEIGHT = 100.0F;
    constexpr float CHAT_WINDOW_CONTENT_WIDTH = 750.0F;
}

//-----------------------------------------------------------------------------
// [SECTION] Structs and Enums
//-----------------------------------------------------------------------------

/**
 * @brief A struct to store the ImFont pointers for different markdown styles
 *
 * The MarkdownFonts struct stores the ImFont pointers for different markdown styles,
 * such as regular, bold, italic, bold italic, and code.
 */
struct MarkdownFonts
{
    ImFont *regular = nullptr;
    ImFont *bold = nullptr;
    ImFont *italic = nullptr;
    ImFont *boldItalic = nullptr;
    ImFont *code = nullptr;
};

/**
 * @brief A struct to store the ImFont pointers for different icon fonts
 *
 * The IconFonts struct stores the ImFont pointers for different icon fonts,
 * such as regular and brands.
 */
struct IconFonts
{
    ImFont *regular = nullptr;
    ImFont *brands = nullptr;
};

/**
 * @brief A struct to store the configuration for a button
 *
 * The ButtonConfig struct stores the configuration for a button, including the label,
 * icon, size, padding, and the onClick function.
 */
struct ButtonConfig
{
    std::optional<std::string> label;
    std::string icon;
    ImVec2 size;
    float padding;
    std::function<void()> onClick;
};

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations and Global Variables
//-----------------------------------------------------------------------------

// Forward declaration of GLFWwindow to avoid including GLFW/glfw3.h
struct GLFWwindow;

// Global chat bot instance
extern class ChatBot chatBot;

// Global markdown fonts
extern MarkdownFonts g_mdFonts;

// Global icon fonts
extern IconFonts g_iconFonts;

//-----------------------------------------------------------------------------
// [SECTION] Classes
//-----------------------------------------------------------------------------

/**
 * @brief A class to represent a chat message
 *
 * The Message class represents a chat message with the message content, a flag
 * to indicate whether the message is from the user, and a timestamp.
 */
class Message
{
public:
    // Constructors
    Message(std::string content, bool isUser);

    // Accessors
    auto getContent() const -> std::string;
    auto isUserMessage() const -> bool;
    auto getTimestamp() const -> std::string;

private:
    std::string content;
    bool isUser;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief A class to store chat history
 *
 * The ChatHistory class stores a list of Message objects to represent the chat
 * history. It provides a method to add a new message to the chat history.
 * The chat history can be retrieved as a vector of Message objects.
 */
class ChatHistory
{
public:
    void addMessage(const Message &message);
    auto getMessages() const -> const std::vector<Message> &;

private:
    std::vector<Message> messages;
};

/**
 * @brief A simple chat bot that echoes user input
 *
 * The ChatBot class processes user input and generates a response by echoing
 * the user's input. The chat history is stored in a ChatHistory object.
 */
class ChatBot
{
public:
    ChatBot() = default; // Use default constructor
    void processUserInput(const std::string &input);
    auto getChatHistory() const -> const ChatHistory &;

private:
    ChatHistory chatHistory;
};

//-----------------------------------------------------------------------------
// [SECTION] Function Prototypes
//-----------------------------------------------------------------------------

// Initialization and Cleanup Functions
auto initializeGLFW() -> bool;
auto createWindow() -> GLFWwindow *;
auto initializeGLAD() -> bool;
auto LoadIconFont(ImGuiIO &io, const char *iconFontPath, float fontSize) -> ImFont *;
auto LoadFont(ImGuiIO &imguiIO, const char *fontPath, ImFont *fallbackFont, float fontSize) -> ImFont *;
void setupImGui(GLFWwindow *window);
void mainLoop(GLFWwindow *window);
void cleanup(GLFWwindow *window);

// Custom UI Functions
void renderSingleButton(const ButtonConfig &config);
void renderButtonGroup(const std::vector<ButtonConfig> &buttons, float startX, float startY, float spacing = Config::Button::SPACING);

namespace ChatWindow
{
    void renderChatWindow(bool &focusInputField, float inputHeight);
    void renderChatHistory(const ChatHistory &chatHistory, float contentWidth);

    namespace MessageBubble
    {
        void renderMessage(const Message &msg, int index, float contentWidth);
        void pushIDAndColors(const Message &msg, int index);
        auto calculateDimensions(const Message &msg, float windowWidth) -> std::tuple<float, float, float>;
        void renderMessageContent(const Message &msg, float bubbleWidth, float bubblePadding);
        void renderTimestamp(const Message &msg, float bubblePadding);
        void renderButtons(const Message &msg, int index, float bubbleWidth, float bubblePadding);
    }

    namespace InputField
    {
        void setInputFieldStyle();
        void restoreInputFieldStyle();
        void handleInputSubmission(char *inputText, bool &focusInputField);
        void renderInputField(bool &focusInputField, float inputHeight, float inputWidth);
    }
}

#endif // KOLASAL_H

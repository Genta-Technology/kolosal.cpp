#include <gtest/gtest.h>
#include <chrono>
#include "kolosal.h"

//-----------------------------------------------------------------------------
// [SECTION] Test Cases
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Message Class Tests
//-----------------------------------------------------------------------------

// Test case for Message class constructor and content retrieval
TEST(MessageTest, ConstructorAndGetContent) {
    Message msg("Hello, World!", true);
    EXPECT_EQ(msg.getContent(), "Hello, World!");
    EXPECT_TRUE(msg.isUserMessage());
}

// Test case for timestamp formatting
TEST(MessageTest, GetTimestamp) {
    Message msg("Test Timestamp", false);
    std::string timestamp = msg.getTimestamp();
    
    // Check if timestamp is in the correct format (YYYY-MM-DD HH:MM:SS)
    EXPECT_EQ(timestamp.size(), 19);  // Format length should be 19 characters
    EXPECT_EQ(timestamp[4], '-');
    EXPECT_EQ(timestamp[7], '-');
    EXPECT_EQ(timestamp[10], ' ');
    EXPECT_EQ(timestamp[13], ':');
    EXPECT_EQ(timestamp[16], ':');
}

// Test case for adding and retrieving chat history
TEST(ChatHistoryTest, AddAndGetMessages) {
    ChatHistory chatHistory;
    Message msg1("Hello, World!", true);
    Message msg2("Bot: Hello, World!", false);

    chatHistory.addMessage(msg1);
    chatHistory.addMessage(msg2);

    const auto& messages = chatHistory.getMessages();
    EXPECT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].getContent(), "Hello, World!");
    EXPECT_EQ(messages[1].getContent(), "Bot: Hello, World!");
}

// Test case for ChatBot processing input and generating response
TEST(ChatBotTest, ProcessUserInput) {
    ChatBot chatBot;
    std::string userInput = "Hi!";
    chatBot.processUserInput(userInput);

    const auto& messages = chatBot.getChatHistory().getMessages();
    EXPECT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].getContent(), "Hi!");  // User message
    EXPECT_EQ(messages[1].getContent(), "Bot: Hi!");  // Bot response
}

//-----------------------------------------------------------------------------
// [SECTION] ImGui Functions Tests
//-----------------------------------------------------------------------------

// Test case for GLFW initialization
TEST(GLFWTest, InitializeGLFWSuccess) {
    // Assume glfwInit is mocked to return true for testing
    EXPECT_TRUE(initializeGLFW());
}

// Test case for GLFW window creation
TEST(GLFWTest, CreateWindowSuccess) {
    // Assume glfwCreateWindow is mocked to return a valid pointer
    GLFWwindow* window = createWindow();
    EXPECT_NE(window, nullptr);
}

// Test case for GLAD initialization
TEST(GLADTest, InitializeGLADSuccess) {
    // Assume gladLoadGLLoader is mocked to return true
    EXPECT_TRUE(initializeGLAD());
}

// Test case for ImGui setup
TEST(ImGuiTest, SetupImGui) {
    GLFWwindow* mockWindow = createWindow();  // Assuming a valid mock window
    ASSERT_NE(mockWindow, nullptr);

    setupImGui(mockWindow);
    // You can further test specific ImGui settings if necessary
}

//-----------------------------------------------------------------------------
// [SECTION] Main Function
//-----------------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}